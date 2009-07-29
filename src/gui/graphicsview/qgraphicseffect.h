/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGRAPHICSEFFECT_H
#define QGRAPHICSEFFECT_H

#include <QtCore/qobject.h>
#include <QtCore/qglobal.h>
#include <QtCore/qpoint.h>
#include <QtCore/qvariant.h>
#include <QtGui/qtransform.h>
#include <QtGui/qcolor.h>

QT_FORWARD_DECLARE_CLASS(QGraphicsItem);
QT_FORWARD_DECLARE_CLASS(QStyleOptionGraphicsItem);
QT_FORWARD_DECLARE_CLASS(QPainter);
QT_FORWARD_DECLARE_CLASS(QPixmap);
QT_FORWARD_DECLARE_CLASS(QWidget);
QT_FORWARD_DECLARE_CLASS(QPixmapColorizeFilter);

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

class QGraphicsEffectSourcePrivate;
class QStyleOption;
class Q_GUI_EXPORT QGraphicsEffectSource : public QObject
{
    Q_OBJECT
public:
    ~QGraphicsEffectSource();
    QRectF boundingRect(bool deviceCoordinates = false) const;
    const QGraphicsItem *graphicsItem() const;
    const QStyleOption *styleOption() const;
    void draw(QPainter *painter);
    QPixmap pixmap(bool deviceCoordinates, QPoint *offset = 0) const;

protected:
    QGraphicsEffectSource(QGraphicsEffectSourcePrivate &dd, QObject *parent = 0);

private:
    Q_DECLARE_PRIVATE(QGraphicsEffectSource);
    Q_DISABLE_COPY(QGraphicsEffectSource);
    friend class QGraphicsEffect;
    friend class QGraphicsEffectPrivate;
    friend class QGraphicsScenePrivate;
    friend class QGraphicsItem;
};

class QGraphicsEffectPrivate;
class Q_GUI_EXPORT QGraphicsEffect : public QObject
{
    Q_OBJECT
public:
    QGraphicsEffect();
    virtual ~QGraphicsEffect();
    QRectF boundingRect() const;

    void setSourcePixmap(const QPixmap &pixmap);
    QPixmap sourcePixmap() const;
    bool hasSourcePixmap() const;
    virtual QRectF boundingRectFor(const QRectF &rect) const;

protected:
    QGraphicsEffect(QGraphicsEffectPrivate &d);
    virtual void draw(QPainter *painter, QGraphicsEffectSource *source) = 0;

private:
    Q_DECLARE_PRIVATE(QGraphicsEffect)
    Q_DISABLE_COPY(QGraphicsEffect)
    friend class QGraphicsItem;
    friend class QGraphicsItemPrivate;
    friend class QGraphicsScenePrivate;
};

class QGraphicsGrayscaleEffectPrivate;
class Q_GUI_EXPORT QGraphicsGrayscaleEffect: public QGraphicsEffect
{
    Q_OBJECT
public:
    QGraphicsGrayscaleEffect();
    ~QGraphicsGrayscaleEffect();

protected:
    void draw(QPainter *painter, QGraphicsEffectSource *source);

private:
    Q_DECLARE_PRIVATE(QGraphicsGrayscaleEffect)
    Q_DISABLE_COPY(QGraphicsGrayscaleEffect)
};

class QGraphicsColorizeEffectPrivate;
class Q_GUI_EXPORT QGraphicsColorizeEffect: public QGraphicsEffect {
    Q_OBJECT
public:
    QGraphicsColorizeEffect();
    ~QGraphicsColorizeEffect();

    QColor color() const;
    void setColor(const QColor &c);

protected:
    void draw(QPainter *painter, QGraphicsEffectSource *source);

private:
    Q_DECLARE_PRIVATE(QGraphicsColorizeEffect)
    Q_DISABLE_COPY(QGraphicsColorizeEffect)
};

class QGraphicsPixelizeEffectPrivate;
class Q_GUI_EXPORT QGraphicsPixelizeEffect: public QGraphicsEffect {
    Q_OBJECT
public:
    QGraphicsPixelizeEffect();
    ~QGraphicsPixelizeEffect();

