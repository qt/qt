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

#ifndef QMLGRAPHICSPATH_H
#define QMLGRAPHICSPATH_H

#include "qmlgraphicsitem.h"

#include <qml.h>

#include <QtCore/QObject>
#include <QtGui/QPainterPath>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class Q_DECLARATIVE_EXPORT QmlGraphicsPathElement : public QObject
{
    Q_OBJECT
public:
    QmlGraphicsPathElement(QObject *parent=0) : QObject(parent) {}
Q_SIGNALS:
    void changed();
};

class Q_DECLARATIVE_EXPORT QmlGraphicsPathAttribute : public QmlGraphicsPathElement
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(qreal value READ value WRITE setValue NOTIFY changed)
public:
    QmlGraphicsPathAttribute(QObject *parent=0) : QmlGraphicsPathElement(parent), _value(0) {}


    QString name() const;
    void setName(const QString &name);

    qreal value() const;
    void setValue(qreal value);

private:
    QString _name;
    qreal _value;
};

class Q_DECLARATIVE_EXPORT QmlGraphicsCurve : public QmlGraphicsPathElement
{
    Q_OBJECT

    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY changed)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY changed)
public:
    QmlGraphicsCurve(QObject *parent=0) : QmlGraphicsPathElement(parent), _x(0), _y(0) {}

    qreal x() const;
    void setX(qreal x);

    qreal y() const;
    void setY(qreal y);

    virtual void addToPath(QPainterPath &) {}

private:
    qreal _x;
    qreal _y;
};

class Q_DECLARATIVE_EXPORT QmlGraphicsPathLine : public QmlGraphicsCurve
{
    Q_OBJECT
public:
    QmlGraphicsPathLine(QObject *parent=0) : QmlGraphicsCurve(parent) {}

    void addToPath(QPainterPath &path);
};

class Q_DECLARATIVE_EXPORT QmlGraphicsPathQuad : public QmlGraphicsCurve
{
    Q_OBJECT

    Q_PROPERTY(qreal controlX READ controlX WRITE setControlX NOTIFY changed)
    Q_PROPERTY(qreal controlY READ controlY WRITE setControlY NOTIFY changed)
public:
    QmlGraphicsPathQuad(QObject *parent=0) : QmlGraphicsCurve(parent), _controlX(0), _controlY(0) {}

    qreal controlX() const;
    void setControlX(qreal x);

    qreal controlY() const;
    void setControlY(qreal y);

    void addToPath(QPainterPath &path);

private:
    qreal _controlX;
    qreal _controlY;
};

class Q_DECLARATIVE_EXPORT QmlGraphicsPathCubic : public QmlGraphicsCurve
{
    Q_OBJECT

    Q_PROPERTY(qreal control1X READ control1X WRITE setControl1X NOTIFY changed)
    Q_PROPERTY(qreal control1Y READ control1Y WRITE setControl1Y NOTIFY changed)
    Q_PROPERTY(qreal control2X READ control2X WRITE setControl2X NOTIFY changed)
    Q_PROPERTY(qreal control2Y READ control2Y WRITE setControl2Y NOTIFY changed)
public:
    QmlGraphicsPathCubic(QObject *parent=0) : QmlGraphicsCurve(parent), _control1X(0), _control1Y(0), _control2X(0), _control2Y(0) {}

    qreal control1X() const;
    void setControl1X(qreal x);

    qreal control1Y() const;
    void setControl1Y(qreal y);

    qreal control2X() const;
    void setControl2X(qreal x);

    qreal control2Y() const;
    void setControl2Y(qreal y);

    void addToPath(QPainterPath &path);

private:
    int _control1X;
    int _control1Y;
    int _control2X;
    int _control2Y;
};

class Q_DECLARATIVE_EXPORT QmlGraphicsPathPercent : public QmlGraphicsPathElement
{
    Q_OBJECT
    Q_PROPERTY(qreal value READ value WRITE setValue)
public:
    QmlGraphicsPathPercent(QObject *parent=0) : QmlGraphicsPathElement(parent) {}

    qreal value() const;
    void setValue(qreal value);

private:
    qreal _value;
};

class QmlGraphicsPathPrivate;
class Q_DECLARATIVE_EXPORT QmlGraphicsPath : public QObject, public QmlParserStatus
{
    Q_OBJECT

    Q_INTERFACES(QmlParserStatus)
    Q_PROPERTY(QList<QmlGraphicsPathElement *>* pathElements READ pathElements)
    Q_PROPERTY(qreal startX READ startX WRITE setStartX)
    Q_PROPERTY(qreal startY READ startY WRITE setStartY)
    Q_PROPERTY(bool closed READ isClosed NOTIFY changed)
    Q_CLASSINFO("DefaultProperty", "pathElements")
    Q_INTERFACES(QmlParserStatus)
public:
    QmlGraphicsPath(QObject *parent=0);
    ~QmlGraphicsPath();

    QList<QmlGraphicsPathElement *>* pathElements();

    qreal startX() const;
    void setStartX(qreal x);

    qreal startY() const;
    void setStartY(qreal y);

    bool isClosed() const;

    QPainterPath path() const;
    QStringList attributes() const;
    qreal attributeAt(const QString &, qreal) const;
    QPointF pointAt(qreal) const;

Q_SIGNALS:
    void changed();

protected:
    virtual void componentComplete();

private Q_SLOTS:
    void processPath();

private:
    struct AttributePoint {
        AttributePoint() : percent(0), scale(1), origpercent(0) {}
        AttributePoint(const AttributePoint &other)
            : percent(other.percent), scale(other.scale), origpercent(other.origpercent), values(other.values) {}
        AttributePoint &operator=(const AttributePoint &other) {
            percent = other.percent; scale = other.scale; origpercent = other.origpercent; values = other.values; return *this;
        }
        qreal percent;      //massaged percent along the painter path
        qreal scale;
        qreal origpercent;  //'real' percent along the painter path
        QHash<QString, qreal> values;
    };

    void interpolate(int idx, const QString &name, qreal value);
    void endpoint(const QString &name);
    void createPointCache() const;

private:
    Q_DISABLE_COPY(QmlGraphicsPath)
    Q_DECLARE_PRIVATE(QmlGraphicsPath)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlGraphicsPathElement)
QML_DECLARE_TYPE(QmlGraphicsPathAttribute)
QML_DECLARE_TYPE(QmlGraphicsCurve)
QML_DECLARE_TYPE(QmlGraphicsPathLine)
QML_DECLARE_TYPE(QmlGraphicsPathQuad)
QML_DECLARE_TYPE(QmlGraphicsPathCubic)
QML_DECLARE_TYPE(QmlGraphicsPathPercent)
QML_DECLARE_TYPE(QmlGraphicsPath)

QT_END_HEADER

#endif // QMLGRAPHICSPATH_H
