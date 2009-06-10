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

#include "qwidget.h"
#include "qmutex.h"
#include "qdebug.h"
#include "qcoreapplication.h"
#include "qsimplecanvasitem.h"
#include "qsimplecanvasitem_p.h"
#include "qsimplecanvas_p.h"
#include "qtimer.h"
#include "qdatetime.h"
#include "qgraphicssceneevent.h"
#if defined(QFX_RENDER_OPENGL)
#include <glheaders.h>
#endif
#include "qboxlayout.h"
#include "qsimplecanvasdebugplugin_p.h"
#include "qsimplecanvas.h"


QT_BEGIN_NAMESPACE
DEFINE_BOOL_CONFIG_OPTION(fullUpdate, GFX_CANVAS_FULL_UPDATE);
DEFINE_BOOL_CONFIG_OPTION(continuousUpdate, GFX_CANVAS_CONTINUOUS_UPDATE);
DEFINE_BOOL_CONFIG_OPTION(useSimpleCanvas, QFX_USE_SIMPLECANVAS);

template<class T, int s = 60>
class CircularList
{
public:
    CircularList()
        : _first(0), _size(0) {}

    void append(const T &t)
    {
        int entry = (_first + _size) % s;
        _array[entry] = t;
        if (_size == s)
            _first = (_first + 1) % s;
        else
            _size++;
    }

    int size() const
    {
        return _size;
    }

    T &operator[](int idx)
    {
        Q_ASSERT(idx < _size);
        int entry = (_first + idx) % s;
        return _array[entry];
    }

    void clear()
    {
        _first = 0;
        _size = 0;
    }
private:
    int _first;
    int _size;
    T _array[s];
};


class QSimpleCanvasRootLayer : public QSimpleCanvasLayer
{
public:
    QSimpleCanvasRootLayer(QSimpleCanvas *);
    virtual void addDirty(QSimpleCanvasItem *);
    virtual void remDirty(QSimpleCanvasItem *);

private:
    friend class QSimpleCanvasItem;
    QSimpleCanvas *_canvas;
};

void QSimpleCanvasRootLayer::addDirty(QSimpleCanvasItem *i)
{
    _canvas->addDirty(i);
}

void QSimpleCanvasRootLayer::remDirty(QSimpleCanvasItem *i)
{
    _canvas->remDirty(i);
}

void QSimpleCanvasPrivate::clearFocusPanel(QSimpleCanvasItem *panel)
{
    if (q->activeFocusPanel() == panel) {
        focusPanels.pop();

        switchToFocusPanel(q->activeFocusPanel(), panel, Qt::OtherFocusReason);
        panel->activePanelOutEvent();
    } else {
        for (int ii = 0; ii < focusPanels.count(); ++ii)
            if (focusPanels.at(ii) == panel) {
                focusPanels.remove(ii);
                break;
            }
    }
}

void QSimpleCanvasPrivate::switchToFocusPanel(QSimpleCanvasItem *panel, QSimpleCanvasItem *wasPanel, Qt::FocusReason focusReason)
{
    if (panel)
        panel->activePanelInEvent();

    QSimpleCanvasItem *wasFocusRoot = focusPanelData.value(wasPanel);
    if (wasFocusRoot)
        clearActiveFocusItem(wasFocusRoot, focusReason);

    QSimpleCanvasItem *newFocusRoot = focusPanelData.value(panel);
    if (newFocusRoot)
        setFocusItem(newFocusRoot, focusReason);
}

void QSimpleCanvasPrivate::setActiveFocusPanel(QSimpleCanvasItem *panel, Qt::FocusReason focusReason)
{
    if (q->activeFocusPanel() == panel)
        return;

    if (panel) {
        for (int ii = 0; ii < focusPanels.count(); ++ii)
            if (focusPanels.at(ii) == panel) {
                focusPanels.remove(ii);
                break;
            }
    }
    QSimpleCanvasItem *old = q->activeFocusPanel();
    if (panel)
        focusPanels << panel;
    switchToFocusPanel(panel, old, focusReason);
    if (old)
        old->activePanelOutEvent();
}

