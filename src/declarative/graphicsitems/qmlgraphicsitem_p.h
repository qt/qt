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

#ifndef QMLGRAPHICSITEM_P_H
#define QMLGRAPHICSITEM_P_H

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

#include "qmlgraphicsitem.h"

#include "qmlgraphicsanchors_p.h"
#include "qmlgraphicsanchors_p_p.h"
#include "qmlgraphicsitemchangelistener_p.h"
#include <private/qpodvector_p.h>

#include "../util/qmlstate_p.h"
#include "../util/qmlnullablevalue_p_p.h"
#include <qml.h>
#include <qmlcontext.h>

#include <QtCore/qlist.h>
#include <QtCore/qdebug.h>

#include <private/qgraphicsitem_p.h>

QT_BEGIN_NAMESPACE

class QNetworkReply;
class QmlGraphicsItemKeyFilter;

//### merge into private?
class QmlGraphicsContents : public QObject
{
    Q_OBJECT
public:
    QmlGraphicsContents();

    QRectF rectF() const;

    void setItem(QmlGraphicsItem *item);

public Q_SLOTS:
    void calcHeight();
    void calcWidth();

Q_SIGNALS:
    void rectChanged();

private:
    QmlGraphicsItem *m_item;
    qreal m_x;
    qreal m_y;
    qreal m_width;
    qreal m_height;
};

class QmlGraphicsItemPrivate : public QGraphicsItemPrivate
{
    Q_DECLARE_PUBLIC(QmlGraphicsItem)

public:
    QmlGraphicsItemPrivate()
    : _anchors(0), _contents(0),
      _baselineOffset(0),
      _anchorLines(0),
      _stateGroup(0), origin(QmlGraphicsItem::Center),
      widthValid(false), heightValid(false),
      _componentComplete(true), _keepMouse(false),
      smooth(false), keyHandler(0),
      width(0), height(0), implicitWidth(0), implicitHeight(0)
    {
        QGraphicsItemPrivate::acceptedMouseButtons = 0;
        QGraphicsItemPrivate::flags = QGraphicsItem::GraphicsItemFlags(
                                      QGraphicsItem::ItemHasNoContents
                                      | QGraphicsItem::ItemIsFocusable
                                      | QGraphicsItem::ItemNegativeZStacksBehindParent);

    }

    void init(QmlGraphicsItem *parent)
    {
        Q_Q(QmlGraphicsItem);

        if (parent)
            q->setParentItem(parent);
        _baselineOffset.invalidate();
        mouseSetsFocus = false;
    }

    QString _id;

    // data property
    static void data_append(QmlListProperty<QObject> *, QObject *);

    // resources property
    static QObject *resources_at(QmlListProperty<QObject> *, int);
    static void resources_append(QmlListProperty<QObject> *, QObject *);
    static int resources_count(QmlListProperty<QObject> *);

    // children property
    static QmlGraphicsItem *children_at(QmlListProperty<QmlGraphicsItem> *, int);
    static void children_append(QmlListProperty<QmlGraphicsItem> *, QmlGraphicsItem *);
    static int children_count(QmlListProperty<QmlGraphicsItem> *);

    // transform property
    static int transform_count(QmlListProperty<QGraphicsTransform> *list);
    static void transform_append(QmlListProperty<QGraphicsTransform> *list, QGraphicsTransform *);
    static QGraphicsTransform *transform_at(QmlListProperty<QGraphicsTransform> *list, int);
    static void transform_clear(QmlListProperty<QGraphicsTransform> *list);

    QmlGraphicsAnchors *anchors() {
        if (!_anchors) {
            Q_Q(QmlGraphicsItem);
            _anchors = new QmlGraphicsAnchors(q);
            if (!_componentComplete)
                _anchors->classBegin();
        }
        return _anchors;
    }
    QmlGraphicsAnchors *_anchors;
    QmlGraphicsContents *_contents;

    QmlNullableValue<qreal> _baselineOffset;

    struct AnchorLines {
        AnchorLines(QmlGraphicsItem *);
        QmlGraphicsAnchorLine left;
        QmlGraphicsAnchorLine right;
        QmlGraphicsAnchorLine hCenter;
        QmlGraphicsAnchorLine top;
        QmlGraphicsAnchorLine bottom;
        QmlGraphicsAnchorLine vCenter;
        QmlGraphicsAnchorLine baseline;
    };
    mutable AnchorLines *_anchorLines;
    AnchorLines *anchorLines() const {
        Q_Q(const QmlGraphicsItem);
        if (!_anchorLines) _anchorLines =
            new AnchorLines(const_cast<QmlGraphicsItem *>(q));
        return _anchorLines;
    }

    enum ChangeType {
        Geometry = 0x01,
        SiblingOrder = 0x02,
        Visibility = 0x04,
        Opacity = 0x08,
        Destroyed = 0x10
    };

    Q_DECLARE_FLAGS(ChangeTypes, ChangeType)

