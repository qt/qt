/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qdirectfbintegration.h"
#include "qdirectfbwindowsurface.h"
#include "qdirectfbblitter.h"
#include "qdirectfbconvenience.h"
#include "qdirectfbcursor.h"
#include "qdirectfbwindow.h"

#include <QtGui/private/qpixmap_blitter_p.h>
#include <QtGui/private/qpixmap_raster_p.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QThread>

#include "qgenericunixfontdatabase.h"

QT_BEGIN_NAMESPACE

QDirectFbIntegration::QDirectFbIntegration()
    : m_fontDb(new QGenericUnixFontDatabase())
{}

void QDirectFbIntegration::initialize()
{
    initializeDirectFB();
    initializeScreen();
    initializeInput();
}

void QDirectFbIntegration::initializeDirectFB()
{
    const QStringList args = QCoreApplication::arguments();
    int argc = args.size();
    char **argv = new char*[argc];

    for (int i = 0; i < argc; ++i)
        argv[i] = qstrdup(args.at(i).toLocal8Bit().constData());

    DFBResult result = DirectFBInit(&argc, &argv);
    if (result != DFB_OK) {
        DirectFBError("QDirectFBScreen: error initializing DirectFB",
                      result);
    }

    for (int i = 0; i < argc; ++i)
        delete[] argv[i];
    delete[] argv;

    // This must happen after DirectFBInit.
    m_dfb.reset(QDirectFbConvenience::dfbInterface());
}

void QDirectFbIntegration::initializeScreen()
{
    m_primaryScreen.reset(new QDirectFbScreen(0));
    mScreens.append(m_primaryScreen.data());
}

void QDirectFbIntegration::initializeInput()
{
    m_input.reset(new QDirectFbInput(m_dfb.data(), m_primaryScreen->dfbLayer()));
    m_input->start();
}

QDirectFbIntegration::~QDirectFbIntegration()
{
    m_input->stopInputEventLoop();
    m_input->wait();
}

QPixmapData *QDirectFbIntegration::createPixmapData(QPixmapData::PixelType type) const
{
    if (type == QPixmapData::BitmapType)
        return new QRasterPixmapData(type);
    else
        return new QDirectFbBlitterPlatformPixmap;
}

QPlatformWindow *QDirectFbIntegration::createPlatformWindow(QWidget *window, WId) const
{
    return new QDirectFbWindow(window,m_input.data());
}

QWindowSurface *QDirectFbIntegration::createWindowSurface(QWidget *window, WId) const
{
    return new QDirectFbWindowSurface(window);
}

QPlatformFontDatabase *QDirectFbIntegration::fontDatabase() const
{
    return m_fontDb.data();
}

QT_END_NAMESPACE
