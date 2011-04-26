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

#include "qsgcanvas.h"
#include "qsgcanvas_p.h"

#include "qsgitem.h"
#include "qsgitem_p.h"

#include <private/qsgrenderer_p.h>
#include <private/qsgflashnode_p.h>

#include <QtGui/qpainter.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qinputcontext.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qabstractanimation.h>

#include <private/qdeclarativedebugtrace_p.h>

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(qmlThreadedRenderer, QML_THREADED_RENDERER)

/*
Focus behavior
==============

Prior to being added to a valid canvas items can set and clear focus with no 
effect.  Only once items are added to a canvas (by way of having a parent set that
already belongs to a canvas) do the focus rules apply.  Focus goes back to 
having no effect if an item is removed from a canvas.

When an item is moved into a new focus scope (either being added to a canvas
for the first time, or having its parent changed), if the focus scope already has 
a scope focused item that takes precedence over the item being added.  Otherwise,
the focus of the added tree is used.  In the case of of a tree of items being 
added to a canvas for the first time, which may have a conflicted focus state (two
or more items in one scope having focus set), the same rule is applied item by item - 
thus the first item that has focus will get it (assuming the scope doesn't already 
have a scope focused item), and the other items will have their focus cleared.
*/

// #define FOCUS_DEBUG
// #define MOUSE_DEBUG
// #define TOUCH_DEBUG
// #define DIRTY_DEBUG
// #define THREAD_DEBUG

// #define FRAME_TIMING

#ifdef FRAME_TIMING
static QTime frameTimer;
int sceneGraphRenderTime;
int readbackTime;
#endif


class QSGAnimationDriver : public QAnimationDriver
{
public:
    QSGAnimationDriver(QWidget *w, QObject *parent)
        : QAnimationDriver(parent), widget(w)
    {
        Q_ASSERT(w);
    }

    void started()
    {
        widget->update();
    }

    QWidget *widget;
};

QSGItem::UpdatePaintNodeData::UpdatePaintNodeData()
: transformNode(0)
{
}

QSGRootItem::QSGRootItem()
{
}

QSGThreadedRendererAnimationDriver::QSGThreadedRendererAnimationDriver(QSGCanvasPrivate *r, QObject *parent)
    : QAnimationDriver(parent)
    , renderer(r)
{
}

void QSGThreadedRendererAnimationDriver::started()
{
#ifdef THREAD_DEBUG
    qWarning("AnimationDriver: Main Thread: started");
#endif
    renderer->mutex.lock();
    renderer->animationRunning = true;
    if (renderer->idle)
        renderer->wait.wakeOne();
    renderer->mutex.unlock();


}

void QSGThreadedRendererAnimationDriver::stopped()
{
#ifdef THREAD_DEBUG
    qWarning("AnimationDriver: Main Thread: stopped");
#endif
    renderer->mutex.lock();
    renderer->animationRunning = false;
    renderer->mutex.unlock();
}

void QSGCanvas::paintEvent(QPaintEvent *)
{
    Q_D(QSGCanvas);

    if (!d->threadedRendering) {
#ifdef FRAME_TIMING
        int lastFrame = frameTimer.restart();
#endif

        if (d->animationDriver->isRunning())
            d->animationDriver->advance();

#ifdef FRAME_TIMING
        int animationTime = frameTimer.elapsed();
#endif

        Q_ASSERT(d->context);

        d->polishItems();

        QDeclarativeDebugTrace::addEvent(QDeclarativeDebugTrace::FramePaint);
        QDeclarativeDebugTrace::startRange(QDeclarativeDebugTrace::Painting);

#ifdef FRAME_TIMING
        int polishTime = frameTimer.elapsed();
#endif

        makeCurrent();

#ifdef FRAME_TIMING
        int makecurrentTime = frameTimer.elapsed();
#endif

        d->syncSceneGraph();

#ifdef FRAME_TIMING
        int syncTime = frameTimer.elapsed();
#endif

        d->renderSceneGraph();

#ifdef FRAME_TIMING
        printf("FrameTimes, last=%d, animations=%d, polish=%d, makeCurrent=%d, sync=%d, sgrender=%d, readback=%d, total=%d\n",
               lastFrame,
               animationTime,
               polishTime - animationTime,
               makecurrentTime - polishTime,
               syncTime - makecurrentTime,
               sceneGraphRenderTime - syncTime,
               readbackTime - sceneGraphRenderTime,
               frameTimer.elapsed());
#endif

        QDeclarativeDebugTrace::endRange(QDeclarativeDebugTrace::Painting);

        if (d->animationDriver->isRunning())
            update();
    }
}

void QSGCanvas::resizeEvent(QResizeEvent *e)
{
    Q_D(QSGCanvas);
    if (d->threadedRendering) {
        d->mutex.lock();
        QGLWidget::resizeEvent(e);
        d->widgetSize = e->size();
        d->mutex.unlock();
    } else {
        d->widgetSize = e->size();
        d->viewportSize = d->widgetSize;
        QGLWidget::resizeEvent(e);
    }
}

void QSGCanvas::showEvent(QShowEvent *e)
{
    Q_D(QSGCanvas);

    QGLWidget::showEvent(e);

    if (d->threadedRendering) {
        d->contextInThread = true;
        doneCurrent();
        if (!d->animationDriver)
            d->animationDriver = new QSGThreadedRendererAnimationDriver(d, this);
        d->animationDriver->install();
        d->mutex.lock();
        d->thread->start();
        d->wait.wait(&d->mutex);
        d->mutex.unlock();
    } else {
        makeCurrent();

        if (!d->context || !d->context->isReady()) {
            d->initializeSceneGraph();
            d->animationDriver = new QSGAnimationDriver(this, this);
        }

        d->animationDriver->install();
    }
}

void QSGCanvas::hideEvent(QHideEvent *e)
{
    Q_D(QSGCanvas);

    if (d->threadedRendering) {
        d->mutex.lock();
        d->exitThread = true;
        d->wait.wakeOne();
        d->wait.wait(&d->mutex);
        d->exitThread = false;
        d->mutex.unlock();
        d->thread->wait();
    }

    d->animationDriver->uninstall();

    QGLWidget::hideEvent(e);
}


void QSGCanvasPrivate::initializeSceneGraph()
{
    if (!context)
        context = QSGContext::createDefaultContext();

    if (context->isReady())
        return;

    QGLContext *glctx = const_cast<QGLContext *>(QGLContext::currentContext());
    context->initialize(glctx);

    if (!threadedRendering) {
        Q_Q(QSGCanvas);
        QObject::connect(context->renderer(), SIGNAL(sceneGraphChanged()), q, SLOT(maybeUpdate()),
                         Qt::DirectConnection);
    }

    if (!QSGItemPrivate::get(rootItem)->itemNode()->parent()) {
        context->rootNode()->appendChildNode(QSGItemPrivate::get(rootItem)->itemNode());
    }

    emit q_func()->sceneGraphInitialized();
}

void QSGCanvasPrivate::polishItems()
{
    while (!itemsToPolish.isEmpty()) {
        QSet<QSGItem *>::Iterator iter = itemsToPolish.begin();
        QSGItem *item = *iter;
        itemsToPolish.erase(iter);
        QSGItemPrivate::get(item)->polishScheduled = false;
        item->updatePolish();
    }
}


void QSGCanvasPrivate::syncSceneGraph()
{
    updateDirtyNodes();
}


void QSGCanvasPrivate::renderSceneGraph()
{
    QGLContext *glctx = const_cast<QGLContext *>(QGLContext::currentContext());

    context->renderer()->setDeviceRect(QRect(QPoint(0, 0), viewportSize));
    context->renderer()->setViewportRect(QRect(QPoint(0, 0), viewportSize));
    context->renderer()->setProjectMatrixToDeviceRect();

    context->renderNextFrame();

#ifdef FRAME_TIMING
    sceneGraphRenderTime = frameTimer.elapsed();
#endif


#ifdef FRAME_TIMING
    int pixel;
    glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
    readbackTime = frameTimer.elapsed();
#endif

    glctx->swapBuffers();
}