void QSimpleCanvasPrivate::clearActiveFocusItem(QSimpleCanvasItem *item, 
                                            Qt::FocusReason focusReason)
{
    if (!item || !item->d_func())
        return;

    if (!item->d_func()->hasActiveFocus)
        return;

    item->d_func()->hasActiveFocus = false;
    if (item->options() & QSimpleCanvasItem::IsFocusRealm) {
        QSimpleCanvasItem *newItem = focusPanelData.value(item);
        if (newItem) {
            clearActiveFocusItem(newItem, focusReason);
        } else {
            focusItem = 0;
            QFocusEvent event(QEvent::FocusOut, focusReason);
            item->focusOutEvent(&event);
        }
    } else {
        focusItem = 0;
        QFocusEvent event(QEvent::FocusOut, focusReason);
        item->focusOutEvent(&event);
    }

    if (item->options() & QSimpleCanvasItem::AcceptsInputMethods){
        if (q->testAttribute(Qt::WA_InputMethodEnabled))
            q->setAttribute(Qt::WA_InputMethodEnabled,false);
    }
    item->activeFocusChanged(true);
}

void QSimpleCanvasPrivate::setActiveFocusItem(QSimpleCanvasItem *item, 
                                          Qt::FocusReason focusReason)
{
    while(true) {
        item->d_func()->setActiveFocus(true);
        item->activeFocusChanged(true);
        if (item->options() & QSimpleCanvasItem::IsFocusRealm) {
            QSimpleCanvasItem *newItem = focusPanelData.value(item);
            if (newItem)
                item = newItem;
            else
                break;
        } else {
            break;
        }
    }

    if (item->options() & QSimpleCanvasItem::AcceptsInputMethods){
        if (!q->testAttribute(Qt::WA_InputMethodEnabled))
            q->setAttribute(Qt::WA_InputMethodEnabled,true);
    }
    focusItem = item;
    QFocusEvent event(QEvent::FocusIn, focusReason);
    focusItem->focusInEvent(&event);
}

void QSimpleCanvasPrivate::clearFocusItem(QSimpleCanvasItem *item)
{
    // XXX
#if 0
    while(item->focusProxy())
        item = item->focusProxy();
#endif

    QSimpleCanvasItem *scope = 0;
    QSimpleCanvasItem *citem = item;
    while(citem && !scope) {
        if (citem->options() & QSimpleCanvasItem::IsFocusPanel) 
            scope = citem;
        else if (citem != item && citem->options() & QSimpleCanvasItem::IsFocusRealm)
            scope = citem;
        citem = citem->parent();
    }
    Q_ASSERT(scope); // At the very least we'll find the canvas root

   bool isActive = false;

   if (scope->options() & QSimpleCanvasItem::IsFocusPanel) 
       isActive = (scope == q->activeFocusPanel());
   else if (scope->options() & QSimpleCanvasItem::IsFocusRealm)
       isActive = scope->hasActiveFocus();

   if (isActive) clearActiveFocusItem(item, Qt::OtherFocusReason);
   item->d_func()->setFocus(false);
   item->focusChanged(false);

   focusPanelData.insert(scope, 0);

   if (lastFocusItem == item)
       lastFocusItem = 0;
   if (focusItem == item)
       focusItem = 0;

   if (scope->options() & QSimpleCanvasItem::IsFocusRealm && scope->hasActiveFocus()) {
       setActiveFocusItem(scope, Qt::OtherFocusReason);
   } else {
       QSimpleCanvasItem *item = QSimpleCanvasItem::findNextFocus(scope);
       if (item)
           item->setFocus(true);
   }
}

