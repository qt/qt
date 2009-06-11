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

#ifndef QSIMPLECANVASITEM_H
#define QSIMPLECANVASITEM_H

#include <QtDeclarative/qfxglobal.h>
#include <QtDeclarative/qmldebuggerstatus.h>
#include <QtDeclarative/qsimplecanvas.h>
#if defined(QFX_RENDER_OPENGL)
#include <QtDeclarative/gltexture.h>
#endif
#include <QtCore/qobject.h>
#include <QtGui/qgraphicsitem.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QPainter;
class QRect;
class QSimpleCanvas;
class QMouseEvent;
class QKeyEvent;
class QSimpleCanvasItemPrivate;
class QSimpleCanvasLayer;
class QPointF;
class QRectF;
class QGraphicsSceneHoverEvent;
class QSimpleCanvasFilter;
class GLTexture;
class QGLShaderProgram;

class Q_DECLARATIVE_EXPORT QSimpleCanvasItem : public QObject
{
    Q_OBJECT
    Q_CAST_INTERFACES(QGraphicsItem)
    Q_CAST_INTERFACES(QmlDebuggerStatus)
    Q_DECLARE_PRIVATE(QSimpleCanvasItem)
    Q_ENUMS(TransformOrigin)
    Q_PROPERTY(TransformOrigin transformOrigin READ transformOrigin WRITE setTransformOrigin)

public:
    enum ClipType { NoClip = 0x00,
                    ClipToHeight = 0x01, 
                    ClipToWidth = 0x02, 
                    ClipToRect = 0x03 };
    enum Option { NoOption = 0x00000000,
                  MouseFilter = 0x00000001,
                  ChildMouseFilter = 0x00000002,
                  HoverEvents = 0x00000004,
                  MouseEvents = 0x00000008,
                  HasContents = 0x00000010,
                  SimpleItem = 0x00000020,
                  IsFocusPanel = 0x00000040,
                  IsFocusRealm = 0x00000080,
                  AcceptsInputMethods = 0x00000100,
                  IsOpaque = 0x00000200 };
    Q_DECLARE_FLAGS(Options, Option)

    QSimpleCanvasItem(QSimpleCanvasItem *parent=0);
    virtual ~QSimpleCanvasItem();
    operator QGraphicsItem *();
    operator QmlDebuggerStatus *();

    bool clip() const;
    void setClip(bool);
    ClipType clipType() const;
    void setClipType(ClipType);

    Options options() const;
    void setOptions(Options, bool set = true);

    Qt::MouseButtons acceptedMouseButtons() const;
    void setAcceptedMouseButtons(Qt::MouseButtons buttons);

    qreal x() const;
    qreal y() const;
    qreal z() const;
    QPointF pos() const;
    void setX(qreal);
    void setY(qreal);
    virtual void setZ(qreal);
    void setPos(const QPointF &);

    qreal width() const;
    void setWidth(qreal);
    void setImplicitWidth(qreal);
    bool widthValid() const;
    qreal height() const;
    void setHeight(qreal);
    void setImplicitHeight(qreal);
    bool heightValid() const;

    QPointF scenePos() const;

    enum TransformOrigin {
        TopLeft, TopCenter, TopRight,
        MiddleLeft, Center, MiddleRight,
        BottomLeft, BottomCenter, BottomRight 
    };
    TransformOrigin transformOrigin() const;
    void setTransformOrigin(TransformOrigin);
    QPointF transformOriginPoint() const;


    qreal scale() const;
    virtual void setScale(qreal);

    enum Flip { NoFlip = 0, 
                VerticalFlip = 0x01, 
                HorizontalFlip = 0x02, 
                VerticalAndHorizontalFlip = 0x03 };
    Flip flip() const;
    void setFlip(Flip);

    qreal visible() const;
    virtual void setVisible(qreal);
    bool isVisible() const;

    QSimpleCanvas *canvas() const;

    QSimpleCanvasItem *parent() const;
    void setParent(QSimpleCanvasItem *);
    void stackUnder(QSimpleCanvasItem *);
    void stackOver(QSimpleCanvasItem *);
    void stackAt(int idx);
    int indexForChild(QSimpleCanvasItem *);

    QRect itemBoundingRect();

    class GLPainter 
    {
    public:
        GLPainter();
        GLPainter(QSimpleCanvasItem *i);
        QSimpleCanvasItem *item;
        QSimpleCanvas::Matrix activeTransform;
        qreal activeOpacity;
        QRect sceneClipRect;

        QGLShaderProgram *useTextureShader();
        QGLShaderProgram *useColorShader(const QColor &);
        void drawPixmap(const QPointF &, const GLTexture &);
        void drawPixmap(const QRectF &, const GLTexture &);
        void fillRect(const QRectF &, const QColor &);

