/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qmlwatcher_p.h"

#include "qmlexpression.h"
#include "qmlcontext.h"
#include "qml.h"

#include <qmldebugservice_p.h>

#include <QtCore/qmetaobject.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE


class QmlWatchProxy : public QObject
{
    Q_OBJECT
public:
    QmlWatchProxy(int id,
                  QObject *object,
                  int debugId,
                  const QMetaProperty &prop,
                  QmlWatcher *parent = 0);

    QmlWatchProxy(int id,
                  QmlExpression *exp,
                  int debugId,
                  QmlWatcher *parent = 0);

public slots:
    void notifyValueChanged();

private:
    friend class QmlWatcher;
    int m_id;
    QmlWatcher *m_watch;
    QObject *m_object;
    int m_debugId;
    QMetaProperty m_property;

    QmlExpression *m_expr;
};

QmlWatchProxy::QmlWatchProxy(int id,
                             QmlExpression *exp,
                             int debugId,
                             QmlWatcher *parent)
: QObject(parent), m_id(id), m_watch(parent), m_object(0), m_debugId(debugId), m_expr(exp)
{
    QObject::connect(m_expr, SIGNAL(valueChanged()), this, SLOT(notifyValueChanged()));
}

QmlWatchProxy::QmlWatchProxy(int id,
                             QObject *object,
                             int debugId,
                             const QMetaProperty &prop,
                             QmlWatcher *parent)
: QObject(parent), m_id(id), m_watch(parent), m_object(object), m_debugId(debugId), m_property(prop), m_expr(0)
{
    static int refreshIdx = -1;
    if(refreshIdx == -1)
        refreshIdx = QmlWatchProxy::staticMetaObject.indexOfMethod("notifyValueChanged()");

    if (prop.hasNotifySignal())
        QMetaObject::connect(m_object, prop.notifySignalIndex(), this, refreshIdx);
}

void QmlWatchProxy::notifyValueChanged()
{
    QVariant v;
    if (m_expr)
        v = m_expr->value();
    else
        v = m_property.read(m_object);

    emit m_watch->propertyChanged(m_id, m_debugId, m_property, v);
}


QmlWatcher::QmlWatcher(QObject *parent)
    : QObject(parent)
{
}

bool QmlWatcher::addWatch(int id, quint32 debugId)
{
    QObject *object = QmlDebugService::objectForId(debugId);
    if (object) {
        int propCount = object->metaObject()->propertyCount();
        for (int ii=0; ii<propCount; ii++)
            addPropertyWatch(id, object, debugId, object->metaObject()->property(ii));
        return true;
    }
    return false;
}

bool QmlWatcher::addWatch(int id, quint32 debugId, const QByteArray &property)
{
    QObject *object = QmlDebugService::objectForId(debugId);
    if (object) {
        int index = object->metaObject()->indexOfProperty(property.constData());
        if (index >= 0) {
            addPropertyWatch(id, object, debugId, object->metaObject()->property(index));
            return true;
        }
    }
    return false;
}

bool QmlWatcher::addWatch(int id, quint32 objectId, const QString &expr)
{
    QObject *object = QmlDebugService::objectForId(objectId);
    QmlContext *context = qmlContext(object);
    if (context) {
        QmlExpression *exprObj = new QmlExpression(context, expr, object);
        QmlWatchProxy *proxy = new QmlWatchProxy(id, exprObj, objectId, this);
        exprObj->setParent(proxy);
        m_proxies[id].append(proxy);
        proxy->notifyValueChanged();
        return true;
    }
    return false;
}

void QmlWatcher::removeWatch(int id)
{
    if (!m_proxies.contains(id))
        return;

    QList<QPointer<QmlWatchProxy> > proxies = m_proxies.take(id);
    qDeleteAll(proxies);
}

void QmlWatcher::addPropertyWatch(int id, QObject *object, quint32 debugId, const QMetaProperty &property)
{
    QmlWatchProxy *proxy = new QmlWatchProxy(id, object, debugId, property, this);
    m_proxies[id].append(proxy);

    proxy->notifyValueChanged();
}

QT_END_NAMESPACE

#include <qmlwatcher.moc>
