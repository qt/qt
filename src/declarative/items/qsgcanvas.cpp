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

#include "renderer.h"

#include <QtGui/qpainter.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qmatrix4x4.h>
#include <QtCore/qvarlengtharray.h>

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

QSGItem::UpdatePaintNodeData::UpdatePaintNodeData()
: transformNode(0)
{
}

QSGRootItem::QSGRootItem()
{
}

QSGThreadedRendererAnimationDriver::QSGThreadedRendererAnimationDriver(QSGCanvasPrivate *r)
    : QAnimationDriver(0)
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
        Q_ASSERT(d->context);

        d->polishItems();

        QDeclarativeDebugTrace::addEvent(QDeclarativeDebugTrace::FramePaint);
        QDeclarativeDebugTrace::startRange(QDeclarativeDebugTrace::Painting);

        makeCurrent();
        d->syncSceneGraph();
        d->renderSceneGraph();

        QDeclarativeDebugTrace::endRange(QDeclarativeDebugTrace::Painting);
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
        d->animationDriver = new QSGThreadedRendererAnimationDriver(d);
        d->animationDriver->install();
        d->mutex.lock();
        d->thread->start();
        d->wait.wait(&d->mutex);
        d->mutex.unlock();
    } else {
        makeCurrent();
        d->initializeSceneGraph();
        d->animationDriver = d->context->createAnimationDriver(this);
        if (d->animationDriver)
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

        if (exitThread)
            break;

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

        QFocusEvent event(QEvent::FocusIn, Qt::OtherFocusReason);
        q->sendEvent(newActiveFocusItem, &event); 
    }

    updateInputMethodData();

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

        QFocusEvent event(QEvent::FocusIn, Qt::OtherFocusReason);
        q->sendEvent(newActiveFocusItem, &event); 
    }

    updateInputMethodData();

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
            item->itemChange(QSGItem::ItemActiveFocusHasChanged, qVariantFromValue(itemPrivate->activeFocus));
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

void QSGCanvasPrivate::cleanup(Node *n)
{
    Q_Q(QSGCanvas);

    Q_ASSERT(!cleanupNodeList.contains(n));
    cleanupNodeList.append(n);
    q->maybeUpdate();
}

QSGCanvas::QSGCanvas(QWidget *parent, Qt::WindowFlags f)
    : QGLWidget(*(new QSGCanvasPrivate), QGLFormat(), parent, (QGLWidget *) 0, f)
{
    Q_D(QSGCanvas);

    d->init(this);
}

QSGCanvas::QSGCanvas(const QGLFormat &format, QWidget *parent, Qt::WindowFlags f)
    : QGLWidget(*(new QSGCanvasPrivate), format, parent, (QGLWidget *) 0, f)
{
    Q_D(QSGCanvas);

    d->init(this);
}

QSGCanvas::QSGCanvas(QSGCanvasPrivate &dd, QWidget *parent, Qt::WindowFlags f)
: QGLWidget(dd, QGLFormat(), parent, 0, f)
{
    Q_D(QSGCanvas);

    d->init(this);
}

QSGCanvas::QSGCanvas(QSGCanvasPrivate &dd, const QGLFormat &format, QWidget *parent, Qt::WindowFlags f)
: QGLWidget(dd, format, parent, 0, f)
{
    Q_D(QSGCanvas);

    d->init(this);
}

QSGCanvas::~QSGCanvas()
{
    Q_D(QSGCanvas);

    delete d->rootItem; d->rootItem = 0;
    d->cleanupNodes();
}

void QSGCanvas::setSceneGraphContext(QSGContext *context)
{
    Q_D(QSGCanvas);

    if (d->contextInThread || d->context) {
        qWarning("QSGCanvas::setSceneGraphContext: Context already exists.");
        return;
    }

    d->context = context;
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
#ifdef TOUCH_DEBUG
        qWarning("touchBeginEvent");
#endif
        return true;
    case QEvent::TouchUpdate:
#ifdef TOUCH_DEBUG
        qWarning("touchUpdateEvent");
#endif
        return true;
    case QEvent::TouchEnd:
#ifdef TOUCH_DEBUG
        qWarning("touchEndEvent");
#endif
        return true;
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
        if (!child->isEnabled())
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
    event->setPos(transform.map(event->scenePos()));
    hoverEvent.setScenePos(event->scenePos());
    hoverEvent.setScreenPos(event->screenPos());
    event->setLastPos(transform.map(event->lastScenePos()));
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
        qWarning() << "   Node:" << item << qPrintable(itemPriv->dirtyToString());
#endif
        updateDirtyNode(item);
    }
}

