/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVEVIEWOBSERVER_P_P_H
#define QDECLARATIVEVIEWOBSERVER_P_P_H

#include "qdeclarativeviewobserver_p.h"

#include <QtCore/QWeakPointer>
#include <QtCore/QPointF>

#include "QtDeclarative/private/qdeclarativeobserverservice_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeViewObserver;
class LiveSelectionTool;
class ZoomTool;
class ColorPickerTool;
class LiveLayerItem;
class BoundingRectHighlighter;
class ToolBox;
class AbstractLiveEditTool;

class QDeclarativeViewObserverPrivate : public QObject
{
    Q_OBJECT
public:
    QDeclarativeViewObserverPrivate(QDeclarativeViewObserver *);
    ~QDeclarativeViewObserverPrivate();

    QDeclarativeView *view;
    QDeclarativeViewObserver *q;
    QDeclarativeObserverService *debugService;
    QWeakPointer<QWidget> viewport;
    QHash<int, QString> stringIdForObjectId;

    QPointF cursorPos;
    QList<QWeakPointer<QGraphicsObject> > currentSelection;

    Constants::DesignTool currentToolMode;
    AbstractLiveEditTool *currentTool;

    LiveSelectionTool *selectionTool;
    ZoomTool *zoomTool;
    ColorPickerTool *colorPickerTool;
    LiveLayerItem *manipulatorLayer;

    BoundingRectHighlighter *boundingRectHighlighter;

    bool designModeBehavior;
    bool showAppOnTop;

    bool animationPaused;
    qreal slowDownFactor;

    ToolBox *toolBox;

    void setViewport(QWidget *widget);

    void clearEditorItems();
    void createToolBox();
    void changeToSelectTool();
    QList<QGraphicsItem*> filterForSelection(QList<QGraphicsItem*> &itemlist) const;

    QList<QGraphicsItem*> selectableItems(const QPoint &pos) const;
    QList<QGraphicsItem*> selectableItems(const QPointF &scenePos) const;
    QList<QGraphicsItem*> selectableItems(const QRectF &sceneRect, Qt::ItemSelectionMode selectionMode) const;

    void setSelectedItemsForTools(const QList<QGraphicsItem *> &items);
    void setSelectedItems(const QList<QGraphicsItem *> &items);
    QList<QGraphicsItem *> selectedItems() const;

    void changeTool(Constants::DesignTool tool,
                    Constants::ToolFlags flags = Constants::NoToolFlags);

    void clearHighlight();
    void highlight(const QList<QGraphicsObject *> &item);
    inline void highlight(QGraphicsObject *item)
    { highlight(QList<QGraphicsObject*>() << item); }

    bool isEditorItem(QGraphicsItem *item) const;

public slots:
    void _q_setToolBoxVisible(bool visible);

    void _q_reloadView();
    void _q_onStatusChanged(QDeclarativeView::Status status);
    void _q_onCurrentObjectsChanged(QList<QObject*> objects);
    void _q_applyChangesFromClient();
    void _q_createQmlObject(const QString &qml, QObject *parent,
                            const QStringList &imports, const QString &filename = QString());
    void _q_reparentQmlObject(QObject *, QObject *);

    void _q_changeToSingleSelectTool();
    void _q_changeToMarqueeSelectTool();
    void _q_changeToZoomTool();
    void _q_changeToColorPickerTool();
    void _q_clearComponentCache();
    void _q_removeFromSelection(QObject *);

public:
    static QDeclarativeViewObserverPrivate *get(QDeclarativeViewObserver *v) { return v->d_func(); }
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDECLARATIVEVIEWOBSERVER_P_P_H
