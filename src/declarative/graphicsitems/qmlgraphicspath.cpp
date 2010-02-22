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

#include "qmlgraphicspath_p.h"
#include "qmlgraphicspath_p_p.h"

#include <qfxperf_p_p.h>

#include <QSet>

#include <private/qbezier_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass PathElement QmlGraphicsPathElement
    \brief PathElement is the base path type.

    This type is the base for all path types.  It cannot
    be instantiated.

    \sa Path, PathAttribute, PathPercent, PathLine, PathQuad, PathCubic
*/

/*!
    \internal
    \class QmlGraphicsPathElement
    \ingroup group_utility
*/

/*!
    \qmlclass Path QmlGraphicsPath
    \brief A Path object defines a path for use by \l PathView.

    A Path is composed of one or more path segments - PathLine, PathQuad,
    PathCubic.

    The spacing of the items along the Path can be adjusted via a
    PathPercent object.

    PathAttribute allows named attributes with values to be defined
    along the path.

    \sa PathView, PathAttribute, PathPercent, PathLine, PathQuad, PathCubic
*/

/*!
    \internal
    \class QmlGraphicsPath
    \ingroup group_utility
    \brief The QmlGraphicsPath class defines a path.
    \sa QmlGraphicsPathView
*/
QmlGraphicsPath::QmlGraphicsPath(QObject *parent)
 : QObject(*(new QmlGraphicsPathPrivate), parent)
{
}

QmlGraphicsPath::~QmlGraphicsPath()
{
}

/*!
    \qmlproperty real Path::startX
    \qmlproperty real Path::startY
    These properties hold the starting position of the path.
*/
qreal QmlGraphicsPath::startX() const
{
    Q_D(const QmlGraphicsPath);
    return d->startX;
}

void QmlGraphicsPath::setStartX(qreal x)
{
    Q_D(QmlGraphicsPath);
    d->startX = x;
}

qreal QmlGraphicsPath::startY() const
{
    Q_D(const QmlGraphicsPath);
    return d->startY;
}

void QmlGraphicsPath::setStartY(qreal y)
{
    Q_D(QmlGraphicsPath);
    d->startY = y;
}

/*!
    \qmlproperty bool Path::closed
    This property holds whether the start and end of the path are identical.
*/
bool QmlGraphicsPath::isClosed() const
{
    Q_D(const QmlGraphicsPath);
    return d->closed;
}

/*!
    \qmlproperty list<PathElement> Path::pathElements
    This property holds the objects composing the path.

    \default

    A path can contain the following path objects:
    \list
        \i \l PathLine - a straight line to a given position.
        \i \l PathQuad - a quadratic Bezier curve to a given position with a control point.
        \i \l PathCubic - a cubic Bezier curve to a given position with two control points.
        \i \l PathAttribute - an attribute at a given position in the path.
        \i \l PathPercent - a way to spread out items along various segments of the path.
    \endlist

    \snippet doc/src/snippets/declarative/pathview/pathattributes.qml 2
*/

QmlListProperty<QmlGraphicsPathElement> QmlGraphicsPath::pathElements()
{
    Q_D(QmlGraphicsPath);
    return QmlListProperty<QmlGraphicsPathElement>(this, d->_pathElements);
}

void QmlGraphicsPath::interpolate(int idx, const QString &name, qreal value)
{
    Q_D(QmlGraphicsPath);
    if (!idx)
        return;

    qreal lastValue = 0;
    qreal lastPercent = 0;
    int search = idx - 1;
    while(search >= 0) {
        const AttributePoint &point = d->_attributePoints.at(search);
        if (point.values.contains(name)) {
            lastValue = point.values.value(name);
            lastPercent = point.origpercent;
            break;
        }
        --search;
    }

    ++search;

    const AttributePoint &curPoint = d->_attributePoints.at(idx);

    for (int ii = search; ii < idx; ++ii) {
        AttributePoint &point = d->_attributePoints[ii];

        qreal val = lastValue + (value - lastValue) * (point.origpercent - lastPercent) / (curPoint.origpercent - lastPercent);
        point.values.insert(name, val);
    }
}

