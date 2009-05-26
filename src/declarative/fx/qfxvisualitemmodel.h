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

#ifndef QFXVISUALITEMMODEL_H
#define QFXVISUALITEMMODEL_H

#include <QtCore/qobject.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtDeclarative/qml.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
/*****************************************************************************
 *****************************************************************************
 XXX Experimental
 *****************************************************************************
*****************************************************************************/

class QFxItem;
class QmlComponent;
class QmlPackage;
class QFxVisualItemModelPrivate;
class Q_DECLARATIVE_EXPORT QFxVisualItemModel : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QFxVisualItemModel)

    Q_PROPERTY(QVariant model READ model WRITE setModel)
    Q_PROPERTY(QmlComponent *delegate READ delegate WRITE setDelegate)
    Q_PROPERTY(QString part READ part WRITE setPart)
    Q_PROPERTY(QObject *parts READ parts)
    Q_CLASSINFO("DefaultProperty", "delegate")
public:
    QFxVisualItemModel();
    QFxVisualItemModel(QmlContext *);
    virtual ~QFxVisualItemModel();

    QVariant model() const;
    void setModel(const QVariant &);

    QmlComponent *delegate() const;
    void setDelegate(QmlComponent *);

    QString part() const;
    void setPart(const QString &);

    enum ReleaseFlag { Referenced = 0x01, Destroyed = 0x02 };
    Q_DECLARE_FLAGS(ReleaseFlags, ReleaseFlag)

    int count() const;
    QFxItem *item(int index, bool complete=true);
    QFxItem *item(int index, const QByteArray &, bool complete=true);
    ReleaseFlags release(QFxItem *item);
    void completeItem();
    QVariant evaluate(int index, const QString &expression, QObject *objectContext);

    int indexOf(QFxItem *item, QObject *objectContext) const;

    QObject *parts();

Q_SIGNALS:
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void itemsMoved(int from, int to, int count);
    void createdItem(int index, QFxItem *item);
    void createdPackage(int index, QmlPackage *package);
    void destroyingItem(QFxItem *item);
    void destroyingPackage(QmlPackage *package);

private Q_SLOTS:
    void _q_itemsChanged(int, int, const QList<int> &);
    void _q_itemsInserted(int index, int count);
    void _q_itemsRemoved(int index, int count);
    void _q_itemsMoved(int from, int to, int count);
    void _q_rowsInserted(const QModelIndex &,int,int);
    void _q_rowsRemoved(const QModelIndex &,int,int);
    void _q_dataChanged(const QModelIndex&,const QModelIndex&);
    void _q_createdPackage(int index, QmlPackage *package);
    void _q_destroyingPackage(QmlPackage *package);

private:
    Q_DISABLE_COPY(QFxVisualItemModel)
};
QML_DECLARE_TYPE(QFxVisualItemModel)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QFXVISUALITEMMODEL_H