void QSGCanvas::sceneGraphChanged()
{
    Q_D(QSGCanvas);
    d->needsRepaint = true;
}


void QSGCanvasPrivate::runThread()
{
#ifdef THREAD_DEBUG
    qWarning("QSGRenderer: Render thread running");
#endif
    Q_Q(QSGCanvas);

    printf("QSGCanvas::runThread(), rendering in a thread...\n");

    q->makeCurrent();
    initializeSceneGraph();

    QObject::connect(context->renderer(), SIGNAL(sceneGraphChanged()),
                      q, SLOT(sceneGraphChanged()),
                      Qt::DirectConnection);

    mutex.lock();
    wait.wakeOne(); // Wake the main thread waiting for us to start

    while (true) {
        QSize s;
        s = widgetSize;

        if (exitThread)
            break;

        if (s != viewportSize) {
            glViewport(0, 0, s.width(), s.height());
            viewportSize = s;
        }

#ifdef THREAD_DEBUG
        qWarning("QSGRenderer: Render Thread: Waiting for main thread to stop");
#endif
        QCoreApplication::postEvent(q, new QEvent(QEvent::User));
        wait.wait(&mutex);

        if (exitThread) {
#ifdef THREAD_DEBUG
            qWarning("QSGRenderer: Render Thread: Shutting down...");
#endif
            break;
        }

#ifdef THREAD_DEBUG
        qWarning("QSGRenderer: Render Thread: Main thread has stopped, syncing scene");
#endif

        // Do processing while main thread is frozen
        syncSceneGraph();

#ifdef THREAD_DEBUG
        qWarning("QSGRenderer: Render Thread: Resuming main thread");
#endif

        // Read animationRunning while inside the locked section
        bool continous = animationRunning;

        wait.wakeOne();
        mutex.unlock();

        bool enterIdle = false;
        if (needsRepaint) {
#ifdef THREAD_DEBUG
            qWarning("QSGRenderer: Render Thread: rendering scene");
#endif
            renderSceneGraph();
            needsRepaint = false;
        } else if (continous) {
#ifdef THREAD_DEBUG
            qWarning("QSGRenderer: Render Thread: waiting a while...");
#endif
            MyThread::doWait();
        } else {
            enterIdle = true;
        }

        mutex.lock();

        if (enterIdle) {
#ifdef THREAD_DEBUG
            qWarning("QSGRenderer: Render Thread: Nothing has changed, going idle...");
#endif
            idle = true;
            wait.wait(&mutex);
            idle = false;
#ifdef THREAD_DEBUG
            qWarning("QSGRenderer: Render Thread: waking up from idle");
#endif
        }

    }


#ifdef THREAD_DEBUG
    qWarning("QSGRenderer: Render Thread: shutting down, waking up main thread");
#endif
    wait.wakeOne();
    mutex.unlock();

    q->doneCurrent();
}

QSGCanvasPrivate::QSGCanvasPrivate()
    : rootItem(0)
    , activeFocusItem(0)
    , mouseGrabberItem(0)
    , hoverItem(0)
    , dirtyItemList(0)
    , context(0)
    , contextInThread(false)
    , threadedRendering(false)
    , exitThread(false)
    , animationRunning(false)
    , idle(false)
    , needsRepaint(true)
    , renderThreadAwakened(false)
    , thread(new MyThread(this))
    , animationDriver(0)
{
    threadedRendering = qmlThreadedRenderer();
}

QSGCanvasPrivate::~QSGCanvasPrivate()
{
}

void QSGCanvasPrivate::init(QSGCanvas *c)
{
    q_ptr = c;

    Q_Q(QSGCanvas);

    q->setAttribute(Qt::WA_AcceptTouchEvents);
    q->setFocusPolicy(Qt::StrongFocus);

    rootItem = new QSGRootItem;
    QSGItemPrivate *rootItemPrivate = QSGItemPrivate::get(rootItem);
    rootItemPrivate->canvas = q;
    rootItemPrivate->flags |= QSGItem::ItemIsFocusScope;
    rootItemPrivate->focus = true;
    rootItemPrivate->activeFocus = true;
    activeFocusItem = rootItem;

    context = QSGContext::createDefaultContext();
}

void QSGCanvasPrivate::sceneMouseEventForTransform(QGraphicsSceneMouseEvent &sceneEvent,
                                                   const QTransform &transform)
{
    sceneEvent.setPos(transform.map(sceneEvent.scenePos()));
    sceneEvent.setLastPos(transform.map(sceneEvent.lastScenePos()));
    for (int ii = 0; ii < 5; ++ii) {
        if (sceneEvent.buttons() & (1 << ii)) {
            sceneEvent.setButtonDownPos((Qt::MouseButton)(1 << ii), 
                                        transform.map(sceneEvent.buttonDownScenePos((Qt::MouseButton)(1 << ii))));
        }
    }
}

void QSGCanvasPrivate::transformTouchPoints(QList<QTouchEvent::TouchPoint> &touchPoints, const QTransform &transform)
{
    for (int i=0; i<touchPoints.count(); i++) {
        QTouchEvent::TouchPoint &touchPoint = touchPoints[i];
        touchPoint.setRect(transform.mapRect(touchPoint.sceneRect()));
        touchPoint.setStartPos(transform.map(touchPoint.startScenePos()));
        touchPoint.setLastPos(transform.map(touchPoint.lastScenePos()));
    }
}

QEvent::Type QSGCanvasPrivate::sceneMouseEventTypeFromMouseEvent(QMouseEvent *event)
{
    switch(event->type()) {
    default:
        Q_ASSERT(!"Unknown event type");
    case QEvent::MouseButtonPress:
        return QEvent::GraphicsSceneMousePress;
    case QEvent::MouseButtonRelease:
        return QEvent::GraphicsSceneMouseRelease;
    case QEvent::MouseButtonDblClick:
        return QEvent::GraphicsSceneMouseDoubleClick;
    case QEvent::MouseMove:
        return QEvent::GraphicsSceneMouseMove;
    }
}

/*!
Fill in the data in \a sceneEvent based on \a event.  This method leaves the item local positions in
\a sceneEvent untouched.  Use sceneMouseEventForTransform() to fill in those details.
*/
void QSGCanvasPrivate::sceneMouseEventFromMouseEvent(QGraphicsSceneMouseEvent &sceneEvent, QMouseEvent *event)
{
    Q_Q(QSGCanvas);

    Q_ASSERT(event);

    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick) {
        if ((event->button() & event->buttons()) == event->buttons()) {
            lastMousePosition = event->pos();
        }

        switch (event->button()) {
        default:
            Q_ASSERT(!"Unknown button");
        case Qt::LeftButton:
            buttonDownPositions[0] = event->pos();
            break;
        case Qt::RightButton:
            buttonDownPositions[1] = event->pos();
            break;
        case Qt::MiddleButton:
            buttonDownPositions[2] = event->pos();
            break;
        case Qt::XButton1:
            buttonDownPositions[3] = event->pos();
            break;
        case Qt::XButton2:
            buttonDownPositions[4] = event->pos();
            break;
        }
    }

    sceneEvent.setScenePos(event->pos());
    sceneEvent.setScreenPos(event->globalPos());
    sceneEvent.setLastScenePos(lastMousePosition);
    sceneEvent.setLastScreenPos(q->mapToGlobal(lastMousePosition));
    sceneEvent.setButtons(event->buttons());
    sceneEvent.setButton(event->button());
    sceneEvent.setModifiers(event->modifiers());
    sceneEvent.setWidget(q);

    for (int ii = 0; ii < 5; ++ii) {
        if (sceneEvent.buttons() & (1 << ii)) {
            sceneEvent.setButtonDownScenePos((Qt::MouseButton)(1 << ii), buttonDownPositions[ii]);
            sceneEvent.setButtonDownScreenPos((Qt::MouseButton)(1 << ii), q->mapToGlobal(buttonDownPositions[ii]));
        }
    }

    lastMousePosition = event->pos();
}