void QSimpleCanvasPrivate::setFocusItem(QSimpleCanvasItem *item, 
                                    Qt::FocusReason focusReason,
                                    bool overwrite)
{
    Q_ASSERT(item);

    // XXX
#if 0
    while(item->focusProxy())
        item = item->focusProxy();
#endif

    if (item == focusItem)
        return;

    QSimpleCanvasItem *scope = 0;
    QSimpleCanvasItem *citem = item;
    while(citem && !scope) {
        if (citem->options() & QSimpleCanvasItem::IsFocusPanel) 
            scope = citem;
        else if (citem != item && citem->options() & QSimpleCanvasItem::IsFocusRealm)
            scope = citem;
        citem = citem->parent();
    }
    Q_ASSERT(scope); // At the very least we'll find the canvas root

   if (!overwrite && focusPanelData.contains(scope)) {
       item->d_func()->setFocus(false);
       item->focusChanged(false);
       return;
   }

   QSimpleCanvasItem *oldFocus = focusPanelData.value(scope);
   bool isActive = false;

   if (scope->options() & QSimpleCanvasItem::IsFocusPanel) 
       isActive = (scope == q->activeFocusPanel());
   else if (scope->options() & QSimpleCanvasItem::IsFocusRealm)
       isActive = scope->hasActiveFocus();

   if (oldFocus) {
       if (isActive) clearActiveFocusItem(oldFocus, focusReason);
       oldFocus->d_func()->setFocus(false);
       oldFocus->focusChanged(false);
   }

   focusPanelData.insert(scope, item);

   if (isActive)
       lastFocusItem = item;

   if (item) {
       item->d_func()->setFocus(true);
       item->focusChanged(true);
       if (isActive)
           setActiveFocusItem(item, focusReason);
   }
}


bool QSimpleCanvas::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
    switch (event->type()) {
        case QEvent::GraphicsSceneMouseMove: {
            QGraphicsSceneMouseEvent *me = (QGraphicsSceneMouseEvent*)event;
            if (!me->buttons())
                break;
        }
        case QEvent::GraphicsSceneMousePress:
        case QEvent::GraphicsSceneMouseRelease: {
            //same logic as filter() function
            for (int ii = 0; ii < d->mouseFilters.count(); ++ii) {
                if (d->mouseFilters.at(ii)->mouseFilter((QGraphicsSceneMouseEvent*)event))
                    return true;
            }
            break;
        }
        default:
            break;
    }
    return false;
}

void QSimpleCanvasPrivate::installMouseFilter(QSimpleCanvasItem *item)
{
    mouseFilters << item;
}

void QSimpleCanvasPrivate::removeMouseFilter(QSimpleCanvasItem *item)
{
    mouseFilters.removeAll(item);
}

bool QSimpleCanvasPrivate::filter(QMouseEvent *e)
{
    if (mouseFilters.isEmpty())
        return false;

    QGraphicsSceneMouseEvent *me = mouseEventToSceneMouseEvent(e, e->pos());
    for (int ii = 0; ii < mouseFilters.count(); ++ii) {
        if (mouseFilters.at(ii)->mouseFilter(me)) {
            delete me;
            return true;
        }
    }
    delete me;
    return false;
}

QGraphicsSceneMouseEvent *QSimpleCanvasPrivate::mouseEventToSceneMouseEvent(QMouseEvent *e, const QPoint &item)
{
    QEvent::Type t;
    switch(e->type()) {
    default:
    case QEvent::MouseButtonPress:
        t = QEvent::GraphicsSceneMousePress;
        break;
    case QEvent::MouseButtonRelease:
        t = QEvent::GraphicsSceneMouseRelease;
        break;
    case QEvent::MouseMove:
        t = QEvent::GraphicsSceneMouseMove;
        break;
    case QEvent::MouseButtonDblClick:
        t = QGraphicsSceneEvent::GraphicsSceneMouseDoubleClick;
        break;
    }

    QGraphicsSceneMouseEvent *me = new QGraphicsSceneMouseEvent(t);
    me->setButton(e->button());
    me->setButtons(e->buttons());
    me->setModifiers(e->modifiers());
    me->setPos(item);
    me->setScreenPos(e->pos());
    me->setScenePos(e->pos());
    return me;
}