void QmlGraphicsPath::endpoint(const QString &name)
{
    Q_D(QmlGraphicsPath);
    const AttributePoint &first = d->_attributePoints.first();
    qreal val = first.values.value(name);
    for (int ii = d->_attributePoints.count() - 1; ii >= 0; ii--) {
        const AttributePoint &point = d->_attributePoints.at(ii);
        if (point.values.contains(name)) {
            for (int jj = ii + 1; jj < d->_attributePoints.count(); ++jj) {
                AttributePoint &setPoint = d->_attributePoints[jj];
                setPoint.values.insert(name, val);
            }
            return;
        }
    }
}

void QmlGraphicsPath::processPath()
{
    Q_D(QmlGraphicsPath);

    d->_pointCache.clear();
    d->_attributePoints.clear();
    d->_path = QPainterPath();

    AttributePoint first;
    for (int ii = 0; ii < d->_attributes.count(); ++ii)
        first.values[d->_attributes.at(ii)] = 0;
    d->_attributePoints << first;

    d->_path.moveTo(d->startX, d->startY);

    QmlGraphicsCurve *lastCurve = 0;
    foreach (QmlGraphicsPathElement *pathElement, d->_pathElements) {
        if (QmlGraphicsCurve *curve = qobject_cast<QmlGraphicsCurve *>(pathElement)) {
            curve->addToPath(d->_path);
            AttributePoint p;
            p.origpercent = d->_path.length();
            d->_attributePoints << p;
            lastCurve = curve;
        } else if (QmlGraphicsPathAttribute *attribute = qobject_cast<QmlGraphicsPathAttribute *>(pathElement)) {
            AttributePoint &point = d->_attributePoints.last();
            point.values[attribute->name()] = attribute->value();
            interpolate(d->_attributePoints.count() - 1, attribute->name(), attribute->value());
        } else if (QmlGraphicsPathPercent *percent = qobject_cast<QmlGraphicsPathPercent *>(pathElement)) {
            AttributePoint &point = d->_attributePoints.last();
            point.values[QLatin1String("_qfx_percent")] = percent->value();
            interpolate(d->_attributePoints.count() - 1, QLatin1String("_qfx_percent"), percent->value());
        }
    }

    // Fixup end points
    const AttributePoint &last = d->_attributePoints.last();
    for (int ii = 0; ii < d->_attributes.count(); ++ii) {
        if (!last.values.contains(d->_attributes.at(ii)))
            endpoint(d->_attributes.at(ii));
    }

    // Adjust percent
    qreal length = d->_path.length();
    qreal prevpercent = 0;
    qreal prevorigpercent = 0;
    for (int ii = 0; ii < d->_attributePoints.count(); ++ii) {
        const AttributePoint &point = d->_attributePoints.at(ii);
        if (point.values.contains(QLatin1String("_qfx_percent"))) { //special string for QmlGraphicsPathPercent
            if ( ii > 0) {
                qreal scale = (d->_attributePoints[ii].origpercent/length - prevorigpercent) /
                            (point.values.value(QLatin1String("_qfx_percent"))-prevpercent);
                d->_attributePoints[ii].scale = scale;
            }
            d->_attributePoints[ii].origpercent /= length;
            d->_attributePoints[ii].percent = point.values.value(QLatin1String("_qfx_percent"));
            prevorigpercent = d->_attributePoints[ii].origpercent;
            prevpercent = d->_attributePoints[ii].percent;
        } else {
            d->_attributePoints[ii].origpercent /= length;
            d->_attributePoints[ii].percent = d->_attributePoints[ii].origpercent;
        }
    }

    d->closed = lastCurve && d->startX == lastCurve->x() && d->startY == lastCurve->y();

    emit changed();
}

void QmlGraphicsPath::componentComplete()
{
    Q_D(QmlGraphicsPath);
    QSet<QString> attrs;
    // First gather up all the attributes
    foreach (QmlGraphicsPathElement *pathElement, d->_pathElements) {
        if (QmlGraphicsPathAttribute *attribute =
            qobject_cast<QmlGraphicsPathAttribute *>(pathElement))
            attrs.insert(attribute->name());
    }
    d->_attributes = attrs.toList();

    processPath();

    foreach (QmlGraphicsPathElement *pathElement, d->_pathElements)
        connect(pathElement, SIGNAL(changed()), this, SLOT(processPath()));
}

QPainterPath QmlGraphicsPath::path() const
{
    Q_D(const QmlGraphicsPath);
    return d->_path;
}

QStringList QmlGraphicsPath::attributes() const
{
    Q_D(const QmlGraphicsPath);
    return d->_attributes;
}
#include <QTime>

