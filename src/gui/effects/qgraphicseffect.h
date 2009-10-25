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

#ifndef QGRAPHICSEFFECT_H
#define QGRAPHICSEFFECT_H

#include <QtCore/qobject.h>
#include <QtCore/qpoint.h>
#include <QtCore/qrect.h>
#include <QtGui/qcolor.h>
#include <QtGui/qbrush.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QGraphicsItem;
class QStyleOption;
class QPainter;
class QPixmap;

class QGraphicsEffectSourcePrivate;
class Q_GUI_EXPORT QGraphicsEffectSource : public QObject
{
    Q_OBJECT
public:
    ~QGraphicsEffectSource();
    const QGraphicsItem *graphicsItem() const;
    const QWidget *widget() const;
    const QStyleOption *styleOption() const;

    bool isPixmap() const;
    void draw(QPainter *painter);
    void update();

    QRectF boundingRect(Qt::CoordinateSystem coordinateSystem = Qt::LogicalCoordinates) const;
    QRect deviceRect() const;
    QPixmap pixmap(Qt::CoordinateSystem system = Qt::LogicalCoordinates, QPoint *offset = 0) const;

protected:
    QGraphicsEffectSource(QGraphicsEffectSourcePrivate &dd, QObject *parent = 0);

private:
    Q_DECLARE_PRIVATE(QGraphicsEffectSource)
    Q_DISABLE_COPY(QGraphicsEffectSource)
    friend class QGraphicsEffect;
    friend class QGraphicsEffectPrivate;
    friend class QGraphicsScenePrivate;
    friend class QGraphicsItem;
    friend class QGraphicsItemPrivate;
    friend class QWidget;
    friend class QWidgetPrivate;
};

class QGraphicsEffectPrivate;
class Q_GUI_EXPORT QGraphicsEffect : public QObject
{
    Q_OBJECT
    Q_FLAGS(ChangeFlags)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
public:
    enum ChangeFlag {
        SourceAttached = 0x1,
        SourceDetached = 0x2,
        SourceBoundingRectChanged = 0x4,
        SourceInvalidated = 0x8
    };
    Q_DECLARE_FLAGS(ChangeFlags, ChangeFlag)

    QGraphicsEffect(QObject *parent = 0);
    virtual ~QGraphicsEffect();

    virtual QRectF boundingRectFor(const QRectF &rect) const;
    QRectF boundingRect() const;

    QGraphicsEffectSource *source() const;

    bool isEnabled() const;

public Q_SLOTS:
    void setEnabled(bool enable);
    void update();

Q_SIGNALS:
    void enabledChanged(bool enabled);

protected:
    QGraphicsEffect(QGraphicsEffectPrivate &d, QObject *parent = 0);
    virtual void draw(QPainter *painter, QGraphicsEffectSource *source) = 0;
    virtual void sourceChanged(ChangeFlags flags);
    void updateBoundingRect();

private:
    Q_DECLARE_PRIVATE(QGraphicsEffect)
    Q_DISABLE_COPY(QGraphicsEffect)
    friend class QGraphicsItem;
    friend class QGraphicsItemPrivate;
    friend class QGraphicsScenePrivate;
    friend class QWidget;
    friend class QWidgetPrivate;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QGraphicsEffect::ChangeFlags)

class QGraphicsGrayscaleEffectPrivate;
class Q_GUI_EXPORT QGraphicsGrayscaleEffect: public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(qreal strength READ strength WRITE setStrength NOTIFY strengthChanged)
public:
    QGraphicsGrayscaleEffect(QObject *parent = 0);
    ~QGraphicsGrayscaleEffect();

    qreal strength() const;

protected:
    void draw(QPainter *painter, QGraphicsEffectSource *source);

public Q_SLOTS:
    void setStrength(qreal strength);

Q_SIGNALS:
    void strengthChanged(qreal strength);

private:
    Q_DECLARE_PRIVATE(QGraphicsGrayscaleEffect)
    Q_DISABLE_COPY(QGraphicsGrayscaleEffect)
};

class QGraphicsColorizeEffectPrivate;
class Q_GUI_EXPORT QGraphicsColorizeEffect: public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(qreal strength READ strength WRITE setStrength NOTIFY strengthChanged)
public:
    QGraphicsColorizeEffect(QObject *parent = 0);
    ~QGraphicsColorizeEffect();

    QColor color() const;
    qreal strength() const;

public Q_SLOTS:
    void setColor(const QColor &c);
    void setStrength(qreal strength);

Q_SIGNALS:
    void colorChanged(const QColor &color);
    void strengthChanged(qreal strength);

protected:
    void draw(QPainter *painter, QGraphicsEffectSource *source);

private:
    Q_DECLARE_PRIVATE(QGraphicsColorizeEffect)
    Q_DISABLE_COPY(QGraphicsColorizeEffect)
};

class QGraphicsPixelizeEffectPrivate;
class Q_GUI_EXPORT QGraphicsPixelizeEffect: public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(int pixelSize READ pixelSize WRITE setPixelSize NOTIFY pixelSizeChanged)
public:
    QGraphicsPixelizeEffect(QObject *parent = 0);
    ~QGraphicsPixelizeEffect();

    int pixelSize() const;

public Q_SLOTS:
    void setPixelSize(int pixelSize);

Q_SIGNALS:
    void pixelSizeChanged(int pixelSize);

protected:
    void draw(QPainter *painter, QGraphicsEffectSource *source);

