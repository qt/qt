/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsimplecanvas.h"
#include "qsimplecanvasitem_p.h"
#include "qsimplecanvas_p.h"


QT_BEGIN_NAMESPACE
QRect QSimpleCanvasItemPrivate::setupPainting(int version, const QRect &bounding)
{
    Q_Q(QSimpleCanvasItem);

    bool hasContents = options & QSimpleCanvasItem::HasContents;

    QRect rv;

    if (hasContents || q->clip()) {
        QRectF boundingRectActive = q->boundingRect();
        rv = data()->transformActive.mapRect(boundingRectActive).toAlignedRect() & bounding;
    } 

    data()->doNotPaint = rv.isEmpty();
    data()->doNotPaintChildren = data()->doNotPaint && q->clip();
    QRect myBounding = bounding;
    if (q->clip())
        myBounding &= rv;

    if (!data()->doNotPaintChildren) {
       for (int ii = 0; ii < children.count(); ++ii) {
           QSimpleCanvasItem *child = children.at(ii);
    
           int childVersion = version;
           if (!child->d_func()->data()->transformValid)
               childVersion = canvas->d->paintVersion;

           bool recalcNeeded = 
               (childVersion > child->d_func()->data()->transformVersion);

           if (recalcNeeded) {
               qreal visible = child->visible();
               child->d_func()->data()->activeOpacity = data()->activeOpacity;
               if (visible != 1)
                   child->d_func()->data()->activeOpacity *= visible;
           }

           if (child->d_func()->data()->activeOpacity != 0) {
               if (recalcNeeded) {
                   // Calculate child's transform
                   qreal x = child->x();
                   qreal y = child->y();
                   qreal scale = child->scale();
                   QSimpleCanvasItem::Flip flip = child->flip();
        
                   QSimpleCanvas::Matrix &am = 
                       child->d_func()->data()->transformActive;
                   am = data()->transformActive;
                   if (x != 0 || y != 0)
                       am.translate(x, y);
                   if (scale != 1) {
                       QPointF to = child->d_func()->transformOrigin();
                       if (to.x() != 0. || to.y() != 0.)
                           am.translate(to.x(), to.y());
                       am.scale(scale, scale);
                       if (to.x() != 0. || to.y() != 0.)
                           am.translate(-to.x(), -to.y());
                   }
        
                   if (child->d_func()->data()->transformUser)
                       am = *child->d_func()->data()->transformUser * am;
        
                   if (flip) {
                       QRectF br = child->boundingRect();
                       am.translate(br.width() / 2., br.height() / 2);
                       am.scale((flip & QSimpleCanvasItem::HorizontalFlip)?-1:1,
                               (flip & QSimpleCanvasItem::VerticalFlip)?-1:1);
                       am.translate(-br.width() / 2., -br.height() / 2);
                   }
                   child->d_func()->data()->transformValid = true;
                   child->d_func()->data()->transformVersion = childVersion;
               } 
               rv |= child->d_func()->setupPainting(child->d_func()->data()->transformVersion, myBounding);
           }
       }
    } 

    data()->lastPaintRect = rv;
    if (!data()->doNotPaintChildren) {
        if (!bounding.intersects(rv)) {
            data()->doNotPaintChildren = true;
            data()->doNotPaint = true;
        }
    }
            
    return rv;
}

void QSimpleCanvasItemPrivate::paint(QPainter &p)
{
    Q_Q(QSimpleCanvasItem);

    QRect oldUcr;
    if (clip) {

        p.save();
        QRectF boundingRectActive = q->boundingRect();

        QRect cr;
        switch(clip) {
        case QSimpleCanvasItem::ClipToHeight:
            {
                qWarning("QSimpleCanvasItem: ClipToHeight not implemented");
                QRect r = p.clipRegion().boundingRect();
                cr = QRect(r.x(), 0, r.width(), 
                           boundingRectActive.height());
            }
            break;
        case QSimpleCanvasItem::ClipToWidth:
            {
                qWarning("QSimpleCanvasItem: ClipToWidth not implemented");
                QRect r = p.clipRegion().boundingRect();
                cr = QRect(0, r.y(), boundingRectActive.width(),
                           r.height());
            }
            break;
        case QSimpleCanvasItem::ClipToRect:
            cr = boundingRectActive.toAlignedRect();
            break;
        default:
            break;
        }

        p.setWorldTransform(data()->transformActive);
        if (p.clipRegion().isEmpty()) {
            p.setClipRect(cr);
        } else {
            p.setClipRect(cr, Qt::IntersectClip);
        }

        if (p.clipRegion().isEmpty()) {
            p.restore();
            return;
        }
    }

    zOrderChildren();

    int upto = 0;
    if (!data()->doNotPaintChildren) {
        for (upto = 0; upto < children.count(); ++upto) {
            QSimpleCanvasItem *c = children.at(upto);
            if (c->z() < 0) {
                paintChild(p, c);
            } else {
                break;
            }
        }
    }

    if (!data()->doNotPaint) {
        p.setWorldTransform(data()->transformActive);
        q->paintContents(p);
    } 

    if (!data()->doNotPaintChildren) {
        for (; upto < children.count(); ++upto) {
            QSimpleCanvasItem *c = children.at(upto);
	    paintChild(p, c);
	}
    } 

    if (clip) 
        p.restore();

    if (debuggerStatus && debuggerStatus->selected)  {
        p.setWorldTransform(data()->transformActive);
        p.fillRect(q->boundingRect(), QColor(255, 0, 0, 80));
    }
}

void QSimpleCanvasItemPrivate::paintChild(QPainter &p, QSimpleCanvasItem *c)
{
    if (c->d_func()->data()->activeOpacity != 0) {

        qreal op = p.opacity();
        p.setOpacity(c->d_func()->data()->activeOpacity);

        c->d_func()->paint(p);

        p.setOpacity(op);
    }
}

QT_END_NAMESPACE
