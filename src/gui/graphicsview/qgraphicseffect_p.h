/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QGRAPHICSEFFECT_P_H
#define QGRAPHICSEFFECT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include "qgraphicseffect.h"
#include <private/qobject_p.h>
#include <private/qpixmapfilter_p.h>

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

QT_BEGIN_NAMESPACE

class QGraphicsEffectSourcePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsEffectSource)
public:
    QGraphicsEffectSourcePrivate() : QObjectPrivate() {}
    virtual ~QGraphicsEffectSourcePrivate() {}
    virtual void detach() = 0;
    virtual QRectF boundingRect(bool deviceCoordinates = false) const = 0;
    virtual const QGraphicsItem *graphicsItem() const = 0;
    virtual const QStyleOption *styleOption() const = 0;
    virtual void draw(QPainter *p) = 0;
    virtual QPixmap pixmap(bool deviceCoordinates, QPoint *offset = 0) const = 0;
    friend class QGraphicsScenePrivate;
    friend class QGraphicsItem;
    friend class QGraphicsItemPrivate;
};

class QGraphicsEffectPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsEffect)
public:
    QGraphicsEffectPrivate() : source(0), hasSourcePixmap(0) {}
    QGraphicsEffectSource *source;
    inline void setGraphicsEffectSource(QGraphicsEffectSource *newSource)
    {
        if (source) {
            source->d_func()->detach();
            delete source;
        }
        source = newSource;
    }
    QRectF boundingRect;
    QPixmap sourcePixmap;
    quint32 hasSourcePixmap : 1;
    quint32 padding : 31; // feel free to use
};

class QGraphicsGrayscaleEffectPrivate : public QGraphicsEffectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsGrayscaleEffect)
public:
    QGraphicsGrayscaleEffectPrivate()
    {
        filter = new QPixmapColorizeFilter;
        filter->setColor(Qt::black);
    }
    ~QGraphicsGrayscaleEffectPrivate() { delete filter; }

    QPixmapColorizeFilter *filter;
};

class QGraphicsColorizeEffectPrivate : public QGraphicsEffectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsColorizeEffect)
public:
    QGraphicsColorizeEffectPrivate() { filter = new QPixmapColorizeFilter; }
    ~QGraphicsColorizeEffectPrivate() { delete filter; }

    QPixmapColorizeFilter *filter;
};

class QGraphicsPixelizeEffectPrivate : public QGraphicsEffectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsPixelizeEffect)
public:
    QGraphicsPixelizeEffectPrivate() : pixelSize(3) {}

    int pixelSize;
};

class QGraphicsBlurEffectPrivate : public QGraphicsEffectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsBlurEffect)
public:
    QGraphicsBlurEffectPrivate() : filter(new QPixmapBlurFilter), blurRadius(4) {}
    ~QGraphicsBlurEffectPrivate() { delete filter; }

    QPixmapBlurFilter *filter;
    int blurRadius;
};

class QGraphicsBloomEffectPrivate : public QGraphicsEffectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsBlurEffect)
public:
    QGraphicsBloomEffectPrivate() : blurRadius(6), opacity(0.7) {}

    int blurRadius;
    qreal opacity;
};

class QGraphicsFrameEffectPrivate : public QGraphicsEffectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsFrameEffect)
public:
    QGraphicsFrameEffectPrivate() : color(Qt::blue), width(5), alpha(0.6) {}

    QColor color;
    qreal width;
    qreal alpha;
};

class QGraphicsShadowEffectPrivate : public QGraphicsEffectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsShadowEffect)
public:
    QGraphicsShadowEffectPrivate() : offset(4, 4), radius(8), alpha(0.7) {}

    QPointF offset;
    int radius;
    qreal alpha;
};

QT_END_NAMESPACE

#endif // QT_NO_GRAPHICSVIEW

#endif // QGRAPHICSEFFECT_P_H
