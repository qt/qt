/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QOPENKODEGLINTEGRATION_H
#define QOPENKODEGLINTEGRATION_H

#include <QtOpenGL/qglplatformintegration_lite.h>
#include <QtGui/private/qeglproperties_p.h>
#include <EGL/egl.h>

void qt_eglproperties_set_glformat(QEglProperties& eglProperties, const QGLFormat& glFormat);
// Updates "format" with the parameters of the selected configuration.
void qt_glformat_from_eglconfig(QGLFormat& format, const EGLConfig config);

class QEGLPlatformWidgetSurface : public QPlatformGLWidgetSurface
{
public:
    QEGLPlatformWidgetSurface();
    virtual ~QEGLPlatformWidgetSurface();

    bool create(QGLWidget*, QGLFormat&);
    void setGeometry(const QRect&);
    bool filterEvent(QEvent *);
};

class QEGLPlatformContext : public QPlatformGLContext
{
public:
    QEGLPlatformContext();
    ~QEGLPlatformContext();

    bool create(QPaintDevice* device, QGLFormat& format, QPlatformGLContext* shareContext);
    void makeCurrent();
    void doneCurrent();
    void swapBuffers();
    void* getProcAddress(const QString& procName);

private:
    EGLContext m_context;
    EGLSurface m_eglSurface;
};

#endif //QOPENKODEGLINTEGRATION_H
