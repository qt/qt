// Commit: 5c783d0a9a912816813945387903857a314040b5
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

#ifndef QSGITEM_P_H
#define QSGITEM_P_H

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

#include "qsganchors_p.h"
#include "qsganchors_p_p.h"
#include "qsgitemchangelistener_p.h"

#include "qsgcanvas_p.h"

#include "qsgnode.h"
#include "qsgclipnode_p.h"

#include <private/qpodvector_p.h>
#include <private/qdeclarativestate_p.h>
#include <private/qdeclarativenullablevalue_p_p.h>
#include <private/qdeclarativenotifier_p.h>
#include <private/qdeclarativeglobal_p.h>

#include <qdeclarative.h>
#include <qdeclarativecontext.h>

#include <QtCore/qlist.h>
#include <QtCore/qdebug.h>
#include <QtCore/qelapsedtimer.h>

QT_BEGIN_NAMESPACE

class QNetworkReply;
class QSGItemKeyFilter;
class QSGLayoutMirroringAttached;

//### merge into private?
class QSGContents : public QObject, public QSGItemChangeListener
{
    Q_OBJECT
public:
    QSGContents(QSGItem *item);
    ~QSGContents();

    QRectF rectF() const;

    void childRemoved(QSGItem *item);
    void childAdded(QSGItem *item);

    void calcGeometry() { calcWidth(); calcHeight(); }
    void complete();

Q_SIGNALS:
    void rectChanged(QRectF);

protected:
    void itemGeometryChanged(QSGItem *item, const QRectF &newGeometry, const QRectF &oldGeometry);
    void itemDestroyed(QSGItem *item);
    //void itemVisibilityChanged(QSGItem *item)

private:
    void calcHeight(QSGItem *changed = 0);
    void calcWidth(QSGItem *changed = 0);

    QSGItem *m_item;
    qreal m_x;
    qreal m_y;
    qreal m_width;
    qreal m_height;
};

class QSGTransformPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QSGTransform);
public:
    static QSGTransformPrivate* get(QSGTransform *transform) { return transform->d_func(); }

    QSGTransformPrivate();

    QList<QSGItem *> items;
};

class Q_DECLARATIVE_EXPORT QSGItemPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QSGItem)

