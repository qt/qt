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

#include "qmlgraphicsgraphicsobjectcontainer_p.h"

#include "qmlgraphicsitem_p.h"

#include <QGraphicsObject>
#include <QGraphicsWidget>
#include <QGraphicsSceneResizeEvent>

QT_BEGIN_NAMESPACE

class QmlGraphicsGraphicsObjectContainerPrivate : public QmlGraphicsItemPrivate
{
    Q_DECLARE_PUBLIC(QmlGraphicsGraphicsObjectContainer)

public:
    QmlGraphicsGraphicsObjectContainerPrivate() : QmlGraphicsItemPrivate(), graphicsObject(0), syncedResize(false)
    { }

    void _q_updateSize();

    void setFiltering(bool on)
    {
        Q_Q(QmlGraphicsGraphicsObjectContainer);
        if (graphicsObject && graphicsObject->isWidget()) {
            if (!on) {
                graphicsObject->removeEventFilter(q);
                QObject::disconnect(q, SIGNAL(widthChanged()), q, SLOT(_q_updateSize()));
                QObject::disconnect(q, SIGNAL(heightChanged()), q, SLOT(_q_updateSize()));
            } else {
                graphicsObject->installEventFilter(q);
                QObject::connect(q, SIGNAL(widthChanged()), q, SLOT(_q_updateSize()));
                QObject::connect(q, SIGNAL(heightChanged()), q, SLOT(_q_updateSize()));
            }
        }
    }


    QGraphicsObject *graphicsObject;
    bool syncedResize;
};


/*!
    \qmlclass GraphicsObjectContainer QmlGraphicsGraphicsObjectContainer
    \since 4.7
    \brief The GraphicsObjectContainer element allows you to add QGraphicsObjects into Fluid UI elements.

    While any QObject based class can be exposed to QML, QmlGraphicsItem
    provides a lot of important functionality, including anchors and proper
    management of child items. GraphicsObjectContainer helps provide these
    functions to other QGraphicsObjects, so that they can be used unaltered in
    a QML scene. QGraphicsObjects, which are not QmlGraphicsItems, and which are
    placed in a QML scene outside of a GraphicsObjectContainer, will not appear
    on screen at all.

    A GraphicsObjectContainer can have one element inside it, and it must be a
    QGraphicsObject or subclass which has been exposed to the QML engine.
    The graphics object inside the GraphicsObjectContainer can then be used
    like any other item in QML with the exception of not being reparentable
    and not having the standard properties of QML items (such as anchors).

    As the contained object is positioned relative to the container, anchors
    affecting the container item will affect the onscreen position of the
    contained item. If synchronizedResizing is set to true, then anchors
    affecting the container item's size will also affect the contained item's
    size.

    Example:
    \code
    import Qt 4.6
    import MyApp 2.1 as Widgets
    Rectangle{
        id: rect
        property alias widgetPropertyThree: widget.myThirdProperty;
        GraphicsObjectContainer{
            synchronizedResizing: true
            anchors.margins: 10
            anchors.fill: parent
            Widgets.MyWidget{
                myProperty: "A Value"
                myOtherProperty: rect.color
            }
        }
    }
    \endcode
*/

/*!
    \internal
    \class QmlGraphicsGraphicsObjectContainer
    \brief The QmlGraphicsGraphicsObjectContainer class allows you to add QGraphicsObjects into Fluid UI applications.
*/

QML_DEFINE_NOCREATE_TYPE(QGraphicsObject)
QML_DEFINE_TYPE(Qt,4,6,GraphicsObjectContainer,QmlGraphicsGraphicsObjectContainer)

QmlGraphicsGraphicsObjectContainer::QmlGraphicsGraphicsObjectContainer(QmlGraphicsItem *parent)
: QmlGraphicsItem(*new QmlGraphicsGraphicsObjectContainerPrivate, parent)
{
}

QmlGraphicsGraphicsObjectContainer::~QmlGraphicsGraphicsObjectContainer()
{
}

QGraphicsObject *QmlGraphicsGraphicsObjectContainer::graphicsObject() const
{
    Q_D(const QmlGraphicsGraphicsObjectContainer);
    return d->graphicsObject;
}

