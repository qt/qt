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

#ifndef QSIMPLECANVAS_P_H
#define QSIMPLECANVAS_P_H

#include "qsimplecanvas.h"
#include "qsimplecanvasitem.h"
#include <qstack.h>
#include <qdatetime.h>

#if defined(QFX_RENDER_OPENGL)

#if defined(QFX_RENDER_OPENGL2)
#include "glbasicshaders.h"
#endif

#include <QGLWidget>
QT_BEGIN_NAMESPACE

class CanvasEGLWidget : public QGLWidget
{
public:
    CanvasEGLWidget(QSimpleCanvas *parent, QSimpleCanvasPrivate *canvas)
    : 
        QGLWidget(parent), 
    _canvas(canvas)
    {
    }

    virtual void paintGL();
    virtual void resizeGL(int,int);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void initializeGL();

    void updateGL();
    void updateGL(const QRect &);

    QRect map(const QRectF &) const;
private:
    QRect _clip;
    QSimpleCanvasPrivate *_canvas;
    QSimpleCanvas::Matrix defaultTransform;
    QSimpleCanvas::Matrix invDefaultTransform;
};
#endif

#include <QGraphicsView>
#include <QGraphicsScene>

struct QSimpleCanvasGraphicsView : public QGraphicsView
{
public:
    QSimpleCanvasGraphicsView(QSimpleCanvasPrivate *parent);
    ~QSimpleCanvasGraphicsView();

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void focusInEvent(QFocusEvent *);

private:
    QSimpleCanvasPrivate *canvas;
    QGraphicsScene _scene;
};

class QGLFramebufferObject;
class QSimpleCanvasDebugPlugin;
class QSimpleCanvasPrivate
{
public:
    QSimpleCanvasPrivate(QSimpleCanvas *canvas)
    : q(canvas), timer(0), root(0), lrpTime(0), debugPlugin(0), focusItem(0), 
      lastFocusItem(0), lastMouseItem(0),
      isSetup(false), view(0)
#if defined(QFX_RENDER_OPENGL)
      ,egl(q, this), basicShadersInstance(0)
#endif
      , paintVersion(1)
    {
    }

    QSimpleCanvas *q;
    QSimpleCanvas::CanvasMode mode;
    bool isSimpleCanvas() const { return mode == QSimpleCanvas::SimpleCanvas; }
    bool isGraphicsView() const { return mode == QSimpleCanvas::GraphicsView; }

#if defined(QFX_RENDER_OPENGL)
    QRectF oldDirty;
#else
    QRect oldDirty;
#endif
    QRect resetDirty();
    void paint(QPainter &p);

    QSimpleCanvasItem *opaqueList;

    int timer;

    QSimpleCanvasLayer *root;
    QList<QSimpleCanvasItem *> dirtyItems;
    int lrpTime;

    QTime frameTimer;
    QTime lrpTimer;

    QSimpleCanvasDebugPlugin *debugPlugin;

    QStack<QSimpleCanvasItem *> focusPanels;
    QHash<QSimpleCanvasItem *, QSimpleCanvasItem *> focusPanelData;
    QSimpleCanvasItem *focusItem;
    QSimpleCanvasItem *lastFocusItem;

    QRect dirtyItemClip() const;
    void clearFocusPanel(QSimpleCanvasItem *);
    void setActiveFocusPanel(QSimpleCanvasItem *, Qt::FocusReason focusReason = Qt::OtherFocusReason);
    void switchToFocusPanel(QSimpleCanvasItem *, QSimpleCanvasItem *, Qt::FocusReason focusReason);

    void setFocusItem(QSimpleCanvasItem *item, Qt::FocusReason focusReason,
                      bool overwrite = true);
    void clearFocusItem(QSimpleCanvasItem *item);

    void clearActiveFocusItem(QSimpleCanvasItem *, Qt::FocusReason focusReason);
    void setActiveFocusItem(QSimpleCanvasItem *, Qt::FocusReason focusReason);
    void installMouseFilter(QSimpleCanvasItem *);
    void removeMouseFilter(QSimpleCanvasItem *);
    QList<QSimpleCanvasItem *> mouseFilters;
    bool filter(QMouseEvent *e);
    bool deliverMousePress(QSimpleCanvasItem *, QMouseEvent *, bool = false);
    QGraphicsSceneMouseEvent *mouseEventToSceneMouseEvent(QMouseEvent *, const QPoint &);
    QSimpleCanvasItem *lastMouseItem;
    void sendMouseEvent(QSimpleCanvasItem *, QGraphicsSceneMouseEvent *);

    bool isSetup;

    void init(QSimpleCanvas::CanvasMode mode);

    QSimpleCanvasGraphicsView *view;

#if defined(QFX_RENDER_OPENGL)
    CanvasEGLWidget egl;
    GLBasicShaders *basicShaders() const
    {
#if defined(QFX_RENDER_OPENGL2)
        if (!basicShadersInstance)
            basicShadersInstance = new GLBasicShaders;
        return basicShadersInstance;
#else
        return 0;
#endif
    }
    mutable GLBasicShaders *basicShadersInstance;

    QHash<QString, QSimpleCanvasItem::CachedTexture *> cachedTextures;

    QList<QGLFramebufferObject *> frameBuffers;
    QGLFramebufferObject *acquire(int, int);
    void release(QGLFramebufferObject *);
    void paintGL();
#endif
    int paintVersion;
};

#endif

QT_END_NAMESPACE
