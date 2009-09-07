/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <private/qglpaintdevice_p.h>
#include <private/qgl_p.h>
#include <private/qglpixelbuffer_p.h>
#include <private/qglframebufferobject_p.h>

QGLPaintDevice::QGLPaintDevice()
{
}

QGLPaintDevice::~QGLPaintDevice()
{
}


void QGLPaintDevice::beginPaint()
{
    // Record the currently bound FBO so we can restore it again
    // in endPaint()
    QGLContext *ctx = context();
    ctx->makeCurrent();
    m_previousFBO = ctx->d_func()->current_fbo;
    if (m_previousFBO != 0) {
        ctx->d_ptr->current_fbo = 0;
        glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
    }
}

void QGLPaintDevice::ensureActiveTarget()
{
    QGLContext* ctx = context();
    if (ctx != QGLContext::currentContext())
        ctx->makeCurrent();

    if (ctx->d_ptr->current_fbo != 0)
        glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
}

void QGLPaintDevice::endPaint()
{
    // Make sure the FBO bound at beginPaint is re-bound again here:
    QGLContext *ctx = context();
    if (m_previousFBO != ctx->d_func()->current_fbo) {
        ctx->d_ptr->current_fbo = m_previousFBO;
        glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_previousFBO);
    }
}

//QColor QGLPaintDevice::backgroundColor() const
//{
//    return QApplication::palette().brush(QPalette::Background).color();
//}

//bool QGLPaintDevice::autoFillBackground() const
//{
//    return false;
//}

//bool QGLPaintDevice::hasTransparentBackground() const
//{
//    return false;
//}

QGLFormat QGLPaintDevice::format() const
{
    return context()->format();
}

QSize QGLPaintDevice::size() const
{
    return QSize();
}



QGLWidgetGLPaintDevice::QGLWidgetGLPaintDevice()
{
}

QPaintEngine* QGLWidgetGLPaintDevice::paintEngine() const
{
    return glWidget->paintEngine();
}

//QColor QGLWidgetGLPaintDevice::backgroundColor() const
//{
//    return glWidget->palette().brush(glWidget->backgroundRole()).color();
//}

//bool QGLWidgetGLPaintDevice::autoFillBackground() const
//{
//    return glWidget->autoFillBackground();
//}

//bool QGLWidgetGLPaintDevice::hasTransparentBackground() const
//{
//    return glWidget->testAttribute(Qt::WA_TranslucentBackground);
//}

void QGLWidgetGLPaintDevice::setWidget(QGLWidget* w)
{
    glWidget = w;
}

void QGLWidgetGLPaintDevice::beginPaint()
{
    QGLPaintDevice::beginPaint();
    if (!glWidget->d_func()->disable_clear_on_painter_begin && glWidget->autoFillBackground()) {
        if (glWidget->testAttribute(Qt::WA_TranslucentBackground))
            glClearColor(0.0, 0.0, 0.0, 0.0);
        else {
            const QColor &c = glWidget->palette().brush(glWidget->backgroundRole()).color();
            float alpha = c.alphaF();
            glClearColor(c.redF() * alpha, c.greenF() * alpha, c.blueF() * alpha, alpha);
        }
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

void QGLWidgetGLPaintDevice::endPaint()
{
    if (glWidget->autoBufferSwap())
        glWidget->swapBuffers();
    QGLPaintDevice::endPaint();
}


QSize QGLWidgetGLPaintDevice::size() const
{
    return glWidget->size();
}

QGLContext* QGLWidgetGLPaintDevice::context() const
{
    return const_cast<QGLContext*>(glWidget->context());
}

// returns the QGLPaintDevice for the given QPaintDevice
QGLPaintDevice* QGLPaintDevice::getDevice(QPaintDevice* pd)
{
    QGLPaintDevice* glpd = 0;

    switch(pd->devType()) {
        case QInternal::Widget:
            // Should not be called on a non-gl widget:
            Q_ASSERT(qobject_cast<QGLWidget*>(static_cast<QWidget*>(pd)));
            glpd = &(static_cast<QGLWidget*>(pd)->d_func()->glDevice);
            break;
        case QInternal::Pbuffer:
            glpd = &(static_cast<QGLPixelBuffer*>(pd)->d_func()->glDevice);
            break;
        case QInternal::FramebufferObject:
            glpd = &(static_cast<QGLFramebufferObject*>(pd)->d_func()->glDevice);
            break;
        default:
            qWarning("QGLPaintDevice::getDevice() - Unknown device type %d", pd->devType());
            break;
    }

    return glpd;
}