bool QSimpleCanvasPrivate::deliverMousePress(QSimpleCanvasItem *base, QMouseEvent *e, bool seenChildFilter)
{
    if (base->clipType()) {
        QRectF br = base->boundingRect();
        QPointF pos = base->mapFromScene(e->pos());
        if (!br.contains(pos.toPoint()))
            return false;
    }

    const QList<QSimpleCanvasItem *> &children = base->d_func()->children;

    if (base->options() & QSimpleCanvasItem::ChildMouseFilter) 
        seenChildFilter = true;

    for (int ii = children.count() - 1; ii >= 0; --ii) {
        if (children.at(ii)->visible() != 0.)
            if (deliverMousePress(children.at(ii), e, seenChildFilter))
                return true;
    }

    if (base->acceptedMouseButtons() & e->button() || base->options() & QSimpleCanvasItem::ChildMouseFilter) {

        QRectF br = base->boundingRect();
        QPoint pos = base->mapFromScene(e->pos()).toPoint();

        if (br.contains(pos)) {
            QGraphicsSceneMouseEvent *me = mouseEventToSceneMouseEvent(e, pos);

            sendMouseEvent(base, me);
            bool isAccepted = me->isAccepted();
            delete me;
            if (isAccepted) {
                lastMouseItem = base;
                return true;
            }
        }
    }
    return false;
}

// Delivers e to item
void QSimpleCanvasPrivate::sendMouseEvent(QSimpleCanvasItem *item, QGraphicsSceneMouseEvent *e)
{
    QSimpleCanvasItem *p = item->parent();
    while(p) {
        if (p->options() & QSimpleCanvasItem::ChildMouseFilter) {
            if (p->mouseFilter(e))
                return;
        }
        p = p->parent();
    }
    switch(e->type()) {
    case QEvent::GraphicsSceneMousePress:
        item->mousePressEvent(e);
        break;
    case QEvent::GraphicsSceneMouseRelease:
        item->mouseReleaseEvent(e);
        break;
    case QEvent::GraphicsSceneMouseMove:
        item->mouseMoveEvent(e);
        break;
    case QEvent::GraphicsSceneMouseDoubleClick:
        item->mouseDoubleClickEvent(e);
        break;
    default:
        break;
    }
}


QSimpleCanvasRootLayer::QSimpleCanvasRootLayer(QSimpleCanvas *c)
: _canvas(c)
{
    QSimpleCanvasItem::d_func()->canvas = c;
    setOptions(IsFocusPanel);
    update();
}


struct QSimpleCanvasTiming
{
    QSimpleCanvasTiming()
        : time(-1), paintTime(-1), timeBetweenFrames(-1) {}
    QSimpleCanvasTiming(const QRegion &_r, int _time, int _paintTime, 
           int _timeBetweenFrames)
        : region(_r), time(_time), paintTime(_paintTime), 
          timeBetweenFrames(_timeBetweenFrames) {}
    QSimpleCanvasTiming(const QSimpleCanvasTiming &other)
        : region(other.region), time(other.time), paintTime(other.paintTime),
          timeBetweenFrames(other.timeBetweenFrames) {}
    QSimpleCanvasTiming &operator=(const QSimpleCanvasTiming &other) {
        region = other.region; time = other.time; paintTime = other.paintTime;
        timeBetweenFrames = other.timeBetweenFrames; return *this;
    }
    QRegion region;
    int time;
    int paintTime;
    int timeBetweenFrames;
};

// XXX
static CircularList<QSimpleCanvasTiming> gfxCanvasTiming;

void QSimpleCanvasGraphicsView::paintEvent(QPaintEvent *pe)
{
    QRegion r = pe->region();
    int tbf = canvas->frameTimer.restart();

    canvas->lrpTimer.start();
    QGraphicsView::paintEvent(pe);
    canvas->lrpTime = canvas->lrpTimer.elapsed();

    int frametimer = canvas->frameTimer.elapsed();
    gfxCanvasTiming.append(QSimpleCanvasTiming(r, frametimer, canvas->lrpTime, tbf));
    canvas->lrpTime = 0;
    if (canvas->debugPlugin)
        canvas->debugPlugin->addTiming(canvas->lrpTime, frametimer, tbf);
}

