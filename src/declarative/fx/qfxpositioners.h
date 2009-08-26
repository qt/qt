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

#ifndef QFXLAYOUTS_H
#define QFXLAYOUTS_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtDeclarative/qfxitem.h>
#include <QtDeclarative/qmlstate.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QFxBasePositionerPrivate;

class Q_DECLARATIVE_EXPORT QFxBasePositioner : public QFxItem
{
    Q_OBJECT

    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
    Q_PROPERTY(QmlTransition *move READ move WRITE setMove)
    Q_PROPERTY(QmlTransition *add READ add WRITE setAdd)
    Q_PROPERTY(QmlTransition *remove READ remove WRITE setRemove)
public:
    enum AutoUpdateType { None = 0x0, Horizontal = 0x1, Vertical = 0x2, Both = 0x3 };
    QFxBasePositioner(AutoUpdateType, QFxItem *parent);

    int spacing() const;
    void setSpacing(int);

    QmlTransition *move() const;
    void setMove(QmlTransition *);

    QmlTransition *add() const;
    void setAdd(QmlTransition *);

    QmlTransition *remove() const;
    void setRemove(QmlTransition *);

protected:
    virtual void componentComplete();
    virtual QVariant itemChange(GraphicsItemChange, const QVariant &);
    virtual bool event(QEvent *);
    QSet<QFxItem *>* newItems();
    QSet<QFxItem *>* leavingItems();
    QSet<QFxItem *>* items();
    void applyAdd(const QList<QPair<QString, QVariant> >& changes, QFxItem* target);
    void applyMove(const QList<QPair<QString, QVariant> >& changes, QFxItem* target);
    void applyRemove(const QList<QPair<QString, QVariant> >& changes, QFxItem* target);
    void finishApplyTransitions();

Q_SIGNALS:
    void layoutItemChanged();

protected Q_SLOTS:
    virtual void doPositioning()=0;

private Q_SLOTS:
    void prePositioning();

protected:
    QFxBasePositioner(QFxBasePositionerPrivate &dd, AutoUpdateType at, QFxItem *parent);
    void setMovingItem(QFxItem *);

private:
    void applyTransition(const QList<QPair<QString, QVariant> >& changes, QFxItem* target,
            QmlStateOperation::ActionList &actions);
    Q_DISABLE_COPY(QFxBasePositioner)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QFxBasePositioner)
};

class Q_DECLARATIVE_EXPORT QFxColumn : public QFxBasePositioner
{
    Q_OBJECT
public:
    QFxColumn(QFxItem *parent=0);
protected Q_SLOTS:
    virtual void doPositioning();
private:
    Q_DISABLE_COPY(QFxColumn)
};

class Q_DECLARATIVE_EXPORT QFxRow: public QFxBasePositioner
{
    Q_OBJECT
public:
    QFxRow(QFxItem *parent=0);
protected Q_SLOTS:
    virtual void doPositioning();
private:
    Q_DISABLE_COPY(QFxRow)
};

class Q_DECLARATIVE_EXPORT QFxGrid : public QFxBasePositioner
{
    Q_OBJECT
    Q_PROPERTY(int rows READ rows WRITE setRows)
    Q_PROPERTY(int columns READ columns WRITE setcolumns)
public:
    QFxGrid(QFxItem *parent=0);

    int rows() const {return _rows;}
    void setRows(const int rows){_rows = rows;}

    int columns() const {return _columns;}
    void setcolumns(const int columns){_columns = columns;}
protected Q_SLOTS:
    virtual void doPositioning();

private:
    int _rows;
    int _columns;
    Q_DISABLE_COPY(QFxGrid)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxColumn)
QML_DECLARE_TYPE(QFxRow)
QML_DECLARE_TYPE(QFxGrid)

QT_END_HEADER

#endif
