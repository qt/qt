// Commit: ac5c099cc3c5b8c7eec7a49fdeb8a21037230350
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QSGRECTANGLE_P_H
#define QSGRECTANGLE_P_H

#include "qsgitem.h"

#include <QtGui/qbrush.h>

#include <private/qdeclarativeglobal_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class Q_DECLARATIVE_PRIVATE_EXPORT QSGPen : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY penChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY penChanged)
public:
    QSGPen(QObject *parent=0);

    int width() const;
    void setWidth(int w);

    QColor color() const;
    void setColor(const QColor &c);

    bool isValid() const;

Q_SIGNALS:
    void penChanged();

private:
    int _width;
    QColor _color;
    bool _valid;
};

class Q_AUTOTEST_EXPORT QSGGradientStop : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal position READ position WRITE setPosition)
    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    QSGGradientStop(QObject *parent=0);

    qreal position() const;
    void setPosition(qreal position);

    QColor color() const;
    void setColor(const QColor &color);

private:
    void updateGradient();

private:
    qreal m_position;
    QColor m_color;
};

class Q_AUTOTEST_EXPORT QSGGradient : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeListProperty<QSGGradientStop> stops READ stops)
    Q_CLASSINFO("DefaultProperty", "stops")

public:
    QSGGradient(QObject *parent=0);
    ~QSGGradient();

    QDeclarativeListProperty<QSGGradientStop> stops();

    const QGradient *gradient() const;

Q_SIGNALS:
    void updated();

private:
    void doUpdate();

private:
    QList<QSGGradientStop *> m_stops;
    mutable QGradient *m_gradient;
    friend class QSGRectangle;
    friend class QSGGradientStop;
};

class QSGRectanglePrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QSGRectangle : public QSGItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QSGGradient *gradient READ gradient WRITE setGradient)
    Q_PROPERTY(QSGPen * border READ border CONSTANT)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
public:
    QSGRectangle(QSGItem *parent=0);

    QColor color() const;
    void setColor(const QColor &);

    QSGPen *border();

    QSGGradient *gradient() const;
    void setGradient(QSGGradient *gradient);

    qreal radius() const;
    void setRadius(qreal radius);

    QRectF boundingRect() const;

Q_SIGNALS:
    void colorChanged();
    void radiusChanged();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

private Q_SLOTS:
    void doUpdate();

private:
    Q_DISABLE_COPY(QSGRectangle)
    Q_DECLARE_PRIVATE(QSGRectangle)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QSGPen)
QML_DECLARE_TYPE(QSGGradientStop)
QML_DECLARE_TYPE(QSGGradient)
QML_DECLARE_TYPE(QSGRectangle)

QT_END_HEADER

#endif // QSGRECTANGLE_P_H
