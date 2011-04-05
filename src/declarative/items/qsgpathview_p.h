// Commit: ac5c099cc3c5b8c7eec7a49fdeb8a21037230350
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

#ifndef QSGPATHVIEW_P_H
#define QSGPATHVIEW_P_H

#include "qsgitem.h"

#include <private/qdeclarativepath_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGPathViewPrivate;
class QSGPathViewAttached;
class Q_AUTOTEST_EXPORT QSGPathView : public QSGItem
{
    Q_OBJECT

    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativePath *path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(qreal offset READ offset WRITE setOffset NOTIFY offsetChanged)

    Q_PROPERTY(QDeclarativeComponent *highlight READ highlight WRITE setHighlight NOTIFY highlightChanged)
    Q_PROPERTY(QSGItem *highlightItem READ highlightItem NOTIFY highlightItemChanged)

    Q_PROPERTY(qreal preferredHighlightBegin READ preferredHighlightBegin WRITE setPreferredHighlightBegin NOTIFY preferredHighlightBeginChanged)
    Q_PROPERTY(qreal preferredHighlightEnd READ preferredHighlightEnd WRITE setPreferredHighlightEnd NOTIFY preferredHighlightEndChanged)
    Q_PROPERTY(HighlightRangeMode highlightRangeMode READ highlightRangeMode WRITE setHighlightRangeMode NOTIFY highlightRangeModeChanged)
    Q_PROPERTY(int highlightMoveDuration READ highlightMoveDuration WRITE setHighlightMoveDuration NOTIFY highlightMoveDurationChanged)

    Q_PROPERTY(qreal dragMargin READ dragMargin WRITE setDragMargin NOTIFY dragMarginChanged)
    Q_PROPERTY(qreal flickDeceleration READ flickDeceleration WRITE setFlickDeceleration NOTIFY flickDecelerationChanged)
    Q_PROPERTY(bool interactive READ isInteractive WRITE setInteractive NOTIFY interactiveChanged)

    Q_PROPERTY(bool moving READ isMoving NOTIFY movingChanged)
    Q_PROPERTY(bool flicking READ isFlicking NOTIFY flickingChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QDeclarativeComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(int pathItemCount READ pathItemCount WRITE setPathItemCount NOTIFY pathItemCountChanged)

    Q_ENUMS(HighlightRangeMode)

public:
    QSGPathView(QSGItem *parent=0);
    virtual ~QSGPathView();

    QVariant model() const;
    void setModel(const QVariant &);

    QDeclarativePath *path() const;
    void setPath(QDeclarativePath *);

    int currentIndex() const;
    void setCurrentIndex(int idx);

    qreal offset() const;
    void setOffset(qreal offset);

    QDeclarativeComponent *highlight() const;
    void setHighlight(QDeclarativeComponent *highlight);
    QSGItem *highlightItem();

    enum HighlightRangeMode { NoHighlightRange, ApplyRange, StrictlyEnforceRange };
    HighlightRangeMode highlightRangeMode() const;
    void setHighlightRangeMode(HighlightRangeMode mode);

    qreal preferredHighlightBegin() const;
    void setPreferredHighlightBegin(qreal);

    qreal preferredHighlightEnd() const;
    void setPreferredHighlightEnd(qreal);

    int highlightMoveDuration() const;
    void setHighlightMoveDuration(int);

    qreal dragMargin() const;
    void setDragMargin(qreal margin);

    qreal flickDeceleration() const;
    void setFlickDeceleration(qreal dec);

    bool isInteractive() const;
    void setInteractive(bool);

    bool isMoving() const;
    bool isFlicking() const;

    int count() const;

    QDeclarativeComponent *delegate() const;
    void setDelegate(QDeclarativeComponent *);

    int pathItemCount() const;
    void setPathItemCount(int);

    static QSGPathViewAttached *qmlAttachedProperties(QObject *);

public Q_SLOTS:
    void incrementCurrentIndex();
    void decrementCurrentIndex();

Q_SIGNALS:
    void currentIndexChanged();
    void offsetChanged();
    void modelChanged();
    void countChanged();
    void pathChanged();
    void preferredHighlightBeginChanged();
    void preferredHighlightEndChanged();
    void highlightRangeModeChanged();
    void dragMarginChanged();
    void snapPositionChanged();
    void delegateChanged();
    void pathItemCountChanged();
    void flickDecelerationChanged();
    void interactiveChanged();
    void movingChanged();
    void flickingChanged();
    void highlightChanged();
    void highlightItemChanged();
    void highlightMoveDurationChanged();
    void movementStarted();
    void movementEnded();
    void flickStarted();
    void flickEnded();

protected:
    virtual void updatePolish();
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
    bool sendMouseEvent(QGraphicsSceneMouseEvent *event);
    bool childMouseEventFilter(QSGItem *, QEvent *);
    void componentComplete();

private Q_SLOTS:
    void refill();
    void ticked();
    void movementEnding();
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void itemsMoved(int,int,int);
    void modelReset();
    void createdItem(int index, QSGItem *item);
    void destroyingItem(QSGItem *item);
    void pathUpdated();

private:
    friend class QSGPathViewAttached;
    Q_DISABLE_COPY(QSGPathView)
    Q_DECLARE_PRIVATE(QSGPathView)
};

class QDeclarativeOpenMetaObject;
class QSGPathViewAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QSGPathView *view READ view CONSTANT)
    Q_PROPERTY(bool isCurrentItem READ isCurrentItem NOTIFY currentItemChanged)
    Q_PROPERTY(bool onPath READ isOnPath NOTIFY pathChanged)

public:
    QSGPathViewAttached(QObject *parent);
    ~QSGPathViewAttached();

    QSGPathView *view() { return m_view; }

    bool isCurrentItem() const { return m_isCurrent; }
    void setIsCurrentItem(bool c) {
        if (m_isCurrent != c) {
            m_isCurrent = c;
            emit currentItemChanged();
        }
    }

    QVariant value(const QByteArray &name) const;
    void setValue(const QByteArray &name, const QVariant &val);

    bool isOnPath() const { return m_onPath; }
    void setOnPath(bool on) {
        if (on != m_onPath) {
            m_onPath = on;
            emit pathChanged();
        }
    }
    qreal m_percent;

Q_SIGNALS:
    void currentItemChanged();
    void pathChanged();

private:
    friend class QSGPathViewPrivate;
    friend class QSGPathView;
    QSGPathView *m_view;
    QDeclarativeOpenMetaObject *m_metaobject;
    bool m_onPath : 1;
    bool m_isCurrent : 1;
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QSGPathView)
QML_DECLARE_TYPEINFO(QSGPathView, QML_HAS_ATTACHED_PROPERTIES)
QT_END_HEADER

#endif // QSGPATHVIEW_P_H