    int pixelSize() const;
    void setPixelSize(int pixelSize);

protected:
    void draw(QPainter *painter, QGraphicsEffectSource *source);

private:
    Q_DECLARE_PRIVATE(QGraphicsPixelizeEffect)
    Q_DISABLE_COPY(QGraphicsPixelizeEffect)
};

class QGraphicsBlurEffectPrivate;
class Q_GUI_EXPORT QGraphicsBlurEffect: public QGraphicsEffect {
    Q_OBJECT
public:
    QGraphicsBlurEffect();
    ~QGraphicsBlurEffect();

    int blurRadius() const;
    void setBlurRadius(int blurRadius);

    //    QRectF boundingRect() const;

protected:
    QRectF boundingRectFor(const QRectF &rect) const;
    void draw(QPainter *painter, QGraphicsEffectSource *source);

private:
    Q_DECLARE_PRIVATE(QGraphicsBlurEffect)
    Q_DISABLE_COPY(QGraphicsBlurEffect)
};

class QGraphicsBloomEffectPrivate;
class Q_GUI_EXPORT QGraphicsBloomEffect: public QGraphicsEffect {
    Q_OBJECT
public:
    QGraphicsBloomEffect();
    ~QGraphicsBloomEffect();

    int blurRadius() const;
    void setBlurRadius(int blurRadius);

    qreal opacity() const;
    void setOpacity(qreal opacity);

    //    QRectF boundingRect() const;

protected:
    QRectF boundingRectFor(const QRectF &rect) const;
    void draw(QPainter *painter, QGraphicsEffectSource *source);

private:
    Q_DECLARE_PRIVATE(QGraphicsBloomEffect)
    Q_DISABLE_COPY(QGraphicsBloomEffect)
};

class QGraphicsFrameEffectPrivate;
class Q_GUI_EXPORT QGraphicsFrameEffect: public QGraphicsEffect {
    Q_OBJECT
public:
    QGraphicsFrameEffect();
    ~QGraphicsFrameEffect();

    QColor frameColor() const;
    void setFrameColor(const QColor &c);

    qreal frameWidth() const;
    void setFrameWidth(qreal frameWidth);

    qreal frameOpacity() const;
    void setFrameOpacity(qreal opacity);

    //    QRectF boundingRect() const;

protected:
    QRectF boundingRectFor(const QRectF &rect) const;
    void draw(QPainter *painter, QGraphicsEffectSource *source);

private:
    Q_DECLARE_PRIVATE(QGraphicsFrameEffect)
    Q_DISABLE_COPY(QGraphicsFrameEffect)
};

class QGraphicsShadowEffectPrivate;
class Q_GUI_EXPORT QGraphicsShadowEffect: public QGraphicsEffect {
    Q_OBJECT
public:
    QGraphicsShadowEffect();
    ~QGraphicsShadowEffect();

    QPointF shadowOffset() const;
    void setShadowOffset(const QPointF &ofs);
    inline void setShadowOffset(qreal dx, qreal dy) { setShadowOffset(QPointF(dx, dy)); }
    inline void setShadowOffset(qreal d) { setShadowOffset(QPointF(d, d)); }

    int blurRadius() const;
    void setBlurRadius(int blurRadius);

    qreal opacity() const;
    void setOpacity(qreal opacity);

    //    QRectF boundingRect() const;

protected:
    QRectF boundingRectFor(const QRectF &rect) const;
    void draw(QPainter *painter, QGraphicsEffectSource *source);

private:
    Q_DECLARE_PRIVATE(QGraphicsShadowEffect)
    Q_DISABLE_COPY(QGraphicsShadowEffect)
};

Q_DECLARE_METATYPE(QGraphicsEffect *)
Q_DECLARE_METATYPE(QGraphicsGrayscaleEffect *)
Q_DECLARE_METATYPE(QGraphicsColorizeEffect *)
Q_DECLARE_METATYPE(QGraphicsPixelizeEffect *)
Q_DECLARE_METATYPE(QGraphicsBlurEffect *)
Q_DECLARE_METATYPE(QGraphicsBloomEffect *)
Q_DECLARE_METATYPE(QGraphicsFrameEffect *)
Q_DECLARE_METATYPE(QGraphicsShadowEffect *)

#endif // QT_NO_GRAPHICSVIEW

QT_END_NAMESPACE

QT_END_HEADER


#endif // QGRAPHICSEFFECT_H
