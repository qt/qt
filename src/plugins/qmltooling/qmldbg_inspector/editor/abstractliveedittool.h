/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ABSTRACTLIVEEDITTOOL_H
#define ABSTRACTLIVEEDITTOOL_H

#include <QtCore/QList>
#include "../abstracttool.h"

QT_BEGIN_NAMESPACE
class QMouseEvent;
class QGraphicsItem;
class QDeclarativeItem;
class QKeyEvent;
class QGraphicsScene;
class QGraphicsObject;
class QWheelEvent;
class QDeclarativeView;
QT_END_NAMESPACE

namespace QmlJSDebugger {

class QDeclarativeViewInspector;

class AbstractLiveEditTool : public AbstractTool
{
    Q_OBJECT
public:
    AbstractLiveEditTool(QDeclarativeViewInspector *inspector);

    virtual ~AbstractLiveEditTool();

    void leaveEvent(QEvent *) {}

    virtual void itemsAboutToRemoved(const QList<QGraphicsItem*> &itemList) = 0;

    virtual void clear() = 0;

    void updateSelectedItems();
    QList<QGraphicsItem*> items() const;

    bool topItemIsMovable(const QList<QGraphicsItem*> &itemList);
    bool topItemIsResizeHandle(const QList<QGraphicsItem*> &itemList);
    bool topSelectedItemIsMovable(const QList<QGraphicsItem*> &itemList);

    QString titleForItem(QGraphicsItem *item);

    static QList<QGraphicsObject*> toGraphicsObjectList(const QList<QGraphicsItem*> &itemList);
    static QGraphicsItem* topMovableGraphicsItem(const QList<QGraphicsItem*> &itemList);
    static QDeclarativeItem* topMovableDeclarativeItem(const QList<QGraphicsItem*> &itemList);
    static QDeclarativeItem *toQDeclarativeItem(QGraphicsItem *item);

protected:
    virtual void selectedItemsChanged(const QList<QGraphicsItem*> &objectList) = 0;

    QDeclarativeViewInspector *inspector() const;
    QDeclarativeView *view() const;
    QGraphicsScene *scene() const;

private:
    QList<QGraphicsItem*> m_itemList;
};

}

#endif // ABSTRACTLIVEEDITTOOL_H
