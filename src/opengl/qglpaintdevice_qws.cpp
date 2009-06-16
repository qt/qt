/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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

#include <private/qglpaintdevice_qws_p.h>
#include <private/qgl_p.h>
#include <private/qpaintengine_opengl_p.h>
#include <private/qglwindowsurface_qws_p.h>

QT_BEGIN_NAMESPACE

class QWSGLPaintDevicePrivate
{
public:
    QWidget *widget;
};

class QMetricAccessor : public QWidget {
public:
    int metric(PaintDeviceMetric m) {
        return QWidget::metric(m);
    }
};

QWSGLPaintDevice::QWSGLPaintDevice(QWidget *widget) :
    d_ptr(new QWSGLPaintDevicePrivate)
{
    Q_D(QWSGLPaintDevice);
    d->widget = widget;
}

QWSGLPaintDevice::~QWSGLPaintDevice()
{
    Q_D(QWSGLPaintDevice);
    delete d;
}

QPaintEngine* QWSGLPaintDevice::paintEngine() const
{
#if !defined(QT_OPENGL_ES_2)
    return qt_qgl_paint_engine();
#else
    return 0; // XXX
#endif
}

int QWSGLPaintDevice::metric(PaintDeviceMetric m) const
{
    Q_D(const QWSGLPaintDevice);
    Q_ASSERT(d->widget);

    return ((QMetricAccessor *) d->widget)->metric(m);
}

QWSGLWindowSurface* QWSGLPaintDevice::windowSurface() const
{
     Q_D(const QWSGLPaintDevice);
     return static_cast<QWSGLWindowSurface*>(d->widget->windowSurface());
}

QT_END_NAMESPACE