static inline QBezier nextBezier(const QPainterPath &path, int *from, qreal *bezLength)
{
    const int lastElement = path.elementCount() - 1;
    for (int i=*from; i <= lastElement; ++i) {
        const QPainterPath::Element &e = path.elementAt(i);

        switch (e.type) {
        case QPainterPath::MoveToElement:
            break;
        case QPainterPath::LineToElement:
        {
            QLineF line(path.elementAt(i-1), e);
            *bezLength = line.length();
            QPointF a = path.elementAt(i-1);
            QPointF delta = e - a;
            *from = i+1;
            return QBezier::fromPoints(a, a + delta / 3, a + 2 * delta / 3, e);
        }
        case QPainterPath::CurveToElement:
        {
            QBezier b = QBezier::fromPoints(path.elementAt(i-1),
                                            e,
                                            path.elementAt(i+1),
                                            path.elementAt(i+2));
            *bezLength = b.length();
            *from = i+3;
            return b;
        }
        default:
            break;
        }
    }
    *from = lastElement;
    *bezLength = 0;
    return QBezier();
}

void QmlGraphicsPath::createPointCache() const
{
    Q_D(const QmlGraphicsPath);
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QmlPerfTimer<QmlPerf::QmlGraphicsPathViewPathCache> pc;
#endif
    qreal pathLength = d->_path.length();
    const int points = int(pathLength*2);
    const int lastElement = d->_path.elementCount() - 1;
    d->_pointCache.resize(points+1);

    int currElement = 0;
    qreal bezLength = 0;
    QBezier currBez = nextBezier(d->_path, &currElement, &bezLength);
    qreal currLength = bezLength;
    qreal epc = currLength / pathLength;

    for (int i = 0; i < d->_pointCache.size(); i++) {
        //find which set we are in
        qreal prevPercent = 0;
        qreal prevOrigPercent = 0;
        for (int ii = 0; ii < d->_attributePoints.count(); ++ii) {
            qreal percent = qreal(i)/points;
            const AttributePoint &point = d->_attributePoints.at(ii);
            if (percent < point.percent || ii == d->_attributePoints.count() - 1) { //### || is special case for very last item
                qreal elementPercent = (percent - prevPercent);

                qreal spc = prevOrigPercent + elementPercent * point.scale;

                while (spc > epc) {
                    if (currElement > lastElement)
                        break;
                    currBez = nextBezier(d->_path, &currElement, &bezLength);
                    if (bezLength == 0.0) {
                        currLength = pathLength;
                        epc = 1.0;
                        break;
                    }
                    currLength += bezLength;
                    epc = currLength / pathLength;
                }
                qreal realT = (pathLength * spc - (currLength - bezLength)) / bezLength;
                d->_pointCache[i] = currBez.pointAt(qBound(qreal(0), realT, qreal(1)));
                break;
            }
            prevOrigPercent = point.origpercent;
            prevPercent = point.percent;
        }
    }
}

QPointF QmlGraphicsPath::pointAt(qreal p) const
{
    Q_D(const QmlGraphicsPath);
    if (d->_pointCache.isEmpty()) {
        createPointCache();
    }
    int idx = qRound(p*d->_pointCache.size());
    if (idx >= d->_pointCache.size())
        idx = d->_pointCache.size() - 1;
    else if (idx < 0)
        idx = 0;
    return d->_pointCache.at(idx);
}

qreal QmlGraphicsPath::attributeAt(const QString &name, qreal percent) const
{
    Q_D(const QmlGraphicsPath);
    if (percent < 0 || percent > 1)
        return 0;

    for (int ii = 0; ii < d->_attributePoints.count(); ++ii) {
        const AttributePoint &point = d->_attributePoints.at(ii);

        if (point.percent == percent) {
            return point.values.value(name);
        } else if (point.percent > percent) {
            qreal lastValue =
                ii?(d->_attributePoints.at(ii - 1).values.value(name)):0;
            qreal lastPercent =
                ii?(d->_attributePoints.at(ii - 1).percent):0;
            qreal curValue = point.values.value(name);
            qreal curPercent = point.percent;

            return lastValue + (curValue - lastValue) * (percent - lastPercent) / (curPercent - lastPercent);
        }
    }

    return 0;
}

/****************************************************************************/

qreal QmlGraphicsCurve::x() const
{
    return _x;
}

void QmlGraphicsCurve::setX(qreal x)
{
    if (_x != x) {
        _x = x;
        emit changed();
    }
}

