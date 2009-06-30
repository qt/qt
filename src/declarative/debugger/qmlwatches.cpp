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

#include "qmlwatches_p.h"
#include <QtCore/qmetaobject.h>
#include <QtCore/qdebug.h>
#include <QtGui/qcolor.h>
#include <QtDeclarative/qmlmetatype.h>
#include <QtDeclarative/qmlexpression.h>

QT_BEGIN_NAMESPACE

QString QmlWatches::objectToString(QObject *obj)
{
    if(!obj)
        return QLatin1String("NULL");

    QString objectName = obj->objectName();
    if(objectName.isEmpty())
        objectName = QLatin1String("<unnamed>");

    QString rv = QLatin1String(obj->metaObject()->className()) + 
                 QLatin1String(": ") + objectName;

    return rv;
}

class QmlWatchesProxy : public QObject
{
    Q_OBJECT
public:
    QmlWatchesProxy(QObject *object, 
                    const QMetaProperty &prop, 
                    int column,
                    QmlWatches *parent = 0);

    QmlWatchesProxy(QmlExpressionObject *object,
                    int column,
                    QmlWatches *parent = 0);

public slots:
    void refresh();

private:
    QmlWatches *m_watches;
    QObject *m_object;
    QMetaProperty m_property;

    QmlExpressionObject *m_expr;
    int m_column;
};

QmlWatchesProxy::QmlWatchesProxy(QmlExpressionObject *object,
                                 int column,
                                 QmlWatches *parent)
: QObject(parent), m_watches(parent), m_object(0), m_expr(object), m_column(column)
{
    QObject::connect(m_expr, SIGNAL(valueChanged()), this, SLOT(refresh()));
}

QmlWatchesProxy::QmlWatchesProxy(QObject *object, 
                                 const QMetaProperty &prop, 
                                 int column,
                                 QmlWatches *parent)
: QObject(parent), m_watches(parent), m_object(object), m_property(prop), 
  m_expr(0), m_column(column)
{
    static int refreshIdx = -1;
    if(refreshIdx == -1)
        refreshIdx = QmlWatchesProxy::staticMetaObject.indexOfMethod("refresh()");

    QMetaObject::connect(m_object, prop.notifySignalIndex(), 
                         this, refreshIdx);
}

void QmlWatchesProxy::refresh()
{
    QVariant v;
    if(m_expr) v = m_expr->value();
    else v= m_property.read(m_object);

    m_watches->addValue(m_column, v);
}

QmlWatches::QmlWatches(QObject *parent)
: QAbstractTableModel(parent), m_uniqueId(0)
{
}

bool QmlWatches::hasWatch(quint32 objectId, const QByteArray &property)
{
    return m_watches.contains(qMakePair(objectId, property));
}

void QmlWatches::addWatch(QmlExpressionObject *expr)
{
    int oldColumn = columnCount(QModelIndex());

    m_watches.append(qMakePair(quint32(0), QByteArray()));

    beginInsertColumns(QModelIndex(), oldColumn, oldColumn);
    endInsertColumns();

    m_columnNames.append(expr->expression());

    QmlWatchesProxy *proxy = new QmlWatchesProxy(expr, oldColumn, this);
    m_proxies.append(proxy);

    proxy->refresh();
    m_values[m_values.count() - 1].first = true;
}

void QmlWatches::addWatch(quint32 objectId, const QByteArray &property)
{
    if(hasWatch(objectId, property))
        return;

    int oldColumn = columnCount(QModelIndex());

    m_watches.append(qMakePair(objectId, property));

    beginInsertColumns(QModelIndex(), oldColumn, oldColumn);
    endInsertColumns();

    QObject *obj = object(objectId);
    m_columnNames.append(QLatin1String(property) + QLatin1String(" on ") + objectToString(obj));
    QMetaProperty prop = 
        obj->metaObject()->property(obj->metaObject()->indexOfProperty(property.constData()));
    QmlWatchesProxy *proxy = new QmlWatchesProxy(obj, prop, oldColumn, this);
    m_proxies.append(proxy);
    proxy->refresh();
    m_values[m_values.count() - 1].first = true;
}

