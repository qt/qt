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
#include <v8.h>
#include "qscriptv8objectwrapper_p.h"

QT_BEGIN_NAMESPACE

class QScriptEnginePrivate;
class QScriptValuePrivate;

class QScriptClassPrivate
{
public:
    inline static QScriptClassPrivate* get(const QScriptClass* q);
    inline static QScriptClass* get(const QScriptClassPrivate* d);
    inline static QScriptClassPrivate* safeGet(const QScriptClass* q);
    inline static QScriptClass* safeGet(const QScriptClassPrivate* d);

    inline QScriptClassPrivate(QScriptEnginePrivate* engine, QScriptClass* q);
    inline QScriptEnginePrivate* engine() const;

    inline QScriptClass* userCallback() const;

    static v8::Handle<v8::FunctionTemplate> createToStringTemplate();

private:
    QScriptClass* q_ptr;
    // FIXME that should shared data pointer but we need to avoid circular references too
    // Note that now this pointer could be invalid and there is now way to check it
    QScriptEnginePrivate* m_engine;
};

class QScriptClassObject : public QScriptV8ObjectWrapper<QScriptClassObject, &QScriptEnginePrivate::scriptClassTemplate> {
public:
    QScriptClassObject() {}
    ~QScriptClassObject()
    {
        m_original.Dispose();
    }

    v8::Handle<v8::Value> property(v8::Handle<v8::String> property);
    v8::Handle<v8::Value> property(uint32_t property);
    v8::Handle<v8::Integer> propertyFlags(v8::Handle<v8::String> property);
    v8::Handle<v8::Integer> propertyFlags(uint32_t property);
    v8::Handle<v8::Value> setProperty(v8::Handle<v8::String> property, v8::Local<v8::Value> value);
    v8::Handle<v8::Value> setProperty(uint32_t property, v8::Local<v8::Value> value);
    v8::Handle<v8::Array> enumerate();
    v8::Handle<v8::Boolean> removeProperty(uint32_t property);
    v8::Handle<v8::Boolean> removeProperty(v8::Handle<v8::String> property);

    static v8::Handle<v8::Value> call(const v8::Arguments &args);

    static v8::Handle<v8::FunctionTemplate> createFunctionTemplate(QScriptEnginePrivate *engine);
    static v8::Handle<v8::Value> newInstance(QScriptClassPrivate* m_scriptclass, v8::Handle<v8::Object> previousValue, QScriptEnginePrivate *engine);

    void setOriginal(v8::Handle<v8::Object> o)
    {
        m_original.Dispose();
        m_original = v8::Persistent<v8::Object>::New(o);
    }

    v8::Handle<v8::Object> original() const
    {
        return m_original;
    }

    QScriptClassPrivate *scriptClass() const
    {
        return m_scriptclass;
    }

    void setScriptClass(QScriptClassPrivate *scriptclass)
    {
        m_scriptclass = scriptclass;
    }

private:
    Q_DISABLE_COPY(QScriptClassObject)
    // FIXME should it be a smart pointer?
    QScriptClassPrivate *m_scriptclass;
    v8::Persistent<v8::Object> m_original;
};

QT_END_NAMESPACE

#endif // QSCRIPTCLASSPRIVATE_P_H