public:
    static QSGItemPrivate* get(QSGItem *item) { return item->d_func(); }
    static const QSGItemPrivate* get(const QSGItem *item) { return item->d_func(); }

    QSGItemPrivate();
    void init(QSGItem *parent);

    QDeclarativeListProperty<QObject> data();
    QDeclarativeListProperty<QObject> resources();
    QDeclarativeListProperty<QSGItem> children();

    QDeclarativeListProperty<QDeclarativeState> states();
    QDeclarativeListProperty<QDeclarativeTransition> transitions();

    QString state() const;
    void setState(const QString &);

    QSGAnchorLine left() const;
    QSGAnchorLine right() const;
    QSGAnchorLine horizontalCenter() const;
    QSGAnchorLine top() const;
    QSGAnchorLine bottom() const;
    QSGAnchorLine verticalCenter() const;
    QSGAnchorLine baseline() const;

    // data property
    static void data_append(QDeclarativeListProperty<QObject> *, QObject *);
    static int data_count(QDeclarativeListProperty<QObject> *);
    static QObject *data_at(QDeclarativeListProperty<QObject> *, int);
    static void data_clear(QDeclarativeListProperty<QObject> *);

    // resources property
    static QObject *resources_at(QDeclarativeListProperty<QObject> *, int);
    static void resources_append(QDeclarativeListProperty<QObject> *, QObject *);
    static int resources_count(QDeclarativeListProperty<QObject> *);
    static void resources_clear(QDeclarativeListProperty<QObject> *);

    // children property
    static void children_append(QDeclarativeListProperty<QSGItem> *, QSGItem *);
    static int children_count(QDeclarativeListProperty<QSGItem> *);
    static QSGItem *children_at(QDeclarativeListProperty<QSGItem> *, int);
    static void children_clear(QDeclarativeListProperty<QSGItem> *);

    // transform property
    static int transform_count(QDeclarativeListProperty<QSGTransform> *list);
    static void transform_append(QDeclarativeListProperty<QSGTransform> *list, QSGTransform *);
    static QSGTransform *transform_at(QDeclarativeListProperty<QSGTransform> *list, int);
    static void transform_clear(QDeclarativeListProperty<QSGTransform> *list);

    QSGAnchors *anchors() const;
    mutable QSGAnchors *_anchors;
    QSGContents *_contents;

    QDeclarativeNullableValue<qreal> baselineOffset;

    struct AnchorLines {
        AnchorLines(QSGItem *);
        QSGAnchorLine left;
        QSGAnchorLine right;
        QSGAnchorLine hCenter;
        QSGAnchorLine top;
        QSGAnchorLine bottom;
        QSGAnchorLine vCenter;
        QSGAnchorLine baseline;
    };
    mutable AnchorLines *_anchorLines;
    AnchorLines *anchorLines() const;

    enum ChangeType {
        Geometry = 0x01,
        SiblingOrder = 0x02,
        Visibility = 0x04,
        Opacity = 0x08,
        Destroyed = 0x10,
        Parent = 0x20,
        Children = 0x40,
        Rotation = 0x80,
    };

    Q_DECLARE_FLAGS(ChangeTypes, ChangeType)

    struct ChangeListener {
        ChangeListener(QSGItemChangeListener *l, QSGItemPrivate::ChangeTypes t) : listener(l), types(t) {}
        QSGItemChangeListener *listener;
        QSGItemPrivate::ChangeTypes types;
        bool operator==(const ChangeListener &other) const { return listener == other.listener && types == other.types; }
    };

    void addItemChangeListener(QSGItemChangeListener *listener, ChangeTypes types) {
        changeListeners.append(ChangeListener(listener, types));
    }
    void removeItemChangeListener(QSGItemChangeListener *, ChangeTypes types);
    QPODVector<ChangeListener,4> changeListeners;

    QDeclarativeStateGroup *_states();
    QDeclarativeStateGroup *_stateGroup;

    QSGItem::TransformOrigin origin:5;
    quint32 flags:4;
    bool widthValid:1;
    bool heightValid:1;
    bool componentComplete:1;
    bool keepMouse:1;
    bool hoverEnabled:1;
    bool smooth:1;
    bool focus:1;
    bool activeFocus:1;
    bool notifiedFocus:1;
    bool notifiedActiveFocus:1;
    bool filtersChildMouseEvents:1;
    bool explicitVisible:1;
    bool effectiveVisible:1;
    bool explicitEnable:1;
    bool effectiveEnable:1;
    bool polishScheduled:1;
    bool inheritedLayoutMirror:1;
    bool effectiveLayoutMirror:1;
    bool isMirrorImplicit:1;
    bool inheritMirrorFromParent:1;
    bool inheritMirrorFromItem:1;
    quint32 dummy:2;

    QSGCanvas *canvas;
    QSGContext *sceneGraphContext() const { return static_cast<QSGCanvasPrivate *>(QObjectPrivate::get(canvas))->context; }

    QSGItem *parentItem;
    QList<QSGItem *> childItems;
    QList<QSGItem *> paintOrderChildItems() const;
    void addChild(QSGItem *);
    void removeChild(QSGItem *);
    void siblingOrderChanged();

    class InitializationState {
    public:
        QSGItem *getFocusScope(QSGItem *item);
        void clear();
        void clear(QSGItem *focusScope);
    private:
        QSGItem *focusScope;
    };
    void initCanvas(InitializationState *, QSGCanvas *);

    QSGItem *subFocusItem;

    QTransform canvasToItemTransform() const;
    QTransform itemToCanvasTransform() const;
    void itemToParentTransform(QTransform &) const;

    qreal x;
    qreal y;
    qreal width;
    qreal height;
    qreal implicitWidth;
    qreal implicitHeight;

    qreal z;
    qreal scale;
    qreal rotation;
    qreal opacity;

    QSGLayoutMirroringAttached* attachedLayoutDirection;

    Qt::MouseButtons acceptedMouseButtons;
    Qt::InputMethodHints imHints;

    virtual qreal getImplicitWidth() const;
    virtual qreal getImplicitHeight() const;
    virtual void implicitWidthChanged();
    virtual void implicitHeightChanged();

    void resolveLayoutMirror();
    void setImplicitLayoutMirror(bool mirror, bool inherit);
    void setLayoutMirror(bool mirror);
    bool isMirrored() const {
        return effectiveLayoutMirror;
    }

    QPointF computeTransformOrigin() const;
    QList<QSGTransform *> transforms;
    virtual void transformChanged();

    QSGItemKeyFilter *keyHandler;
    void deliverKeyEvent(QKeyEvent *);
    void deliverInputMethodEvent(QInputMethodEvent *);
    void deliverFocusEvent(QFocusEvent *);
    void deliverMouseEvent(QGraphicsSceneMouseEvent *);
    void deliverWheelEvent(QGraphicsSceneWheelEvent *);
    void deliverTouchEvent(QTouchEvent *);
    void deliverHoverEvent(QGraphicsSceneHoverEvent *);

    bool calcEffectiveVisible() const;
    void setEffectiveVisibleRecur(bool);
    bool calcEffectiveEnable() const;
    void setEffectiveEnableRecur(bool);

    // XXX todo
    enum DirtyType {
        TransformOrigin         = 0x00000001,
        Transform               = 0x00000002,
        BasicTransform          = 0x00000004,
        Position                = 0x00000008,
        Size                    = 0x00000010,

        ZValue                  = 0x00000020,
        Content                 = 0x00000040,
        Smooth                  = 0x00000080,
        OpacityValue            = 0x00000100,
        ChildrenChanged         = 0x00000200,
        ChildrenStackingChanged = 0x00000400,
        ParentChanged           = 0x00000800,

        Clip                    = 0x00001000,
        Canvas                  = 0x00002000,

        EffectReference         = 0x00008000,
        Visible                 = 0x00010000,
        HideReference           = 0x00020000,
        // When you add an attribute here, don't forget to update
        // dirtyToString()

        TransformUpdateMask     = TransformOrigin | Transform | BasicTransform | Position | Size | Canvas,
        ComplexTransformUpdateMask     = Transform | Canvas,
        ContentUpdateMask       = Size | Content | Smooth | Canvas,
        ChildrenUpdateMask      = ChildrenChanged | ChildrenStackingChanged | EffectReference | Canvas,

    };
    quint32 dirtyAttributes;
    QString dirtyToString() const;
    void dirty(DirtyType);
    void addToDirtyList();
    void removeFromDirtyList();
    QSGItem *nextDirtyItem;
    QSGItem**prevDirtyItem;

    inline QSGTransformNode *itemNode();
    inline QSGNode *childContainerNode();

    /*
      QSGNode order is:
         - itemNode
         - (opacityNode)
         - (clipNode)
         - (effectNode)
         - groupNode
     */

    QSGTransformNode *itemNodeInstance;
    QSGOpacityNode *opacityNode;
    QSGDefaultClipNode *clipNode;
    QSGRootNode *rootNode;
    QSGNode *groupNode;
    QSGNode *paintNode;
    int paintNodeIndex;

    virtual QSGTransformNode *createTransformNode();

    // A reference from an effect item means that this item is used by the effect, so
    // it should insert a root node.
    void refFromEffectItem(bool hide);
    void derefFromEffectItem(bool unhide);
    int effectRefCount;
    int hideRefCount;

    void itemChange(QSGItem::ItemChange, const QSGItem::ItemChangeData &);

    virtual void mirrorChange() {}

    static qint64 consistentTime;
    static void setConsistentTime(qint64 t);
    static void start(QElapsedTimer &);
    static qint64 elapsed(QElapsedTimer &);
    static qint64 restart(QElapsedTimer &);
};

