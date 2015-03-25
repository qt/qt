/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT_NO_QWS_INTEGRITYFB

#include <qscreenintegrityfb_qws.h>
#include <qwindowsystem_qws.h>
#include <qsocketnotifier.h>
#include <qapplication.h>
#include <qscreen_qws.h>
#include "qmouseintegrity_qws.h"
#include "qkbdintegrity_qws.h"
#include <qmousedriverfactory_qws.h>
#include <qkbddriverfactory_qws.h>
#include <qdebug.h>

#include <INTEGRITY.h>
#include <device/fbdriver.h>

QT_BEGIN_NAMESPACE

class QIntfbScreenPrivate
{
public:
    QIntfbScreenPrivate();
    ~QIntfbScreenPrivate();

    FBHandle handle;
    FBInfo fbinfo;
    FBDriver *fbdrv;

    QWSMouseHandler *mouse;
#ifndef QT_NO_QWS_KEYBOARD
    QWSKeyboardHandler *keyboard;
#endif
};

QIntfbScreenPrivate::QIntfbScreenPrivate()
    : mouse(0)

{
#ifndef QT_NO_QWS_KEYBOARD
    keyboard = 0;
#endif
}

QIntfbScreenPrivate::~QIntfbScreenPrivate()
{
    delete mouse;
#ifndef QT_NO_QWS_KEYBOARD
    delete keyboard;
#endif
}

/*!
    \internal

    \class QIntfbScreen
    \ingroup qws

    \brief The QIntfbScreen class implements a screen driver for the
    INTEGRITY framebuffer drivers.

    Note that this class is only available in \l{Qt for INTEGRITY}.
    Custom screen drivers can be added by subclassing the
    QScreenDriverPlugin class, using the QScreenDriverFactory class to
    dynamically load the driver into the application, but there should
    only be one screen object per application.

    \sa QScreen, QScreenDriverPlugin, {Running Applications}
*/

/*!
    \fn bool QIntfbScreen::connect(const QString & displaySpec)
    \reimp
*/

/*!
    \fn void QIntfbScreen::disconnect()
    \reimp
*/

/*!
    \fn bool QIntfbScreen::initDevice()
    \reimp
*/

/*!
    \fn void QIntfbScreen::restore()
    \reimp
*/

/*!
    \fn void QIntfbScreen::save()
    \reimp
*/

/*!
    \fn void QIntfbScreen::setDirty(const QRect & r)
    \reimp
*/

/*!
    \fn void QIntfbScreen::setMode(int nw, int nh, int nd)
    \reimp
*/

/*!
    \fn void QIntfbScreen::shutdownDevice()
    \reimp
*/

/*!
    \fn QIntfbScreen::QIntfbScreen(int displayId)

    Constructs a QVNCScreen object. The \a displayId argument
    identifies the Qt for Embedded Linux server to connect to.
*/
QIntfbScreen::QIntfbScreen(int display_id)
    : QScreen(display_id, IntfbClass), d_ptr(new QIntfbScreenPrivate)
{
    d_ptr->handle = 0;
    data = 0;
}

/*!
    Destroys this QIntfbScreen object.
*/
QIntfbScreen::~QIntfbScreen()
{
    delete d_ptr;
}

static QIntfbScreen *connected = 0;

