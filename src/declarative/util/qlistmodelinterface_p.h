/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QLISTMODELINTERFACE_H
#define QLISTMODELINTERFACE_H

#include <QtCore/QHash>
#include <QtCore/QVariant>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class Q_DECLARATIVE_EXPORT QListModelInterface : public QObject
{
    Q_OBJECT
 public:
    QListModelInterface(QObject *parent = 0) : QObject(parent) {}
    virtual ~QListModelInterface() {}

    virtual int count() const = 0;
    virtual QHash<int,QVariant> data(int index, const QList<int>& roles = QList<int>()) const = 0;
    virtual QVariant data(int index, int role) const = 0;
    virtual bool setData(int index, const QHash<int,QVariant>& values)
    { Q_UNUSED(index); Q_UNUSED(values); return false; }

    virtual QList<int> roles() const = 0;
    virtual QString toString(int role) const = 0;

 Q_SIGNALS:
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void itemsMoved(int from, int to, int count);
    void itemsChanged(int index, int count, const QList<int> &roles);

 protected:
    QListModelInterface(QObjectPrivate &dd, QObject *parent) 
        : QObject(dd, parent) {}
};


QT_END_NAMESPACE

QT_END_HEADER
#endif //QTREEMODELINTERFACE_H