void QSimpleCanvasGraphicsView::focusInEvent(QFocusEvent *)
{
}

/*!
  \internal
    \class QSimpleCanvas
    \brief The QSimpleCanvas class implements the canvas used by Qt Declarative
 */

QSimpleCanvas::QSimpleCanvas(CanvasMode mode, QWidget *parent)
: QWidget(parent), d(new QSimpleCanvasPrivate(this))
{
    d->init(mode);
}

QSimpleCanvas::QSimpleCanvas(QWidget *parent)
: QWidget(parent), d(new QSimpleCanvasPrivate(this))
{
    d->init(useSimpleCanvas()?SimpleCanvas:GraphicsView);
}

void QSimpleCanvasPrivate::init(QSimpleCanvas::CanvasMode mode)
{
    this->mode = mode;

    if (mode == QSimpleCanvas::SimpleCanvas)
        qWarning("QSimpleCanvas: Using simple canvas");
    else
        qWarning("QSimpleCanvas: Using GraphicsView canvas");

    if (fullUpdate())
        qWarning("QSimpleCanvas: Full update enabled");
    if (continuousUpdate())
        qWarning("QSimpleCanvas: Continuous update enabled");

    if (QmlDebugServerPlugin::isDebuggingEnabled()) {
        debugPlugin = new QSimpleCanvasDebugPlugin(q);
        new QSimpleCanvasSceneDebugPlugin(q);
    }

    root = new QSimpleCanvasRootLayer(q);
    root->setActiveFocusPanel(true);
    q->setFocusPolicy(Qt::StrongFocus);

    if (mode == QSimpleCanvas::GraphicsView) {
        view = new QSimpleCanvasGraphicsView(this);
        QHBoxLayout *layout = new QHBoxLayout(q); 
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);
        q->setLayout(layout);
        layout->addWidget(view);
        view->setOptimizationFlags(QGraphicsView::DontSavePainterState);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setFrameStyle(0);
        static_cast<QSimpleCanvasItemPrivate*>(root->d_ptr)->convertToGraphicsItem();
        view->scene()->addItem(static_cast<QSimpleCanvasItemPrivate*>(root->d_ptr)->graphicsItem);

        // These seem to give the best performance
        view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
        view->scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
        view->viewport()->setFocusPolicy(Qt::NoFocus);
    }

#if defined(QFX_RENDER_OPENGL) && defined(Q_WS_X11)
    QTimer *t = new QTimer(q);
    t->setInterval(200);
    QObject::connect(t, SIGNAL(timeout()), &egl, SLOT(updateGL()));
    t->start();
#endif
}

QSimpleCanvas::~QSimpleCanvas()
{
    delete d->root;
    delete d;
}

void QSimpleCanvasPrivate::paint(QPainter &p)
{
#if defined(QFX_RENDER_QPAINTER)
    if (!isSetup) {
        ++paintVersion;
        root->d_func()->setupPainting(0, q->rect());
    }

    lrpTimer.start();

    root->d_func()->paint(p);

    lrpTime = lrpTimer.elapsed();
#else
    Q_UNUSED(p);
#endif
}

QSimpleCanvas::CanvasMode QSimpleCanvas::canvasMode() const
{
    return d->mode;
}

QSimpleCanvasItem *QSimpleCanvas::root()
{
    return d->root;
}

void QSimpleCanvas::keyPressEvent(QKeyEvent *event)
{
    if (d->focusItem)
        d->focusItem->keyPressEvent(event);
    QWidget::keyPressEvent(event);
}

void QSimpleCanvas::keyReleaseEvent(QKeyEvent *event)
{
    if (d->focusItem)
        d->focusItem->keyReleaseEvent(event);
    QWidget::keyReleaseEvent(event);
}

