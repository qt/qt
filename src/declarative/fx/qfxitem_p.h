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

#ifndef QFXITEM_P_H
#define QFXITEM_P_H

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

#include <QtDeclarative/qfxitem.h>
#include <QtDeclarative/qmlstate.h>
#include <QtDeclarative/qfxanchors.h>
#include <private/qmlnullablevalue_p.h>
#include <private/qfxanchors_p.h>
#include <QtDeclarative/qml.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtCore/qlist.h>
#include <private/qgraphicsitem_p.h>
#include <QtCore/qdebug.h>

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
    {}
    ~QmlGraphicsItemPrivate()
    { delete _anchors; }

    void init(QmlGraphicsItem *parent)
    {
        Q_Q(QmlGraphicsItem);

        if (parent)
            q->setParentItem(parent);
        _baselineOffset.invalidate();
        q->setAcceptedMouseButtons(Qt::NoButton);
        q->setFlags(QGraphicsItem::ItemHasNoContents |
                    QGraphicsItem::ItemIsFocusable |
                    QGraphicsItem::ItemNegativeZStacksBehindParent);
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
            _anchors = new QmlGraphicsAnchors;
            _anchors->setItem(q);
            if (!_componentComplete)
                _anchors->classBegin();
        }
        return _anchors;
    }
    QList<QmlGraphicsAnchors *> dependantAnchors;
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

    static int consistentTime;
    static QTime currentTime();
    static void Q_DECLARATIVE_EXPORT setConsistentTime(int t);
    static void start(QTime &);
    static int elapsed(QTime &);
    static int restart(QTime &);
};

QT_END_NAMESPACE

#endif // QFXITEM_P_H
