// Commit: 6f78a6080b84cc3ef96b73a4ff58d1b5a72f08f4
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QSGITEM_H
#define QSGITEM_H

#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativecomponent.h>

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qfont.h>
#include <QtGui/qaction.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGItem;
class QSGTransformPrivate;
class QSGTransform : public QObject
{
    Q_OBJECT
public:
    QSGTransform(QObject *parent = 0);
    ~QSGTransform();

    void appendToItem(QSGItem *);
    void prependToItem(QSGItem *);

    virtual void applyTo(QMatrix4x4 *matrix) const = 0;

protected Q_SLOTS:
    void update();

protected:
    QSGTransform(QSGTransformPrivate &dd, QObject *parent);

private:
    Q_DECLARE_PRIVATE(QSGTransform);
};

class QDeclarativeState;
class QSGAnchorLine;
class QDeclarativeTransition;
class QSGKeyEvent;
class QSGAnchors;
class QSGItemPrivate;
class QSGCanvas;
class QSGEngine;
class QTouchEvent;
class QSGNode;
class QSGTransformNode;
class Q_DECLARATIVE_EXPORT QSGItem : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(QSGItem *parent READ parentItem WRITE setParentItem NOTIFY parentChanged DESIGNABLE false FINAL)
    Q_PRIVATE_PROPERTY(QSGItem::d_func(), QDeclarativeListProperty<QObject> data READ data DESIGNABLE false)
    Q_PRIVATE_PROPERTY(QSGItem::d_func(), QDeclarativeListProperty<QObject> resources READ resources DESIGNABLE false)
    Q_PRIVATE_PROPERTY(QSGItem::d_func(), QDeclarativeListProperty<QSGItem> children READ children NOTIFY childrenChanged DESIGNABLE false)

    Q_PROPERTY(QPointF pos READ pos FINAL)
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged FINAL)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged FINAL)
    Q_PROPERTY(qreal z READ z WRITE setZ NOTIFY zChanged FINAL)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged RESET resetWidth FINAL)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged RESET resetHeight FINAL)

    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged FINAL)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged FINAL)

    Q_PRIVATE_PROPERTY(QSGItem::d_func(), QDeclarativeListProperty<QDeclarativeState> states READ states DESIGNABLE false)
    Q_PRIVATE_PROPERTY(QSGItem::d_func(), QDeclarativeListProperty<QDeclarativeTransition> transitions READ transitions DESIGNABLE false)
    Q_PRIVATE_PROPERTY(QSGItem::d_func(), QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QRectF childrenRect READ childrenRect NOTIFY childrenRectChanged DESIGNABLE false FINAL)
    Q_PRIVATE_PROPERTY(QSGItem::d_func(), QSGAnchors * anchors READ anchors DESIGNABLE false CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(QSGItem::d_func(), QSGAnchorLine left READ left CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(QSGItem::d_func(), QSGAnchorLine right READ right CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(QSGItem::d_func(), QSGAnchorLine horizontalCenter READ horizontalCenter CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(QSGItem::d_func(), QSGAnchorLine top READ top CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(QSGItem::d_func(), QSGAnchorLine bottom READ bottom CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(QSGItem::d_func(), QSGAnchorLine verticalCenter READ verticalCenter CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(QSGItem::d_func(), QSGAnchorLine baseline READ baseline CONSTANT FINAL)
    Q_PROPERTY(qreal baselineOffset READ baselineOffset WRITE setBaselineOffset NOTIFY baselineOffsetChanged)

    Q_PROPERTY(bool clip READ clip WRITE setClip NOTIFY clipChanged)

    Q_PROPERTY(bool focus READ hasFocus WRITE setFocus NOTIFY focusChanged FINAL)
    Q_PROPERTY(bool activeFocus READ hasActiveFocus NOTIFY activeFocusChanged FINAL)

    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(TransformOrigin transformOrigin READ transformOrigin WRITE setTransformOrigin NOTIFY transformOriginChanged)
    Q_PROPERTY(QPointF transformOriginPoint READ transformOriginPoint)  // XXX todo - notify?
    Q_PROPERTY(QDeclarativeListProperty<QSGTransform> transform READ transform DESIGNABLE false FINAL)

    Q_PROPERTY(bool smooth READ smooth WRITE setSmooth NOTIFY smoothChanged)
    Q_PROPERTY(qreal implicitWidth READ implicitWidth WRITE setImplicitWidth NOTIFY implicitWidthChanged)
    Q_PROPERTY(qreal implicitHeight READ implicitHeight WRITE setImplicitHeight NOTIFY implicitHeightChanged)

    Q_ENUMS(TransformOrigin)
    Q_CLASSINFO("DefaultProperty", "data")

public:
    enum Flag {
        ItemClipsChildrenToShape  = 0x01,
        ItemAcceptsInputMethod    = 0x02,
        ItemIsFocusScope          = 0x04,
        ItemHasContents           = 0x08,
        // Remember to increment the size of QSGItemPrivate::flags
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    enum ItemChange {
        ItemChildAddedChange,      // value.item
        ItemChildRemovedChange,    // value.item
        ItemSceneChange,           // value.canvas
        ItemVisibleHasChanged,     // value.realValue
        ItemParentHasChanged,      // value.item
        ItemOpacityHasChanged,     // value.realValue
        ItemActiveFocusHasChanged, // value.boolValue
        ItemRotationHasChanged,    // value.realValue
    };

    union ItemChangeData {
        ItemChangeData(QSGItem *v) : item(v) {}
        ItemChangeData(QSGCanvas *v) : canvas(v) {}
        ItemChangeData(qreal v) : realValue(v) {}
        ItemChangeData(bool v) : boolValue(v) {}

        QSGItem *item;
        QSGCanvas *canvas;
        qreal realValue;
        bool boolValue;
    };

    enum TransformOrigin {
        TopLeft, Top, TopRight,
        Left, Center, Right,
        BottomLeft, Bottom, BottomRight
    };

    QSGItem(QSGItem *parent = 0);
    virtual ~QSGItem();

    QSGEngine *sceneGraphEngine() const;

    QSGCanvas *canvas() const;
    QSGItem *parentItem() const;
    void setParentItem(QSGItem *parent);
    void stackBefore(const QSGItem *);
    void stackAfter(const QSGItem *);

    QRectF childrenRect();
    QList<QSGItem *> childItems() const;

    bool clip() const;
    void setClip(bool);

    qreal baselineOffset() const;
    void setBaselineOffset(qreal);

    QDeclarativeListProperty<QSGTransform> transform();

    qreal x() const;
    qreal y() const;
    QPointF pos() const;
    void setX(qreal);
    void setY(qreal);
    void setPos(const QPointF &);

    qreal width() const;
    void setWidth(qreal);
    void resetWidth();
    qreal implicitWidth() const;

    qreal height() const;
    void setHeight(qreal);
    void resetHeight();
    qreal implicitHeight() const;

    void setSize(const QSizeF &size);

    TransformOrigin transformOrigin() const;
    void setTransformOrigin(TransformOrigin);
    QPointF transformOriginPoint() const;

    qreal z() const;
    void setZ(qreal);

    qreal rotation() const;
    void setRotation(qreal);
    qreal scale() const;
    void setScale(qreal);

    qreal opacity() const;
    void setOpacity(qreal);

    bool isVisible() const;
    void setVisible(bool);

    bool isEnabled() const;
    void setEnabled(bool);

    bool smooth() const;
    void setSmooth(bool);

    Flags flags() const;
    void setFlag(Flag flag, bool enabled = true);
    void setFlags(Flags flags);

    QRectF boundingRect() const;

    bool hasActiveFocus() const;
    bool hasFocus() const;
    void setFocus(bool);
    bool isFocusScope() const;
    QSGItem *scopedFocusItem() const;

    Qt::MouseButtons acceptedMouseButtons() const;
    void setAcceptedMouseButtons(Qt::MouseButtons buttons);
    bool acceptHoverEvents() const;
    void setAcceptHoverEvents(bool enabled);

    bool isUnderMouse() const;
    void grabMouse();
    void ungrabMouse();
    bool keepMouseGrab() const;
    void setKeepMouseGrab(bool);
    bool filtersChildMouseEvents() const;
    void setFiltersChildMouseEvents(bool filter);

    QTransform itemTransform(QSGItem *, bool *) const;
    QPointF mapToItem(const QSGItem *item, const QPointF &point) const;
    QPointF mapToScene(const QPointF &point) const;
    QRectF mapRectToItem(const QSGItem *item, const QRectF &rect) const;
    QRectF mapRectToScene(const QRectF &rect) const;
    QPointF mapFromItem(const QSGItem *item, const QPointF &point) const;
    QPointF mapFromScene(const QPointF &point) const;
    QRectF mapRectFromItem(const QSGItem *item, const QRectF &rect) const;
    QRectF mapRectFromScene(const QRectF &rect) const;

    void polish();

    Q_INVOKABLE QScriptValue mapFromItem(const QScriptValue &item, qreal x, qreal y) const;
    Q_INVOKABLE QScriptValue mapToItem(const QScriptValue &item, qreal x, qreal y) const;
    Q_INVOKABLE void forceActiveFocus();
    Q_INVOKABLE QSGItem *childAt(qreal x, qreal y) const;

    Qt::InputMethodHints inputMethodHints() const;
    void setInputMethodHints(Qt::InputMethodHints hints);
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;

    struct UpdatePaintNodeData {
       QSGTransformNode *transformNode;
    private:
       friend class QSGCanvasPrivate;
       UpdatePaintNodeData();
    };

public Q_SLOTS:
    void update();
    void updateMicroFocus();

Q_SIGNALS:
    void childrenRectChanged(const QRectF &);
    void baselineOffsetChanged(qreal);
    void stateChanged(const QString &);
    void focusChanged(bool);
    void activeFocusChanged(bool);
    void parentChanged(QSGItem *);
    void transformOriginChanged(TransformOrigin);
    void smoothChanged(bool);
    void clipChanged(bool);

    // XXX todo
    void childrenChanged();
    void opacityChanged();
    void enabledChanged();
    void visibleChanged();
    void rotationChanged();
    void scaleChanged();

    void xChanged();
    void yChanged();
    void widthChanged();
    void heightChanged();
    void zChanged();
    void implicitWidthChanged();
    void implicitHeightChanged();

protected:
    virtual bool event(QEvent *);

    bool isComponentComplete() const;
    virtual void itemChange(ItemChange, const ItemChangeData &);

    void setImplicitWidth(qreal);
    bool widthValid() const; // ### better name?
    void setImplicitHeight(qreal);
    bool heightValid() const; // ### better name?

    virtual void classBegin();
    virtual void componentComplete();

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void inputMethodEvent(QInputMethodEvent *);
    virtual void focusInEvent(QFocusEvent *);
    virtual void focusOutEvent(QFocusEvent *);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseUngrabEvent(); // XXX todo - params?
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event);
    virtual void touchEvent(QTouchEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual bool childMouseEventFilter(QSGItem *, QEvent *);

    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);

    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    virtual void updatePolish();

protected:
    QSGItem(QSGItemPrivate &dd, QSGItem *parent = 0);

private:
    friend class QSGCanvas;
    friend class QSGCanvasPrivate;
    friend class QSGRenderer;
    Q_DISABLE_COPY(QSGItem)
    Q_DECLARE_PRIVATE(QSGItem)
};

// XXX todo
Q_DECLARE_OPERATORS_FOR_FLAGS(QSGItem::Flags)

#ifndef QT_NO_DEBUG_STREAM
QDebug Q_DECLARATIVE_EXPORT operator<<(QDebug debug, QSGItem *item);
#endif

QT_END_NAMESPACE

QML_DECLARE_TYPE(QSGItem)
QML_DECLARE_TYPE(QSGTransform)

QT_END_HEADER

#endif // QSGITEM_H
