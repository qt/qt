/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
      _stateGroup(0), origin(QmlGraphicsItem::TopLeft),
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
    void data_removeAt(int);
    int data_count() const;
    void data_append(QObject *);
    void data_insert(int, QObject *);
    QObject *data_at(int) const;
    void data_clear();
    QML_DECLARE_LIST_PROXY(QmlGraphicsItemPrivate, QObject *, data)

    // resources property
    void resources_removeAt(int);
    int resources_count() const;
    void resources_append(QObject *);
    void resources_insert(int, QObject *);
    QObject *resources_at(int) const;
    void resources_clear();
    QML_DECLARE_LIST_PROXY(QmlGraphicsItemPrivate, QObject *, resources)

    // children property
    void children_removeAt(int);
    int children_count() const;
    void children_append(QmlGraphicsItem *);
    void children_insert(int, QmlGraphicsItem *);
    QmlGraphicsItem *children_at(int) const;
    void children_clear();
    QML_DECLARE_LIST_PROXY(QmlGraphicsItemPrivate, QmlGraphicsItem *, children)

    // transform property
    void transform_removeAt(int);
    int transform_count() const;
    void transform_append(QGraphicsTransform *);
    void transform_insert(int, QGraphicsTransform *);
    QGraphicsTransform *transform_at(int) const;
    void transform_clear();
    QML_DECLARE_LIST_PROXY(QmlGraphicsItemPrivate, QGraphicsTransform *, transform)

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

Q_DECLARE_OPERATORS_FOR_FLAGS(QmlGraphicsItemPrivate::ChangeTypes);

QT_END_NAMESPACE

#endif // QMLGRAPHICSITEM_P_H
