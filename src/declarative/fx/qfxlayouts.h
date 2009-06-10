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
class QFxBaseLayoutPrivate;

class Q_DECLARATIVE_EXPORT QFxBaseLayout : public QFxItem
{
    Q_OBJECT

    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
    Q_PROPERTY(int margin READ margin WRITE setMargin)
    Q_PROPERTY(QmlTransition *move READ move WRITE setMove)
    Q_PROPERTY(QmlTransition *add READ add WRITE setAdd)
    Q_PROPERTY(QmlTransition *remove READ remove WRITE setRemove)
    Q_PROPERTY(QFxItem *item READ layoutItem NOTIFY layoutItemChanged)
public:
    enum AutoUpdateType { None = 0x0, Horizontal = 0x1, Vertical = 0x2, Both = 0x3 };
    QFxBaseLayout(AutoUpdateType, QFxItem *parent);

    int spacing() const;
    void setSpacing(int);

    int margin() const;
    void setMargin(int);

    QmlTransition *move() const;
    void setMove(QmlTransition *);

    QmlTransition *add() const;
    void setAdd(QmlTransition *);

    QmlTransition *remove() const;
    void setRemove(QmlTransition *);

    QFxItem *layoutItem() const;

protected:
    virtual void componentComplete();
    virtual void childrenChanged();
    virtual bool event(QEvent *);
    QSet<QFxItem *>* newItems();
    QSet<QFxItem *>* leavingItems();
    QSet<QFxItem *>* items();
    void applyAdd(const QList<QPair<QString, QVariant> >& changes, QFxItem* target);
    void applyMove(const QList<QPair<QString, QVariant> >& changes, QFxItem* target);
    void applyRemove(const QList<QPair<QString, QVariant> >& changes, QFxItem* target);

Q_SIGNALS:
    void layoutItemChanged();

protected Q_SLOTS:
    virtual void doLayout()=0;
    void setLayoutItem(QFxItem *);

private Q_SLOTS:
    void preLayout();

protected:
    QFxBaseLayout(QFxBaseLayoutPrivate &dd, AutoUpdateType at, QFxItem *parent);
    void setMovingItem(QFxItem *);

private:
    void applyTransition(const QList<QPair<QString, QVariant> >& changes, QFxItem* target,
            QmlTransition* transition);
    Q_DISABLE_COPY(QFxBaseLayout)
    Q_DECLARE_PRIVATE(QFxBaseLayout)
};

class Q_DECLARATIVE_EXPORT QFxVerticalLayout : public QFxBaseLayout
{
    Q_OBJECT
public:
    QFxVerticalLayout(QFxItem *parent=0);
protected Q_SLOTS:
    virtual void doLayout();
private:
    Q_DISABLE_COPY(QFxVerticalLayout)
};
QML_DECLARE_TYPE(QFxVerticalLayout)

class Q_DECLARATIVE_EXPORT QFxHorizontalLayout: public QFxBaseLayout
{
    Q_OBJECT
public:
    QFxHorizontalLayout(QFxItem *parent=0);
protected Q_SLOTS:
    virtual void doLayout();
private:
    Q_DISABLE_COPY(QFxHorizontalLayout)
};
QML_DECLARE_TYPE(QFxHorizontalLayout)

class Q_DECLARATIVE_EXPORT QFxGridLayout : public QFxBaseLayout
{
    Q_OBJECT
    Q_PROPERTY(int rows READ rows WRITE setRows)
    Q_PROPERTY(int columns READ columns WRITE setcolumns)
public:
    QFxGridLayout(QFxItem *parent=0);

    int rows() const {return _rows;}
    void setRows(const int rows){_rows = rows;}

    int columns() const {return _columns;}
    void setcolumns(const int columns){_columns = columns;}
protected Q_SLOTS:
    virtual void doLayout();

private:
    int _rows;
    int _columns;
    Q_DISABLE_COPY(QFxGridLayout)
};
QML_DECLARE_TYPE(QFxGridLayout)

QT_END_NAMESPACE

QT_END_HEADER
#endif