/*!
Fill in the data in \a hoverEvent based on \a mouseEvent.  This method leaves the item local positions in
\a hoverEvent untouched (these are filled in later).
*/
void QSGCanvasPrivate::sceneHoverEventFromMouseEvent(QGraphicsSceneHoverEvent &hoverEvent, QMouseEvent *mouseEvent)
{
    Q_Q(QSGCanvas);
    hoverEvent.setWidget(q);
    hoverEvent.setScenePos(mouseEvent->pos());
    hoverEvent.setScreenPos(mouseEvent->globalPos());
    if (lastMousePosition.isNull()) lastMousePosition = mouseEvent->pos();
    hoverEvent.setLastScenePos(lastMousePosition);
    hoverEvent.setLastScreenPos(q->mapToGlobal(lastMousePosition));
    hoverEvent.setModifiers(mouseEvent->modifiers());
    hoverEvent.setAccepted(mouseEvent->isAccepted());

    lastMousePosition = mouseEvent->pos();
}

/*!
Translates the data in \a touchEvent to this canvas.  This method leaves the item local positions in
\a touchEvent untouched (these are filled in later).
*/
void QSGCanvasPrivate::translateTouchEvent(QTouchEvent *touchEvent)
{
    Q_Q(QSGCanvas);

    touchEvent->setWidget(q);

    QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
    for (int i = 0; i < touchPoints.count(); ++i) {
        QTouchEvent::TouchPoint &touchPoint = touchPoints[i];

        touchPoint.setScreenRect(touchPoint.sceneRect());
        touchPoint.setStartScreenPos(touchPoint.startScenePos());
        touchPoint.setLastScreenPos(touchPoint.lastScenePos());

        touchPoint.setSceneRect(touchPoint.rect());
        touchPoint.setStartScenePos(touchPoint.startPos());
        touchPoint.setLastScenePos(touchPoint.lastPos());

        if (touchPoint.isPrimary())
            lastMousePosition = touchPoint.pos().toPoint();
    }
    touchEvent->setTouchPoints(touchPoints);
}

void QSGCanvasPrivate::setFocusInScope(QSGItem *scope, QSGItem *item, FocusOptions options)
{
    Q_Q(QSGCanvas);

    Q_ASSERT(item);
    Q_ASSERT(scope);

#ifdef FOCUS_DEBUG
    qWarning() << "QSGCanvasPrivate::setFocusInScope():";
    qWarning() << "    scope:" << (QObject *)scope;
    qWarning() << "    scopeSubFocusItem:" << (QObject *)QSGItemPrivate::get(scope)->subFocusItem;
    qWarning() << "    item:" << (QObject *)item;
    qWarning() << "    activeFocusItem:" << (QObject *)activeFocusItem;
#endif

    QSGItemPrivate *scopePrivate = QSGItemPrivate::get(scope);
    QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item);

    QSGItem *oldActiveFocusItem = 0;
    QSGItem *newActiveFocusItem = 0;

    QVarLengthArray<QSGItem *, 20> changed;

    // Does this change the active focus?
    if (scopePrivate->activeFocus) {
        oldActiveFocusItem = activeFocusItem;
        newActiveFocusItem = item;
        while (newActiveFocusItem->isFocusScope() && newActiveFocusItem->scopedFocusItem())
            newActiveFocusItem = newActiveFocusItem->scopedFocusItem();

        Q_ASSERT(oldActiveFocusItem);

#ifndef QT_NO_IM
        if (QInputContext *ic = inputContext())
            ic->reset();
#endif

        activeFocusItem = 0;
        QFocusEvent event(QEvent::FocusOut, Qt::OtherFocusReason);
        q->sendEvent(oldActiveFocusItem, &event);

        QSGItem *afi = oldActiveFocusItem;
        while (afi != scope) {
            if (QSGItemPrivate::get(afi)->activeFocus) {
                QSGItemPrivate::get(afi)->activeFocus = false;
                changed << afi;
            }
            afi = afi->parentItem();
        }
    }

    QSGItem *oldSubFocusItem = scopePrivate->subFocusItem;
    // Correct focus chain in scope
    if (oldSubFocusItem) {
        QSGItem *sfi = scopePrivate->subFocusItem->parentItem();
        while (sfi != scope) {
            QSGItemPrivate::get(sfi)->subFocusItem = 0;
            sfi = sfi->parentItem();
        }
    }
    {
        scopePrivate->subFocusItem = item;
        QSGItem *sfi = scopePrivate->subFocusItem->parentItem();
        while (sfi != scope) {
            QSGItemPrivate::get(sfi)->subFocusItem = item;
            sfi = sfi->parentItem();
        }
    }

    if (oldSubFocusItem) {
        QSGItemPrivate::get(oldSubFocusItem)->focus = false;
        changed << oldSubFocusItem;
    }

    if (!(options & DontChangeFocusProperty)) {
        itemPrivate->focus = true;
        changed << item;
    }

    if (newActiveFocusItem) {
        activeFocusItem = newActiveFocusItem;

        QSGItemPrivate::get(newActiveFocusItem)->activeFocus = true;
        changed << newActiveFocusItem;

        QSGItem *afi = newActiveFocusItem->parentItem();
        while (afi != scope) {
            if (afi->isFocusScope()) {
                QSGItemPrivate::get(afi)->activeFocus = true;
                changed << afi;
            }
            afi = afi->parentItem();
        }

        updateInputMethodData();

        QFocusEvent event(QEvent::FocusIn, Qt::OtherFocusReason);
        q->sendEvent(newActiveFocusItem, &event); 
    } else {
        updateInputMethodData();
    }

    if (!changed.isEmpty()) 
        notifyFocusChangesRecur(changed.data(), changed.count() - 1);
}

void QSGCanvasPrivate::clearFocusInScope(QSGItem *scope, QSGItem *item, FocusOptions options)
{
    Q_Q(QSGCanvas);

    Q_ASSERT(item);
    Q_ASSERT(scope);

#ifdef FOCUS_DEBUG
    qWarning() << "QSGCanvasPrivate::clearFocusInScope():";
    qWarning() << "    scope:" << (QObject *)scope;
    qWarning() << "    item:" << (QObject *)item;
    qWarning() << "    activeFocusItem:" << (QObject *)activeFocusItem;
#endif

    QSGItemPrivate *scopePrivate = QSGItemPrivate::get(scope);

    QSGItem *oldActiveFocusItem = 0;
    QSGItem *newActiveFocusItem = 0;

    QVarLengthArray<QSGItem *, 20> changed;

    Q_ASSERT(item == scopePrivate->subFocusItem);

    // Does this change the active focus?
    if (scopePrivate->activeFocus) {
        oldActiveFocusItem = activeFocusItem;
        newActiveFocusItem = scope;
        
        Q_ASSERT(oldActiveFocusItem);

#ifndef QT_NO_IM
        if (QInputContext *ic = inputContext())
            ic->reset();
#endif

        activeFocusItem = 0;
        QFocusEvent event(QEvent::FocusOut, Qt::OtherFocusReason);
        q->sendEvent(oldActiveFocusItem, &event);

        QSGItem *afi = oldActiveFocusItem;
        while (afi != scope) {
            if (QSGItemPrivate::get(afi)->activeFocus) {
                QSGItemPrivate::get(afi)->activeFocus = false;
                changed << afi;
            }
            afi = afi->parentItem();
        }
    }

    QSGItem *oldSubFocusItem = scopePrivate->subFocusItem;
    // Correct focus chain in scope
    if (oldSubFocusItem) {
        QSGItem *sfi = scopePrivate->subFocusItem->parentItem();
        while (sfi != scope) {
            QSGItemPrivate::get(sfi)->subFocusItem = 0;
            sfi = sfi->parentItem();
        }
    }
    scopePrivate->subFocusItem = 0;

    if (oldSubFocusItem && !(options & DontChangeFocusProperty)) {
        QSGItemPrivate::get(oldSubFocusItem)->focus = false;
        changed << oldSubFocusItem;
    }

    if (newActiveFocusItem) {
        Q_ASSERT(newActiveFocusItem == scope);
        activeFocusItem = scope;

        updateInputMethodData();

        QFocusEvent event(QEvent::FocusIn, Qt::OtherFocusReason);
        q->sendEvent(newActiveFocusItem, &event); 
    } else {
        updateInputMethodData();
    }

    if (!changed.isEmpty()) 
        notifyFocusChangesRecur(changed.data(), changed.count() - 1);
}

