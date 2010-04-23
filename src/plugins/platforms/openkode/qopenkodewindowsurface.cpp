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

#include "qopenkodewindowsurface.h"
#include "qopenkodeintegration.h"

#include "qopenkodewindow.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

QOpenKODEWindowSurface::QOpenKODEWindowSurface
        (QWidget *window, WId winId)
    : QWindowSurface(window),
      mSurface(EGL_NO_SURFACE),
      mWin((EGLNativeWindowType) winId)
{
    EGLConfig config = QEgl::defaultConfig(QInternal::Widget,QEgl::OpenGL,QEgl::Renderable);
    mContext.setConfig(config);
    if (!mContext.createContext()) {
        qWarning("QOpenKODEWindowSurface: Unable to create context");
        return;
    }
}

QOpenKODEWindowSurface::~QOpenKODEWindowSurface()
{
}

QPaintDevice *QOpenKODEWindowSurface::paintDevice()
{
    return &mImage;
}

// ### TODO - this updates the entire toplevel, should only update the region
void QOpenKODEWindowSurface::flush(QWidget *, const QRegion &region, const QPoint &offset)
{
    mContext.makeCurrent(mSurface);

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

    GLuint shaderProgram = QOpenKODEIntegration::blitterProgram();

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
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, blitImage.bits());

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

    mContext.swapBuffers(mSurface);
    mContext.doneCurrent();
}

void QOpenKODEWindowSurface::resize(const QSize &size)
{
    QWindowSurface::resize(size);
    mContext.destroySurface(mSurface);
    mSurface = EGL_NO_SURFACE;
    mImage = QImage();

}
void QOpenKODEWindowSurface::beginPaint(const QRegion &region)
{
    if (mSurface == EGL_NO_SURFACE)  {
        EGLConfig config = QEgl::defaultConfig(QInternal::Widget,QEgl::OpenGL,QEgl::Renderable);
        EGLint windowAttrs[] = { EGL_NONE };
        mSurface = eglCreateWindowSurface(QEgl::display(), config, mWin, windowAttrs);
        mImage = QImage(size(),QImage::Format_RGB32);
    }
}

void QOpenKODEWindowSurface::endPaint(const QRegion &region)
{
    Q_UNUSED(region);
}

QT_END_NAMESPACE
