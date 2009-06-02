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

#include <QtCore/qglobal.h>
#include <QtCore/qpoint.h>
#include <QtCore/qvariant.h>
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

class Q_GUI_EXPORT QGraphicsEffect {
public:

    QGraphicsEffect();
    virtual ~QGraphicsEffect();

    virtual QRectF boundingRectFor(const QGraphicsItem *item);

    virtual void drawItem(QGraphicsItem *item, QPainter *painter,
                          const QStyleOptionGraphicsItem *option = 0,
                          QWidget *widget = 0) = 0;

protected:
    QPixmap* drawItemOnPixmap(QPainter *painter, QGraphicsItem *item,
                              const QStyleOptionGraphicsItem *option, QWidget *widget, int flags);

private:
    Q_DISABLE_COPY(QGraphicsEffect);
};

class Q_GUI_EXPORT QGraphicsGrayscaleEffect: public QGraphicsEffect {
public:

    QGraphicsGrayscaleEffect();
    ~QGraphicsGrayscaleEffect();

    void drawItem(QGraphicsItem *item, QPainter *painter,
                  const QStyleOptionGraphicsItem *option = 0,
                  QWidget *widget = 0);

private:
    QPixmapColorizeFilter *filter;
    Q_DISABLE_COPY(QGraphicsGrayscaleEffect);
};

class Q_GUI_EXPORT QGraphicsColorizeEffect: public QGraphicsEffect {
public:

    QGraphicsColorizeEffect();
    ~QGraphicsColorizeEffect();

    QColor color() const;
    void setColor(const QColor &c);

    void drawItem(QGraphicsItem *item, QPainter *painter,
                  const QStyleOptionGraphicsItem *option = 0,
                  QWidget *widget = 0);

private:
    QPixmapColorizeFilter *filter;
    Q_DISABLE_COPY(QGraphicsColorizeEffect);
};

class Q_GUI_EXPORT QGraphicsPixelizeEffect: public QGraphicsEffect {
public:

    QGraphicsPixelizeEffect();
    ~QGraphicsPixelizeEffect();

    int pixelSize() const { return size; }
    void setPixelSize(int pixelSize) { size = pixelSize; }

    void drawItem(QGraphicsItem *item, QPainter *painter,
                  const QStyleOptionGraphicsItem *option = 0,
                  QWidget *widget = 0);

private:
    int size;
    Q_DISABLE_COPY(QGraphicsPixelizeEffect);
};

class Q_GUI_EXPORT QGraphicsBlurEffect: public QGraphicsEffect {
public:

    QGraphicsBlurEffect();
    ~QGraphicsBlurEffect();

    int blurRadius() const { return radius; }
    void setBlurRadius(int blurRadius) { radius = blurRadius; }

    QRectF boundingRectFor(const QGraphicsItem *item);

    void drawItem(QGraphicsItem *item, QPainter *painter,
                  const QStyleOptionGraphicsItem *option = 0,
                  QWidget *widget = 0);

private:
    int radius;
    Q_DISABLE_COPY(QGraphicsBlurEffect);
};

class Q_GUI_EXPORT QGraphicsBloomEffect: public QGraphicsEffect {
public:

    QGraphicsBloomEffect();
    ~QGraphicsBloomEffect();

    int blurRadius() const { return radius; }
    void setBlurRadius(int blurRadius) { radius = blurRadius; }

    QRectF boundingRectFor(const QGraphicsItem *item);

    qreal opacity() const { return alpha; }
    void setOpacity(qreal opacity) { alpha = opacity; }

    void drawItem(QGraphicsItem *item, QPainter *painter,
                  const QStyleOptionGraphicsItem *option = 0,
                  QWidget *widget = 0);

private:
    int radius;
    qreal alpha;
    Q_DISABLE_COPY(QGraphicsBloomEffect);
};

class Q_GUI_EXPORT QGraphicsFrameEffect: public QGraphicsEffect {
public:

    QGraphicsFrameEffect();
    ~QGraphicsFrameEffect();

    QColor frameColor() const { return color; }
    void setFrameColor(const QColor &c) { color = c; }

    qreal frameWidth() const { return width; }
    void setFrameWidth(qreal frameWidth) { width = frameWidth; }

    qreal frameOpacity() const { return alpha; }
    void setFrameOpacity(qreal opacity) { alpha = opacity; }


    QRectF boundingRectFor(const QGraphicsItem *item);

    void drawItem(QGraphicsItem *item, QPainter *painter,
                  const QStyleOptionGraphicsItem *option = 0,
                  QWidget *widget = 0);
private:
    QColor color;
    qreal width;
    qreal alpha;
    Q_DISABLE_COPY(QGraphicsFrameEffect);
};

class Q_GUI_EXPORT QGraphicsShadowEffect: public QGraphicsEffect {
public:

    QGraphicsShadowEffect();
    ~QGraphicsShadowEffect();

    QPointF shadowOffset() const;
    void setShadowOffset(const QPointF &ofs) { offset = ofs; }
    inline void setShadowOffset(qreal dx, qreal dy) { setShadowOffset(QPointF(dx, dy)); }
    inline void setShadowOffset(qreal d) { setShadowOffset(QPointF(d, d)); }

    int blurRadius() const { return radius; }
    void setBlurRadius(int blurRadius) { radius = blurRadius; }

    qreal opacity() const { return alpha; }
    void setOpacity(qreal opacity) { alpha = opacity; }

    QRectF boundingRectFor(const QGraphicsItem *item);

    void drawItem(QGraphicsItem *item, QPainter *painter,
                  const QStyleOptionGraphicsItem *option = 0,
                  QWidget *widget = 0);

protected:
    QPointF offset;
    int radius;
    qreal alpha;

private:
    Q_DISABLE_COPY(QGraphicsShadowEffect);
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
