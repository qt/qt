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
#include "qvsyncanimationdriver_p.h"

#include "qsgcontext.h"

#include <QtCore/qthread.h>
#include <QtCore/qmutex.h>
#include <QtCore/qwaitcondition.h>
#include <private/qwidget_p.h>

QT_BEGIN_NAMESPACE

class QSGRootItem : public QSGItem
{
    Q_OBJECT
public:
    QSGRootItem();
};

class QSGThreadedRendererAnimationDriver : public QAnimationDriver
{
public:
    QSGThreadedRendererAnimationDriver(QObject * = 0);

protected:
    virtual void started();
    virtual void stopped();
};

class QSGCanvasPrivate;
class QSGRenderer : public QGLWidget
{
    Q_OBJECT
public:
    QSGRenderer(QSGCanvasPrivate *canvas, const QGLFormat &format, QWidget *p);

public slots:
    void maybeUpdate();

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void hideEvent(QHideEvent *);

    virtual bool event(QEvent *);

private:
    friend class QSGCanvas;

    void initializeSceneGraph();
    void polishItems();
    void syncSceneGraph();
    void renderSceneGraph();

    void runThread();

    struct MyThread : public QThread {
        MyThread(QSGRenderer *r) : r(r) {}
        virtual void run() { r->runThread(); }
        QSGRenderer *r;
    };
    QSGContext *context;
    QSGCanvasPrivate *canvas;

    bool contextInThread;
    bool threadedRendering;
    bool inUpdate;
    bool exitThread;

    MyThread *thread;
    QMutex mutex;
    QWaitCondition wait;
    QSize widgetSize;
    QSize viewportSize;

    QAnimationDriver *animationDriver;
};

class QSGCanvasPrivate : public QWidgetPrivate
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
    static QEvent::Type sceneMouseEventTypeFromMouseEvent(QMouseEvent *);
    static void sceneMouseEventForTransform(QGraphicsSceneMouseEvent &, const QTransform &);
    bool deliverInitialMousePressEvent(QSGItem *, QGraphicsSceneMouseEvent *);
    bool deliverMouseEvent(QGraphicsSceneMouseEvent *);
    bool sendFilteredMouseEvent(QSGItem *, QSGItem *, QGraphicsSceneMouseEvent *);

    enum FocusOption {
        DontChangeFocusProperty = 0x01,
    };
    Q_DECLARE_FLAGS(FocusOptions, FocusOption)

    void setFocusInScope(QSGItem *scope, QSGItem *item, FocusOptions = 0);
    void clearFocusInScope(QSGItem *scope, QSGItem *item, FocusOptions = 0);
    void notifyFocusChangesRecur(QSGItem **item, int remaining);

    void dirtyItem(QSGItem *);
    void cleanup(Node *);

    QSGRenderer *renderer;
    QSGItem::UpdatePaintNodeData updatePaintNodeData;

    QSGItem *dirtyItemList;
    QList<Node *> cleanupNodeList;

    QSet<QSGItem *> polishItems;

    void updateDirtyNodes();
    void cleanupNodes();
    bool updateEffectiveOpacity(QSGItem *);
    void updateEffectiveOpacityRoot(QSGItem *, qreal);
    void updateDirtyNode(QSGItem *);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSGCanvasPrivate::FocusOptions)

QT_END_NAMESPACE

#endif // QSGCANVAS_P_H