/*
    Key filters can be installed on a QSGItem, but not removed.  Currently they
    are only used by attached objects (which are only destroyed on Item
    destruction), so this isn't a problem.  If in future this becomes any form
    of public API, they will have to support removal too.
*/
class QSGItemKeyFilter
{
public:
    QSGItemKeyFilter(QSGItem * = 0);
    virtual ~QSGItemKeyFilter();

    virtual void keyPressed(QKeyEvent *event, bool post);
    virtual void keyReleased(QKeyEvent *event, bool post);
    virtual void inputMethodEvent(QInputMethodEvent *event, bool post);
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
    virtual void componentComplete();

    bool m_processPost;

private:
    QSGItemKeyFilter *m_next;
};

class QSGKeyNavigationAttachedPrivate : public QObjectPrivate
{
public:
    QSGKeyNavigationAttachedPrivate()
        : QObjectPrivate(),
          left(0), right(0), up(0), down(0), tab(0), backtab(0),
          leftSet(false), rightSet(false), upSet(false), downSet(false),
          tabSet(false), backtabSet(false) {}

    QSGItem *left;
    QSGItem *right;
    QSGItem *up;
    QSGItem *down;
    QSGItem *tab;
    QSGItem *backtab;
    bool leftSet : 1;
    bool rightSet : 1;
    bool upSet : 1;
    bool downSet : 1;
    bool tabSet : 1;
    bool backtabSet : 1;
};

