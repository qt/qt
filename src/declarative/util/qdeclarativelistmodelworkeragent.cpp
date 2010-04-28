/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "private/qdeclarativelistmodelworkeragent_p.h"
#include "private/qdeclarativelistmodel_p_p.h"
#include "private/qdeclarativedata_p.h"
#include "private/qdeclarativeengine_p.h"
#include "qdeclarativeinfo.h"

#include <QtCore/qcoreevent.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdebug.h>


QT_BEGIN_NAMESPACE


void QDeclarativeListModelWorkerAgent::Data::clearChange() 
{ 
    changes.clear(); 
}

void QDeclarativeListModelWorkerAgent::Data::insertChange(int index, int count) 
{
    Change c = { Change::Inserted, index, count, 0 };
    changes << c;
}

void QDeclarativeListModelWorkerAgent::Data::removeChange(int index, int count) 
{
    Change c = { Change::Removed, index, count, 0 };
    changes << c;
}

void QDeclarativeListModelWorkerAgent::Data::moveChange(int index, int count, int to)
{
    Change c = { Change::Moved, index, count, to };
    changes << c;
}

void QDeclarativeListModelWorkerAgent::Data::changedChange(int index, int count)
{
    Change c = { Change::Changed, index, count, 0 };
    changes << c;
}

QDeclarativeListModelWorkerAgent::QDeclarativeListModelWorkerAgent(QDeclarativeListModel *model)
: m_engine(0), m_ref(1), m_orig(model), m_copy(new QDeclarativeListModel(true, this))
{
    m_copy->m_flat->m_roles = m_orig->m_flat->m_roles;
    m_copy->m_flat->m_strings = m_orig->m_flat->m_strings;
    m_copy->m_flat->m_values = m_orig->m_flat->m_values;
}

QDeclarativeListModelWorkerAgent::~QDeclarativeListModelWorkerAgent()
{
}

void QDeclarativeListModelWorkerAgent::setScriptEngine(QScriptEngine *eng)
{
    m_engine = eng;
    if (m_copy->m_flat)
        m_copy->m_flat->m_scriptEngine = eng;
}

QScriptEngine *QDeclarativeListModelWorkerAgent::scriptEngine() const
{
    return m_engine;
}

void QDeclarativeListModelWorkerAgent::addref()
{
    m_ref.ref();
}

void QDeclarativeListModelWorkerAgent::release()
{
    bool del = !m_ref.deref();

    if (del)
        delete this;
}

int QDeclarativeListModelWorkerAgent::count() const
{
    return m_copy->count();
}

void QDeclarativeListModelWorkerAgent::clear()
{
    data.clearChange();
    data.removeChange(0, m_copy->count());
    m_copy->clear();
}

void QDeclarativeListModelWorkerAgent::remove(int index)
{
    int count = m_copy->count();
    m_copy->remove(index);

    if (m_copy->count() != count)
        data.removeChange(index, 1);
}

void QDeclarativeListModelWorkerAgent::append(const QScriptValue &value)
{
    int count = m_copy->count();
    m_copy->append(value);

    if (m_copy->count() != count)
        data.insertChange(m_copy->count() - 1, 1);
}

void QDeclarativeListModelWorkerAgent::insert(int index, const QScriptValue &value)
{
    int count = m_copy->count();
    m_copy->insert(index, value);

    if (m_copy->count() != count)
        data.insertChange(index, 1);
}

QScriptValue QDeclarativeListModelWorkerAgent::get(int index) const
{
    return m_copy->get(index);
}

void QDeclarativeListModelWorkerAgent::set(int index, const QScriptValue &value)
{
    m_copy->set(index, value);
    data.changedChange(index, 1);
}

void QDeclarativeListModelWorkerAgent::setProperty(int index, const QString& property, const QVariant& value)
{
    m_copy->setProperty(index, property, value);
    data.changedChange(index, 1);
}

void QDeclarativeListModelWorkerAgent::move(int from, int to, int count)
{
    m_copy->move(from, to, count);
    data.moveChange(from, to, count);
}

void QDeclarativeListModelWorkerAgent::sync()
{
    Sync *s = new Sync;
    s->data = data;
    s->list = m_copy;
    data.changes.clear();
    QCoreApplication::postEvent(this, s);
}

bool QDeclarativeListModelWorkerAgent::event(QEvent *e)
{
    if (e->type() == QEvent::User) {
        Sync *s = static_cast<Sync *>(e);

        const QList<Change> &changes = s->data.changes;

        if (m_copy) {
            bool cc = m_copy->count() != s->list->count();

            FlatListModel *orig = m_orig->m_flat;
            FlatListModel *copy = s->list->m_flat;
            if (!orig || !copy) 
                return QObject::event(e);
            
            orig->m_roles = copy->m_roles;
            orig->m_strings = copy->m_strings;
            orig->m_values = copy->m_values;

            for (int ii = 0; ii < changes.count(); ++ii) {
                const Change &change = changes.at(ii);
                switch (change.type) {
                case Change::Inserted:
                    emit m_orig->itemsInserted(change.index, change.count);
                    break;
                case Change::Removed:
                    emit m_orig->itemsRemoved(change.index, change.count);
                    break;
                case Change::Moved:
                    emit m_orig->itemsMoved(change.index, change.to, change.count);
                    break;
                case Change::Changed:
                    emit m_orig->itemsMoved(change.index, change.to, change.count);
                    break;
                }
            }

            if (cc)
                emit m_orig->countChanged();
        }
    }

    return QObject::event(e);
}

QT_END_NAMESPACE