private:
    Q_DECLARE_PRIVATE(QGraphicsPixelizeEffect)
    Q_DISABLE_COPY(QGraphicsPixelizeEffect)
};

class QGraphicsBlurEffectPrivate;
class Q_GUI_EXPORT QGraphicsBlurEffect: public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(int blurRadius READ blurRadius WRITE setBlurRadius NOTIFY blurRadiusChanged)
    Q_PROPERTY(Qt::RenderHint blurHint READ blurHint WRITE setBlurHint NOTIFY blurHintChanged)
public:
    QGraphicsBlurEffect(QObject *parent = 0);
    ~QGraphicsBlurEffect();

    QRectF boundingRectFor(const QRectF &rect) const;
    int blurRadius() const;
    Qt::RenderHint blurHint() const;

public Q_SLOTS:
    void setBlurRadius(int blurRadius);
    void setBlurHint(Qt::RenderHint hint);

Q_SIGNALS:
    void blurRadiusChanged(int blurRadius);
    void blurHintChanged(Qt::RenderHint hint);

protected:
    void draw(QPainter *painter, QGraphicsEffectSource *source);

private:
    Q_DECLARE_PRIVATE(QGraphicsBlurEffect)
    Q_DISABLE_COPY(QGraphicsBlurEffect)
};

class QGraphicsDropShadowEffectPrivate;
class Q_GUI_EXPORT QGraphicsDropShadowEffect: public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(QPointF offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(qreal xOffset READ xOffset WRITE setXOffset NOTIFY offsetChanged)
    Q_PROPERTY(qreal yOffset READ yOffset WRITE setYOffset NOTIFY offsetChanged)
    Q_PROPERTY(int blurRadius READ blurRadius WRITE setBlurRadius NOTIFY blurRadiusChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
public:
    QGraphicsDropShadowEffect(QObject *parent = 0);
    ~QGraphicsDropShadowEffect();

    QRectF boundingRectFor(const QRectF &rect) const;
    QPointF offset() const;

    inline qreal xOffset() const
    { return offset().x(); }

    inline qreal yOffset() const
    { return offset().y(); }

    int blurRadius() const;
    QColor color() const;

public Q_SLOTS:
    void setOffset(const QPointF &ofs);

    inline void setOffset(qreal dx, qreal dy)
    { setOffset(QPointF(dx, dy)); }

    inline void setOffset(qreal d)
    { setOffset(QPointF(d, d)); }

    inline void setXOffset(qreal dx)
    { setOffset(QPointF(dx, yOffset())); }

    inline void setYOffset(qreal dy)
    { setOffset(QPointF(xOffset(), dy)); }

    void setBlurRadius(int blurRadius);
    void setColor(const QColor &color);

Q_SIGNALS:
    void offsetChanged(const QPointF &offset);
    void blurRadiusChanged(int blurRadius);
    void colorChanged(const QColor &color);

protected:
    void draw(QPainter *painter, QGraphicsEffectSource *source);

private:
    Q_DECLARE_PRIVATE(QGraphicsDropShadowEffect)
    Q_DISABLE_COPY(QGraphicsDropShadowEffect)
};

class QGraphicsOpacityEffectPrivate;
class Q_GUI_EXPORT QGraphicsOpacityEffect: public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
    Q_PROPERTY(QBrush opacityMask READ opacityMask WRITE setOpacityMask NOTIFY opacityMaskChanged)
public:
    QGraphicsOpacityEffect(QObject *parent = 0);
    ~QGraphicsOpacityEffect();

    qreal opacity() const;
    QBrush opacityMask() const;

public Q_SLOTS:
    void setOpacity(qreal opacity);
    void setOpacityMask(const QBrush &mask);

Q_SIGNALS:
    void opacityChanged(qreal opacity);
    void opacityMaskChanged(const QBrush &mask);

protected:
    void draw(QPainter *painter, QGraphicsEffectSource *source);

private:
    Q_DECLARE_PRIVATE(QGraphicsOpacityEffect)
    Q_DISABLE_COPY(QGraphicsOpacityEffect)
};

class QGraphicsBloomEffectPrivate;
class Q_GUI_EXPORT QGraphicsBloomEffect: public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(int blurRadius READ blurRadius WRITE setBlurRadius NOTIFY blurRadiusChanged)
    Q_PROPERTY(Qt::RenderHint blurHint READ blurHint WRITE setBlurHint NOTIFY blurHintChanged)
    Q_PROPERTY(int brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(qreal strength READ strength WRITE setStrength NOTIFY strengthChanged)
public:
    QGraphicsBloomEffect(QObject *parent = 0);
    ~QGraphicsBloomEffect();

    QRectF boundingRectFor(const QRectF &rect) const;
    int blurRadius() const;
    Qt::RenderHint blurHint() const;
    int brightness() const;
    qreal strength() const;

public Q_SLOTS:
    void setBlurRadius(int blurRadius);
    void setBlurHint(Qt::RenderHint hint);
    void setBrightness(int brightness);
    void setStrength(qreal strength);

Q_SIGNALS:
    void blurRadiusChanged(int blurRadius);
    void blurHintChanged(Qt::RenderHint hint);
    void brightnessChanged(int brightness);
    void strengthChanged(qreal strength);

protected:
    void draw(QPainter *painter, QGraphicsEffectSource *source);

private:
    Q_DECLARE_PRIVATE(QGraphicsBloomEffect)
    Q_DISABLE_COPY(QGraphicsBloomEffect)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGRAPHICSEFFECT_H

