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

#ifndef QMLGRAPHICSLISTVIEW_H
#define QMLGRAPHICSLISTVIEW_H

#include <QtDeclarative/qmlgraphicsflickable.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


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
    Q_PROPERTY(bool highlightFollowsCurrentItem READ highlightFollowsCurrentItem WRITE setHighlightFollowsCurrentItem)

    Q_PROPERTY(qreal preferredHighlightBegin READ preferredHighlightBegin WRITE setPreferredHighlightBegin)
    Q_PROPERTY(qreal preferredHighlightEnd READ preferredHighlightEnd WRITE setPreferredHighlightEnd)
    Q_PROPERTY(HighlightRangeMode highlightRangeMode READ highlightRangeMode WRITE setHighlightRangeMode)

    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(bool keyNavigationWraps READ isWrapEnabled WRITE setWrapEnabled)
    Q_PROPERTY(int cacheBuffer READ cacheBuffer WRITE setCacheBuffer)
    Q_PROPERTY(QString sectionExpression READ sectionExpression WRITE setSectionExpression NOTIFY sectionExpressionChanged)
    Q_PROPERTY(QString currentSection READ currentSection NOTIFY currentSectionChanged)

    Q_PROPERTY(qreal highlightMoveSpeed READ highlightMoveSpeed WRITE setHighlightMoveSpeed NOTIFY highlightMoveSpeedChanged)
    Q_PROPERTY(qreal highlightResizeSpeed READ highlightResizeSpeed WRITE setHighlightResizeSpeed NOTIFY highlightResizeSpeedChanged)
    Q_ENUMS(HighlightRangeMode)
    Q_ENUMS(Orientation)
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

    QString sectionExpression() const;
    void setSectionExpression(const QString &);
    QString currentSection() const;

    qreal highlightMoveSpeed() const;
    void setHighlightMoveSpeed(qreal);

    qreal highlightResizeSpeed() const;
    void setHighlightResizeSpeed(qreal);

    static QmlGraphicsListViewAttached *qmlAttachedProperties(QObject *);

public Q_SLOTS:
    void incrementCurrentIndex();
    void decrementCurrentIndex();

Q_SIGNALS:
    void countChanged();
    void spacingChanged();
    void orientationChanged();
    void currentIndexChanged();
    void currentSectionChanged();
    void sectionExpressionChanged();
    void highlightMoveSpeedChanged();
    void highlightResizeSpeedChanged();

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
    void itemResized();
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void itemsMoved(int from, int to, int count);
    void destroyRemoved();
    void createdItem(int index, QmlGraphicsItem *item);
    void destroyingItem(QmlGraphicsItem *item);
};

QT_END_NAMESPACE

QML_DECLARE_TYPEINFO(QmlGraphicsListView, QML_HAS_ATTACHED_PROPERTIES)
QML_DECLARE_TYPE(QmlGraphicsListView)

QT_END_HEADER

#endif