        void invalidate();
        
        bool blendEnabled;

    private:
        int flags;
        GLPainter(const GLPainter &);
        GLPainter &operator=(const GLPainter &);
    };


    void setPaintMargin(qreal margin);
    QRectF boundingRect() const;
    virtual void paintContents(QPainter &);
    virtual void paintGLContents(GLPainter &);
    virtual uint glSimpleItemData(float *vertices, float *texVertices,
                                  GLTexture **texture, uint count);

    void update();

    virtual QSimpleCanvasLayer *layer();

    bool hasChildren() const;
    const QList<QSimpleCanvasItem *> &children() const;

    QPointF mapFromScene(const QPointF &) const;
    QRectF mapFromScene(const QRectF &) const;
    QPointF mapToScene(const QPointF &) const;
    QRectF mapToScene(const QRectF &) const;

    QSimpleCanvas::Matrix transform() const;
    void setTransform(const QSimpleCanvas::Matrix &);

    QSimpleCanvasFilter *filter() const;
    void setFilter(QSimpleCanvasFilter *);

    QSimpleCanvasItem *mouseGrabberItem() const;
    void ungrabMouse();
    void grabMouse();

    virtual bool isFocusable() const;
    void setFocusable(bool);
    virtual bool hasFocus() const;
    void setFocus(bool);
    bool activeFocusPanel() const;
    void setActiveFocusPanel(bool);

    bool hasActiveFocus() const;

    QSimpleCanvasItem *findFirstFocusChild() const;
    QSimpleCanvasItem *findLastFocusChild() const;
    static QSimpleCanvasItem *findPrevFocus(QSimpleCanvasItem *item);
    static QSimpleCanvasItem *findNextFocus(QSimpleCanvasItem *item);

    GLBasicShaders *basicShaders() const;

#if defined(QFX_RENDER_OPENGL)
    class CachedTexture : public GLTexture 
    {
    public:
        void addRef();
        void release();

        int pixmapWidth() const;
        int pixmapHeight() const;

    private:
        CachedTexture();
        friend class QSimpleCanvasItem;
        QSimpleCanvasPrivate *d;
        QString s;
        int r, w, h;
    };

    CachedTexture *cachedTexture(const QString &);
    CachedTexture *cachedTexture(const QString &, const QPixmap &);
#endif
  
    static QPixmap string(const QString &, const QColor & = Qt::black, const QFont & = QFont());

protected:
    virtual void geometryChanged(const QRectF &newGeometry, 
                                 const QRectF &oldGeometry);
    virtual void addChild(QSimpleCanvasItem *);
    virtual void remChild(QSimpleCanvasItem *);
    virtual void canvasChanged();
    virtual void childrenChanged();
    virtual void parentChanged(QSimpleCanvasItem *, QSimpleCanvasItem *);
    virtual void focusChanged(bool);
    virtual void activeFocusChanged(bool);
    virtual bool eventFilter(QObject *, QEvent *);

public:
    // Events
    virtual bool mouseFilter(QGraphicsSceneMouseEvent *);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseUngrabEvent();
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void focusOutEvent(QFocusEvent *e);
    virtual void focusInEvent(QFocusEvent *e);
    virtual void activePanelInEvent();
    virtual void activePanelOutEvent();
    virtual void inputMethodEvent(QInputMethodEvent* event);
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;

private:
    friend class QSimpleCanvas;
    friend class QSimpleCanvasPrivate;
    friend class QSimpleCanvasRootLayer;
    friend class QSimpleCanvasItem::GLPainter;
    friend class QSimpleCanvasFilter;
    friend class QGraphicsQSimpleCanvasItem;
    friend class QSimpleGraphicsItem;
    friend class CanvasEGLWidget;
    friend class QSimpleCanvasFilterPrivate;

public:
    QSimpleCanvasItem(QSimpleCanvasItemPrivate &dd, QSimpleCanvasItem *parent);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSimpleCanvasItem::Options)

class QSimpleCanvasLayer : public QSimpleCanvasItem
{
public:
    QSimpleCanvasLayer(QSimpleCanvasItem *parent);

    virtual void addChild(QSimpleCanvasItem *);
    virtual void addDirty(QSimpleCanvasItem *);
    virtual void remDirty(QSimpleCanvasItem *);
    virtual QSimpleCanvasLayer *layer();

private:
    friend class QSimpleCanvas;
    friend class QSimpleCanvasRootLayer;
    QSimpleCanvasLayer();
};



#endif // _GFXCANVASITEM_H_


QT_END_NAMESPACE

QT_END_HEADER
