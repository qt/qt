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

#include "qopenkodeintegration.h"
#include "qopenkodewindowsurface.h"
#include "qopenkodewindow.h"

#include <QtGui/private/qpixmap_raster_p.h>

#include <QtCore/qdebug.h>
#include <QtCore/qthread.h>
#include <QtCore/qfile.h>

#include <KD/kd.h>
#include <KD/NV_display.h>
#include <KD/NV_initialize.h>

#include "GLES2/gl2ext.h"

#include <nvgl2demo_common.h>


QT_BEGIN_NAMESPACE

QOpenKODEScreen::QOpenKODEScreen()
{
    KDDesktopNV *kdDesktop = KD_NULL;
    KDDisplayNV *kdDisplay = KD_NULL;

    qDebug() << "QOpenKODEScreen::QOpenKODEIntegrationScreen()";

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
    mFormat = QImage::Format_RGB32;
}

static GLuint loadShaders(const QString &vertexShader, const QString &fragmentShader)
{
    GLuint prog = 0;
    GLuint vertShader;
    GLuint fragShader;

   // Create the program
    prog = glCreateProgram();

    // Create the GL shader objects
    vertShader = glCreateShader(GL_VERTEX_SHADER);
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Load shader sources into GL and compile
    QFile vertexFile(vertexShader);
    vertexFile.open(QFile::ReadOnly);
    QByteArray vertSource = vertexFile.readAll();
    const char *vertChar = vertSource.constData();
    int vertSize = vertSource.size();

    QFile fragFile(fragmentShader);
    fragFile.open(QFile::ReadOnly);
    QByteArray fragSource = fragFile.readAll();
    const char *fragChar = fragSource.constData();
    int fragSize = fragSource.size();

    glShaderSource(vertShader, 1, (const char**)&vertChar, &vertSize);
    glCompileShader(vertShader);

    glShaderSource(fragShader, 1, (const char**)&fragChar, &fragSize);
    glCompileShader(fragShader);

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
        if (kdInitializeNV() == KD_ENOTINITIALIZED) {
            qFatal("Did not manage to initialize openkode");
        }
        eventMutex->release();

        const KDEvent *event;
        while ((event = kdWaitEvent(-1)) != 0) {
            qDebug() << "!!! received event!";
            kdDefaultEvent(event);
        }
    }

private:
    QSemaphore *eventMutex;
};

QOpenKODEIntegration::QOpenKODEIntegration()
    : eventMutex(1)
{
    QOpenKODEEventLoopHelper *loop = new QOpenKODEEventLoopHelper(&eventMutex);
    loop->start();
    eventMutex.acquire(); // block until initialization done

    QOpenKODEScreen *mPrimaryScreen = new QOpenKODEScreen();

    mScreens.append(mPrimaryScreen);

}

QPixmapData *QOpenKODEIntegration::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}

QPlatformWindow *QOpenKODEIntegration::createPlatformWindow(QWidget *tlw, WId ) const
{
    return new QOpenKODEWindow(tlw);
}

QWindowSurface *QOpenKODEIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    return new QOpenKODEWindowSurface(widget,winId);
}

GLuint QOpenKODEIntegration::blitterProgram()
{
    static GLuint shaderProgram = 0;
    if (!shaderProgram) {

        shaderProgram = loadShaders(":/shaders/vert.glslv",":/shaders/frag.glslf");
        if (!shaderProgram)
            qFatal("QOpenKodeGraphicsSystem(): Cannot load shaders!");
    }
    return shaderProgram;
}


QT_END_NAMESPACE