void QmlWatches::remWatch(quint32 objectId, const QByteArray &property)
{
    QPair<quint32, QByteArray> watch = qMakePair(objectId, property);
    for(int ii = 0; ii < m_watches.count(); ++ii) {
        if(m_watches.at(ii) == watch) {
            m_watches.removeAt(ii);
            m_columnNames.removeAt(ii);
            if(m_proxies.at(ii))
                delete m_proxies.at(ii);
            m_proxies.removeAt(ii);


            for(QList<Value>::Iterator iter = m_values.begin();
                    iter != m_values.end();
               ) {
                if(iter->column == ii) {
                    iter = m_values.erase(iter);
                } else {
                    if(iter->column > ii)
                        --iter->column;
                    ++iter;
                }
            }
            reset();
            return;
        }
    }
}

quint32 QmlWatches::objectId(QObject *object)
{
    Q_ASSERT(object);

    QHash<QObject *, QPair<QPointer<QObject>, quint32> *>::Iterator iter =
        m_objects.find(object);
    if(iter == m_objects.end()) {
        iter = m_objects.insert(object, new QPair<QPointer<QObject>, quint32>(QPointer<QObject>(object), m_uniqueId++));
        m_objectIds.insert(iter.value()->second, iter.value());
    }

    if(iter.value()->first != object) {
        iter.value()->first = object;
        iter.value()->second = m_uniqueId++;

        m_objectIds.insert(iter.value()->second, iter.value());
    }
    return iter.value()->second;
}

QObject *QmlWatches::object(quint32 id)
{
    QHash<quint32, QPair<QPointer<QObject>, quint32> *>::Iterator iter = 
        m_objectIds.find(id);
    if(iter == m_objectIds.end())
        return 0;

    if(!iter.value()->first) {
        m_objectIds.erase(iter);
        return 0;
    }

    return iter.value()->first;
}

void QmlWatches::addValue(int column, const QVariant &value)
{
    int row = m_values.count();

    beginInsertRows(QModelIndex(), row, row);
    Value v;
    v.column = column;
    v.variant = value;
    v.first = false;
    m_values.append(v);
    endInsertRows();
}

int QmlWatches::columnCount(const QModelIndex &) const
{
    return m_watches.count();
}

int QmlWatches::rowCount(const QModelIndex &) const
{
    return m_values.count();
}

QVariant QmlWatches::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && section < m_columnNames.count() && 
        role == Qt::DisplayRole)
        return m_columnNames.at(section);
    else
        return QVariant();
}

QVariant QmlWatches::data(const QModelIndex &idx, int role) const
{
    if(m_values.at(idx.row()).column == idx.column()) {
        if(role == Qt::DisplayRole) {
            const QVariant &value = m_values.at(idx.row()).variant;

            QString str = value.toString();

            if(str.isEmpty() && QmlMetaType::isObject(value.userType())) {
                QObject *o = QmlMetaType::toQObject(value);
                if(o) {
                    QString objectName = o->objectName();
                    if(objectName.isEmpty())
                        objectName = QLatin1String("<unnamed>");
                    str = QLatin1String(o->metaObject()->className()) + 
                          QLatin1String(": ") + objectName;
                }
            }

            if(str.isEmpty()) {
                QDebug d(&str);
                d << value;
            }
            return QVariant(str);
        } else if(role == Qt::BackgroundRole) {
            if(m_values.at(idx.row()).first) 
                return QColor(Qt::green);
            else
                return QVariant();
        } else {
            return QVariant();
        }
    } else {
        return QVariant();
    }
}

QT_END_NAMESPACE

#include "qmlwatches.moc"