class QSGKeyNavigationAttached : public QObject, public QSGItemKeyFilter
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSGKeyNavigationAttached)

    Q_PROPERTY(QSGItem *left READ left WRITE setLeft NOTIFY leftChanged)
    Q_PROPERTY(QSGItem *right READ right WRITE setRight NOTIFY rightChanged)
    Q_PROPERTY(QSGItem *up READ up WRITE setUp NOTIFY upChanged)
    Q_PROPERTY(QSGItem *down READ down WRITE setDown NOTIFY downChanged)
    Q_PROPERTY(QSGItem *tab READ tab WRITE setTab NOTIFY tabChanged)
    Q_PROPERTY(QSGItem *backtab READ backtab WRITE setBacktab NOTIFY backtabChanged)
    Q_PROPERTY(Priority priority READ priority WRITE setPriority NOTIFY priorityChanged)

    Q_ENUMS(Priority)

public:
    QSGKeyNavigationAttached(QObject * = 0);

    QSGItem *left() const;
    void setLeft(QSGItem *);
    QSGItem *right() const;
    void setRight(QSGItem *);
    QSGItem *up() const;
    void setUp(QSGItem *);
    QSGItem *down() const;
    void setDown(QSGItem *);
    QSGItem *tab() const;
    void setTab(QSGItem *);
    QSGItem *backtab() const;
    void setBacktab(QSGItem *);

    enum Priority { BeforeItem, AfterItem };
    Priority priority() const;
    void setPriority(Priority);

    static QSGKeyNavigationAttached *qmlAttachedProperties(QObject *);

Q_SIGNALS:
    void leftChanged();
    void rightChanged();
    void upChanged();
    void downChanged();
    void tabChanged();
    void backtabChanged();
    void priorityChanged();

private:
    virtual void keyPressed(QKeyEvent *event, bool post);
    virtual void keyReleased(QKeyEvent *event, bool post);
    void setFocusNavigation(QSGItem *currentItem, const char *dir);
};

class QSGLayoutMirroringAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled RESET resetEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool childrenInherit READ childrenInherit WRITE setChildrenInherit NOTIFY childrenInheritChanged)

public:
    explicit QSGLayoutMirroringAttached(QObject *parent = 0);

    bool enabled() const;
    void setEnabled(bool);
    void resetEnabled();

    bool childrenInherit() const;
    void setChildrenInherit(bool);

    static QSGLayoutMirroringAttached *qmlAttachedProperties(QObject *);
Q_SIGNALS:
    void enabledChanged();
    void childrenInheritChanged();
private:
    friend class QSGItemPrivate;
    QSGItemPrivate *itemPrivate;
};

class QSGKeysAttachedPrivate : public QObjectPrivate
{
public:
    QSGKeysAttachedPrivate()
        : QObjectPrivate(), inPress(false), inRelease(false)
        , inIM(false), enabled(true), imeItem(0), item(0)
    {}

    bool isConnected(const char *signalName);

    //loop detection
    bool inPress:1;
    bool inRelease:1;
    bool inIM:1;

    bool enabled : 1;

    QSGItem *imeItem;
    QList<QSGItem *> targets;
    QSGItem *item;
};

class QSGKeysAttached : public QObject, public QSGItemKeyFilter
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSGKeysAttached)

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QDeclarativeListProperty<QSGItem> forwardTo READ forwardTo)
    Q_PROPERTY(Priority priority READ priority WRITE setPriority NOTIFY priorityChanged)

    Q_ENUMS(Priority)

public:
    QSGKeysAttached(QObject *parent=0);
    ~QSGKeysAttached();

    bool enabled() const { Q_D(const QSGKeysAttached); return d->enabled; }
    void setEnabled(bool enabled) {
        Q_D(QSGKeysAttached);
        if (enabled != d->enabled) {
            d->enabled = enabled;
            emit enabledChanged();
        }
    }

    enum Priority { BeforeItem, AfterItem};
    Priority priority() const;
    void setPriority(Priority);

    QDeclarativeListProperty<QSGItem> forwardTo() {
        Q_D(QSGKeysAttached);
        return QDeclarativeListProperty<QSGItem>(this, d->targets);
    }

    virtual void componentComplete();

    static QSGKeysAttached *qmlAttachedProperties(QObject *);

