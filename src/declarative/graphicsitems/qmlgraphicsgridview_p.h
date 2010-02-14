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

#ifndef QMLGRAPHICSGRIDVIEW_H
#define QMLGRAPHICSGRIDVIEW_H

#include "qmlgraphicsflickable_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QmlGraphicsVisualModel;
class QmlGraphicsGridViewAttached;
class QmlGraphicsGridViewPrivate;
class Q_DECLARATIVE_EXPORT QmlGraphicsGridView : public QmlGraphicsFlickable
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QmlGraphicsGridView)

    Q_PROPERTY(QVariant model READ model WRITE setModel)
    Q_PROPERTY(QmlComponent *delegate READ delegate WRITE setDelegate)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QmlGraphicsItem *currentItem READ currentItem NOTIFY currentIndexChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_PROPERTY(QmlComponent *highlight READ highlight WRITE setHighlight)
    Q_PROPERTY(QmlGraphicsItem *highlightItem READ highlightItem NOTIFY highlightChanged)
    Q_PROPERTY(bool highlightFollowsCurrentItem READ highlightFollowsCurrentItem WRITE setHighlightFollowsCurrentItem)

    Q_PROPERTY(Flow flow READ flow WRITE setFlow)
    Q_PROPERTY(bool keyNavigationWraps READ isWrapEnabled WRITE setWrapEnabled)
    Q_PROPERTY(int cacheBuffer READ cacheBuffer WRITE setCacheBuffer)
    Q_PROPERTY(int cellWidth READ cellWidth WRITE setCellWidth NOTIFY cellWidthChanged)
    Q_PROPERTY(int cellHeight READ cellHeight WRITE setCellHeight NOTIFY cellHeightChanged)
    Q_CLASSINFO("DefaultProperty", "data")

public:
    QmlGraphicsGridView(QmlGraphicsItem *parent=0);
    ~QmlGraphicsGridView();

    QVariant model() const;
    void setModel(const QVariant &);

    QmlComponent *delegate() const;
    void setDelegate(QmlComponent *);

    int currentIndex() const;
    void setCurrentIndex(int idx);

    QmlGraphicsItem *currentItem();
    QmlGraphicsItem *highlightItem();
    int count() const;

    QmlComponent *highlight() const;
    void setHighlight(QmlComponent *highlight);

    bool highlightFollowsCurrentItem() const;
    void setHighlightFollowsCurrentItem(bool);

    Q_ENUMS(Flow)
    enum Flow { LeftToRight, TopToBottom };
    Flow flow() const;
    void setFlow(Flow);

    bool isWrapEnabled() const;
    void setWrapEnabled(bool);

    int cacheBuffer() const;
    void setCacheBuffer(int);

    int cellWidth() const;
    void setCellWidth(int);

    int cellHeight() const;
    void setCellHeight(int);

    static QmlGraphicsGridViewAttached *qmlAttachedProperties(QObject *);

public Q_SLOTS:
    void moveCurrentIndexUp();
    void moveCurrentIndexDown();
    void moveCurrentIndexLeft();
    void moveCurrentIndexRight();
    void positionViewAtIndex(int index);

Q_SIGNALS:
    void countChanged();
    void currentIndexChanged();
    void cellWidthChanged();
    void cellHeightChanged();
    void highlightChanged();

protected:
    virtual void viewportMoved();
    virtual qreal minYExtent() const;
    virtual qreal maxYExtent() const;
    virtual qreal minXExtent() const;
    virtual qreal maxXExtent() const;
    virtual void keyPressEvent(QKeyEvent *);
    virtual void componentComplete();

private Q_SLOTS:
    void trackedPositionChanged();
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void itemsMoved(int from, int to, int count);
    void destroyRemoved();
    void createdItem(int index, QmlGraphicsItem *item);
    void destroyingItem(QmlGraphicsItem *item);
    void sizeChange();
    void layout();

private:
    void refill();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlGraphicsGridView)
QML_DECLARE_TYPEINFO(QmlGraphicsGridView, QML_HAS_ATTACHED_PROPERTIES)

QT_END_HEADER

#endif