void QSimpleCanvas::inputMethodEvent(QInputMethodEvent *event)
{
    if (d->focusItem)
        d->focusItem->inputMethodEvent(event);
    else
        QWidget::inputMethodEvent(event);
}

QVariant QSimpleCanvas::inputMethodQuery(Qt::InputMethodQuery query) const
{
    if (d->focusItem)
        return d->focusItem->inputMethodQuery(query);
    return QWidget::inputMethodQuery(query);
}

void QSimpleCanvas::mousePressEvent(QMouseEvent *e)
{
    if (d->isSimpleCanvas() && 
       (d->filter(e) || d->deliverMousePress(d->root, e))) {
        e->accept();
    } else {
        QWidget::mousePressEvent(e);
    }
}

void QSimpleCanvas::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (d->isSimpleCanvas() && 
       (d->filter(e) || d->deliverMousePress(d->root, e))) {
        e->accept();
    } else {
        QWidget::mouseDoubleClickEvent(e);
    }
}

void QSimpleCanvas::mouseMoveEvent(QMouseEvent *e)
{
    if (d->isSimpleCanvas() && d->filter(e)) {
        e->accept();
    } else if (d->isSimpleCanvas() && d->lastMouseItem) {
        QPoint p = d->lastMouseItem->mapFromScene(e->pos()).toPoint();
        QGraphicsSceneMouseEvent *me = d->mouseEventToSceneMouseEvent(e, p);
        d->sendMouseEvent(d->lastMouseItem, me);
        e->setAccepted(me->isAccepted());
        delete me;
    } else {
        QWidget::mouseMoveEvent(e);
    }
}

void QSimpleCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    if (d->isSimpleCanvas() && d->filter(e)) {
        e->accept();
    } else if (d->isSimpleCanvas() && d->lastMouseItem) {
        QPoint p = d->lastMouseItem->mapFromScene(e->pos()).toPoint();
        QGraphicsSceneMouseEvent *me = d->mouseEventToSceneMouseEvent(e, p);
        d->sendMouseEvent(d->lastMouseItem, me);
        d->lastMouseItem->mouseUngrabEvent();
        e->setAccepted(me->isAccepted());
        delete me;
        d->lastMouseItem = 0;
    } else {
        QWidget::mouseReleaseEvent(e);
    }
}

void QSimpleCanvas::focusInEvent(QFocusEvent *event)
{
    // XXX
#if 0
    if (d->lastFocusItem && event->reason() == Qt::ActiveWindowFocusReason) {
        d->setFocusItem(d->lastFocusItem, event->reason());
    } else {
        QSimpleCanvasItem *panel = activeFocusPanel();
        QSimpleCanvasItem *focusItem = 0;
        if (panel->isFocusable())
            focusItem = panel;
        else
            focusItem = QSimpleCanvasItem::findNextFocus(panel);

        if (focusItem)
            d->setFocusItem(focusItem, event->reason());
        else
            QWidget::focusNextPrevChild(true);
    }
#endif
    QWidget::focusInEvent(event);
}

void QSimpleCanvas::focusOutEvent(QFocusEvent *event)
{
    // XXX
#if 0
    if (event->reason() == Qt::ActiveWindowFocusReason) {
        d->lastFocusItem = activeFocusPanel();
        d->setActiveFocusPanel(0, Qt::ActiveWindowFocusReason);
    }
#endif
    QWidget::focusOutEvent(event);
}

bool QSimpleCanvas::focusNextPrevChild(bool)
{
    // XXX
#if 0
    if (d->focusItem) {
        QSimpleCanvasItem *item = next ? QSimpleCanvasItem::findNextFocus(d->focusItem) : QSimpleCanvasItem::findPrevFocus(d->focusItem);
        if (item) {
            d->setFocusItem(item, 
                            next ? Qt::TabFocusReason : Qt::BacktabFocusReason);
            return true;
        }
    }

    QSimpleCanvasItem *panel = activeFocusPanel();
    QSimpleCanvasItem *item = 0;
    if (panel->isFocusable())
        item = panel;
    else
        item = next ? QSimpleCanvasItem::findNextFocus(panel) : QSimpleCanvasItem::findPrevFocus(panel);
    if (item && item != d->focusItem) {
        d->setFocusItem(item, 
                        next ? Qt::TabFocusReason : Qt::BacktabFocusReason);
        return true;
    }

    if (d->focusItem) 
        d->setActiveFocusPanel(0, next ? Qt::TabFocusReason : Qt::BacktabFocusReason);

#endif
    return false;
}


