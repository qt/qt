/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QMLGRAPHICSRECT_H
#define QMLGRAPHICSRECT_H

#include "qmlgraphicsitem.h"

#include <QtGui/qbrush.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class Q_DECLARATIVE_EXPORT QmlGraphicsPen : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY penChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY penChanged)
public:
    QmlGraphicsPen(QObject *parent=0)
        : QObject(parent), _width(1), _color("#000000"), _valid(false)
    {}

    int width() const { return _width; }
    void setWidth(int w);

    QColor color() const { return _color; }
    void setColor(const QColor &c);

    bool isValid() { return _valid; };

Q_SIGNALS:
    void penChanged();

private:
    int _width;
    QColor _color;
    bool _valid;
};

class Q_DECLARATIVE_EXPORT QmlGraphicsGradientStop : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal position READ position WRITE setPosition)
    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    QmlGraphicsGradientStop(QObject *parent=0) : QObject(parent) {}

    qreal position() const { return m_position; }
    void setPosition(qreal position) { m_position = position; updateGradient(); }

    QColor color() const { return m_color; }
    void setColor(const QColor &color) { m_color = color; updateGradient(); }

private:
    void updateGradient();

private:
    qreal m_position;
    QColor m_color;
};

class Q_DECLARATIVE_EXPORT QmlGraphicsGradient : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QmlListProperty<QmlGraphicsGradientStop> stops READ stops)
    Q_CLASSINFO("DefaultProperty", "stops")

public:
    QmlGraphicsGradient(QObject *parent=0) : QObject(parent), m_gradient(0) {}
    ~QmlGraphicsGradient() { delete m_gradient; }

    QmlListProperty<QmlGraphicsGradientStop> stops() { return QmlListProperty<QmlGraphicsGradientStop>(this, m_stops); }

    const QGradient *gradient() const;

Q_SIGNALS:
    void updated();

private:
    void doUpdate();

private:
    QList<QmlGraphicsGradientStop *> m_stops;
    mutable QGradient *m_gradient;
    friend class QmlGraphicsGradientStop;
};

class QmlGraphicsRectanglePrivate;
class Q_DECLARATIVE_EXPORT QmlGraphicsRectangle : public QmlGraphicsItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QmlGraphicsGradient *gradient READ gradient WRITE setGradient)
    Q_PROPERTY(QmlGraphicsPen * border READ border CONSTANT)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
public:
    QmlGraphicsRectangle(QmlGraphicsItem *parent=0);

    QColor color() const;
    void setColor(const QColor &);

    QmlGraphicsPen *border();

    QmlGraphicsGradient *gradient() const;
    void setGradient(QmlGraphicsGradient *gradient);

    qreal radius() const;
    void setRadius(qreal radius);

    QRectF boundingRect() const;

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

Q_SIGNALS:
    void colorChanged();
    void radiusChanged();

private Q_SLOTS:
    void doUpdate();

private:
    void generateRoundedRect();
    void generateBorderedRect();
    void drawRect(QPainter &painter);

private:
    Q_DISABLE_COPY(QmlGraphicsRectangle)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QmlGraphicsRectangle)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlGraphicsPen)
QML_DECLARE_TYPE(QmlGraphicsGradientStop)
QML_DECLARE_TYPE(QmlGraphicsGradient)
QML_DECLARE_TYPE(QmlGraphicsRectangle)

QT_END_HEADER

#endif // QMLGRAPHICSRECT_H
