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

#ifndef QSGCANVAS_P_H
#define QSGCANVAS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qsgitem.h"
#include "qsgcanvas.h"
#include <private/qdeclarativeguard_p.h>

#include <private/qsgcontext_p.h>

#include <QtCore/qthread.h>
#include <QtCore/qmutex.h>
#include <QtCore/qwaitcondition.h>
#include <private/qwidget_p.h>
#include <private/qgl_p.h>

QT_BEGIN_NAMESPACE

//Make it easy to identify and customize the root item if needed
class QSGRootItem : public QSGItem
{
    Q_OBJECT
public:
    QSGRootItem();
};

class QSGCanvasPrivate;

class QSGThreadedRendererAnimationDriver : public QAnimationDriver
{
public:
    QSGThreadedRendererAnimationDriver(QSGCanvasPrivate *r, QObject *parent);

protected:
    virtual void started();
    virtual void stopped();

    QSGCanvasPrivate *renderer;
};

class QTouchEvent;
class QSGCanvasPrivate : public QGLWidgetPrivate
{
public:
    Q_DECLARE_PUBLIC(QSGCanvas)

    static inline QSGCanvasPrivate *get(QSGCanvas *c) { return c->d_func(); }

    QSGCanvasPrivate();
    virtual ~QSGCanvasPrivate();

    void init(QSGCanvas *);

    QSGRootItem *rootItem;

    QSGItem *activeFocusItem;
    QSGItem *mouseGrabberItem;

    // Mouse positions are saved in widget coordinates
    QPoint lastMousePosition;
    QPoint buttonDownPositions[5]; // Left, Right, Middle, XButton1, XButton2
    void sceneMouseEventFromMouseEvent(QGraphicsSceneMouseEvent &, QMouseEvent *);
    void translateTouchEvent(QTouchEvent *touchEvent);
    static QEvent::Type sceneMouseEventTypeFromMouseEvent(QMouseEvent *);
    static void sceneMouseEventForTransform(QGraphicsSceneMouseEvent &, const QTransform &);
    static void transformTouchPoints(QList<QTouchEvent::TouchPoint> &touchPoints, const QTransform &transform);
    bool deliverInitialMousePressEvent(QSGItem *, QGraphicsSceneMouseEvent *);
    bool deliverMouseEvent(QGraphicsSceneMouseEvent *);
    bool sendFilteredMouseEvent(QSGItem *, QSGItem *, QGraphicsSceneMouseEvent *);
    bool deliverWheelEvent(QSGItem *, QGraphicsSceneWheelEvent *);
    bool deliverTouchPoints(QSGItem *, QTouchEvent *, const QList<QTouchEvent::TouchPoint> &, QSet<int> *,
            QHash<QSGItem *, QList<QTouchEvent::TouchPoint> > *);
    bool deliverTouchEvent(QTouchEvent *);
    void sceneHoverEventFromMouseEvent(QGraphicsSceneHoverEvent &, QMouseEvent *);
    bool deliverHoverEvent(QSGItem *, QGraphicsSceneHoverEvent *);
    void sendHoverEvent(QEvent::Type, QSGItem *, QGraphicsSceneHoverEvent *);
    void clearHover();

    QDeclarativeGuard<QSGItem> hoverItem;
    enum FocusOption {
        DontChangeFocusProperty = 0x01,
    };
    Q_DECLARE_FLAGS(FocusOptions, FocusOption)

    void setFocusInScope(QSGItem *scope, QSGItem *item, FocusOptions = 0);
    void clearFocusInScope(QSGItem *scope, QSGItem *item, FocusOptions = 0);
    void notifyFocusChangesRecur(QSGItem **item, int remaining);

    void updateInputMethodData();

    void dirtyItem(QSGItem *);
    void cleanup(QSGNode *);

    void initializeSceneGraph();
    void polishItems();
    void syncSceneGraph();
    void renderSceneGraph();
    void runThread();

    QSGItem::UpdatePaintNodeData updatePaintNodeData;

    QSGItem *dirtyItemList;
    QList<QSGNode *> cleanupNodeList;

    QSet<QSGItem *> itemsToPolish;

    void updateDirtyNodes();
    void cleanupNodes();
    bool updateEffectiveOpacity(QSGItem *);
    void updateEffectiveOpacityRoot(QSGItem *, qreal);
    void updateDirtyNode(QSGItem *);

    QSGContext *context;

    uint contextInThread : 1;
    uint threadedRendering : 1;
    uint exitThread : 1;
    uint animationRunning: 1;
    uint idle : 1;              // Set to true when render thread sees no change and enters a wait()
    uint needsRepaint : 1;      // Set by callback from render if scene needs repainting.
    uint renderThreadAwakened : 1;

    struct MyThread : public QThread {
        MyThread(QSGCanvasPrivate *r) : renderer(r) {}
        virtual void run() { renderer->runThread(); }
        static void doWait() { QThread::msleep(16); }
        QSGCanvasPrivate *renderer;
    };
    MyThread *thread;
    QMutex mutex;
    QWaitCondition wait;
    QSize widgetSize;
    QSize viewportSize;

    QAnimationDriver *animationDriver;

    QHash<int, QSGItem *> itemForTouchPointId;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSGCanvasPrivate::FocusOptions)

QT_END_NAMESPACE

#endif // QSGCANVAS_P_H
