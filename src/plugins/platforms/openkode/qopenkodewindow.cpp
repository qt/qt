/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qopenkodewindow.h"
#include "qopenkodeintegration.h"
#include "../eglconvenience/qeglplatformcontext.h"
#include "../eglconvenience/qeglconvenience.h"

#include <KD/kd.h>
#include <KD/NV_display.h>

#include <EGL/egl.h>

#include <QtGui/qwidget.h>
#include <QtGui/private/qwidget_p.h>
#include <QtGui/private/qapplication_p.h>

#include <QtCore/qvector.h>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

QOpenKODEWindow::QOpenKODEWindow(QWidget *tlw)
    : QPlatformWindow(tlw)
{
    if (tlw->platformWindowFormat().windowApi() == QPlatformWindowFormat::OpenVG) {
        m_eglApi = EGL_OPENVG_API;
    } else {
        m_eglContextAttrs.append(EGL_CONTEXT_CLIENT_VERSION);
        m_eglContextAttrs.append(2);

        m_eglApi = EGL_OPENGL_ES_API;
    }
    eglBindAPI(m_eglApi);

    m_eglContextAttrs.append(EGL_NONE);
    m_eglWindowAttrs.append(EGL_NONE);

    QList<QPlatformScreen *> screens = QApplicationPrivate::platformIntegration()->screens();
    //XXXX: jl figure out how to pick the correct screen.
//    Q_ASSERT(screens.size() > tlw->d_func()->screenNumber);
//    QOpenKODEScreen *screen = qobject_cast<QOpenKODEScreen *>(screens.at(tlw->d_func()->screenNumber));
    QOpenKODEScreen *screen = qobject_cast<QOpenKODEScreen *>(screens.at(0));
    if (!screen) {
        qErrnoWarning("Could not make QOpenKODEWindow without a screen");
    }

    QPlatformWindowFormat format = tlw->platformWindowFormat();
    format.setRedBufferSize(5);
    format.setGreenBufferSize(6);
    format.setBlueBufferSize(5);
    tlw->setPlatformWindowFormat(format);

    m_eglConfig = q_configFromQPlatformWindowFormat(screen->eglDisplay(),tlw->platformWindowFormat());

    m_kdWindow = kdCreateWindow(screen->eglDisplay(),
                              m_eglConfig,
                              KD_NULL);
    if (!m_kdWindow) {
        qErrnoWarning(kdGetError(), "Error creating native window");
        return;
    }

    const KDint windowSize[2]  = { tlw->width(), tlw->height()-1 };
    if (kdSetWindowPropertyiv(m_kdWindow, KD_WINDOWPROPERTY_SIZE, windowSize)) {
        qErrnoWarning(kdGetError(), "Could not set native window size");
        return;
    }

    KDboolean visibillity(false);
    if (kdSetWindowPropertybv(m_kdWindow, KD_WINDOWPROPERTY_VISIBILITY, &visibillity)) {
        qErrnoWarning(kdGetError(), "Could not set visibillity to false");
    }

    const KDint windowPos[2] = { tlw->x(), tlw->y() };
    if (kdSetWindowPropertyiv(m_kdWindow, KD_WINDOWPROPERTY_DESKTOP_OFFSET_NV, windowPos)) {
        qErrnoWarning(kdGetError(), "Could not set native window position");
        return;
    }

    if (kdRealizeWindow(m_kdWindow, &m_eglWindow)) {
        qErrnoWarning(kdGetError(), "Could not realize native window");
        return;
    }

    EGLSurface surface = eglCreateWindowSurface(screen->eglDisplay(),m_eglConfig,m_eglWindow,m_eglWindowAttrs.constData());

    m_platformGlContext = new QEGLPlatformContext(screen->eglDisplay(), m_eglConfig,
                                                  m_eglContextAttrs.data(), surface, m_eglApi);
}

QOpenKODEWindow::~QOpenKODEWindow()
{
    qDebug() << "destroying window" << m_kdWindow;
    delete m_platformGlContext;
    kdDestroyWindow(m_kdWindow);
}
void QOpenKODEWindow::setGeometry(const QRect &rect)
{
    const QRect geo = geometry();
    if (geo.size() != rect.size()) {
        const KDint windowSize[2]  = { rect.width(), rect.height() -1 };
        if (kdSetWindowPropertyiv(m_kdWindow, KD_WINDOWPROPERTY_SIZE, windowSize)) {
            qErrnoWarning(kdGetError(), "Could not set native window size");
            //return;
        }
    }

    if (geo.topLeft() != rect.topLeft()) {
        const KDint windowPos[2] = { rect.x(), rect.y() };
        if (kdSetWindowPropertyiv(m_kdWindow, KD_WINDOWPROPERTY_DESKTOP_OFFSET_NV, windowPos)) {
            qErrnoWarning(kdGetError(), "Could not set native window position");
            //return;
        }
    }

    //need to recreate context
    delete m_platformGlContext;

    QList<QPlatformScreen *> screens = QApplicationPrivate::platformIntegration()->screens();
    QOpenKODEScreen *screen = qobject_cast<QOpenKODEScreen *>(screens.at(0));
    EGLSurface surface = eglCreateWindowSurface(screen->eglDisplay(),m_eglConfig,m_eglWindow,m_eglWindowAttrs.constData());
    m_platformGlContext = new QEGLPlatformContext(screen->eglDisplay(),m_eglConfig,
                                                  m_eglContextAttrs.data(),surface,m_eglApi);
}

void QOpenKODEWindow::setVisible(bool visible)
{
    KDboolean visibillity(visible);
    if (kdSetWindowPropertybv(m_kdWindow, KD_WINDOWPROPERTY_VISIBILITY, &visibillity)) {
        qErrnoWarning(kdGetError(), "Could not set visibillity to false");
    }
}

QPlatformGLContext *QOpenKODEWindow::glContext()
{
    return m_platformGlContext;
}
QT_END_NAMESPACE
