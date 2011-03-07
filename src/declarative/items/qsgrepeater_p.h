// Commit: ebd4bc73c46c2962742a682b6a391fb68c482aec
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

#ifndef QSGREPEATER_P_H
#define QSGREPEATER_P_H

#include "qsgitem.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGRepeaterPrivate;
class Q_AUTOTEST_EXPORT QSGRepeater : public QSGItem
{
    Q_OBJECT

    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativeComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_CLASSINFO("DefaultProperty", "delegate")

public:
    QSGRepeater(QSGItem *parent=0);
    virtual ~QSGRepeater();

    QVariant model() const;
    void setModel(const QVariant &);

    QDeclarativeComponent *delegate() const;
    void setDelegate(QDeclarativeComponent *);

    int count() const;

    Q_INVOKABLE QSGItem *itemAt(int index) const;

Q_SIGNALS:
    void modelChanged();
    void delegateChanged();
    void countChanged();

    void itemAdded(int index, QSGItem *item);
    void itemRemoved(int index, QSGItem *item);

private:
    void clear();
    void regenerate();

protected:
    virtual void componentComplete();
    void itemChange(ItemChange change, const ItemChangeData &value);

private Q_SLOTS:
    void itemsInserted(int,int);
    void itemsRemoved(int,int);
    void itemsMoved(int,int,int);
    void modelReset();

private:
    Q_DISABLE_COPY(QSGRepeater)
    Q_DECLARE_PRIVATE(QSGRepeater)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QSGRepeater)

QT_END_HEADER

#endif // QSGREPEATER_P_H