/*!
    \qmlproperty QGraphicsObject GraphicsObjectContainer::graphicsObject
    The QGraphicsObject associated with this element.
*/
void QmlGraphicsGraphicsObjectContainer::setGraphicsObject(QGraphicsObject *object)
{
    Q_D(QmlGraphicsGraphicsObjectContainer);
    if (object == d->graphicsObject)
        return;

    //### remove previously set item?

    d->setFiltering(false);

    d->graphicsObject = object;

    if (d->graphicsObject) {
        d->graphicsObject->setParentItem(this);

        if (d->syncedResize && d->graphicsObject->isWidget()) {
            QGraphicsWidget *gw = static_cast<QGraphicsWidget*>(d->graphicsObject);
            QSizeF gwSize = gw->size(); //### should we use sizeHint?
            QSizeF newSize = gwSize;
            if (heightValid())
                newSize.setHeight(height());
            if (widthValid())
                newSize.setWidth(width());
            if (gwSize != newSize)
                gw->resize(newSize);

            gwSize = gw->size();
            setImplicitWidth(gwSize.width());
            setImplicitHeight(gwSize.height());

            d->setFiltering(true);
        }
    }
}

QVariant QmlGraphicsGraphicsObjectContainer::itemChange(GraphicsItemChange change, const QVariant &value)
{
    Q_D(QmlGraphicsGraphicsObjectContainer);
    if (change == ItemSceneHasChanged) {
        QGraphicsObject *o = d->graphicsObject;
        d->graphicsObject = 0;
        setGraphicsObject(o);
    }
    return QmlGraphicsItem::itemChange(change, value);
}

bool QmlGraphicsGraphicsObjectContainer::eventFilter(QObject *watched, QEvent *e)
{
    Q_D(QmlGraphicsGraphicsObjectContainer);
    if (watched == d->graphicsObject && e->type() == QEvent::GraphicsSceneResize) {
        if (d->graphicsObject && d->graphicsObject->isWidget() && d->syncedResize) {
           QSizeF newSize = static_cast<QGraphicsWidget*>(d->graphicsObject)->size();
           setImplicitWidth(newSize.width());
           setImplicitHeight(newSize.height());
       }
    }
    return QmlGraphicsItem::eventFilter(watched, e);
}

/*!
    \qmlproperty bool GraphicsObjectContainer::synchronizedResizing

    This property determines whether or not the container and graphics object will synchronize their
    sizes.

    \note This property only applies when wrapping a QGraphicsWidget.

    If synchronizedResizing is enabled, the container and widget will
    synchronize their sizes as follows.
    \list
    \o If a size has been set on the container, the widget will be resized to the container.
    Any changes in the container's size will be reflected in the widget.

    \o \e Otherwise, the container will initially be sized to the preferred size of the widget.
    Any changes to the container's size will be reflected in the widget, and any changes to the
    widget's size will be reflected in the container.
    \endlist
*/
bool QmlGraphicsGraphicsObjectContainer::synchronizedResizing() const
{
    Q_D(const QmlGraphicsGraphicsObjectContainer);
    return d->syncedResize;
}

void QmlGraphicsGraphicsObjectContainer::setSynchronizedResizing(bool on)
{
    Q_D(QmlGraphicsGraphicsObjectContainer);
    if (on == d->syncedResize)
        return;

    d->syncedResize = on;
    d->setFiltering(on);
}

void QmlGraphicsGraphicsObjectContainerPrivate::_q_updateSize()
{
    if (!graphicsObject || !graphicsObject->isWidget() || !syncedResize)
        return;

    QGraphicsWidget *gw = static_cast<QGraphicsWidget*>(graphicsObject);
    const QSizeF newSize(width, height);
    gw->resize(newSize);

    //### will respecting the widgets min/max ever get us in trouble? (all other items always
    //    size to exactly what you tell them)
    /*QSizeF constrainedSize = newSize.expandedTo(gw->minimumSize()).boundedTo(gw->maximumSize());
    gw->resize(constrainedSize);
    if (constrainedSize != newSize) {
        setImplicitWidth(constrainedSize.width());
        setImplicitHeight(constrainedSize.height());
    }*/
}

#include <moc_qmlgraphicsgraphicsobjectcontainer_p.cpp>

QT_END_NAMESPACE
