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

QGLPaintDevice::QGLPaintDevice()
    : m_context(0)
{
}

QGLPaintDevice::~QGLPaintDevice()
{
}

//extern QPaintEngine* qt_gl_engine(); // in qgl.cpp
//extern QPaintEngine* qt_gl_2_engine(); // in qgl.cpp

//inline bool qt_gl_preferGL2Engine()
//{
//#if defined(QT_OPENGL_ES_2)
//    return true;
//#else
//    return (QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_0)
//           && qgetenv("QT_GL_USE_OPENGL1ENGINE").isEmpty();
//#endif
//}

//QPaintEngine* QGLPaintDevice::paintEngine() const
//{
//#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_1_CL)
//    return qt_gl_engine();
//#elif defined(QT_OPENGL_ES_2)
//    return qt_gl_2_engine();
//#else
//    if (!qt_gl_preferGL2Engine())
//        return qt_gl_engine();
//    else
//        return qt_gl_2_engine();
//#endif
//}

void QGLPaintDevice::beginPaint()
{
    m_context->makeCurrent();
}

void QGLPaintDevice::endPaint()
{
    glFlush();
}

QColor QGLPaintDevice::backgroundColor() const
{
    return QApplication::palette().brush(QPalette::Background).color();
}

bool QGLPaintDevice::autoFillBackground() const
{
    return false;
}

bool QGLPaintDevice::hasTransparentBackground() const
{
    return false;
}

QGLContext* QGLPaintDevice::context() const
{
    return m_context;
}

QGLFormat QGLPaintDevice::format() const
{
    return m_context->format();
}

QSize QGLPaintDevice::size() const
{
    return QSize();
}

void QGLPaintDevice::setContext(QGLContext* c)
{
    m_context = c;
}



QGLWidgetGLPaintDevice::QGLWidgetGLPaintDevice()
{
}

QPaintEngine* QGLWidgetGLPaintDevice::paintEngine() const
{
    return glWidget->paintEngine();
}

QColor QGLWidgetGLPaintDevice::backgroundColor() const
{
    return glWidget->palette().brush(glWidget->backgroundRole()).color();
}

bool QGLWidgetGLPaintDevice::autoFillBackground() const
{
    return glWidget->autoFillBackground();
}

bool QGLWidgetGLPaintDevice::hasTransparentBackground() const
{
    return glWidget->testAttribute(Qt::WA_TranslucentBackground);
}

void QGLWidgetGLPaintDevice::setWidget(QGLWidget* w)
{
    glWidget = w;
}

//void QGLWidgetGLPaintDevice::beginPaint()
//{
//    glWidget->makeCurrent();
//}

void QGLWidgetGLPaintDevice::endPaint()
{
    if (glWidget->autoBufferSwap())
        glWidget->swapBuffers();
}


QSize QGLWidgetGLPaintDevice::size() const
{
    return glWidget->size();
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
        default:
            qWarning("QGLPaintDevice::getDevice() - Unknown device type %d", pd->devType());
            break;
    }

    return glpd;
}


