/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QSCRIPTCLASSPRIVATE_P_H
#define QSCRIPTCLASSPRIVATE_P_H

#include "qscriptclass.h"
#include "qscriptengine_p.h"
#include <v8.h>
#include "qscriptv8objectwrapper_p.h"

class QScriptEnginePrivate;
class QScriptValuePrivate;

class QScriptClassPrivate
{
public:
    inline static QScriptClassPrivate* get(const QScriptClass* q);
    inline static QScriptClass* get(const QScriptClassPrivate* d);

    inline QScriptClassPrivate(QScriptEnginePrivate* engine, QScriptClass* q);
    inline QScriptEnginePrivate* engine() const;

    inline QScriptClass* userCallback() const;
private:
    QScriptClass* q_ptr;
    // FIXME that should shared data pointer but we need to avoid circular references too
    // Note that now this pointer could be invalid and there is now way to check it
    QScriptEnginePrivate* m_engine;
};

inline QScriptClassPrivate* QScriptClassPrivate::get(const QScriptClass* q)
{
    Q_ASSERT(q);
    Q_ASSERT(q->d_ptr);
    return q->d_ptr.data();
}

inline QScriptClass* QScriptClassPrivate::get(const QScriptClassPrivate* d)
{
    Q_ASSERT(d);
    return d->q_ptr;
}

inline QScriptClassPrivate::QScriptClassPrivate(QScriptEnginePrivate* engine, QScriptClass* q)
    : q_ptr(q)
    , m_engine(engine)
{
    Q_ASSERT(q_ptr);
    Q_ASSERT(engine);
}

inline QScriptEnginePrivate* QScriptClassPrivate::engine() const
{
    return m_engine;
}

inline QScriptClass* QScriptClassPrivate::userCallback() const
{
    return q_ptr;
}


struct QScriptClassObject : QScriptV8ObjectWrapper<QScriptClassObject, &QScriptEnginePrivate::scriptClassTemplate> {
    QScriptClassPrivate *scriptclass;
    v8::Persistent<v8::Object> original;

    ~QScriptClassObject()
    {
        if (!original.IsEmpty())
            original.Dispose();
    }

    v8::Handle<v8::Value> property(v8::Local<v8::String> property);
    v8::Handle<v8::Value> setProperty(v8::Local<v8::String> property, v8::Local<v8::Value> value);

    static v8::Handle<v8::FunctionTemplate> createFunctionTemplate(QScriptEnginePrivate *engine);
    static v8::Handle<v8::Value> newInstance(QScriptClassPrivate* scriptclass, v8::Handle<v8::Object> previousValue);

    void setOriginal(v8::Handle<v8::Object> o)
    {
        original = v8::Persistent<v8::Object>::New(o);
    }

};

#endif // QSCRIPTCLASSPRIVATE_P_H