void QSimpleCanvas::showEvent(QShowEvent *e)
{
#if defined(QFX_RENDER_OPENGL)
    d->egl.resize(width(), height());
#endif
    if (d->isGraphicsView())
        d->view->setSceneRect(rect());

    QWidget::showEvent(e);
}

void QSimpleCanvas::resizeEvent(QResizeEvent *e)
{
#if defined(QFX_RENDER_OPENGL)
    d->egl.resize(width(), height());
#endif
    if (d->isGraphicsView())
        d->view->setSceneRect(rect());
    QWidget::resizeEvent(e);
}


void QSimpleCanvas::remDirty(QSimpleCanvasItem *c)
{
    d->dirtyItems.removeAll(c);
}

void QSimpleCanvas::queueUpdate()
{
    if (!d->timer)  {
        QCoreApplication::postEvent(this, new QEvent(QEvent::User));
        d->timer = 1;
    }
}

void QSimpleCanvas::addDirty(QSimpleCanvasItem *c)
{
    Q_ASSERT(d->isSimpleCanvas());
    queueUpdate();
    d->oldDirty |= c->d_func()->data()->lastPaintRect;
#if defined(QFX_RENDER_OPENGL)
    // ### Is this parent crawl going to be a problem for scenes with nots
    // of things changing?
    // Check for filters
    QSimpleCanvasItem *fi = c->parent();
    while(fi) {
        if (fi->d_func()->data()->dirty) {
            break;
        } else if (fi->filter()) {
            fi->update();
            break;
        }
        fi = fi->parent();
    }
#endif
    d->dirtyItems.append(c);
}

QRect QSimpleCanvasPrivate::dirtyItemClip() const
{
    QRect rv;
    if (isSimpleCanvas()) {
#if defined(QFX_RENDER_OPENGL)
        QRectF r;
        for (int ii = 0; ii < dirtyItems.count(); ++ii)
            r |= dirtyItems.at(ii)->d_func()->data()->lastPaintRect;
        rv = egl.map(r);
#else
        for (int ii = 0; ii < dirtyItems.count(); ++ii)
            rv |= dirtyItems.at(ii)->d_func()->data()->lastPaintRect;
#endif
    }
    return rv;
}

QRect QSimpleCanvasPrivate::resetDirty()
{
    if (isSimpleCanvas()) {
#if defined(QFX_RENDER_OPENGL)
        QRect r = egl.map(oldDirty) |  dirtyItemClip();
#else
        QRect r = oldDirty | dirtyItemClip();
#endif
        if (!r.isEmpty())
            r.adjust(-1,-1,2,2);    //make sure we get everything (since we rounded from floats to ints)
        for (int ii = 0; ii < dirtyItems.count(); ++ii)
            static_cast<QSimpleCanvasItemPrivate*>(dirtyItems.at(ii)->d_ptr)->data()->dirty = false;
        dirtyItems.clear();
        oldDirty = QRect();

        if (fullUpdate())
            return QRect();
        else
            return r;
    } else {
        return QRect();
    }
}

QSimpleCanvasItem *QSimpleCanvas::focusItem() const
{
    return d->focusItem;
}

QSimpleCanvasItem *QSimpleCanvas::activeFocusPanel() const
{
    if (d->focusPanels.isEmpty())
        return 0;
    else
        return d->focusPanels.top();
}

QSimpleCanvasItem *QSimpleCanvas::focusItem(QSimpleCanvasItem *item) const
{
    while (item && d->focusPanelData.contains(item))
        item = d->focusPanelData.value(item);
    return item;
}