qreal QmlGraphicsCurve::y() const
{
    return _y;
}

void QmlGraphicsCurve::setY(qreal y)
{
    if (_y != y) {
        _y = y;
        emit changed();
    }
}

/****************************************************************************/

/*!
    \qmlclass PathAttribute QmlGraphicsPathAttribute
    \brief The PathAttribute allows setting an attribute at a given position in a Path.

    The PathAttribute object allows attibutes consisting of a name and
    a value to be specified for the endpoints of path segments.  The
    attributes are exposed to the delegate as
    \l{qmlintroduction.html#attached-properties} {Attached Properties}.
    The value of an attribute at any particular point is interpolated
    from the PathAttributes bounding the point.

    The example below shows a path with the items scaled to 30% with
    opacity 50% at the top of the path and scaled 100% with opacity
    100% at the bottom.  Note the use of the PathView.scale and
    PathView.opacity attached properties to set the scale and opacity
    of the delegate.

    \table
    \row
    \o \image declarative-pathattribute.png
    \o
    \snippet doc/src/snippets/declarative/pathview/pathattributes.qml 0
    \endtable

   \sa Path
*/

/*!
    \internal
    \class QmlGraphicsPathAttribute
    \ingroup group_utility
    \brief The QmlGraphicsPathAttribute class allows to set the value of an attribute at a given position in the path.

    \sa QmlGraphicsPath
*/


/*!
    \qmlproperty string PathAttribute::name
    the name of the attribute to change.
*/

/*!
    the name of the attribute to change.
*/

QString QmlGraphicsPathAttribute::name() const
{
    return _name;
}

void QmlGraphicsPathAttribute::setName(const QString &name)
{
    _name = name;
}

/*!
   \qmlproperty string PathAttribute::value
   the new value of the attribute.
*/

/*!
    the new value of the attribute.
*/
qreal QmlGraphicsPathAttribute::value() const
{
    return _value;
}

void QmlGraphicsPathAttribute::setValue(qreal value)
{
    if (_value != value) {
        _value = value;
        emit changed();
    }
}

/****************************************************************************/

/*!
    \qmlclass PathLine QmlGraphicsPathLine
    \brief The PathLine defines a straight line.

    The example below creates a path consisting of a straight line from
    0,100 to 200,100:

    \qml
    Path {
        startX: 0; startY: 100
        PathLine { x: 200; y: 100 }
    }
    \endqml

    \sa Path, PathQuad, PathCubic
*/

/*!
    \internal
    \class QmlGraphicsPathLine
    \ingroup group_utility
    \brief The QmlGraphicsPathLine class defines a straight line.

    \sa QmlGraphicsPath
*/

/*!
    \qmlproperty real PathLine::x
    \qmlproperty real PathLine::y

    Defines the end point of the line.
*/

void QmlGraphicsPathLine::addToPath(QPainterPath &path)
{
    path.lineTo(x(), y());
}

/****************************************************************************/

/*!
    \qmlclass PathQuad QmlGraphicsPathQuad
    \brief The PathQuad defines a quadratic Bezier curve with a control point.

    The following QML produces the path shown below:
    \table
    \row
    \o \image declarative-pathquad.png
    \o
    \qml
    Path {
        startX: 0; startY: 0
        PathQuad x: 200; y: 0; controlX: 100; controlY: 150 }
    }
    \endqml
    \endtable

    \sa Path, PathCubic, PathLine
*/

/*!
    \internal
    \class QmlGraphicsPathQuad
    \ingroup group_utility
    \brief The QmlGraphicsPathQuad class defines a quadratic Bezier curve with a control point.

    \sa QmlGraphicsPath
*/


/*!
    \qmlproperty real PathQuad::x
    \qmlproperty real PathQuad::y

    Defines the end point of the curve.
*/

/*!
   \qmlproperty real PathQuad::controlX
   \qmlproperty real PathQuad::controlY

   Defines the position of the control point.
*/

/*!
    the x position of the control point.
*/
qreal QmlGraphicsPathQuad::controlX() const
{
    return _controlX;
}

void QmlGraphicsPathQuad::setControlX(qreal x)
{
    if (_controlX != x) {
        _controlX = x;
        emit changed();
    }
}


/*!
    the y position of the control point.
*/
qreal QmlGraphicsPathQuad::controlY() const
{
    return _controlY;
}