void QSGCanvasPrivate::notifyFocusChangesRecur(QSGItem **items, int remaining)
{
    QDeclarativeGuard<QSGItem> item(*items);

    if (remaining)
        notifyFocusChangesRecur(items + 1, remaining - 1);

    if (item) {
        QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item);

        if (itemPrivate->notifiedFocus != itemPrivate->focus) {
            itemPrivate->notifiedFocus = itemPrivate->focus;
            emit item->focusChanged(itemPrivate->focus);
        }

        if (item && itemPrivate->notifiedActiveFocus != itemPrivate->activeFocus) {
            itemPrivate->notifiedActiveFocus = itemPrivate->activeFocus;
            itemPrivate->itemChange(QSGItem::ItemActiveFocusHasChanged, itemPrivate->activeFocus);
            emit item->activeFocusChanged(itemPrivate->activeFocus);
        }
    } 
}

void QSGCanvasPrivate::updateInputMethodData()
{
    Q_Q(QSGCanvas);
    bool enabled = activeFocusItem
                   && (QSGItemPrivate::get(activeFocusItem)->flags & QSGItem::ItemAcceptsInputMethod);
    q->setAttribute(Qt::WA_InputMethodEnabled, enabled);
    q->setInputMethodHints(enabled ? activeFocusItem->inputMethodHints() : Qt::ImhNone);
}

QVariant QSGCanvas::inputMethodQuery(Qt::InputMethodQuery query) const
{
    Q_D(const QSGCanvas);
    if (!d->activeFocusItem || !(QSGItemPrivate::get(d->activeFocusItem)->flags & QSGItem::ItemAcceptsInputMethod))
        return QVariant();
    QVariant value = d->activeFocusItem->inputMethodQuery(query);

    //map geometry types
    QVariant::Type type = value.type();
    if (type == QVariant::RectF || type == QVariant::Rect) {
        const QTransform transform = QSGItemPrivate::get(d->activeFocusItem)->itemToCanvasTransform();
        value = transform.mapRect(value.toRectF());
    } else if (type == QVariant::PointF || type == QVariant::Point) {
        const QTransform transform = QSGItemPrivate::get(d->activeFocusItem)->itemToCanvasTransform();
        value = transform.map(value.toPointF());
    }
    return value;
}

void QSGCanvasPrivate::dirtyItem(QSGItem *)
{
    Q_Q(QSGCanvas);
    q->maybeUpdate();
}

void QSGCanvasPrivate::cleanup(QSGNode *n)
{
    Q_Q(QSGCanvas);

    Q_ASSERT(!cleanupNodeList.contains(n));
    cleanupNodeList.append(n);
    q->maybeUpdate();
}

static QGLFormat tweakFormat(const QGLFormat &format = QGLFormat::defaultFormat())
{
    QGLFormat f = format;
    f.setSwapInterval(1);
    return f;
}

QSGCanvas::QSGCanvas(QWidget *parent, Qt::WindowFlags f)
    : QGLWidget(*(new QSGCanvasPrivate), tweakFormat(), parent, (QGLWidget *) 0, f)
{
    Q_D(QSGCanvas);

    d->init(this);
}

QSGCanvas::QSGCanvas(const QGLFormat &format, QWidget *parent, Qt::WindowFlags f)
    : QGLWidget(*(new QSGCanvasPrivate), tweakFormat(format), parent, (QGLWidget *) 0, f)
{
    Q_D(QSGCanvas);

    d->init(this);
}

QSGCanvas::QSGCanvas(QSGCanvasPrivate &dd, QWidget *parent, Qt::WindowFlags f)
: QGLWidget(dd, tweakFormat(), parent, 0, f)
{
    Q_D(QSGCanvas);

    d->init(this);
}

QSGCanvas::QSGCanvas(QSGCanvasPrivate &dd, const QGLFormat &format, QWidget *parent, Qt::WindowFlags f)
: QGLWidget(dd, tweakFormat(format), parent, 0, f)
{
    Q_D(QSGCanvas);

    d->init(this);
}

QSGCanvas::~QSGCanvas()
{
    Q_D(QSGCanvas);

    // ### should we change ~QSGItem to handle this better?
    // manually cleanup for the root item (item destructor only handles these when an item is parented)
    QSGItemPrivate *rootItemPrivate = QSGItemPrivate::get(d->rootItem);
    rootItemPrivate->removeFromDirtyList();
    rootItemPrivate->canvas = 0;

    delete d->rootItem; d->rootItem = 0;
    d->cleanupNodes();

    delete d->context;
}

QSGItem *QSGCanvas::rootItem() const
{
    Q_D(const QSGCanvas);
    
    return d->rootItem;
}

QSGItem *QSGCanvas::activeFocusItem() const
{
    Q_D(const QSGCanvas);
    
    return d->activeFocusItem;
}

QSGItem *QSGCanvas::mouseGrabberItem() const
{
    Q_D(const QSGCanvas);
    
    return d->mouseGrabberItem;
}


void QSGCanvasPrivate::clearHover()
{
    Q_Q(QSGCanvas);
    if (!hoverItem)
        return;

    QGraphicsSceneHoverEvent hoverEvent;
    hoverEvent.setWidget(q);

    QPoint cursorPos = QCursor::pos();
    hoverEvent.setScenePos(q->mapFromGlobal(cursorPos));
    hoverEvent.setLastScenePos(hoverEvent.scenePos());
    hoverEvent.setScreenPos(cursorPos);
    hoverEvent.setLastScreenPos(hoverEvent.screenPos());

    QSGItem *item = hoverItem;
    hoverItem = 0;
    sendHoverEvent(QEvent::GraphicsSceneHoverLeave, item, &hoverEvent);
}


bool QSGCanvas::event(QEvent *e)
{
    Q_D(QSGCanvas);

    if (e->type() == QEvent::User) {
        Q_ASSERT(d->threadedRendering);

        d->mutex.lock();
#ifdef THREAD_DEBUG
        qWarning("QSGRenderer: Main Thread: Stopped");
#endif

        d->polishItems();

        d->renderThreadAwakened = false;

        d->wait.wakeOne();

        // The thread is exited when the widget has been hidden. We then need to
        // skip the waiting, otherwise we would be waiting for a wakeup that never
        // comes.
        if (d->thread->isRunning())
            d->wait.wait(&d->mutex);
#ifdef THREAD_DEBUG
        qWarning("QSGRenderer: Main Thread: Resumed");
#endif
        d->mutex.unlock();

        if (d->animationRunning)
            d->animationDriver->advance();
    }

    switch (e->type()) {

    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        QTouchEvent *touch = static_cast<QTouchEvent *>(e);
        d->translateTouchEvent(touch);
        d->deliverTouchEvent(touch);
        if (!touch->isAccepted())
            return false;
    }
    case QEvent::Leave:
        d->clearHover();
        d->lastMousePosition = QPoint();
        break;
    default:
        break;
    }

    return QGLWidget::event(e);
}

