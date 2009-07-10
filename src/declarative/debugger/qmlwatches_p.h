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

#ifndef QMLWATCHES_P_H
#define QMLWATCHES_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qbytearray.h>
#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <QtCore/qset.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qabstractitemmodel.h>

QT_BEGIN_NAMESPACE

class QmlWatchesProxy;
class QmlExpressionObject;

class QmlWatches : public QAbstractTableModel
{
    Q_OBJECT
public:
    QmlWatches(QObject *parent = 0);

    bool hasWatch(quint32 objectId, const QByteArray &property);
    void addWatch(quint32 objectId, const QByteArray &property);
    void remWatch(quint32 objectId, const QByteArray &property);

    void addWatch(QmlExpressionObject *);

    quint32 objectId(QObject *);
    QObject *object(quint32);

    static QString objectToString(QObject *obj);
protected:
    int columnCount(const QModelIndex &) const;
    int rowCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

private:
    friend class QmlWatchesProxy;
    QList<QPair<quint32, QByteArray> > m_watches;

    void addValue(int, const QVariant &);
    struct Value {
        int column;
        QVariant variant;
        bool first;
    };
    QList<Value> m_values;
    QStringList m_columnNames;
    QList<QPointer<QmlWatchesProxy> > m_proxies;

    quint32 m_uniqueId;
    QHash<QObject *, QPair<QPointer<QObject>, quint32> *> m_objects;
    QHash<quint32, QPair<QPointer<QObject>, quint32> *> m_objectIds;
};

QT_END_NAMESPACE

#endif // QMLWATCHES_P_H
