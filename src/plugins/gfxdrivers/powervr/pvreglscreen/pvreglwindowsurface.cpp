/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "pvreglwindowsurface.h"
#include "pvreglscreen.h"
#include <QScreen>
#include <QDebug>
#include <QWSDisplay>

PvrEglWindowSurface::PvrEglWindowSurface
        (QWidget *widget, QScreen *screen, int screenNum)
    : QWSGLWindowSurface(widget)
{
    setSurfaceFlags(QWSWindowSurface::Opaque);

    this->widget = widget;
    this->screen = screen;
    this->holder = 0;
    this->pdevice = 0;

    QPoint pos = offset(widget);
    QSize size = widget->size();

    PvrQwsRect pvrRect;
    pvrRect.x = pos.x();
    pvrRect.y = pos.y();
    pvrRect.width = size.width();
    pvrRect.height = size.height();

    // Try to recover a previous PvrQwsDrawable object for the widget
    // if there is one.  This can happen when a PvrEglWindowSurface
    // is created for a widget, bound to a EGLSurface, and then destroyed.
    // When a new PvrEglWindowSurface is created for the widget, it will
    // pick up the previous PvrQwsDrawable if the EGLSurface has not been
    // destroyed in the meantime.
    drawable = pvrQwsFetchWindow((long)widget);
    if (drawable)
        pvrQwsSetGeometry(drawable, &pvrRect);
    else
        drawable = pvrQwsCreateWindow(screenNum, (long)widget, &pvrRect);
}

PvrEglWindowSurface::PvrEglWindowSurface(PvrEglSurfaceHolder *holder)
    : QWSGLWindowSurface()
{
    setSurfaceFlags(QWSWindowSurface::Opaque);
    drawable = 0;
    widget = 0;
    screen = 0;
    pdevice = 0;

    this->holder = holder;
    holder->addSurface();
}

PvrEglWindowSurface::~PvrEglWindowSurface()
{
    // Release the PvrQwsDrawable.  If it is bound to an EGLSurface,
    // then it will stay around until a new PvrEglWindowSurface is
    // created for the widget.  If it is not bound to an EGLSurface,
    // it will be destroyed immediately.
    if (drawable && pvrQwsReleaseWindow(drawable))
        pvrQwsDestroyDrawable(drawable);

    if (holder)
        holder->removeSurface();
    delete pdevice;
}

bool PvrEglWindowSurface::isValid() const
{
    return (widget != 0);
}

void PvrEglWindowSurface::setGeometry(const QRect &rect)
{
    if (drawable) {
        // XXX: adjust for the screen offset.
        PvrQwsRect pvrRect;
        pvrRect.x = rect.x();
        pvrRect.y = rect.y();
        pvrRect.width = rect.width();
        pvrRect.height = rect.height();
        pvrQwsSetGeometry(drawable, &pvrRect);
    }
    QWSGLWindowSurface::setGeometry(rect);
}

bool PvrEglWindowSurface::move(const QPoint &offset)
{
    QRect rect = geometry().translated(offset); 
    if (drawable) {
        PvrQwsRect pvrRect;
        pvrRect.x = rect.x();
        pvrRect.y = rect.y();
        pvrRect.width = rect.width();
        pvrRect.height = rect.height();
        pvrQwsSetGeometry(drawable, &pvrRect);
    }
    return QWSGLWindowSurface::move(offset);
}

QByteArray PvrEglWindowSurface::permanentState() const
{
    // Nothing interesting to pass to the server just yet.
    return QByteArray();
}

void PvrEglWindowSurface::setPermanentState(const QByteArray &state)
{
    Q_UNUSED(state);
}

QImage PvrEglWindowSurface::image() const
{
    if (drawable) {
        PvrQwsRect pvrRect;
        pvrQwsGetGeometry(drawable, &pvrRect);
        void *data = pvrQwsGetRenderBuffer(drawable);
        if (data) {
            return QImage((uchar *)data, pvrRect.width, pvrRect.height,
                          pvrQwsGetStride(drawable), QImage::Format_RGB16);
        }
    }
    return QImage();
}

QPaintDevice *PvrEglWindowSurface::paintDevice()
{
    // Return a dummy paint device because the widget itself
    // cannot be painted to this way.
    if (!pdevice)
        pdevice = new QImage(50, 50, QImage::Format_RGB16);
    return pdevice;
}

void PvrEglWindowSurface::setDirectRegion(const QRegion &r, int id)
{
    QWSGLWindowSurface::setDirectRegion(r, id);

    if (!drawable)
        return;

    // Clip the region to the window boundaries in case the child
    // is partially outside the geometry of the parent.
    QWidget *window = widget->window();
    QRegion region = r;
    if (widget != window) {
	QRect rect = window->geometry();
        rect.moveTo(window->mapToGlobal(QPoint(0, 0)));
        region = region.intersect(rect);
    }

    if (region.isEmpty()) {
        pvrQwsClearVisibleRegion(drawable);
    } else if (region.numRects() == 1) {
        QRect rect = region.boundingRect();
        PvrQwsRect pvrRect;
        pvrRect.x = rect.x();
        pvrRect.y = rect.y();
        pvrRect.width = rect.width();
        pvrRect.height = rect.height();
        pvrQwsSetVisibleRegion(drawable, &pvrRect, 1);
        if (!pvrQwsSwapBuffers(drawable, 1))
            screen->solidFill(QColor(0, 0, 0), region);
    } else {
        QVector<QRect> rects = region.rects();
        PvrQwsRect *pvrRects = new PvrQwsRect [rects.size()];
        for (int index = 0; index < rects.size(); ++index) {
            QRect rect = rects[index];
            pvrRects[index].x = rect.x();
            pvrRects[index].y = rect.y();
            pvrRects[index].width = rect.width();
            pvrRects[index].height = rect.height();
        }
        pvrQwsSetVisibleRegion(drawable, pvrRects, rects.size());
        if (!pvrQwsSwapBuffers(drawable, 1))
            screen->solidFill(QColor(0, 0, 0), region);
        delete [] pvrRects;
    }
}
