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
#include "qsimplecanvas.h"
#include "qsimplecanvasitem.h"
#include "qsimplecanvasfilter.h"

#if defined(QFX_RENDER_OPENGL2)
#include <glbasicshaders.h>
#endif

#include "qgraphicsitem.h"


QT_BEGIN_NAMESPACE
class QSimpleGraphicsItem : public QGraphicsItem
{
public:
    QSimpleGraphicsItem(QSimpleCanvasItem *);
    virtual ~QSimpleGraphicsItem();

    virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
    virtual QRectF boundingRect() const;

    QTransform transform;
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual bool sceneEvent(QEvent *);
    virtual QVariant itemChange(GraphicsItemChange, const QVariant &);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void focusInEvent(QFocusEvent *event);

private:
    friend class QSimpleCanvasItem;
    QSimpleCanvasItem *owner;
};

class QSimpleCanvasItemData 
{
public:
    QSimpleCanvasItemData();
    ~QSimpleCanvasItemData();

    // 5 bits is all that's needed to store Qt::MouseButtons
    int buttons:5;
    QSimpleCanvasItem::Flip flip:2;
    bool dirty:1;
    bool transformValid:1;
    bool doNotPaint:1;
    bool doNotPaintChildren:1;

    qreal x;
    qreal y;
    qreal z;
    float visible;

    QSimpleCanvas::Matrix *transformUser;
    QSimpleCanvas::Matrix transformActive;
    int transformVersion;

    float activeOpacity;

#if defined(QFX_RENDER_OPENGL)
    QRectF lastPaintRect;
#else
    QRect lastPaintRect;
#endif
};

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

class QSimpleCanvasFilter;
class QGraphicsQSimpleCanvasItem;
class QSimpleCanvasItemPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QSimpleCanvasItem)
public:
    QSimpleCanvasItemPrivate()
    : parent(0), canvas(0), debuggerStatus(0), filter(0),
      clip(QSimpleCanvasItem::NoClip),
      origin(QSimpleCanvasItem::TopLeft), options(QSimpleCanvasItem::NoOption),
      focusable(false), wantsActiveFocusPanelPendingCanvas(false),
      hasBeenActiveFocusPanel(false),
      hasFocus(false), hasActiveFocus(false), needsZOrder(false), 
      widthValid(false), heightValid(false), width(0), height(0), paintmargin(0), scale(1), 
      graphicsItem(0), data_ptr(0)
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
    QSimpleCanvasFilter *filter;

    QSimpleCanvasItem::ClipType clip:3;
    QSimpleCanvasItem::TransformOrigin origin:4;
    int options:10;
    bool focusable:1;
    bool wantsActiveFocusPanelPendingCanvas:1;
    bool hasBeenActiveFocusPanel:1;
    bool hasFocus:1;
    bool hasActiveFocus:1;
    bool needsZOrder:1;
    bool widthValid:1;
    bool heightValid:1;

    void setFocus(bool f);
    void setActiveFocus(bool f);

    qreal width;
    qreal height;
    qreal paintmargin;
    qreal scale;

    QSimpleGraphicsItem *graphicsItem;
    inline QSimpleCanvasItemData *data() const { 
        if (!data_ptr) data_ptr = new QSimpleCanvasItemData;
        return data_ptr; 
    }
    mutable QSimpleCanvasItemData *data_ptr;

    void gvRemoveMouseFilter();
    void gvAddMouseFilter();

    void canvasChanged(QSimpleCanvas *newCanvas, QSimpleCanvas *oldCanvas);

    void freshenTransforms() const;

    QPointF adjustFrom(const QPointF &) const;
    QRectF adjustFrom(const QRectF &) const;
    QPointF adjustTo(const QPointF &) const;
    QRectF adjustTo(const QRectF &) const;

    QPointF transformOrigin() const;

    void setParentInternal(QSimpleCanvasItem *);
    void convertToGraphicsItem(QGraphicsItem * = 0);

#if defined(QFX_RENDER_QPAINTER)
    void paint(QPainter &);
    void paintChild(QPainter &, QSimpleCanvasItem *);
    QRect setupPainting(int version, const QRect &bounding);
#else
    struct GLPaintParameters
    {
        QRect sceneRect;
        QRectF boundingRect;
        QRect clipRect;
#if defined(QFX_RENDER_OPENGL2)
        uchar stencilValue;
#endif
        float opacity;
        bool forceParamRefresh;

        QSimpleCanvasItem::GLPainter *painter;
    };
#if defined(QFX_RENDER_OPENGL2)
    QRectF setupPainting(int version, int &z, QSimpleCanvasItem **);
#elif defined(QFX_RENDER_OPENGL1)
    QRectF setupPainting(int version, const QRect &bounding, unsigned int *zero);
#endif
    void setupChildState(QSimpleCanvasItem *);

    void paint(GLPaintParameters &, QSimpleCanvasFilter::Layer = QSimpleCanvasFilter::All);
#if defined(QFX_RENDER_OPENGL1)
    void paintNoClip(GLPaintParameters &, QSimpleCanvasFilter::Layer = QSimpleCanvasFilter::All);
#endif
    void paintChild(const GLPaintParameters &, QSimpleCanvasItem *);
    void simplePaintChild(const GLPaintParameters &, QSimpleCanvasItem *);

    inline GLBasicShaders *basicShaders() const;

    QSimpleCanvas::Matrix localTransform() const;

#endif

    QSimpleCanvasItem *nextOpaque;

    void zOrderChildren();
    bool freshenNeeded() const;
    void doFreshenTransforms() const;

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