void QSGCanvas::keyPressEvent(QKeyEvent *e)
{
    Q_D(QSGCanvas);
    
    sendEvent(d->activeFocusItem, e);
}

void QSGCanvas::keyReleaseEvent(QKeyEvent *e)
{
    Q_D(QSGCanvas);
    
    sendEvent(d->activeFocusItem, e);
}

void QSGCanvas::inputMethodEvent(QInputMethodEvent *e)
{
    Q_D(QSGCanvas);

    sendEvent(d->activeFocusItem, e);
}

bool QSGCanvasPrivate::deliverInitialMousePressEvent(QSGItem *item, QGraphicsSceneMouseEvent *event)
{
    Q_Q(QSGCanvas);

    QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item);
    if (itemPrivate->opacity == 0.0)
        return false;

    if (itemPrivate->flags & QSGItem::ItemClipsChildrenToShape) {
        QPointF p = item->mapFromScene(event->scenePos());
        if (!QRectF(0, 0, item->width(), item->height()).contains(p))
            return false;
    }

    QList<QSGItem *> children = itemPrivate->paintOrderChildItems();
    for (int ii = children.count() - 1; ii >= 0; --ii) {
        QSGItem *child = children.at(ii);
        if (!child->isVisible() || !child->isEnabled())
            continue;
        if (deliverInitialMousePressEvent(child, event))
            return true;
    }

    if (itemPrivate->acceptedMouseButtons & event->button()) {
        QPointF p = item->mapFromScene(event->scenePos());
        if (QRectF(0, 0, item->width(), item->height()).contains(p)) {
            sceneMouseEventForTransform(*event, itemPrivate->canvasToItemTransform());
            event->accept();
            mouseGrabberItem = item;
            q->sendEvent(item, event);
            if (event->isAccepted()) 
                return true;
            mouseGrabberItem->ungrabMouse();
            mouseGrabberItem = 0;
        }
    }

    return false;
}

bool QSGCanvasPrivate::deliverMouseEvent(QGraphicsSceneMouseEvent *sceneEvent)
{
    Q_Q(QSGCanvas);

    if (!mouseGrabberItem && 
         sceneEvent->type() == QEvent::GraphicsSceneMousePress &&
         (sceneEvent->button() & sceneEvent->buttons()) == sceneEvent->buttons()) {
        
        return deliverInitialMousePressEvent(rootItem, sceneEvent);
    }

    if (mouseGrabberItem) {
        QSGItemPrivate *mgPrivate = QSGItemPrivate::get(mouseGrabberItem);
        sceneMouseEventForTransform(*sceneEvent, mgPrivate->canvasToItemTransform());

        sceneEvent->accept();
        q->sendEvent(mouseGrabberItem, sceneEvent);
        if (sceneEvent->isAccepted())
            return true;
    }

    return false;
}

void QSGCanvas::mousePressEvent(QMouseEvent *event)
{
    Q_D(QSGCanvas);
    
#ifdef MOUSE_DEBUG
    qWarning() << "QSGCanvas::mousePressEvent()" << event->pos() << event->button() << event->buttons();
#endif

    QGraphicsSceneMouseEvent sceneEvent(d->sceneMouseEventTypeFromMouseEvent(event));
    d->sceneMouseEventFromMouseEvent(sceneEvent, event);

    d->deliverMouseEvent(&sceneEvent);
    event->setAccepted(sceneEvent.isAccepted());
}

void QSGCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(QSGCanvas);
    
#ifdef MOUSE_DEBUG
    qWarning() << "QSGCanvas::mouseReleaseEvent()" << event->pos() << event->button() << event->buttons();
#endif

    if (!d->mouseGrabberItem) {
        QGLWidget::mouseReleaseEvent(event);
        return;
    }

    QGraphicsSceneMouseEvent sceneEvent(d->sceneMouseEventTypeFromMouseEvent(event));
    d->sceneMouseEventFromMouseEvent(sceneEvent, event);

    d->deliverMouseEvent(&sceneEvent);
    event->setAccepted(sceneEvent.isAccepted());

    d->mouseGrabberItem = 0;
}

void QSGCanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_D(QSGCanvas);
    
#ifdef MOUSE_DEBUG
    qWarning() << "QSGCanvas::mouseDoubleClickEvent()" << event->pos() << event->button() << event->buttons();
#endif

    QGraphicsSceneMouseEvent sceneEvent(d->sceneMouseEventTypeFromMouseEvent(event));
    d->sceneMouseEventFromMouseEvent(sceneEvent, event);

    if (!d->mouseGrabberItem && (event->button() & event->buttons()) == event->buttons()) {
        if (d->deliverInitialMousePressEvent(d->rootItem, &sceneEvent))
            event->accept();
        else
            event->ignore();
        return;
    } 

    d->deliverMouseEvent(&sceneEvent);
    event->setAccepted(sceneEvent.isAccepted());
}

void QSGCanvasPrivate::sendHoverEvent(QEvent::Type type, QSGItem *item,
                                      QGraphicsSceneHoverEvent *event)
{
    Q_Q(QSGCanvas);
    const QTransform transform = QSGItemPrivate::get(item)->canvasToItemTransform();

    //create copy of event
    QGraphicsSceneHoverEvent hoverEvent(type);
    hoverEvent.setWidget(event->widget());
    hoverEvent.setPos(transform.map(event->scenePos()));
    hoverEvent.setScenePos(event->scenePos());
    hoverEvent.setScreenPos(event->screenPos());
    hoverEvent.setLastPos(transform.map(event->lastScenePos()));
    hoverEvent.setLastScenePos(event->lastScenePos());
    hoverEvent.setLastScreenPos(event->lastScreenPos());
    hoverEvent.setModifiers(event->modifiers());
    hoverEvent.setAccepted(event->isAccepted());

    q->sendEvent(item, &hoverEvent);
}

void QSGCanvas::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(QSGCanvas);
    
#ifdef MOUSE_DEBUG
    qWarning() << "QSGCanvas::mouseMoveEvent()" << event->pos() << event->button() << event->buttons();
#endif

    if (!d->mouseGrabberItem) {
        QGraphicsSceneHoverEvent hoverEvent;
        d->sceneHoverEventFromMouseEvent(hoverEvent, event);

        bool delivered = d->deliverHoverEvent(d->rootItem, &hoverEvent);
        if (!delivered) {
            //take care of any exits
            if (d->hoverItem) {
                QSGItem *item = d->hoverItem;
                d->hoverItem = 0;
                d->sendHoverEvent(QEvent::GraphicsSceneHoverLeave, item, &hoverEvent);
            }
        }
        event->setAccepted(hoverEvent.isAccepted());
        return;
    }

    QGraphicsSceneMouseEvent sceneEvent(d->sceneMouseEventTypeFromMouseEvent(event));
    d->sceneMouseEventFromMouseEvent(sceneEvent, event);

    d->deliverMouseEvent(&sceneEvent);
    event->setAccepted(sceneEvent.isAccepted());
}