    struct ChangeListener {
        ChangeListener(QmlGraphicsItemChangeListener *l, QmlGraphicsItemPrivate::ChangeTypes t) : listener(l), types(t) {}
        QmlGraphicsItemChangeListener *listener;
        QmlGraphicsItemPrivate::ChangeTypes types;
        bool operator==(const ChangeListener &other) const { return listener == other.listener && types == other.types; }
    };

    void addItemChangeListener(QmlGraphicsItemChangeListener *listener, ChangeTypes types) {
        changeListeners.append(ChangeListener(listener, types));
    }
    void removeItemChangeListener(QmlGraphicsItemChangeListener *, ChangeTypes types);
    QPODVector<ChangeListener,4> changeListeners;

    QmlStateGroup *states();
    QmlStateGroup *_stateGroup;

    QmlGraphicsItem::TransformOrigin origin:4;
    bool widthValid:1;
    bool heightValid:1;
    bool _componentComplete:1;
    bool _keepMouse:1;
    bool smooth:1;

    QmlGraphicsItemKeyFilter *keyHandler;

    qreal width;
    qreal height;
    qreal implicitWidth;
    qreal implicitHeight;

    QPointF computeTransformOrigin() const;

    virtual void setPosHelper(const QPointF &pos)
    {
        Q_Q(QmlGraphicsItem);
        QRectF oldGeometry(this->pos.x(), this->pos.y(), width, height);
        QGraphicsItemPrivate::setPosHelper(pos);
        q->geometryChanged(QRectF(this->pos.x(), this->pos.y(), width, height), oldGeometry);
    }

    // Reimplemented from QGraphicsItemPrivate
    virtual void subFocusItemChange()
    {
        emit q_func()->wantsFocusChanged();
    }

    // Reimplemented from QGraphicsItemPrivate
    virtual void siblingOrderChange()
    {
        Q_Q(QmlGraphicsItem);
        for(int ii = 0; ii < changeListeners.count(); ++ii) {
            const QmlGraphicsItemPrivate::ChangeListener &change = changeListeners.at(ii);
            if (change.types & QmlGraphicsItemPrivate::SiblingOrder) {
                change.listener->itemSiblingOrderChanged(q);
            }
        }
    }

    static int consistentTime;
    static QTime currentTime();
    static void Q_DECLARATIVE_EXPORT setConsistentTime(int t);
    static void start(QTime &);
    static int elapsed(QTime &);
    static int restart(QTime &);
};

/*
    Key filters can be installed on a QmlGraphicsItem, but not removed.  Currently they
    are only used by attached objects (which are only destroyed on Item
    destruction), so this isn't a problem.  If in future this becomes any form
    of public API, they will have to support removal too.
*/
class QmlGraphicsItemKeyFilter
{
public:
    QmlGraphicsItemKeyFilter(QmlGraphicsItem * = 0);
    virtual ~QmlGraphicsItemKeyFilter();

    virtual void keyPressed(QKeyEvent *event);
    virtual void keyReleased(QKeyEvent *event);
    virtual void inputMethodEvent(QInputMethodEvent *event);
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
    virtual void componentComplete();

private:
    QmlGraphicsItemKeyFilter *m_next;
};

class QmlGraphicsKeyNavigationAttachedPrivate : public QObjectPrivate
{
public:
    QmlGraphicsKeyNavigationAttachedPrivate()
        : QObjectPrivate(), left(0), right(0), up(0), down(0) {}

    QmlGraphicsItem *left;
    QmlGraphicsItem *right;
    QmlGraphicsItem *up;
    QmlGraphicsItem *down;
};

class QmlGraphicsKeyNavigationAttached : public QObject, public QmlGraphicsItemKeyFilter
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlGraphicsKeyNavigationAttached)

    Q_PROPERTY(QmlGraphicsItem *left READ left WRITE setLeft NOTIFY changed)
    Q_PROPERTY(QmlGraphicsItem *right READ right WRITE setRight NOTIFY changed)
    Q_PROPERTY(QmlGraphicsItem *up READ up WRITE setUp NOTIFY changed)
    Q_PROPERTY(QmlGraphicsItem *down READ down WRITE setDown NOTIFY changed)
public:
    QmlGraphicsKeyNavigationAttached(QObject * = 0);

    QmlGraphicsItem *left() const;
    void setLeft(QmlGraphicsItem *);
    QmlGraphicsItem *right() const;
    void setRight(QmlGraphicsItem *);
    QmlGraphicsItem *up() const;
    void setUp(QmlGraphicsItem *);
    QmlGraphicsItem *down() const;
    void setDown(QmlGraphicsItem *);

    static QmlGraphicsKeyNavigationAttached *qmlAttachedProperties(QObject *);

Q_SIGNALS:
    void changed();

private:
    virtual void keyPressed(QKeyEvent *event);
    virtual void keyReleased(QKeyEvent *event);
};

class QmlGraphicsKeysAttachedPrivate : public QObjectPrivate
{
public:
    QmlGraphicsKeysAttachedPrivate()
        : QObjectPrivate(), inPress(false), inRelease(false)
        , inIM(false), enabled(true), imeItem(0), item(0)
    {}

