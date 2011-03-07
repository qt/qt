// Commit: 57676c237992e0aa5a93a4e8fa66b3e7b90c2c90
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

#ifndef QSGMOUSEAREA_P_H
#define QSGMOUSEAREA_P_H

#include "qsgitem.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class Q_AUTOTEST_EXPORT QSGDrag : public QObject
{
    Q_OBJECT

    Q_ENUMS(Axis)
    Q_PROPERTY(QSGItem *target READ target WRITE setTarget NOTIFY targetChanged RESET resetTarget)
    Q_PROPERTY(Axis axis READ axis WRITE setAxis NOTIFY axisChanged)
    Q_PROPERTY(qreal minimumX READ xmin WRITE setXmin NOTIFY minimumXChanged)
    Q_PROPERTY(qreal maximumX READ xmax WRITE setXmax NOTIFY maximumXChanged)
    Q_PROPERTY(qreal minimumY READ ymin WRITE setYmin NOTIFY minimumYChanged)
    Q_PROPERTY(qreal maximumY READ ymax WRITE setYmax NOTIFY maximumYChanged)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(bool filterChildren READ filterChildren WRITE setFilterChildren NOTIFY filterChildrenChanged)
    //### consider drag and drop

public:
    QSGDrag(QObject *parent=0);
    ~QSGDrag();

    QSGItem *target() const;
    void setTarget(QSGItem *);
    void resetTarget();

    enum Axis { XAxis=0x01, YAxis=0x02, XandYAxis=0x03 };
    Axis axis() const;
    void setAxis(Axis);

    qreal xmin() const;
    void setXmin(qreal);
    qreal xmax() const;
    void setXmax(qreal);
    qreal ymin() const;
    void setYmin(qreal);
    qreal ymax() const;
    void setYmax(qreal);

    bool active() const;
    void setActive(bool);

    bool filterChildren() const;
    void setFilterChildren(bool);

Q_SIGNALS:
    void targetChanged();
    void axisChanged();
    void minimumXChanged();
    void maximumXChanged();
    void minimumYChanged();
    void maximumYChanged();
    void activeChanged();
    void filterChildrenChanged();

private:
    QSGItem *_target;
    Axis _axis;
    qreal _xmin;
    qreal _xmax;
    qreal _ymin;
    qreal _ymax;
    bool _active : 1;
    bool _filterChildren: 1;
    Q_DISABLE_COPY(QSGDrag)
};

class QSGMouseEvent;
class QSGMouseAreaPrivate;
class Q_AUTOTEST_EXPORT QSGMouseArea : public QSGItem
{
    Q_OBJECT

    Q_PROPERTY(qreal mouseX READ mouseX NOTIFY mousePositionChanged)
    Q_PROPERTY(qreal mouseY READ mouseY NOTIFY mousePositionChanged)
    Q_PROPERTY(bool containsMouse READ hovered NOTIFY hoveredChanged)
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(Qt::MouseButtons pressedButtons READ pressedButtons NOTIFY pressedChanged)
    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons WRITE setAcceptedButtons NOTIFY acceptedButtonsChanged)
    Q_PROPERTY(bool hoverEnabled READ hoverEnabled WRITE setHoverEnabled NOTIFY hoverEnabledChanged)
    Q_PROPERTY(QSGDrag *drag READ drag CONSTANT) //### add flicking to QSGDrag or add a QDeclarativeFlick ???
    Q_PROPERTY(bool preventStealing READ preventStealing WRITE setPreventStealing NOTIFY preventStealingChanged)

public:
    QSGMouseArea(QSGItem *parent=0);
    ~QSGMouseArea();

    qreal mouseX() const;
    qreal mouseY() const;

    bool isEnabled() const;
    void setEnabled(bool);

    bool hovered() const;
    bool pressed() const;

    Qt::MouseButtons pressedButtons() const;

    Qt::MouseButtons acceptedButtons() const;
    void setAcceptedButtons(Qt::MouseButtons buttons);

    bool hoverEnabled() const;
    void setHoverEnabled(bool h);

    QSGDrag *drag();

    bool preventStealing() const;
    void setPreventStealing(bool prevent);

Q_SIGNALS:
    void hoveredChanged();
    void pressedChanged();
    void enabledChanged();
    void acceptedButtonsChanged();
    void hoverEnabledChanged();
    void positionChanged(QSGMouseEvent *mouse);
    void mousePositionChanged(QSGMouseEvent *mouse);
    void preventStealingChanged();

    void pressed(QSGMouseEvent *mouse);
    void pressAndHold(QSGMouseEvent *mouse);
    void released(QSGMouseEvent *mouse);
    void clicked(QSGMouseEvent *mouse);
    void doubleClicked(QSGMouseEvent *mouse);
    void entered();
    void exited();
    void canceled();

protected:
    void setHovered(bool);
    bool setPressed(bool);
    bool sendMouseEvent(QGraphicsSceneMouseEvent *event);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseUngrabEvent();
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual bool childMouseEventFilter(QSGItem *i, QEvent *e);
    virtual void timerEvent(QTimerEvent *event);

    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);
    virtual void itemChange(ItemChange change, const ItemChangeData& value);

private:
    void handlePress();
    void handleRelease();

private:
    Q_DISABLE_COPY(QSGMouseArea)
    Q_DECLARE_PRIVATE(QSGMouseArea)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QSGDrag)
QML_DECLARE_TYPE(QSGMouseArea)

QT_END_HEADER

#endif // QSGMOUSEAREA_P_H