bool QSGCanvasPrivate::deliverHoverEvent(QSGItem *item, QGraphicsSceneHoverEvent *event)
{
    QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item);
    if (itemPrivate->opacity == 0.0)
        return false;

    if (itemPrivate->flags & QSGItem::ItemClipsChildrenToShape) {
        QPointF p = item->mapFromScene(event->scenePos());
        if (!QRectF(0, 0, item->width(), item->height()).contains(p))
            return false;
    }

    QList<QSGItem *> children = itemPrivate->paintOrderChildItems();
    for (int ii = children.count() - 1; ii >= 0; --ii) {
        QSGItem *child = children.at(ii);
        if (!child->isEnabled())
            continue;
        if (deliverHoverEvent(child, event))
            return true;
    }

    if (itemPrivate->hoverEnabled) {
        QPointF p = item->mapFromScene(event->scenePos());
        if (QRectF(0, 0, item->width(), item->height()).contains(p)) {
            if (hoverItem == item) {
                //move
                sendHoverEvent(QEvent::GraphicsSceneHoverMove, item, event);
            } else {
                //exit from previous
                if (hoverItem) {
                    QSGItem *item = hoverItem;
                    hoverItem = 0;
                    sendHoverEvent(QEvent::GraphicsSceneHoverLeave, item, event);
                }

                //enter new item
                hoverItem = item;
                sendHoverEvent(QEvent::GraphicsSceneHoverEnter, item, event);
            }
            return true;
        }
    }

    return false;
}

bool QSGCanvasPrivate::deliverWheelEvent(QSGItem *item, QGraphicsSceneWheelEvent *event)
{
    Q_Q(QSGCanvas);
    QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item);
    if (itemPrivate->opacity == 0.0)
        return false;

    if (itemPrivate->flags & QSGItem::ItemClipsChildrenToShape) {
        QPointF p = item->mapFromScene(event->scenePos());
        if (!QRectF(0, 0, item->width(), item->height()).contains(p))
            return false;
    }

    QList<QSGItem *> children = itemPrivate->paintOrderChildItems();
    for (int ii = children.count() - 1; ii >= 0; --ii) {
        QSGItem *child = children.at(ii);
        if (!child->isEnabled())
            continue;
        if (deliverWheelEvent(child, event))
            return true;
    }

    QPointF p = item->mapFromScene(event->scenePos());
    if (QRectF(0, 0, item->width(), item->height()).contains(p)) {
        event->setPos(itemPrivate->canvasToItemTransform().map(event->scenePos()));
        event->accept();
        q->sendEvent(item, event);
        if (event->isAccepted())
            return true;
    }

    return false;
}

#ifndef QT_NO_WHEELEVENT
void QSGCanvas::wheelEvent(QWheelEvent *event)
{
    Q_D(QSGCanvas);
#ifdef MOUSE_DEBUG
    qWarning() << "QSGCanvas::wheelEvent()" << event->pos() << event->delta() << event->orientation();
#endif
    QGraphicsSceneWheelEvent wheelEvent(QEvent::GraphicsSceneWheel);
    wheelEvent.setWidget(this);
    wheelEvent.setScenePos(event->pos());
    wheelEvent.setScreenPos(event->globalPos());
    wheelEvent.setButtons(event->buttons());
    wheelEvent.setModifiers(event->modifiers());
    wheelEvent.setDelta(event->delta());
    wheelEvent.setOrientation(event->orientation());
    wheelEvent.setAccepted(false);

    d->deliverWheelEvent(d->rootItem, &wheelEvent);
    event->setAccepted(wheelEvent.isAccepted());
}
#endif // QT_NO_WHEELEVENT

bool QSGCanvasPrivate::deliverTouchEvent(QTouchEvent *event)
{
#ifdef TOUCH_DEBUG
    if (event->type() == QEvent::TouchBegin)
        qWarning("touchBeginEvent");
    else if (event->type() == QEvent::TouchUpdate)
        qWarning("touchUpdateEvent");
    else if (event->type() == QEvent::TouchEnd)
        qWarning("touchEndEvent");
#endif

    QHash<QSGItem *, QList<QTouchEvent::TouchPoint> > updatedPoints;

    if (event->type() == QTouchEvent::TouchBegin) {     // all points are new touch points
        QSet<int> acceptedNewPoints;
        deliverTouchPoints(rootItem, event, event->touchPoints(), &acceptedNewPoints, &updatedPoints);
        if (acceptedNewPoints.count() > 0)
            event->accept();
        return event->isAccepted();
    }

    const QList<QTouchEvent::TouchPoint> &touchPoints = event->touchPoints();
    QList<QTouchEvent::TouchPoint> newPoints;
    QSGItem *item = 0;
    for (int i=0; i<touchPoints.count(); i++) {
        const QTouchEvent::TouchPoint &touchPoint = touchPoints[i];
        switch (touchPoint.state()) {
            case Qt::TouchPointPressed:
                newPoints << touchPoint;
                break;
            case Qt::TouchPointMoved:
            case Qt::TouchPointStationary:
            case Qt::TouchPointReleased:
                if (itemForTouchPointId.contains(touchPoint.id())) {
                    item = itemForTouchPointId[touchPoint.id()];
                    if (item)
                        updatedPoints[item].append(touchPoint);
                }
                break;
            default:
                break;
        }
    }

    if (newPoints.count() > 0 || updatedPoints.count() > 0) {
        QSet<int> acceptedNewPoints;
        int prevCount = updatedPoints.count();
        deliverTouchPoints(rootItem, event, newPoints, &acceptedNewPoints, &updatedPoints);
        if (acceptedNewPoints.count() > 0 || updatedPoints.count() != prevCount)
            event->accept();
    }

    if (event->touchPointStates() & Qt::TouchPointReleased) {
        for (int i=0; i<touchPoints.count(); i++) {
            if (touchPoints[i].state() == Qt::TouchPointReleased)
                itemForTouchPointId.remove(touchPoints[i].id());
        }
    }

    return event->isAccepted();
}

bool QSGCanvasPrivate::deliverTouchPoints(QSGItem *item, QTouchEvent *event, const QList<QTouchEvent::TouchPoint> &newPoints, QSet<int> *acceptedNewPoints, QHash<QSGItem *, QList<QTouchEvent::TouchPoint> > *updatedPoints)
{
    Q_Q(QSGCanvas);
    QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item);

    if (itemPrivate->opacity == 0.0)
        return false;

    if (itemPrivate->flags & QSGItem::ItemClipsChildrenToShape) {
        QRectF bounds(0, 0, item->width(), item->height());
        for (int i=0; i<newPoints.count(); i++) {
            QPointF p = item->mapFromScene(newPoints[i].scenePos());
            if (!bounds.contains(p))
                return false;
        }
    }

    QList<QSGItem *> children = itemPrivate->paintOrderChildItems();
    for (int ii = children.count() - 1; ii >= 0; --ii) {
        QSGItem *child = children.at(ii);
        if (!child->isEnabled())
            continue;
        if (deliverTouchPoints(child, event, newPoints, acceptedNewPoints, updatedPoints))
            return true;
    }

    QList<QTouchEvent::TouchPoint> matchingPoints;
    if (newPoints.count() > 0 && acceptedNewPoints->count() < newPoints.count()) {
        QRectF bounds(0, 0, item->width(), item->height());
        for (int i=0; i<newPoints.count(); i++) {
            if (acceptedNewPoints->contains(newPoints[i].id()))
                continue;
            QPointF p = item->mapFromScene(newPoints[i].scenePos());
            if (bounds.contains(p))
                matchingPoints << newPoints[i];
        }
    }

    if (matchingPoints.count() > 0 || (*updatedPoints)[item].count() > 0) {
        QList<QTouchEvent::TouchPoint> &eventPoints = (*updatedPoints)[item];
        eventPoints.append(matchingPoints);
        transformTouchPoints(eventPoints, itemPrivate->canvasToItemTransform());

        Qt::TouchPointStates eventStates;
        for (int i=0; i<eventPoints.count(); i++)
            eventStates |= eventPoints[i].state();
        // if all points have the same state, set the event type accordingly
        QEvent::Type eventType;
        switch (eventStates) {
            case Qt::TouchPointPressed:
                eventType = QEvent::TouchBegin;
                break;
            case Qt::TouchPointReleased:
                eventType = QEvent::TouchEnd;
                break;
            default:
                eventType = QEvent::TouchUpdate;
                break;
        }

        if (eventStates != Qt::TouchPointStationary) {
            QTouchEvent touchEvent(eventType);
            touchEvent.setWidget(q);
            touchEvent.setDeviceType(event->deviceType());
            touchEvent.setModifiers(event->modifiers());
            touchEvent.setTouchPointStates(eventStates);
            touchEvent.setTouchPoints(eventPoints);

            touchEvent.accept();
            q->sendEvent(item, &touchEvent);

            if (touchEvent.isAccepted()) {
                for (int i=0; i<matchingPoints.count(); i++) {
                    itemForTouchPointId[matchingPoints[i].id()] = item;
                    acceptedNewPoints->insert(matchingPoints[i].id());
                }
            }
        }
    }

    updatedPoints->remove(item);
    if (acceptedNewPoints->count() == newPoints.count() && updatedPoints->isEmpty())
        return true;

    return false;
}

