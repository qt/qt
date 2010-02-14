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

#ifndef QMLGRAPHICSLISTVIEW_H
#define QMLGRAPHICSLISTVIEW_H

#include "qmlgraphicsflickable_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class Q_DECLARATIVE_EXPORT QmlGraphicsViewSection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString property READ property WRITE setProperty NOTIFY changed)
    Q_PROPERTY(SectionCriteria criteria READ criteria WRITE setCriteria NOTIFY changed)
    Q_PROPERTY(QmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_ENUMS(SectionCriteria)
public:
    QmlGraphicsViewSection(QObject *parent=0) : QObject(parent), m_criteria(FullString), m_delegate(0) {}

    QString property() const { return m_property; }
    void setProperty(const QString &);

    enum SectionCriteria { FullString, FirstCharacter };
    SectionCriteria criteria() const { return m_criteria; }
    void setCriteria(SectionCriteria);

    QmlComponent *delegate() const { return m_delegate; }
    void setDelegate(QmlComponent *delegate);

    QString sectionString(const QString &value);

Q_SIGNALS:
    void changed();
    void delegateChanged();

private:
    QString m_property;
    SectionCriteria m_criteria;
    QmlComponent *m_delegate;
};


class QmlGraphicsVisualModel;
class QmlGraphicsListViewAttached;
class QmlGraphicsListViewPrivate;
class Q_DECLARATIVE_EXPORT QmlGraphicsListView : public QmlGraphicsFlickable
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QmlGraphicsListView)

    Q_PROPERTY(QVariant model READ model WRITE setModel)
    Q_PROPERTY(QmlComponent *delegate READ delegate WRITE setDelegate)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QmlGraphicsItem *currentItem READ currentItem NOTIFY currentIndexChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_PROPERTY(QmlComponent *highlight READ highlight WRITE setHighlight)
    Q_PROPERTY(QmlGraphicsItem *highlightItem READ highlightItem NOTIFY highlightChanged)
    Q_PROPERTY(bool highlightFollowsCurrentItem READ highlightFollowsCurrentItem WRITE setHighlightFollowsCurrentItem)
    Q_PROPERTY(qreal highlightMoveSpeed READ highlightMoveSpeed WRITE setHighlightMoveSpeed NOTIFY highlightMoveSpeedChanged)
    Q_PROPERTY(qreal highlightResizeSpeed READ highlightResizeSpeed WRITE setHighlightResizeSpeed NOTIFY highlightResizeSpeedChanged)

    Q_PROPERTY(qreal preferredHighlightBegin READ preferredHighlightBegin WRITE setPreferredHighlightBegin)
    Q_PROPERTY(qreal preferredHighlightEnd READ preferredHighlightEnd WRITE setPreferredHighlightEnd)
    Q_PROPERTY(HighlightRangeMode highlightRangeMode READ highlightRangeMode WRITE setHighlightRangeMode)

    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(bool keyNavigationWraps READ isWrapEnabled WRITE setWrapEnabled)
    Q_PROPERTY(int cacheBuffer READ cacheBuffer WRITE setCacheBuffer)
    Q_PROPERTY(QmlGraphicsViewSection *section READ sectionCriteria CONSTANT)
    Q_PROPERTY(QString currentSection READ currentSection NOTIFY currentSectionChanged)

    Q_PROPERTY(SnapMode snapMode READ snapMode WRITE setSnapMode)

    Q_PROPERTY(QmlComponent *header READ header WRITE setHeader)
    Q_PROPERTY(QmlComponent *footer READ footer WRITE setFooter)

    Q_ENUMS(HighlightRangeMode)
    Q_ENUMS(Orientation)
    Q_ENUMS(SnapMode)
    Q_CLASSINFO("DefaultProperty", "data")

public:
    QmlGraphicsListView(QmlGraphicsItem *parent=0);
    ~QmlGraphicsListView();

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

    enum HighlightRangeMode { NoHighlightRange, ApplyRange, StrictlyEnforceRange };
    HighlightRangeMode highlightRangeMode() const;
    void setHighlightRangeMode(HighlightRangeMode mode);

    qreal preferredHighlightBegin() const;
    void setPreferredHighlightBegin(qreal);

    qreal preferredHighlightEnd() const;
    void setPreferredHighlightEnd(qreal);

    qreal spacing() const;
    void setSpacing(qreal spacing);

    enum Orientation { Horizontal = Qt::Horizontal, Vertical = Qt::Vertical };
    Orientation orientation() const;
    void setOrientation(Orientation);

    bool isWrapEnabled() const;
    void setWrapEnabled(bool);

    int cacheBuffer() const;
    void setCacheBuffer(int);

    QmlGraphicsViewSection *sectionCriteria();
    QString currentSection() const;

    qreal highlightMoveSpeed() const;
    void setHighlightMoveSpeed(qreal);

    qreal highlightResizeSpeed() const;
    void setHighlightResizeSpeed(qreal);

    enum SnapMode { NoSnap, SnapToItem, SnapOneItem };
    SnapMode snapMode() const;
    void setSnapMode(SnapMode mode);

    QmlComponent *footer() const;
    void setFooter(QmlComponent *);

    QmlComponent *header() const;
    void setHeader(QmlComponent *);

    static QmlGraphicsListViewAttached *qmlAttachedProperties(QObject *);

public Q_SLOTS:
    void incrementCurrentIndex();
    void decrementCurrentIndex();
    void positionViewAtIndex(int index);

Q_SIGNALS:
    void countChanged();
    void spacingChanged();
    void orientationChanged();
    void currentIndexChanged();
    void currentSectionChanged();
    void highlightMoveSpeedChanged();
    void highlightResizeSpeedChanged();
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
    void refill();
    void trackedPositionChanged();
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void itemsMoved(int from, int to, int count);
    void destroyRemoved();
    void createdItem(int index, QmlGraphicsItem *item);
    void destroyingItem(QmlGraphicsItem *item);
    void animStopped();
};

QT_END_NAMESPACE

QML_DECLARE_TYPEINFO(QmlGraphicsListView, QML_HAS_ATTACHED_PROPERTIES)
QML_DECLARE_TYPE(QmlGraphicsListView)
QML_DECLARE_TYPE(QmlGraphicsViewSection)

QT_END_HEADER

#endif
