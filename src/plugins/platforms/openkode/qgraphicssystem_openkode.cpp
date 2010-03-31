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

#include "qgraphicssystem_openkode.h"
#include "qwindowsurface_openkode.h"
#include <QtGui/private/qpixmap_raster_p.h>

#include <QtCore/qdebug.h>
#include <QtCore/qthread.h>

#include <KD/kd.h>
#include <KD/NV_display.h>
#include <KD/NV_initialize.h>

#include "GLES2/gl2ext.h"


QT_BEGIN_NAMESPACE

QOpenKODEGraphicsSystemScreen::QOpenKODEGraphicsSystemScreen()
{
    KDDesktopNV *kdDesktop = KD_NULL;
    KDDisplayNV *kdDisplay = KD_NULL;

    qDebug() << "QOpenKODEGraphicsSystemScreen::QOpenKODEGraphicsSystemScreen()";

    // Get the default desktop and display
    kdDesktop = kdGetDesktopNV(KD_DEFAULT_DESKTOP_NV, KD_NULL);
    if (!kdDesktop || kdDesktop == (void*)-1) {
        qErrnoWarning(kdGetError(), "Could not obtain KDDesktopNV pointer");
        return;
    }

    kdDisplay = kdGetDisplayNV(KD_DEFAULT_DISPLAY_NV, KD_NULL);
    if (!kdDisplay || kdDisplay == (void*)-1) {
        qErrnoWarning(kdGetError(), "Could not obtain KDDisplayNV pointer");
        kdReleaseDesktopNV(kdDesktop);
        return;
    }

    KDDisplayModeNV mode;
    if (kdGetDisplayModeNV(kdDisplay, &mode)) {
        qErrnoWarning(kdGetError(), "Could not get display mode");
        return;
    }

    qDebug() << " - display mode " << mode.width << "x" << mode.height << " refresh " << mode.refresh;

    KDint desktopSize[] = { mode.width, mode.height };

    if (kdSetDesktopPropertyivNV(kdDesktop, KD_DESKTOPPROPERTY_SIZE_NV, desktopSize)) {
        qErrnoWarning(kdGetError(), "Could not set desktop size");
        return;
    }

    // Once we've set up the desktop and display we don't need them anymore
    kdReleaseDisplayNV(kdDisplay);
    kdReleaseDesktopNV(kdDesktop);

    const int defaultDpi = 72;
    mGeometry = QRect(0, 0, mode.width, mode.height);
    mPhysicalSize = QSize(mode.width * 25.4 / defaultDpi, mode.height * 25.4 / defaultDpi);

    mDepth = 24;
    mFormat = QImage::Format_RGB888;


    QEglProperties properties;
    properties.setPixelFormat(QImage::Format_RGB888);
    properties.setValue(EGL_BUFFER_SIZE, EGL_DONT_CARE);
    properties.setRenderableType(QEgl::OpenGL);

    if (!mContext.chooseConfig(properties, QEgl::BestPixelFormat)) {
        qWarning("qEglContext: Unable to choose config!");
        return;
    }

    if (!mContext.display()) {
        qWarning("qEglContext: Unable to open display!");
        return;
    }

    qDebug() << " - QEglContext::openDisplay OK";
}

static GLuint NvKdTestLoadShaders(const char *vertex_shader_binary,
                    const char *fragment_shader_binary,
                    GLuint vertex_shader_binary_size,
                    GLuint fragment_shader_binary_size)
{
    GLuint prog;
    GLuint vertShader;
    GLuint fragShader;

    // Create the program
    prog = glCreateProgram();

    // Create the GL shader objects
    vertShader = glCreateShader(GL_VERTEX_SHADER);
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Load the binary data into the shader objects
    glShaderBinary(1, &vertShader,
                   GL_NVIDIA_PLATFORM_BINARY_NV, vertex_shader_binary, vertex_shader_binary_size);
    glShaderBinary(1, &fragShader,
                   GL_NVIDIA_PLATFORM_BINARY_NV, fragment_shader_binary, fragment_shader_binary_size);

    // Attach the shaders to the program
    glAttachShader(prog, vertShader);
    glAttachShader(prog, fragShader);

    // Delete the shaders
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    // Link and validate the shader program
    glLinkProgram(prog);
    glValidateProgram(prog);

    return prog;
}

class QOpenKODEEventLoopHelper : public QThread
{
public:
    QOpenKODEEventLoopHelper(QSemaphore *m)
            : eventMutex(m)
    {
        m->acquire();
    }

protected:
    void run()
    {
        qDebug() << "initializing KD";
        kdInitializeNV();
        qDebug() << "done initializing KD";
        eventMutex->release();

        const KDEvent *event;
        while ((event = kdWaitEvent(-1)) != 0) {
            qDebug() << "!!! received event!";
            kdDefaultEvent(event);
        }

        qDebug() << "exiting event loop";
    }

private:
    QSemaphore *eventMutex;
};

QOpenKODEGraphicsSystem::QOpenKODEGraphicsSystem()
    : eventMutex(1)
{
    QOpenKODEEventLoopHelper *loop = new QOpenKODEEventLoopHelper(&eventMutex);
    loop->start();
    eventMutex.acquire(); // block until initialization done

    mPrimaryScreen = new QOpenKODEGraphicsSystemScreen();

    mScreens.append(mPrimaryScreen);

}

QPixmapData *QOpenKODEGraphicsSystem::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}

QWindowSurface *QOpenKODEGraphicsSystem::createWindowSurface(QWidget *widget) const
{
    return new QOpenKODEWindowSurface(mPrimaryScreen, widget);
}

GLuint QOpenKODEGraphicsSystem::blitterProgram()
{
    static GLuint shaderProgram = 0;
    if (!shaderProgram) {

        const char vertShaderBinary[] = {
#   include "vert.h"
        };
        const char fragShaderBinary[] = {
#   include "frag.h"
        };

        shaderProgram = NvKdTestLoadShaders(vertShaderBinary, fragShaderBinary,
                    sizeof(vertShaderBinary), sizeof(fragShaderBinary));

        if (!shaderProgram)
            qFatal("QOpenKodeGraphicsSystem(): Cannot load shaders!");
    }
    return shaderProgram;
}


QT_END_NAMESPACE
