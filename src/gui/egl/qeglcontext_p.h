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

#ifndef QEGLCONTEXT_P_H
#define QEGLCONTEXT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience of
// the QtOpenGL and QtOpenVG modules.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qsize.h>
#include <QtGui/qimage.h>

#include <QtGui/private/qegl_p.h>
#include <QtGui/private/qeglproperties_p.h>

QT_BEGIN_NAMESPACE

class Q_GUI_EXPORT QEglContext
{
public:
    QEglContext();
    ~QEglContext();

    bool isValid() const;
    bool isCurrent() const;
    bool isSharing() const { return sharing; }

    QEgl::API api() const { return apiType; }
    void setApi(QEgl::API api) { apiType = api; }

    bool chooseConfig(const QEglProperties& properties, QEgl::PixelFormatMatch match = QEgl::ExactPixelFormat);
    bool createContext(QEglContext *shareContext = 0, const QEglProperties *properties = 0);
    void destroyContext();
    EGLSurface createSurface(QPaintDevice *device, const QEglProperties *properties = 0);
    void destroySurface(EGLSurface surface);

    bool makeCurrent(EGLSurface surface);
    bool doneCurrent();
    bool lazyDoneCurrent();
    bool swapBuffers(EGLSurface surface);
    bool swapBuffersRegion2NOK(EGLSurface surface, const QRegion *region);

    int  configAttrib(int name) const;

    EGLContext context() const { return ctx; }
    void setContext(EGLContext context) { ctx = context; ownsContext = false;}

    EGLDisplay display() {return QEgl::display();}

    EGLConfig config() const { return cfg; }
    void setConfig(EGLConfig config) { cfg = config; }

private:
    QEgl::API apiType;
    EGLContext ctx;
    EGLConfig cfg;
    EGLSurface currentSurface;
    bool current;
    bool ownsContext;
    bool sharing;
    bool apiChanged;

    static QEglContext *currentContext(QEgl::API api);
    static void setCurrentContext(QEgl::API api, QEglContext *context);

    friend class QMeeGoGraphicsSystem;
    friend class QMeeGoPixmapData;
};

QT_END_NAMESPACE

#endif // QEGLCONTEXT_P_H