void QmlGraphicsPathQuad::setControlY(qreal y)
{
    if (_controlY != y) {
        _controlY = y;
        emit changed();
    }
}

void QmlGraphicsPathQuad::addToPath(QPainterPath &path)
{
    path.quadTo(controlX(), controlY(), x(), y());
}

/****************************************************************************/

/*!
   \qmlclass PathCubic QmlGraphicsPathCubic
   \brief The PathCubic defines a cubic Bezier curve with two control points.

    The following QML produces the path shown below:
    \table
    \row
    \o \image declarative-pathcubic.png
    \o
    \qml
    Path {
        startX: 20; startY: 0
        PathCubic {
            x: 180; y: 0; control1X: -10; control1Y: 90
                          control2X: 210; control2Y: 90
        }
    }
    \endqml
    \endtable

    \sa Path, PathQuad, PathLine
*/

/*!
    \internal
    \class QmlGraphicsPathCubic
    \ingroup group_utility
    \brief The QmlGraphicsPathCubic class defines a cubic Bezier curve with two control points.

    \sa QmlGraphicsPath
*/

/*!
    \qmlproperty real PathCubic::x
    \qmlproperty real PathCubic::y

    Defines the end point of the curve.
*/

/*!
   \qmlproperty real PathCubic::control1X
   \qmlproperty real PathCubic::control1Y

    Defines the position of the first control point.
*/
qreal QmlGraphicsPathCubic::control1X() const
{
    return _control1X;
}

void QmlGraphicsPathCubic::setControl1X(qreal x)
{
    if (_control1X != x) {
        _control1X = x;
        emit changed();
    }
}

qreal QmlGraphicsPathCubic::control1Y() const
{
    return _control1Y;
}

void QmlGraphicsPathCubic::setControl1Y(qreal y)
{
    if (_control1Y != y) {
        _control1Y = y;
        emit changed();
    }
}

/*!
   \qmlproperty real PathCubic::control2X
   \qmlproperty real PathCubic::control2Y

    Defines the position of the second control point.
*/
qreal QmlGraphicsPathCubic::control2X() const
{
    return _control2X;
}

void QmlGraphicsPathCubic::setControl2X(qreal x)
{
    if (_control2X != x) {
        _control2X = x;
        emit changed();
    }
}

qreal QmlGraphicsPathCubic::control2Y() const
{
    return _control2Y;
}

void QmlGraphicsPathCubic::setControl2Y(qreal y)
{
    if (_control2Y != y) {
        _control2Y = y;
        emit changed();
    }
}

void QmlGraphicsPathCubic::addToPath(QPainterPath &path)
{
    path.cubicTo(control1X(), control1Y(), control2X(), control2Y(), x(), y());
}

/****************************************************************************/

/*!
    \qmlclass PathPercent QmlGraphicsPathPercent
    \brief The PathPercent manipulates the way a path is interpreted.

    The examples below show the normal distrubution of items along a path
    compared to a distribution which places 50% of the items along the
    PathLine section of the path.
    \table
    \row
    \o \image declarative-nopercent.png
    \o
    \qml
    Path {
        startX: 20; startY: 0
        PathQuad { x: 50; y: 80; controlX: 0; controlY: 80 }
        PathLine { x: 150; y: 80 }
        PathQuad { x: 180; y: 0; controlX: 200; controlY: 80 }
    }
    \endqml
    \row
    \o \image declarative-percent.png
    \o
    \qml
    Path {
        startX: 20; startY: 0
        PathQuad { x: 50; y: 80; controlX: 0; controlY: 80 }
        PathPercent { value: 0.25 }
        PathLine { x: 150; y: 80 }
        PathPercent { value: 0.75 }
        PathQuad { x: 180; y: 0; controlX: 200; controlY: 80 }
        PathPercent { value: 1 }
    }
    \endqml
    \endtable

    \sa Path
*/

/*!
    \internal
    \class QmlGraphicsPathPercent
    \ingroup group_utility
    \brief The QmlGraphicsPathPercent class manipulates the way a path is interpreted.

    QmlGraphicsPathPercent allows you to bunch up items (or spread out items) along various
    segments of a QmlGraphicsPathView's path.

    \sa QmlGraphicsPath

*/

qreal QmlGraphicsPathPercent::value() const
{
    return _value;
}

void QmlGraphicsPathPercent::setValue(qreal value)
{
    _value = value;
}
QT_END_NAMESPACE
