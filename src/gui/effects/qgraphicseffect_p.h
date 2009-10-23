/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
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

#include <QPixmapCache>

#include <private/qobject_p.h>
#include <private/qpixmapfilter_p.h>

QT_BEGIN_NAMESPACE

class QGraphicsEffectSourcePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsEffectSource)
public:
    QGraphicsEffectSourcePrivate() : QObjectPrivate() {}
    virtual ~QGraphicsEffectSourcePrivate() { invalidateCache(); }
    virtual void detach() = 0;
    virtual QRectF boundingRect(Qt::CoordinateSystem system) const = 0;
    virtual QRect deviceRect() const = 0;
    virtual const QGraphicsItem *graphicsItem() const = 0;
    virtual const QWidget *widget() const = 0;
    virtual const QStyleOption *styleOption() const = 0;
    virtual void draw(QPainter *p) = 0;
    virtual void update() = 0;
    virtual bool isPixmap() const = 0;
    virtual QPixmap pixmap(Qt::CoordinateSystem system, QPoint *offset = 0) const = 0;
    virtual void effectBoundingRectChanged() = 0;
    void invalidateCache() const { QPixmapCache::remove(m_cacheKey); }

    friend class QGraphicsScenePrivate;
    friend class QGraphicsItem;
    friend class QGraphicsItemPrivate;

private:
    mutable Qt::CoordinateSystem m_cachedSystem;
    mutable QPoint m_cachedOffset;
    mutable QPixmapCache::Key m_cacheKey;
};

class Q_GUI_EXPORT QGraphicsEffectPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsEffect)
public:
    QGraphicsEffectPrivate() : source(0), isEnabled(1) {}

    inline void setGraphicsEffectSource(QGraphicsEffectSource *newSource)
    {
        QGraphicsEffect::ChangeFlags flags;
        if (source) {
            flags |= QGraphicsEffect::SourceDetached;
            source->d_func()->detach();
            source->d_func()->invalidateCache();
            delete source;
        }
        source = newSource;
        if (newSource)
            flags |= QGraphicsEffect::SourceAttached;
        q_func()->sourceChanged(flags);
    }

    QGraphicsEffectSource *source;
    QRectF boundingRect;
    quint32 isEnabled : 1;
    quint32 padding : 31; // feel free to use
};

class QGraphicsGrayscaleEffectPrivate : public QGraphicsEffectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsGrayscaleEffect)
public:
    QGraphicsGrayscaleEffectPrivate()
        : opaque(true)
    {
        filter = new QPixmapColorizeFilter;
        filter->setColor(Qt::black);
    }
    ~QGraphicsGrayscaleEffectPrivate() { delete filter; }

    QPixmapColorizeFilter *filter;
    quint32 opaque : 1;
    quint32 padding : 31;
};

class QGraphicsColorizeEffectPrivate : public QGraphicsEffectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsColorizeEffect)
public:
    QGraphicsColorizeEffectPrivate()
        : opaque(true)
    {
        filter = new QPixmapColorizeFilter;
    }
    ~QGraphicsColorizeEffectPrivate() { delete filter; }

    QPixmapColorizeFilter *filter;
    quint32 opaque : 1;
    quint32 padding : 31;
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
    QGraphicsBlurEffectPrivate() : filter(new QPixmapBlurFilter) {}
    ~QGraphicsBlurEffectPrivate() { delete filter; }

    QPixmapBlurFilter *filter;
};

class QGraphicsDropShadowEffectPrivate : public QGraphicsEffectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsDropShadowEffect)
public:
    QGraphicsDropShadowEffectPrivate() : filter(new QPixmapDropShadowFilter) {}
    ~QGraphicsDropShadowEffectPrivate() { delete filter; }

    QPixmapDropShadowFilter *filter;
};

class QGraphicsOpacityEffectPrivate : public QGraphicsEffectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsOpacityEffect)
public:
    QGraphicsOpacityEffectPrivate()
        : opacity(qreal(0.7)), isFullyTransparent(0), isFullyOpaque(0), hasOpacityMask(0) {}
    ~QGraphicsOpacityEffectPrivate() {}

    qreal opacity;
    QBrush opacityMask;
    uint isFullyTransparent : 1;
    uint isFullyOpaque : 1;
    uint hasOpacityMask : 1;
};

class QGraphicsBloomEffectPrivate : public QGraphicsEffectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsBlurEffect)
public:
    QGraphicsBloomEffectPrivate() : brightness(70), strength(qreal(0.7)) {}

    QPixmapBlurFilter blurFilter;
    int colorTable[256];
    int brightness;
    qreal strength;
};

QT_END_NAMESPACE

#endif // QGRAPHICSEFFECT_P_H