bool QSGCanvasPrivate::sendFilteredMouseEvent(QSGItem *target, QSGItem *item, QGraphicsSceneMouseEvent *event)
{
    if (!target)
        return false;

    if (sendFilteredMouseEvent(target->parentItem(), item, event))
        return true;

    QSGItemPrivate *targetPrivate = QSGItemPrivate::get(target);
    if (targetPrivate->filtersChildMouseEvents) 
        if (target->childMouseEventFilter(item, event))
            return true;

    return false;
}

bool QSGCanvas::sendEvent(QSGItem *item, QEvent *e) 
{ 
    Q_D(QSGCanvas);
    
    if (!item) {
        qWarning("QSGCanvas::sendEvent: Cannot send event to a null item");
        return false;
    }

    Q_ASSERT(e);

    switch (e->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        QSGItemPrivate::get(item)->deliverKeyEvent(static_cast<QKeyEvent *>(e));
        while (!e->isAccepted() && (item = item->parentItem())) {
            e->accept();
            QSGItemPrivate::get(item)->deliverKeyEvent(static_cast<QKeyEvent *>(e));
        }
        break;
    case QEvent::InputMethod:
        QSGItemPrivate::get(item)->deliverInputMethodEvent(static_cast<QInputMethodEvent *>(e));
        while (!e->isAccepted() && (item = item->parentItem())) {
            e->accept();
            QSGItemPrivate::get(item)->deliverInputMethodEvent(static_cast<QInputMethodEvent *>(e));
        }
        break;
    case QEvent::FocusIn:
    case QEvent::FocusOut:
        QSGItemPrivate::get(item)->deliverFocusEvent(static_cast<QFocusEvent *>(e));
        break;
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseRelease:
    case QEvent::GraphicsSceneMouseDoubleClick:
    case QEvent::GraphicsSceneMouseMove:
        // XXX todo - should sendEvent be doing this?  how does it relate to forwarded events? 
        {
            QGraphicsSceneMouseEvent *se = static_cast<QGraphicsSceneMouseEvent *>(e);
            if (!d->sendFilteredMouseEvent(item->parentItem(), item, se)) {
                se->accept();
                QSGItemPrivate::get(item)->deliverMouseEvent(se);
            }
        }
        break;
    case QEvent::GraphicsSceneWheel:
        QSGItemPrivate::get(item)->deliverWheelEvent(static_cast<QGraphicsSceneWheelEvent *>(e));
        break;
    case QEvent::GraphicsSceneHoverEnter:
    case QEvent::GraphicsSceneHoverLeave:
    case QEvent::GraphicsSceneHoverMove:
        QSGItemPrivate::get(item)->deliverHoverEvent(static_cast<QGraphicsSceneHoverEvent *>(e));
        break;
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        QSGItemPrivate::get(item)->deliverTouchEvent(static_cast<QTouchEvent *>(e));
        break;
    default:
        break;
    }

    return false; 
}

void QSGCanvasPrivate::cleanupNodes()
{
    for (int ii = 0; ii < cleanupNodeList.count(); ++ii)
        delete cleanupNodeList.at(ii);
    cleanupNodeList.clear();
}

void QSGCanvasPrivate::updateDirtyNodes()
{
#ifdef DIRTY_DEBUG
    qWarning() << "QSGCanvasPrivate::updateDirtyNodes():";
#endif

    cleanupNodes();

    QSGItem *updateList = dirtyItemList;
    dirtyItemList = 0;
    if (updateList) QSGItemPrivate::get(updateList)->prevDirtyItem = &updateList;

    while (updateList) {
        QSGItem *item = updateList;
        QSGItemPrivate *itemPriv = QSGItemPrivate::get(item);
        itemPriv->removeFromDirtyList();

#ifdef DIRTY_DEBUG
        qWarning() << "   QSGNode:" << item << qPrintable(itemPriv->dirtyToString());
#endif
        updateDirtyNode(item);
    }
}