void QSGCanvasPrivate::updateDirtyNode(QSGItem *item)
{
    QSGItemPrivate *itemPriv = QSGItemPrivate::get(item);
    quint32 dirty = itemPriv->dirtyAttributes;
    itemPriv->dirtyAttributes = 0;

    if ((dirty & QSGItemPrivate::TransformUpdateMask) ||
        (dirty & QSGItemPrivate::Size && itemPriv->origin != QSGItem::TopLeft && 
         (itemPriv->scale != 1. || itemPriv->rotation != 0.))) {

        QMatrix4x4 matrix;

        if (!itemPriv->effectRefCount) {
            if (itemPriv->x != 0. || itemPriv->y != 0.) 
                matrix.translate(itemPriv->x, itemPriv->y);

            if (itemPriv->scale != 1. || itemPriv->rotation != 0.) {
                QPointF origin = itemPriv->computeTransformOrigin();
                matrix.translate(origin.x(), origin.y());
                if (itemPriv->scale != 1.)
                    matrix.scale(itemPriv->scale, itemPriv->scale);
                if (itemPriv->rotation != 0.)
                    matrix.rotate(itemPriv->rotation, 0, 0, 1);
                matrix.translate(-origin.x(), -origin.y());
            }
        }

        if (dirty & QSGItemPrivate::ComplexTransformUpdateMask) {
            for (int ii = 0; ii < itemPriv->transforms.count(); ++ii)
                itemPriv->transforms.at(ii)->applyTo(&matrix);
        }

        itemPriv->itemNode()->setMatrix(matrix);
    }

    bool clipEffectivelyChanged = dirty & QSGItemPrivate::Clip &&
                                  ((item->clip() == false) != (itemPriv->clipNode == 0));

    if (clipEffectivelyChanged) {
        Node *parent = itemPriv->opacityNode ? (Node *) itemPriv->opacityNode : (Node *)itemPriv->itemNode();
        Node *child = itemPriv->groupNode;

        if (item->clip()) {
            Q_ASSERT(itemPriv->clipNode == 0);
            itemPriv->clipNode = new QSGClipNode(QRectF(0, 0, itemPriv->width, itemPriv->height));

            if (child)
                parent->removeChildNode(child);
            parent->appendChildNode(itemPriv->clipNode);
            if (child)
                itemPriv->clipNode->appendChildNode(child);

        } else {
            Q_ASSERT(itemPriv->clipNode != 0);
            delete itemPriv->clipNode;
            itemPriv->clipNode = 0;
            parent->appendChildNode(child);
        }
    }

    if (dirty & QSGItemPrivate::ChildrenUpdateMask) {

        while (itemPriv->childContainerNode()->childCount())
            itemPriv->childContainerNode()->removeChildNode(itemPriv->childContainerNode()->childAtIndex(0));

        QList<QSGItem *> orderedChildren = itemPriv->paintOrderChildItems();
        int ii = 0;

        itemPriv->paintNodeIndex = 0;
        for (; ii < orderedChildren.count() && orderedChildren.at(ii)->z() < 0; ++ii) {
            QSGItemPrivate *childPrivate = QSGItemPrivate::get(orderedChildren.at(ii));
            if (childPrivate->itemNode()->parent())
                childPrivate->itemNode()->parent()->removeChildNode(childPrivate->itemNode());

            if (childPrivate->effectRefCount == 0) {
                itemPriv->childContainerNode()->appendChildNode(childPrivate->itemNode());
                itemPriv->paintNodeIndex++;
            }
        }

        if (itemPriv->paintNode)
            itemPriv->childContainerNode()->appendChildNode(itemPriv->paintNode);

        for (; ii < orderedChildren.count(); ++ii) {
            QSGItemPrivate *childPrivate = QSGItemPrivate::get(orderedChildren.at(ii));
            if (childPrivate->itemNode()->parent())
                childPrivate->itemNode()->parent()->removeChildNode(childPrivate->itemNode());

            if (childPrivate->effectRefCount == 0) {
                itemPriv->childContainerNode()->appendChildNode(childPrivate->itemNode());
            }
        }
    }

    if ((dirty & QSGItemPrivate::Size || clipEffectivelyChanged) && itemPriv->clipNode) {
        itemPriv->clipNode->setRect(QRectF(0, 0, itemPriv->width, itemPriv->height));
        itemPriv->clipNode->update();
    }

    if (dirty & QSGItemPrivate::OpacityValue) {
        if (!itemPriv->opacityNode) {
            itemPriv->opacityNode = new OpacityNode;

            Node *parent = itemPriv->itemNode();
            Node *child = itemPriv->clipNode ? itemPriv->clipNode : itemPriv->childContainerNode();

            parent->removeChildNode(child);
            parent->appendChildNode(itemPriv->opacityNode);
            itemPriv->opacityNode->appendChildNode(child);
        }
        itemPriv->opacityNode->setOpacity(itemPriv->opacity);
    }

    if (dirty & QSGItemPrivate::ContentUpdateMask) {

        if (itemPriv->flags & QSGItem::ItemHasContents) {
            updatePaintNodeData.transformNode = itemPriv->itemNode(); 
            itemPriv->paintNode = item->updatePaintNode(itemPriv->paintNode, &updatePaintNodeData);

            Q_ASSERT(itemPriv->paintNode == 0 || 
                     itemPriv->paintNode->parent() == 0 ||
                     itemPriv->paintNode->parent() == itemPriv->childContainerNode());

            if (itemPriv->paintNode && itemPriv->paintNode->parent() == 0) {
                if (itemPriv->childContainerNode()->childCount() == 0)
                    itemPriv->childContainerNode()->appendChildNode(itemPriv->paintNode);
                else 
                    itemPriv->childContainerNode()->insertChildNodeBefore(itemPriv->paintNode, itemPriv->childContainerNode()->childAtIndex(itemPriv->paintNodeIndex));
            }
        } else if (itemPriv->paintNode) {
            delete itemPriv->paintNode;
        }
    } 
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


QT_END_NAMESPACE
