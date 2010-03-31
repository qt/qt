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

#include "qwindowsurface_openkode.h"
#include "qgraphicssystem_openkode.h"
#include <QtCore/qdebug.h>

#include "KD/kd.h"
#include "KD/NV_display.h"

QT_BEGIN_NAMESPACE

QOpenKODEWindowSurface::QOpenKODEWindowSurface
        (QOpenKODEGraphicsSystemScreen *screen, QWidget *window)
    : QWindowSurface(window),
      mScreen(screen),
      mSurface(0)
{
    qDebug() << "QOpenKODEWindowSurface::QOpenKODEWindowSurface:" << window << window->width() << "x" << window->height()
            << "pos" << window->x() << "x" << window->y();

    if (!mContext.display()) {
        qWarning("qEglContext: Unable to open display!");
        return;
    }

    QEglProperties properties;
    properties.setPixelFormat(QImage::Format_RGB888);
    properties.setValue(EGL_BUFFER_SIZE, EGL_DONT_CARE);
    properties.setRenderableType(QEgl::OpenGL);

    if (!mContext.chooseConfig(properties, QEgl::BestPixelFormat)) {
        qWarning("qEglContext: Unable to choose config!");
        return;
    }

    createWindow(window);
}

void QOpenKODEWindowSurface::createWindow(QWidget *window)
{
    qDebug() << "createWindow";
    kdWindow = kdCreateWindow(mContext.display(), mContext.config(), KD_NULL);

    if (!kdWindow) {
        qErrnoWarning(kdGetError(), "Error creating native window");
        return;
    }

    const KDint windowSize[2]  = { window->width(), window->height() };
    if (kdSetWindowPropertyiv(kdWindow, KD_WINDOWPROPERTY_SIZE, windowSize)) {
        qErrnoWarning(kdGetError(), "Could not set native window size");
        return;
    }

    //const KDboolean windowExclusive[] = { false };
    //if (kdSetWindowPropertybv(kdWindow, KD_WINDOWPROPERTY_DESKTOP_EXCLUSIVE_NV, windowExclusive)) {
    //    qErrnoWarning(kdGetError(), "Could not set exclusive bit");
    //    //return;
    //}

    //const KDint windowPos[2] = { window->x(), window->y() };
    //if (kdSetWindowPropertyiv(kdWindow, KD_WINDOWPROPERTY_DESKTOP_OFFSET_NV, windowPos)) {
    //    qErrnoWarning(kdGetError(), "Could not set native window position");
    //    //return;
    //}

    EGLNativeWindowType nativeWindow;

    if (kdRealizeWindow(kdWindow, &nativeWindow)) {
        qErrnoWarning(kdGetError(), "Could not realize native window");
        return;
    }
    qDebug() << "kdRealizeWindow" << nativeWindow;

    // Create an EGL window surface for the native window
    EGLint windowAttrs[3] = { EGL_NONE };
    qDebug() << "doing createwindowsurface";
    *mSurface = eglCreateWindowSurface(mContext.display(),
                                        mContext.config(),
                                        nativeWindow,
                                        windowAttrs);
    qDebug() << "create windowsurface";
    if (!mSurface) {
        qWarning("EGL couldn't create window surface: 0x%x", eglGetError());
        return;
    }

    qDebug() << "making context";
    if (!mContext.createContext()) {
        qDebug() << "Unable to create context!";
        return;
    }

    qDebug() << "about to make current";
    mContext.makeCurrent(mSurface);
}

QOpenKODEWindowSurface::~QOpenKODEWindowSurface()
{
}

QPaintDevice *QOpenKODEWindowSurface::paintDevice()
{
    qDebug() << "QOpenKODEWindowSurface::paintDevice";
    return &mImage;
}

// ### TODO - this updates the entire toplevel, should only update the region
void QOpenKODEWindowSurface::flush(QWidget *, const QRegion &region, const QPoint &offset)
{
    qDebug() << "in flush";
    if (!offset.isNull()) {
        qWarning("Offset flushing not supported yet");
        return;
    }

    if (!mContext.makeCurrent(mSurface)) {
        qWarning("EGL couldn't make context/surface current: 0x%x", eglGetError());
        return;
    }

    QRect boundingRect = region.boundingRect();

    int x, y, w, h;
    QImage blitImage;
    if (true || boundingRect == mImage.rect()) { // TODO - check optimization
        blitImage = mImage;
        x = y = 0;
        w = mImage.width();
        h = mImage.height();
    } else {
        blitImage = mImage.copy(boundingRect);
        w = boundingRect.width();
        h = boundingRect.height();
        x = boundingRect.x();
        y = boundingRect.y();
    }

//    qDebug() << "flush" << widget << offset << region.boundingRect() << mImage.format() << blitImage.format();

    GLuint shaderProgram = QOpenKODEGraphicsSystem::blitterProgram();

    glUseProgram(shaderProgram);

    GLuint index = glGetUniformLocation(shaderProgram, "window");
    glUniform2f(index, GLfloat(mImage.width()), GLfloat(mImage.height()));

    // attributes
    GLuint posId      = glGetAttribLocation(shaderProgram, "pos_attr");
    GLuint texcoordId = glGetAttribLocation(shaderProgram, "texcoord_attr");

    // sampler
    index = glGetUniformLocation(shaderProgram, "tex_samp");

    glUniform1i(index, 0);

    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);

    GLuint texId;
    GLfloat coords[8] = {x, y, x, y + h, x + w, y + h, x + w, y };
    GLfloat texcoords[8] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0 };

    // Generate texture for checkered background
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, blitImage.bits());

    // Enable vertex attribute associated with vertex position
    glEnableVertexAttribArray(posId);
    glEnableVertexAttribArray(texcoordId);

    // Set the quad vertices
    glVertexAttribPointer(posId, 2, GL_FLOAT, 0, 0, coords);
    glVertexAttribPointer(texcoordId, 2, GL_FLOAT, 0, 0, texcoords);

    // Draw the quad
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Cleanup
    glDisableVertexAttribArray(posId);
    glDisableVertexAttribArray(texcoordId);

    // Release all textures
    glBindTexture(GL_TEXTURE_2D, 0);
    if (texId)
        glDeleteTextures(1, &texId);

    mContext.doneCurrent();
    mContext.swapBuffers(mSurface);
}

void QOpenKODEWindowSurface::setGeometry(const QRect &rect)
{
    qDebug() << "QOpenKODEWindowSurface::setGeometry:" << rect;
    QWindowSurface::setGeometry(rect);
    if (mImage.size() != rect.size())
        mImage = QImage(rect.size(), mScreen->format());

    mContext.destroySurface(mSurface);
    kdDestroyWindow(kdWindow);
    createWindow(window());
    qDebug() << "set geometry workded";
}

bool QOpenKODEWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    return QWindowSurface::scroll(area, dx, dy);
}

void QOpenKODEWindowSurface::beginPaint(const QRegion &region)
{
    Q_UNUSED(region);
}

void QOpenKODEWindowSurface::endPaint(const QRegion &region)
{
    Q_UNUSED(region);
}

QT_END_NAMESPACE