Q_SIGNALS:
    void enabledChanged();
    void priorityChanged();
    void pressed(QSGKeyEvent *event);
    void released(QSGKeyEvent *event);
    void digit0Pressed(QSGKeyEvent *event);
    void digit1Pressed(QSGKeyEvent *event);
    void digit2Pressed(QSGKeyEvent *event);
    void digit3Pressed(QSGKeyEvent *event);
    void digit4Pressed(QSGKeyEvent *event);
    void digit5Pressed(QSGKeyEvent *event);
    void digit6Pressed(QSGKeyEvent *event);
    void digit7Pressed(QSGKeyEvent *event);
    void digit8Pressed(QSGKeyEvent *event);
    void digit9Pressed(QSGKeyEvent *event);

    void leftPressed(QSGKeyEvent *event);
    void rightPressed(QSGKeyEvent *event);
    void upPressed(QSGKeyEvent *event);
    void downPressed(QSGKeyEvent *event);
    void tabPressed(QSGKeyEvent *event);
    void backtabPressed(QSGKeyEvent *event);

    void asteriskPressed(QSGKeyEvent *event);
    void numberSignPressed(QSGKeyEvent *event);
    void escapePressed(QSGKeyEvent *event);
    void returnPressed(QSGKeyEvent *event);
    void enterPressed(QSGKeyEvent *event);
    void deletePressed(QSGKeyEvent *event);
    void spacePressed(QSGKeyEvent *event);
    void backPressed(QSGKeyEvent *event);
    void cancelPressed(QSGKeyEvent *event);
    void selectPressed(QSGKeyEvent *event);
    void yesPressed(QSGKeyEvent *event);
    void noPressed(QSGKeyEvent *event);
    void context1Pressed(QSGKeyEvent *event);
    void context2Pressed(QSGKeyEvent *event);
    void context3Pressed(QSGKeyEvent *event);
    void context4Pressed(QSGKeyEvent *event);
    void callPressed(QSGKeyEvent *event);
    void hangupPressed(QSGKeyEvent *event);
    void flipPressed(QSGKeyEvent *event);
    void menuPressed(QSGKeyEvent *event);
    void volumeUpPressed(QSGKeyEvent *event);
    void volumeDownPressed(QSGKeyEvent *event);

private:
    virtual void keyPressed(QKeyEvent *event, bool post);
    virtual void keyReleased(QKeyEvent *event, bool post);
    virtual void inputMethodEvent(QInputMethodEvent *, bool post);
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;

    const QByteArray keyToSignal(int key) {
        QByteArray keySignal;
        if (key >= Qt::Key_0 && key <= Qt::Key_9) {
            keySignal = "digit0Pressed";
            keySignal[5] = '0' + (key - Qt::Key_0);
        } else {
            int i = 0;
            while (sigMap[i].key && sigMap[i].key != key)
                ++i;
            keySignal = sigMap[i].sig;
        }
        return keySignal;
    }

    struct SigMap {
        int key;
        const char *sig;
    };

    static const SigMap sigMap[];
};

QSGTransformNode *QSGItemPrivate::itemNode()
{ 
    if (!itemNodeInstance) {
        itemNodeInstance = createTransformNode();
#ifdef QML_RUNTIME_TESTING
        Q_Q(QSGItem);
        itemNodeInstance->description = QString::fromLatin1("QSGItem(%1)").arg(QString::fromLatin1(q->metaObject()->className()));
#endif
    }
    return itemNodeInstance; 
}

QSGNode *QSGItemPrivate::childContainerNode()
{
    if (!groupNode) {
        groupNode = new QSGNode();
        if (rootNode)
            rootNode->appendChildNode(groupNode);
        else if (clipNode)
            clipNode->appendChildNode(groupNode);
        else if (opacityNode)
            opacityNode->appendChildNode(groupNode);
        else
            itemNode()->appendChildNode(groupNode);
#ifdef QML_RUNTIME_TESTING
        groupNode->description = QLatin1String("group");
#endif
    }
    return groupNode;
}

Q_DECLARE_OPERATORS_FOR_FLAGS(QSGItemPrivate::ChangeTypes);

QT_END_NAMESPACE

QML_DECLARE_TYPE(QSGKeysAttached)
QML_DECLARE_TYPEINFO(QSGKeysAttached, QML_HAS_ATTACHED_PROPERTIES)
QML_DECLARE_TYPE(QSGKeyNavigationAttached)
QML_DECLARE_TYPEINFO(QSGKeyNavigationAttached, QML_HAS_ATTACHED_PROPERTIES)
QML_DECLARE_TYPE(QSGLayoutMirroringAttached)
QML_DECLARE_TYPEINFO(QSGLayoutMirroringAttached, QML_HAS_ATTACHED_PROPERTIES)

#endif // QSGITEM_P_H
