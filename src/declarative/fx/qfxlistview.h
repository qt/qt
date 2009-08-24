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

#ifndef QFXLISTVIEW_H
#define QFXLISTVIEW_H

#include <QtDeclarative/qfxflickable.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QFxVisualModel;
class QFxListViewAttached;
class QFxListViewPrivate;
class Q_DECLARATIVE_EXPORT QFxListView : public QFxFlickable
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr, QFxListView)

    Q_ENUMS(CurrentItemPositioning)
    Q_PROPERTY(QVariant model READ model WRITE setModel)
    Q_CLASSINFO("DefaultProperty", "delegate")
    Q_PROPERTY(QmlComponent *delegate READ delegate WRITE setDelegate)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QFxItem *current READ currentItem NOTIFY currentIndexChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QmlComponent *highlight READ highlight WRITE setHighlight)
    Q_PROPERTY(bool autoHighlight READ autoHighlight WRITE setAutoHighlight)
    Q_PROPERTY(CurrentItemPositioning currentItemPositioning READ currentItemPositioning WRITE setCurrentItemPositioning)
    Q_PROPERTY(int snapPosition READ snapPosition WRITE setSnapPosition)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(bool wrap READ isWrapEnabled WRITE setWrapEnabled)
    Q_PROPERTY(int cacheBuffer READ cacheBuffer WRITE setCacheBuffer)
    Q_PROPERTY(QString sectionExpression READ sectionExpression WRITE setSectionExpression NOTIFY sectionExpressionChanged)
    Q_PROPERTY(QString currentSection READ currentSection NOTIFY currentSectionChanged)

public:
    QFxListView(QFxItem *parent=0);
    ~QFxListView();

    QVariant model() const;
    void setModel(const QVariant &);

    QmlComponent *delegate() const;
    void setDelegate(QmlComponent *);

    int currentIndex() const;
    void setCurrentIndex(int idx);

    QFxItem *currentItem();
    int count() const;

    QmlComponent *highlight() const;
    void setHighlight(QmlComponent *highlight);

    bool autoHighlight() const;
    void setAutoHighlight(bool);

    enum CurrentItemPositioning { Free, Snap, SnapAuto };
    CurrentItemPositioning currentItemPositioning() const;
    void setCurrentItemPositioning(CurrentItemPositioning mode);

    int snapPosition() const;
    void setSnapPosition(int pos);

    int spacing() const;
    void setSpacing(int spacing);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation);

    bool isWrapEnabled() const;
    void setWrapEnabled(bool);

    int cacheBuffer() const;
    void setCacheBuffer(int);

    QString sectionExpression() const;
    void setSectionExpression(const QString &);
    QString currentSection() const;

    static QFxListViewAttached *qmlAttachedProperties(QObject *);

Q_SIGNALS:
    void countChanged();
    void spacingChanged();
    void currentIndexChanged();
    void currentSectionChanged();
    void sectionExpressionChanged();

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
    void destroyRemoved();
    void createdItem(int index, QFxItem *item);
    void destroyingItem(QFxItem *item);
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxListView)

QT_END_HEADER

#endif