void QSGCanvasPrivate::updateDirtyNode(QSGItem *item)
{
#ifdef QML_RUNTIME_TESTING
    bool didFlash = false;
#endif

    QSGItemPrivate *itemPriv = QSGItemPrivate::get(item);
    quint32 dirty = itemPriv->dirtyAttributes;
    itemPriv->dirtyAttributes = 0;

    if ((dirty & QSGItemPrivate::TransformUpdateMask) ||
        (dirty & QSGItemPrivate::Size && itemPriv->origin != QSGItem::TopLeft && 
         (itemPriv->scale != 1. || itemPriv->rotation != 0.))) {

        QMatrix4x4 matrix;

        if (itemPriv->x != 0. || itemPriv->y != 0.) 
            matrix.translate(itemPriv->x, itemPriv->y);

        if (dirty & QSGItemPrivate::ComplexTransformUpdateMask) {
            for (int ii = itemPriv->transforms.count() - 1; ii >= 0; --ii)
                itemPriv->transforms.at(ii)->applyTo(&matrix);
        }

        if (itemPriv->scale != 1. || itemPriv->rotation != 0.) {
            QPointF origin = itemPriv->computeTransformOrigin();
            matrix.translate(origin.x(), origin.y());
            if (itemPriv->scale != 1.)
                matrix.scale(itemPriv->scale, itemPriv->scale);
            if (itemPriv->rotation != 0.)
                matrix.rotate(itemPriv->rotation, 0, 0, 1);
            matrix.translate(-origin.x(), -origin.y());
        }

        itemPriv->itemNode()->setMatrix(matrix);
    }

    bool clipEffectivelyChanged = dirty & QSGItemPrivate::Clip &&
                                  ((item->clip() == false) != (itemPriv->clipNode == 0));
    bool effectRefEffectivelyChanged = dirty & QSGItemPrivate::EffectReference &&
                                  ((itemPriv->effectRefCount == 0) != (itemPriv->rootNode == 0));

    if (clipEffectivelyChanged) {
        QSGNode *parent = itemPriv->opacityNode ? (QSGNode *) itemPriv->opacityNode : (QSGNode *)itemPriv->itemNode();
        QSGNode *child = itemPriv->rootNode ? (QSGNode *)itemPriv->rootNode : (QSGNode *)itemPriv->groupNode;

        if (item->clip()) {
            Q_ASSERT(itemPriv->clipNode == 0);
            itemPriv->clipNode = new QSGDefaultClipNode(QRectF(0, 0, itemPriv->width, itemPriv->height));

            if (child)
                parent->removeChildNode(child);
            parent->appendChildNode(itemPriv->clipNode);
            if (child)
                itemPriv->clipNode->appendChildNode(child);

        } else {
            Q_ASSERT(itemPriv->clipNode != 0);
            parent->removeChildNode(itemPriv->clipNode);
            if (child)
                itemPriv->clipNode->removeChildNode(child);
            delete itemPriv->clipNode;
            itemPriv->clipNode = 0;
            if (child)
                parent->appendChildNode(child);
        }
    }

    if (dirty & QSGItemPrivate::ChildrenUpdateMask) {
        while (itemPriv->childContainerNode()->childCount())
            itemPriv->childContainerNode()->removeChildNode(itemPriv->childContainerNode()->childAtIndex(0));
    }

    if (effectRefEffectivelyChanged) {
        QSGNode *parent = itemPriv->clipNode;
        if (!parent)
            parent = itemPriv->opacityNode;
        if (!parent)
            parent = itemPriv->itemNode();
        QSGNode *child = itemPriv->groupNode;

        if (itemPriv->effectRefCount) {
            Q_ASSERT(itemPriv->rootNode == 0);
            itemPriv->rootNode = new QSGRootNode;

            if (child)
                parent->removeChildNode(child);
            parent->appendChildNode(itemPriv->rootNode);
            if (child)
                itemPriv->rootNode->appendChildNode(child);
        } else {
            Q_ASSERT(itemPriv->rootNode != 0);
            parent->removeChildNode(itemPriv->rootNode);
            if (child)
                itemPriv->rootNode->removeChildNode(child);
            delete itemPriv->rootNode;
            itemPriv->rootNode = 0;
            if (child)
                parent->appendChildNode(child);
        }
    }

    if (dirty & QSGItemPrivate::ChildrenUpdateMask) {
        QSGNode *groupNode = itemPriv->groupNode;
        if (groupNode) {
            for (int count = groupNode->childCount(); count; --count)
                groupNode->removeChildNode(groupNode->childAtIndex(0));
        }

        QList<QSGItem *> orderedChildren = itemPriv->paintOrderChildItems();
        int ii = 0;

        itemPriv->paintNodeIndex = 0;
        for (; ii < orderedChildren.count() && orderedChildren.at(ii)->z() < 0; ++ii) {
            QSGItemPrivate *childPrivate = QSGItemPrivate::get(orderedChildren.at(ii));
            if (!childPrivate->explicitVisible && !childPrivate->effectRefCount)
                continue;
            if (childPrivate->itemNode()->parent())
                childPrivate->itemNode()->parent()->removeChildNode(childPrivate->itemNode());

            itemPriv->childContainerNode()->appendChildNode(childPrivate->itemNode());
            itemPriv->paintNodeIndex++;
        }

        if (itemPriv->paintNode)
            itemPriv->childContainerNode()->appendChildNode(itemPriv->paintNode);

        for (; ii < orderedChildren.count(); ++ii) {
            QSGItemPrivate *childPrivate = QSGItemPrivate::get(orderedChildren.at(ii));
            if (!childPrivate->explicitVisible && !childPrivate->effectRefCount)
                continue;
            if (childPrivate->itemNode()->parent())
                childPrivate->itemNode()->parent()->removeChildNode(childPrivate->itemNode());

            itemPriv->childContainerNode()->appendChildNode(childPrivate->itemNode());
        }
    }

    if ((dirty & QSGItemPrivate::Size || clipEffectivelyChanged) && itemPriv->clipNode) {
        itemPriv->clipNode->setRect(QRectF(0, 0, itemPriv->width, itemPriv->height));
        itemPriv->clipNode->update();
    }

    if (dirty & (QSGItemPrivate::OpacityValue | QSGItemPrivate::Visible | QSGItemPrivate::HideReference)) {
        qreal opacity = itemPriv->explicitVisible && itemPriv->hideRefCount == 0
                      ? itemPriv->opacity : qreal(0);

        if (opacity != 1 && !itemPriv->opacityNode) {
            itemPriv->opacityNode = new QSGOpacityNode;

            QSGNode *parent = itemPriv->itemNode();
            QSGNode *child = itemPriv->clipNode;
            if (!child)
                child = itemPriv->rootNode;
            if (!child)
                child = itemPriv->groupNode;

            if (child)
                parent->removeChildNode(child);
            parent->appendChildNode(itemPriv->opacityNode);
            if (child)
                itemPriv->opacityNode->appendChildNode(child);
        }
        if (itemPriv->opacityNode)
            itemPriv->opacityNode->setOpacity(opacity);
    }

    if (dirty & QSGItemPrivate::ContentUpdateMask) {

        if (itemPriv->flags & QSGItem::ItemHasContents) {
            updatePaintNodeData.transformNode = itemPriv->itemNode(); 
            itemPriv->paintNode = item->updatePaintNode(itemPriv->paintNode, &updatePaintNodeData);

            Q_ASSERT(itemPriv->paintNode == 0 || 
                     itemPriv->paintNode->parent() == 0 ||
                     itemPriv->paintNode->parent() == itemPriv->childContainerNode());

            if (itemPriv->paintNode && itemPriv->paintNode->parent() == 0) {
                if (itemPriv->childContainerNode()->childCount() == itemPriv->paintNodeIndex)
                    itemPriv->childContainerNode()->appendChildNode(itemPriv->paintNode);
                else 
                    itemPriv->childContainerNode()->insertChildNodeBefore(itemPriv->paintNode, itemPriv->childContainerNode()->childAtIndex(itemPriv->paintNodeIndex));
            }
        } else if (itemPriv->paintNode) {
            delete itemPriv->paintNode;
        }
    }

#ifndef QT_NO_DEBUG
    // Check consistency.
    const QSGNode *nodeChain[] = {
        itemPriv->itemNodeInstance,
        itemPriv->opacityNode,
        itemPriv->clipNode,
        itemPriv->rootNode,
        itemPriv->groupNode,
        itemPriv->paintNode,
    };

    int ip = 0;
    for (;;) {
        while (ip < 5 && nodeChain[ip] == 0)
            ++ip;
        if (ip == 5)
            break;
        int ic = ip + 1;
        while (ic < 5 && nodeChain[ic] == 0)
            ++ic;
        const QSGNode *parent = nodeChain[ip];
        const QSGNode *child = nodeChain[ic];
        if (child == 0) {
            Q_ASSERT(parent == itemPriv->groupNode || parent->childCount() == 0);
        } else {
            Q_ASSERT(parent == itemPriv->groupNode || parent->childCount() == 1);
            Q_ASSERT(child->parent() == parent);
            bool containsChild = false;
            for (int i = 0; i < parent->childCount(); ++i)
                containsChild |= (parent->childAtIndex(i) == child);
            Q_ASSERT(containsChild);
        }
        ip = ic;
    }
#endif

#ifdef QML_RUNTIME_TESTING
    if (itemPriv->sceneGraphContext()->isFlashModeEnabled()) {
        QSGFlashNode *flash = new QSGFlashNode();
        flash->setRect(item->boundingRect());
        itemPriv->childContainerNode()->appendChildNode(flash);
        didFlash = true;
    }
    Q_Q(QSGCanvas);
    if (didFlash) {
        q->maybeUpdate();
    }
#endif

}

void QSGCanvas::maybeUpdate()
{
    Q_D(QSGCanvas);

    if (d->threadedRendering) {
        if (!d->renderThreadAwakened) {
            d->renderThreadAwakened = true;
            bool locked = d->mutex.tryLock();
            if (d->idle && locked) {
#ifdef THREAD_DEBUG
                qWarning("QSGRenderer: now maybe I should update...");
#endif
                d->wait.wakeOne();
            }
            if (locked)
                d->mutex.unlock();
        }
    } else if (!d->animationDriver || !d->animationDriver->isRunning()) {
        update();
    }
}

/*!
    \fn void QSGEngine::sceneGraphInitialized();

    This signal is emitted when the scene graph has been initialized.

    This signal will be emitted from the scene graph rendering thread.
 */

/*!
    Returns the QSGEngine used for this scene.

    The engine will only be available once the scene graph has been
    initialized. Register for the sceneGraphEngine() signal to get
    notification about this.
 */

QSGEngine *QSGCanvas::sceneGraphEngine() const
{
    Q_D(const QSGCanvas);
    if (d->context->isReady())
        return d->context->engine();
    return 0;
}

QT_END_NAMESPACE
