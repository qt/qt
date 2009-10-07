/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qfxgraphicsobjectcontainer.h"
#include <QGraphicsObject>
#include <QGraphicsWidget>
#include <QGraphicsSceneResizeEvent>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass GraphicsObjectContainer QFxGraphicsObjectContainer
    \brief The GraphicsObjectContainer element allows you to add QGraphicsObjects into Fluid UI elements.
*/

/*!
    \internal
    \class QFxGraphicsObjectContainer
    \brief The QFxGraphicsObjectContainer class allows you to add QGraphicsObjects into Fluid UI applications.
*/

QML_DEFINE_NOCREATE_TYPE(QGraphicsObject)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,GraphicsObjectContainer,QFxGraphicsObjectContainer)

QFxGraphicsObjectContainer::QFxGraphicsObjectContainer(QFxItem *parent)
: QFxItem(parent), _graphicsObject(0), _syncedResize(false)
{
}

QFxGraphicsObjectContainer::~QFxGraphicsObjectContainer()
{
}

QGraphicsObject *QFxGraphicsObjectContainer::graphicsObject() const
{
    return _graphicsObject;
}

/*!
    \qmlproperty QGraphicsObject GraphicsObjectContainer::graphicsObject
    The QGraphicsObject associated with this element.
*/
void QFxGraphicsObjectContainer::setGraphicsObject(QGraphicsObject *object)
{
    if (object == _graphicsObject)
        return;

    //### what should we do with previously set object?

    _graphicsObject = object;

    if (_graphicsObject) {
        _graphicsObject->setParentItem(this);

        if (_syncedResize && _graphicsObject->isWidget()) {
            _graphicsObject->installEventFilter(this);
            QSizeF newSize = static_cast<QGraphicsWidget*>(_graphicsObject)->size();    //### use sizeHint?
            setImplicitWidth(newSize.width());
            setImplicitHeight(newSize.height());
        }
    }
}

QVariant QFxGraphicsObjectContainer::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSceneHasChanged) {
        QGraphicsObject *o = _graphicsObject;
        _graphicsObject = 0;
        setGraphicsObject(o);
    }
    return QFxItem::itemChange(change, value);
}

bool QFxGraphicsObjectContainer::eventFilter(QObject *watched, QEvent *e)
{
    if (watched == _graphicsObject && e->type() == QEvent::GraphicsSceneResize) {
        if (_graphicsObject && _graphicsObject->isWidget() && _syncedResize) {
           QSizeF newSize = static_cast<QGraphicsWidget*>(_graphicsObject)->size();
           setImplicitWidth(newSize.width());
           setImplicitHeight(newSize.height());
       }
    }
    return QFxItem::eventFilter(watched, e);
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
bool QFxGraphicsObjectContainer::synchronizedResizing() const
{
    return _syncedResize;
}

void QFxGraphicsObjectContainer::setSynchronizedResizing(bool on)
{
    if (on == _syncedResize)
        return;

    if (_graphicsObject && _graphicsObject->isWidget()) {
        if (!on) {
            _graphicsObject->removeEventFilter(this);
            disconnect(this, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()));
            disconnect(this, SIGNAL(heightChanged()), this, SLOT(_q_updateSize()));
        }
    }

    _syncedResize = on;

    if (_graphicsObject && _graphicsObject->isWidget()) {
        if (on) {
            _graphicsObject->installEventFilter(this);
            connect(this, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()));
            connect(this, SIGNAL(heightChanged()), this, SLOT(_q_updateSize()));
        }
    }
}

void QFxGraphicsObjectContainer::_q_updateSize()
{
    if (!_graphicsObject || !_graphicsObject->isWidget() || !_syncedResize)
        return;

    QGraphicsWidget *gw = static_cast<QGraphicsWidget*>(_graphicsObject);
    const QSizeF newSize(width(), height());
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

QT_END_NAMESPACE