    bool isConnected(const char *signalName);

    QGraphicsItem *finalFocusProxy(QGraphicsItem *item) const
    {
        QGraphicsItem *fp;
        while ((fp = item->focusProxy()))
            item = fp;
        return item;
    }

    //loop detection
    bool inPress:1;
    bool inRelease:1;
    bool inIM:1;

    bool enabled : 1;

    QGraphicsItem *imeItem;
    QList<QmlGraphicsItem *> targets;
    QmlGraphicsItem *item;
};

class QmlGraphicsKeysAttached : public QObject, public QmlGraphicsItemKeyFilter
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlGraphicsKeysAttached)

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QmlListProperty<QmlGraphicsItem> forwardTo READ forwardTo)

public:
    QmlGraphicsKeysAttached(QObject *parent=0);
    ~QmlGraphicsKeysAttached();

    bool enabled() const { Q_D(const QmlGraphicsKeysAttached); return d->enabled; }
    void setEnabled(bool enabled) {
        Q_D(QmlGraphicsKeysAttached);
        if (enabled != d->enabled) {
            d->enabled = enabled;
            emit enabledChanged();
        }
    }

    QmlListProperty<QmlGraphicsItem> forwardTo() {
        Q_D(QmlGraphicsKeysAttached);
        return QmlListProperty<QmlGraphicsItem>(this, d->targets);
    }

    virtual void componentComplete();

    static QmlGraphicsKeysAttached *qmlAttachedProperties(QObject *);

Q_SIGNALS:
    void enabledChanged();
    void pressed(QmlGraphicsKeyEvent *event);
    void released(QmlGraphicsKeyEvent *event);
    void digit0Pressed(QmlGraphicsKeyEvent *event);
    void digit1Pressed(QmlGraphicsKeyEvent *event);
    void digit2Pressed(QmlGraphicsKeyEvent *event);
    void digit3Pressed(QmlGraphicsKeyEvent *event);
    void digit4Pressed(QmlGraphicsKeyEvent *event);
    void digit5Pressed(QmlGraphicsKeyEvent *event);
    void digit6Pressed(QmlGraphicsKeyEvent *event);
    void digit7Pressed(QmlGraphicsKeyEvent *event);
    void digit8Pressed(QmlGraphicsKeyEvent *event);
    void digit9Pressed(QmlGraphicsKeyEvent *event);

    void leftPressed(QmlGraphicsKeyEvent *event);
    void rightPressed(QmlGraphicsKeyEvent *event);
    void upPressed(QmlGraphicsKeyEvent *event);
    void downPressed(QmlGraphicsKeyEvent *event);

    void asteriskPressed(QmlGraphicsKeyEvent *event);
    void numberSignPressed(QmlGraphicsKeyEvent *event);
    void escapePressed(QmlGraphicsKeyEvent *event);
    void returnPressed(QmlGraphicsKeyEvent *event);
    void enterPressed(QmlGraphicsKeyEvent *event);
    void deletePressed(QmlGraphicsKeyEvent *event);
    void spacePressed(QmlGraphicsKeyEvent *event);
    void backPressed(QmlGraphicsKeyEvent *event);
    void cancelPressed(QmlGraphicsKeyEvent *event);
    void selectPressed(QmlGraphicsKeyEvent *event);
    void yesPressed(QmlGraphicsKeyEvent *event);
    void noPressed(QmlGraphicsKeyEvent *event);
    void context1Pressed(QmlGraphicsKeyEvent *event);
    void context2Pressed(QmlGraphicsKeyEvent *event);
    void context3Pressed(QmlGraphicsKeyEvent *event);
    void context4Pressed(QmlGraphicsKeyEvent *event);
    void callPressed(QmlGraphicsKeyEvent *event);
    void hangupPressed(QmlGraphicsKeyEvent *event);
    void flipPressed(QmlGraphicsKeyEvent *event);
    void menuPressed(QmlGraphicsKeyEvent *event);
    void volumeUpPressed(QmlGraphicsKeyEvent *event);
    void volumeDownPressed(QmlGraphicsKeyEvent *event);

private:
    virtual void keyPressed(QKeyEvent *event);
    virtual void keyReleased(QKeyEvent *event);
    virtual void inputMethodEvent(QInputMethodEvent *);
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

Q_DECLARE_OPERATORS_FOR_FLAGS(QmlGraphicsItemPrivate::ChangeTypes);

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlGraphicsKeysAttached)
QML_DECLARE_TYPEINFO(QmlGraphicsKeysAttached, QML_HAS_ATTACHED_PROPERTIES)
QML_DECLARE_TYPE(QmlGraphicsKeyNavigationAttached)
QML_DECLARE_TYPEINFO(QmlGraphicsKeyNavigationAttached, QML_HAS_ATTACHED_PROPERTIES)

#endif // QMLGRAPHICSITEM_P_H
