/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWINDOWSURFACE_AHIGL_P_H
#define QWINDOWSURFACE_AHIGL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QAhiGLWindowSurface class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <QtOpenGL/private/qglwindowsurface_qws_p.h>
#include <GLES/gl.h>
#include <GLES/egl.h>

QT_BEGIN_NAMESPACE
class QAhiGLWindowSurfacePrivate;
QT_END_NAMESPACE

//! [0]
class QAhiGLWindowSurface : public QWSGLWindowSurface
{
public:
    QAhiGLWindowSurface(QWidget *widget, EGLDisplay eglDisplay,
                        EGLSurface eglSurface, EGLContext eglContext);
    QAhiGLWindowSurface(EGLDisplay eglDisplay, EGLSurface eglSurface,
                        EGLContext eglContext);
    ~QAhiGLWindowSurface();

    QString key() const { return QLatin1String("ahigl"); }
    void setGeometry(const QRect &rect);
    QPaintDevice *paintDevice();
    void beginPaint(const QRegion &region);
    bool isValid() const;

    QByteArray permanentState() const;
    void setPermanentState(const QByteArray &);

    QImage image() const { return QImage(); }

    GLuint textureId() const;

private:
    QAhiGLWindowSurfacePrivate *d_ptr;
};
//! [0]

#endif // QWINDOWSURFACE_AHIGL_P_H
