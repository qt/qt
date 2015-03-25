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

#ifndef QVFBHDR_H
#define QVFBHDR_H

#include <QtGui/qcolor.h>
#include <QtGui/qwindowdefs.h>
#include <QtCore/qrect.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_QWS_TEMP_DIR
#  define QT_QWS_TEMP_DIR QLatin1String("/tmp")
#endif

#ifdef QT_PRIVATE_QWS
#define QT_VFB_DATADIR(DISPLAY)       QString::fromLatin1("%1/qtembedded-%2-%3") \
                                      .arg(QT_QWS_TEMP_DIR).arg(getuid()).arg(DISPLAY)
#define QT_VFB_MOUSE_PIPE(DISPLAY)    QT_VFB_DATADIR(DISPLAY) \
                                      .append(QLatin1String("/qtvfb_mouse"))
#define QT_VFB_KEYBOARD_PIPE(DISPLAY) QT_VFB_DATADIR(DISPLAY) \
                                      .append(QLatin1String("/qtvfb_keyboard"))
#define QT_VFB_MAP(DISPLAY)           QT_VFB_DATADIR(DISPLAY) \
                                      .append(QLatin1String("/qtvfb_map"))
#define QT_VFB_SOUND_PIPE(DISPLAY)    QT_VFB_DATADIR(DISPLAY) \
                                      .append(QLatin1String("/qt_soundserver"))
#define QTE_PIPE(DISPLAY)             QT_VFB_DATADIR(DISPLAY) \
                                      .append(QLatin1String("/QtEmbedded"))
#define QTE_PIPE_QVFB(DISPLAY)        QTE_PIPE(DISPLAY)
#else
#define QT_VFB_DATADIR(DISPLAY)       QString::fromLatin1("%1/qtembedded-%2") \
                                      .arg(QT_QWS_TEMP_DIR).arg(DISPLAY)
#define QT_VFB_MOUSE_PIPE(DISPLAY)    QString::fromLatin1("%1/.qtvfb_mouse-%2") \
                                      .arg(QT_QWS_TEMP_DIR).arg(DISPLAY)
#define QT_VFB_KEYBOARD_PIPE(DISPLAY) QString::fromLatin1("%1/.qtvfb_keyboard-%2") \
                                      .arg(QT_QWS_TEMP_DIR).arg(DISPLAY)
#define QT_VFB_MAP(DISPLAY)           QString::fromLatin1("%1/.qtvfb_map-%2") \
                                      .arg(QT_QWS_TEMP_DIR).arg(DISPLAY)
#define QT_VFB_SOUND_PIPE(DISPLAY)    QString::fromLatin1("%1/.qt_soundserver-%2") \
                                      .arg(QT_QWS_TEMP_DIR).arg(DISPLAY)
#define QTE_PIPE(DISPLAY)             QT_VFB_DATADIR(DISPLAY) \
                                      .append(QLatin1String("/QtEmbedded-%1")).arg(DISPLAY)
#define QTE_PIPE_QVFB(DISPLAY)        QTE_PIPE(DISPLAY)
#endif

struct QVFbHeader
{
    int width;
    int height;
    int depth;
    int linestep;
    int dataoffset;
    QRect update;
    bool dirty;
    int  numcols;
    QRgb clut[256];
    int viewerVersion;
    int serverVersion;
    int brightness; // since 4.4.0
    WId windowId; // since 4.5.0
};

struct QVFbKeyData
{
    unsigned int keycode;
    Qt::KeyboardModifiers modifiers;
    unsigned short int unicode;
    bool press;
    bool repeat;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QVFBHDR_H
