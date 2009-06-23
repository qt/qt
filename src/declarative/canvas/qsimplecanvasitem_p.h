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

#ifndef QSIMPLECANVASITEM_P_H
#define QSIMPLECANVASITEM_P_H

#include "private/qobject_p.h"
#include "private/qgraphicsitem_p.h"
#include "qsimplecanvas.h"
#include "qsimplecanvasitem.h"

#include "qgraphicsitem.h"


QT_BEGIN_NAMESPACE
class QSimpleCanvasItemDebuggerStatus : public QmlDebuggerStatus
{
public:
    QSimpleCanvasItemDebuggerStatus(QSimpleCanvasItem *i)
        : item(i), selected(false) {}

    virtual void setSelectedState(bool state)
    {
        selected = state;
        item->update();
    }

    QSimpleCanvasItem *item;
    bool selected;
};

class QGraphicsQSimpleCanvasItem;
class QSimpleCanvasItemPrivate : public QGraphicsItemPrivate
{
    Q_DECLARE_PUBLIC(QSimpleCanvasItem)
public:
    QSimpleCanvasItemPrivate()
    : parent(0), canvas(0), debuggerStatus(0), 
      clip(QSimpleCanvasItem::NoClip),
      origin(QSimpleCanvasItem::TopLeft), options(QSimpleCanvasItem::NoOption),
      focusable(false), wantsActiveFocusPanelPendingCanvas(false),
      hasBeenActiveFocusPanel(false),
      hasFocus(false), hasActiveFocus(false), 
      widthValid(false), heightValid(false), width(0), height(0), paintmargin(0), scale(1)
    {
    }

    virtual ~QSimpleCanvasItemPrivate() 
    {
        if(debuggerStatus) delete debuggerStatus;
    }

    QSimpleCanvasItem *parent;
    QSimpleCanvas *canvas;
    QList<QSimpleCanvasItem *> children;

    QSimpleCanvasItemDebuggerStatus *debuggerStatus;

    QSimpleCanvasItem::ClipType clip:3;
    QSimpleCanvasItem::TransformOrigin origin:4;
    int options:10;
    bool focusable:1;
    bool wantsActiveFocusPanelPendingCanvas:1;
    bool hasBeenActiveFocusPanel:1;
    bool hasFocus:1;
    bool hasActiveFocus:1;
    bool widthValid:1;
    bool heightValid:1;

    void setFocus(bool f);
    void setActiveFocus(bool f);

    qreal width;
    qreal height;
    qreal paintmargin;
    qreal scale;

    QTransform transform;

    void gvRemoveMouseFilter();
    void gvAddMouseFilter();

    void canvasChanged(QSimpleCanvas *newCanvas, QSimpleCanvas *oldCanvas);

    QPointF transformOrigin() const;

    void setParentInternal(QSimpleCanvasItem *);

    // Debugging
    int dump(int);
    enum FocusStateCheckData { NoCheckData = 0x00, 
                               InActivePanel = 0x01, 
                               InRealm = 0x02, 
                               InActiveFocusedRealm = 0x04
    };
    Q_DECLARE_FLAGS(FocusStateCheckDatas, FocusStateCheckData)
    enum FocusStateCheckRData { NoCheckRData = 0x00, 
                                SeenFocus = 0x01, 
                                SeenActiveFocus = 0x02 };
    Q_DECLARE_FLAGS(FocusStateCheckRDatas, FocusStateCheckRData)
    bool checkFocusState(FocusStateCheckDatas, FocusStateCheckRDatas *);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSimpleCanvasItemPrivate::FocusStateCheckDatas)
Q_DECLARE_OPERATORS_FOR_FLAGS(QSimpleCanvasItemPrivate::FocusStateCheckRDatas)

#endif // QSIMPLECANVASITEM_P_H

QT_END_NAMESPACE