bool QIntfbScreen::connect(const QString &displaySpec)
{
    CheckSuccess(gh_FB_get_driver(0, &d_ptr->fbdrv));
    CheckSuccess(gh_FB_check_info(d_ptr->fbdrv, &d_ptr->fbinfo));
    CheckSuccess(gh_FB_open(d_ptr->fbdrv, &d_ptr->fbinfo, &d_ptr->handle));
    CheckSuccess(gh_FB_get_info(d_ptr->handle, &d_ptr->fbinfo));

    data = (uchar *)d_ptr->fbinfo.Start;

    d = d_ptr->fbinfo.BitsPerPixel;
    switch (d) {
    case 1:
        setPixelFormat(QImage::Format_Mono);
        break;
    case 8:
        setPixelFormat(QImage::Format_Indexed8);
        break;
    case 12:
        setPixelFormat(QImage::Format_RGB444);
        break;
    case 15:
        setPixelFormat(QImage::Format_RGB555);
        break;
    case 16:
        setPixelFormat(QImage::Format_RGB16);
        break;
    case 18:
        setPixelFormat(QImage::Format_RGB666);
        break;
    case 24:
        setPixelFormat(QImage::Format_RGB888);
#ifdef QT_QWS_DEPTH_GENERIC
#if Q_BYTE_ORDER != Q_BIG_ENDIAN
            qt_set_generic_blit(this, 24,
                    d_ptr->fbinfo.Red.Bits,
                    d_ptr->fbinfo.Green.Bits,
                    d_ptr->fbinfo.Blue.Bits,
                    d_ptr->fbinfo.Alpha.Bits,
                    d_ptr->fbinfo.Red.Offset,
                    d_ptr->fbinfo.Green.Offset,
                    d_ptr->fbinfo.Blue.Offset,
                    d_ptr->fbinfo.Alpha.Offset);
#else
            qt_set_generic_blit(this, 24,
                    d_ptr->fbinfo.Red.Bits,
                    d_ptr->fbinfo.Green.Bits,
                    d_ptr->fbinfo.Blue.Bits,
                    d_ptr->fbinfo.Alpha.Bits,
                    16 - d_ptr->fbinfo.Red.Offset,
                    16 - d_ptr->fbinfo.Green.Offset,
                    16 - d_ptr->fbinfo.Blue.Offset,
                    16 - d_ptr->fbinfo.Alpha.Offset);
#endif
#endif
        break;
    case 32:
        setPixelFormat(QImage::Format_ARGB32_Premultiplied);
#ifdef QT_QWS_DEPTH_GENERIC
#if Q_BYTE_ORDER != Q_BIG_ENDIAN
            qt_set_generic_blit(this, 32,
                    d_ptr->fbinfo.Red.Bits,
                    d_ptr->fbinfo.Green.Bits,
                    d_ptr->fbinfo.Blue.Bits,
                    d_ptr->fbinfo.Alpha.Bits,
                    d_ptr->fbinfo.Red.Offset,
                    d_ptr->fbinfo.Green.Offset,
                    d_ptr->fbinfo.Blue.Offset,
                    d_ptr->fbinfo.Alpha.Offset);
#else
            qt_set_generic_blit(this, 32,
                    d_ptr->fbinfo.Red.Bits,
                    d_ptr->fbinfo.Green.Bits,
                    d_ptr->fbinfo.Blue.Bits,
                    d_ptr->fbinfo.Alpha.Bits,
                    24 - d_ptr->fbinfo.Red.Offset,
                    24 - d_ptr->fbinfo.Green.Offset,
                    24 - d_ptr->fbinfo.Blue.Offset,
                    24 - d_ptr->fbinfo.Alpha.Offset);
#endif
#endif
        break;
    }

    dw = w = d_ptr->fbinfo.Width;
    dh = h = d_ptr->fbinfo.Height;

    /* assumes no padding */
    lstep = w * ((d + 7) >> 3);

    mapsize = size =  h * lstep;

    /* default values */
    int dpi = 72;
    physWidth = qRound(dw * 25.4 / dpi);
    physHeight = qRound(dh * 25.4 / dpi);

    qDebug("Connected to INTEGRITYfb server: %d x %d x %d %dx%dmm (%dx%ddpi)",
        w, h, d, physWidth, physHeight, qRound(dw*25.4/physWidth), qRound(dh*25.4/physHeight) );


    QWSServer::setDefaultMouse("integrity");
    QWSServer::setDefaultKeyboard("integrity");

    connected = this;

    return true;
}

void QIntfbScreen::disconnect()
{
    connected = 0;
}

bool QIntfbScreen::initDevice()
{
#ifndef QT_NO_QWS_CURSOR
    QScreenCursor::initSoftwareCursor();
#endif
    return true;
}

void QIntfbScreen::shutdownDevice()
{
    gh_FB_close(d_ptr->handle);
}

void QIntfbScreen::setMode(int ,int ,int)
{
}

// save the state of the graphics card
// This is needed so that e.g. we can restore the palette when switching
// between linux virtual consoles.
void QIntfbScreen::save()
{
    // nothing to do.
}

// restore the state of the graphics card.
void QIntfbScreen::restore()
{
}
void QIntfbScreen::setDirty(const QRect& rect)
{
    FBRect fbrect;
    fbrect.dx = rect.x();
    fbrect.dy = rect.y();
    fbrect.Width = rect.width();
    fbrect.Height = rect.height();
    gh_FB_expose(d_ptr->handle, &fbrect, 0);
}

void QIntfbScreen::setBrightness(int b)
{
    if (connected) {
    }
}

void QIntfbScreen::blank(bool on)
{
}

#endif // QT_NO_QWS_INTEGRITYFB

QT_END_NAMESPACE

