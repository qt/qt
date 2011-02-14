/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTENGINEAGENT_IMPL_P_H
#define QSCRIPTENGINEAGENT_IMPL_P_H

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

#include <QtCore/qobjectdefs.h>
#include "qscriptengineagent.h"
#include "qscriptengineagent_p.h"
#include "qscriptengine_p.h"
#include "qscriptqobject_p.h"

QT_BEGIN_NAMESPACE

inline QScriptEngineAgent* QScriptEngineAgentPrivate::get(QScriptEngineAgentPrivate *p)
{
    return p->q_func();
}

inline QScriptEngineAgentPrivate* QScriptEngineAgentPrivate::get(QScriptEngineAgent *p)
{
    return p->d_func();
}

inline QScriptEngineAgentPrivate::QScriptEngineAgentPrivate(QScriptEngineAgent *q, QScriptEnginePrivate *engine)
    : m_engine(engine)
    , q_ptr(q)
{
    Q_ASSERT(q);
    Q_ASSERT(engine);
    engine->registerAgent(this);
}

inline QScriptEngineAgentPrivate::~QScriptEngineAgentPrivate()
{
    engine()->unregisterAgent(this);
}

inline QScriptEngineAgentPrivate::UnloadData::UnloadData(QScriptEnginePrivate *engine, int64_t id)
    : m_engine(engine)
    , m_scriptId(id)
{
    Q_ASSERT(engine);
    engine->registerScript(this);
}

inline QScriptEngineAgentPrivate::UnloadData::~UnloadData()
{
    QScriptEngineAgentPrivate* agent = engine()->agent();
    engine()->unregisterScript(this);
    if (agent)
        agent->scriptUnload(m_scriptId);
}

inline QScriptEnginePrivate *QScriptEngineAgentPrivate::UnloadData::engine() const
{
    return m_engine;
}

inline int64_t QScriptEngineAgentPrivate::UnloadData::id() const
{
    return m_scriptId;
}

inline void QScriptEngineAgentPrivate::scriptLoad(v8::Handle<v8::Script> script, const QString &program,
                        const QString &fileName, int baseLineNumber)
{
    UnloadData *data = new UnloadData(engine(), script->Id()->IntegerValue());
    v8::Persistent<v8::String> p = v8::Persistent<v8::String>::New(v8::String::New("QScriptEngineAgentPrivate_data_"));
    script->SetData(p);
    p.MakeWeak(data, UnloadData::UnloadHandler);
    userCallback()->scriptLoad(data->id(), program, fileName, baseLineNumber);
}

inline void QScriptEngineAgentPrivate::scriptUnload(int64_t id)
{
    userCallback()->scriptUnload(id);
}

inline void QScriptEngineAgentPrivate::pushContext()
{
    userCallback()->contextPush();
}

inline void QScriptEngineAgentPrivate::popContext()
{
    userCallback()->contextPop();
}

inline QScriptEngineAgent *QScriptEngineAgentPrivate::userCallback()
{
    return q_ptr;
}

inline void QScriptEngineAgentPrivate::attachTo(QScriptEnginePrivate *engine)
{
    Q_ASSERT(engine);
    m_engine = engine;
}

inline QScriptEnginePrivate *QScriptEngineAgentPrivate::engine() const
{
    return m_engine;
}

/*!
  \internal
  This function will delete public agent, which will delete this object
*/
inline void QScriptEngineAgentPrivate::kill()
{
    delete q_func();
}

QT_END_NAMESPACE

#endif //QSCRIPTENGINEAGENT_IMPL_P_H
