// Commit: 2c7cab4172f1acc86fd49345a2847417e162f2c3
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

#ifndef QSGPOSITIONERS_P_H
#define QSGPOSITIONERS_P_H

#include "qsgimplicitsizeitem_p.h"

#include <private/qdeclarativestate_p.h>
#include <private/qpodvector_p.h>

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGBasePositionerPrivate;

class Q_DECLARATIVE_PRIVATE_EXPORT QSGBasePositioner : public QSGImplicitSizeItem
{
    Q_OBJECT

    Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(QDeclarativeTransition *move READ move WRITE setMove NOTIFY moveChanged)
    Q_PROPERTY(QDeclarativeTransition *add READ add WRITE setAdd NOTIFY addChanged)
public:
    enum PositionerType { None = 0x0, Horizontal = 0x1, Vertical = 0x2, Both = 0x3 };
    QSGBasePositioner(PositionerType, QSGItem *parent);
    ~QSGBasePositioner();

    int spacing() const;
    void setSpacing(int);

    QDeclarativeTransition *move() const;
    void setMove(QDeclarativeTransition *);

    QDeclarativeTransition *add() const;
    void setAdd(QDeclarativeTransition *);

protected:
    QSGBasePositioner(QSGBasePositionerPrivate &dd, PositionerType at, QSGItem *parent);
    virtual void componentComplete();
    virtual void itemChange(ItemChange, const ItemChangeData &);
    void finishApplyTransitions();

Q_SIGNALS:
    void spacingChanged();
    void moveChanged();
    void addChanged();

protected Q_SLOTS:
    void prePositioning();

protected:
    virtual void doPositioning(QSizeF *contentSize)=0;
    virtual void reportConflictingAnchors()=0;
    class PositionedItem {
    public :
        PositionedItem(QSGItem *i) : item(i), isNew(false), isVisible(true) {}
        bool operator==(const PositionedItem &other) const { return other.item == item; }
        QSGItem *item;
        bool isNew;
        bool isVisible;
    };

    QPODVector<PositionedItem,8> positionedItems;
    void positionX(int,const PositionedItem &target);
    void positionY(int,const PositionedItem &target);

private:
    Q_DISABLE_COPY(QSGBasePositioner)
    Q_DECLARE_PRIVATE(QSGBasePositioner)
};

class Q_AUTOTEST_EXPORT QSGColumn : public QSGBasePositioner
{
    Q_OBJECT
public:
    QSGColumn(QSGItem *parent=0);
protected:
    virtual void doPositioning(QSizeF *contentSize);
    virtual void reportConflictingAnchors();
private:
    Q_DISABLE_COPY(QSGColumn)
};

class Q_AUTOTEST_EXPORT QSGRow: public QSGBasePositioner
{
    Q_OBJECT
    Q_PROPERTY(Qt::LayoutDirection layoutDirection READ layoutDirection WRITE setLayoutDirection NOTIFY layoutDirectionChanged)
    Q_PROPERTY(Qt::LayoutDirection effectiveLayoutDirection READ effectiveLayoutDirection NOTIFY effectiveLayoutDirectionChanged)
public:
    QSGRow(QSGItem *parent=0);

    Qt::LayoutDirection layoutDirection() const;
    void setLayoutDirection (Qt::LayoutDirection);
    Qt::LayoutDirection effectiveLayoutDirection() const;

Q_SIGNALS:
    void layoutDirectionChanged();
    void effectiveLayoutDirectionChanged();

protected:
    virtual void doPositioning(QSizeF *contentSize);
    virtual void reportConflictingAnchors();
private:
    Q_DISABLE_COPY(QSGRow)
};

class Q_AUTOTEST_EXPORT QSGGrid : public QSGBasePositioner
{
    Q_OBJECT
    Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY rowsChanged)
    Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
    Q_PROPERTY(Flow flow READ flow WRITE setFlow NOTIFY flowChanged)
    Q_PROPERTY(Qt::LayoutDirection layoutDirection READ layoutDirection WRITE setLayoutDirection NOTIFY layoutDirectionChanged)
    Q_PROPERTY(Qt::LayoutDirection effectiveLayoutDirection READ effectiveLayoutDirection NOTIFY effectiveLayoutDirectionChanged)

public:
    QSGGrid(QSGItem *parent=0);

    int rows() const {return m_rows;}
    void setRows(const int rows);

    int columns() const {return m_columns;}
    void setColumns(const int columns);

    Q_ENUMS(Flow)
    enum Flow { LeftToRight, TopToBottom };
    Flow flow() const;
    void setFlow(Flow);

    Qt::LayoutDirection layoutDirection() const;
    void setLayoutDirection (Qt::LayoutDirection);
    Qt::LayoutDirection effectiveLayoutDirection() const;

Q_SIGNALS:
    void rowsChanged();
    void columnsChanged();
    void flowChanged();
    void layoutDirectionChanged();
    void effectiveLayoutDirectionChanged();

protected:
    virtual void doPositioning(QSizeF *contentSize);
    virtual void reportConflictingAnchors();

private:
    int m_rows;
    int m_columns;
    Flow m_flow;
    Q_DISABLE_COPY(QSGGrid)
};

class QSGFlowPrivate;
class Q_AUTOTEST_EXPORT QSGFlow: public QSGBasePositioner
{
    Q_OBJECT
    Q_PROPERTY(Flow flow READ flow WRITE setFlow NOTIFY flowChanged)
    Q_PROPERTY(Qt::LayoutDirection layoutDirection READ layoutDirection WRITE setLayoutDirection NOTIFY layoutDirectionChanged)
    Q_PROPERTY(Qt::LayoutDirection effectiveLayoutDirection READ effectiveLayoutDirection NOTIFY effectiveLayoutDirectionChanged)
public:
    QSGFlow(QSGItem *parent=0);

    Q_ENUMS(Flow)
    enum Flow { LeftToRight, TopToBottom };
    Flow flow() const;
    void setFlow(Flow);

    Qt::LayoutDirection layoutDirection() const;
    void setLayoutDirection (Qt::LayoutDirection);
    Qt::LayoutDirection effectiveLayoutDirection() const;

Q_SIGNALS:
    void flowChanged();
    void layoutDirectionChanged();
    void effectiveLayoutDirectionChanged();

protected:
    virtual void doPositioning(QSizeF *contentSize);
    virtual void reportConflictingAnchors();
protected:
    QSGFlow(QSGFlowPrivate &dd, QSGItem *parent);
private:
    Q_DISABLE_COPY(QSGFlow)
    Q_DECLARE_PRIVATE(QSGFlow)
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QSGColumn)
QML_DECLARE_TYPE(QSGRow)
QML_DECLARE_TYPE(QSGGrid)
QML_DECLARE_TYPE(QSGFlow)

QT_END_HEADER

#endif // QSGPOSITIONERS_P_H