bool QSimpleCanvas::event(QEvent *e)
{
    if (e->type() == QEvent::User && d->isSimpleCanvas()) {
        int tbf = d->frameTimer.restart();
        d->timer = 0;
        d->isSetup = true;
#if defined(QFX_RENDER_OPENGL1)
        unsigned int zero = 0;
        d->root->d_func()->setupPainting(0, rect(), &zero);
#elif defined(QFX_RENDER_OPENGL2)
        ++d->paintVersion;
        d->opaqueList = 0;
        int z = 0;
        d->root->d_func()->setupPainting(0, z, &d->opaqueList);
#else
        ++d->paintVersion;
        d->root->d_func()->setupPainting(0, rect());
#endif

        QRect r = d->resetDirty();

#if defined(QFX_RENDER_QPAINTER)
        if (r.isEmpty() || fullUpdate())
            repaint();
        else 
            repaint(r);
        emit framePainted();
#else 

        QRect nr(r.x(), height() - r.y() - r.height(), r.width(), r.height());

        if (r.isEmpty() || fullUpdate())
            d->egl.updateGL();
        else 
            d->egl.updateGL(nr);
        emit framePainted();
#endif
        d->isSetup = false;

        int frametimer = d->frameTimer.elapsed();
        gfxCanvasTiming.append(QSimpleCanvasTiming(r, frametimer, d->lrpTime, tbf));
        if (d->debugPlugin)
            d->debugPlugin->addTiming(d->lrpTime, frametimer, tbf);
        d->lrpTime = 0;
        if (continuousUpdate())
            queueUpdate();

        return true;
    }

    if (e->type() == QEvent::ShortcutOverride) {
        if (QSimpleCanvasItem *focus = focusItem())
            return focus->event(e);
    }

    return QWidget::event(e);
}

void QSimpleCanvas::paintEvent(QPaintEvent *)
{
#if defined(QFX_RENDER_QPAINTER)
    if (d->mode == SimpleCanvas) {
        QPainter p(this);
        d->paint(p);
    }
#endif
}

void QSimpleCanvas::dumpTiming()
{
    for (int ii = 0; ii < gfxCanvasTiming.size(); ++ii) {
        const QSimpleCanvasTiming &t = gfxCanvasTiming[ii];

        qreal repaintFps = 1000. / qreal(t.time);
        qreal paintFps = 1000. / qreal(t.paintTime);
        qreal tbfFps = 1000. / qreal(t.timeBetweenFrames);

        qWarning() << "repaint():" << t.time << "ms," << repaintFps << "fps.  paint():" << t.paintTime << "ms," << paintFps << "fps.  timeSinceLastFrame:" << t.timeBetweenFrames << "ms," << tbfFps << "fps.";
        qWarning() << t.region;
    }
    gfxCanvasTiming.clear();
}

void QSimpleCanvas::dumpItems()
{
    int items = d->root->d_func()->dump(0);
    qWarning() << "Total:" << items;
}

void QSimpleCanvas::checkState()
{
    if (d->isSimpleCanvas()) {
        QSimpleCanvasItemPrivate::FocusStateCheckRDatas r;
        if (d->root->d_func()->checkFocusState(0, &r))
            qWarning() << "State OK";
    }
}

/*!
  Returns canvas as an image. Not a fast operation.
*/
QImage QSimpleCanvas::asImage() const
{ 
    if (d->isSimpleCanvas()) {
#if defined(QFX_RENDER_QPAINTER)
        QImage img(width(),height(),QImage::Format_RGB32);
        QPainter p(&img);
        const_cast<QSimpleCanvas*>(this)->d->paint(p);
        return img;
#elif defined(QFX_RENDER_OPENGL)
        return d->egl.grabFrameBuffer();
#endif
    } else {
        QImage img(width(),height(),QImage::Format_RGB32);
        QPainter p(&img);
        d->view->render(&p);
        return img;
    }
}
QT_END_NAMESPACE
