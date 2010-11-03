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

#ifndef QSCRIPTCONTEXT_P_H
#define QSCRIPTCONTEXT_P_H

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

#include <QtCore/QVarLengthArray>
#include <QtCore/QPair>
#include "qscriptcontext.h"
#include "qscriptvalue.h"
#include "v8.h"

QT_BEGIN_NAMESPACE

class QScriptEnginePrivate;
class QScriptContext;

class QScriptContextPrivate : public QScriptContext
{
    Q_DECLARE_PUBLIC(QScriptContext);
public:
    static QScriptContextPrivate *get(const QScriptContext *q) { Q_ASSERT(q->d_ptr); return q->d_ptr; }
    static QScriptContext *get(QScriptContextPrivate *d) { return d->q_func(); }

    inline QScriptContextPrivate(QScriptEnginePrivate *engine, const v8::Arguments *args = 0);
    inline QScriptContextPrivate(QScriptEnginePrivate *engine, const v8::AccessorInfo *accessor);
    inline QScriptContextPrivate(QScriptEnginePrivate *engine, v8::Handle<v8::Context> context);
    inline ~QScriptContextPrivate();

    inline QScriptPassPointer<QScriptValuePrivate> argument(int index) const;
    inline int argumentCount() const;
    inline QScriptPassPointer<QScriptValuePrivate> argumentsObject() const;
    inline QScriptPassPointer<QScriptValuePrivate> thisObject() const;
    inline QScriptPassPointer<QScriptValuePrivate> callee() const;

    inline QScriptPassPointer<QScriptValuePrivate> activationObject() const;
    inline QScriptValueList scopeChain() const;
    inline void pushScope(QScriptValuePrivate *object);
    inline QScriptPassPointer<QScriptValuePrivate> popScope();
    inline void setInheritedScope(v8::Handle<v8::Context>);

    QScriptContext* q_ptr;
    QScriptEnginePrivate *engine;
    const v8::Arguments *arguments;
    const v8::AccessorInfo *accessorInfo;
    v8::Persistent<v8::Context> context;
    QList<v8::Persistent<v8::Context> > scopes;
    v8::Persistent<v8::Context> inheritedScope;
    QScriptContextPrivate *parent;
private:
    Q_DISABLE_COPY(QScriptContextPrivate)
};


QT_BEGIN_INCLUDE_NAMESPACE
#include "qscriptvalue_p.h"
#include "qscriptengine_p.h"
QT_END_INCLUDE_NAMESPACE


inline QScriptContextPrivate::QScriptContextPrivate(QScriptEnginePrivate *engine, const v8::Arguments *args)
: q_ptr(this), engine(engine), arguments(args), accessorInfo(0), parent(engine->setCurrentQSContext(this))
{
}

inline QScriptContextPrivate::QScriptContextPrivate(QScriptEnginePrivate *engine, const v8::AccessorInfo *accessor)
: q_ptr(this), engine(engine), arguments(0), accessorInfo(accessor), parent(engine->setCurrentQSContext(this))
{
}

inline QScriptContextPrivate::QScriptContextPrivate(QScriptEnginePrivate *engine, v8::Handle<v8::Context> context)
    : q_ptr(this), engine(engine), arguments(0), accessorInfo(0),
      context(v8::Persistent<v8::Context>::New(context)), parent(engine->setCurrentQSContext(this))
{
    context->Enter();
}

QScriptContextPrivate::~QScriptContextPrivate()
{
    if (!parent)
        return;
    QScriptContextPrivate *old = engine->setCurrentQSContext(parent);
    if (old != this) {
        qWarning("QScriptEngine::pushContext() doesn't match with popContext()");
        old->parent = 0;
        //old is most likely leaking.
    }

    while (!scopes.isEmpty())
        QScriptValuePrivate::get(popScope());

    if (!inheritedScope.IsEmpty())
        inheritedScope.Dispose();

    if (!context.IsEmpty()) {
        context->Exit();
        context.Dispose();
    }
}



inline QScriptPassPointer<QScriptValuePrivate> QScriptContextPrivate::argument(int index) const
{
    if (index < 0)
        return new QScriptValuePrivate();

    if (arguments) {
        if (index >= arguments->Length())
            return new QScriptValuePrivate(engine, QScriptValue::UndefinedValue);

        return new QScriptValuePrivate(engine, (*arguments)[index]);
    }

    Q_UNIMPLEMENTED();
    return new QScriptValuePrivate();
}

inline int QScriptContextPrivate::argumentCount() const
{
    if (arguments) {
        return arguments->Length();
    }

    Q_UNIMPLEMENTED();
    return -1;
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptContextPrivate::argumentsObject() const
{
    if (arguments) {
        // Create a fake arguments object.
        // TODO: Get the real one from v8, if possible.
        int argc = argumentCount();
        QScriptPassPointer<QScriptValuePrivate> args = engine->newArray(argc);
        for (int i = 0; i < argc; ++i) {
            QScriptValue arg = QScriptValuePrivate::get(argument(i));
            args->setProperty(i, QScriptValuePrivate::get(arg), v8::DontEnum);
        }
        QScriptValue callee_ = QScriptValuePrivate::get(callee());
        args->setProperty(QString::fromLatin1("callee"), QScriptValuePrivate::get(callee_), v8::DontEnum);
        return args;
    }

    Q_UNIMPLEMENTED();
    return new QScriptValuePrivate();
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptContextPrivate::thisObject() const
{
    if (arguments) {
        return new QScriptValuePrivate(engine, arguments->This());
    } else if (accessorInfo) {
        return new QScriptValuePrivate(engine, accessorInfo->This());
    }

    return new QScriptValuePrivate();
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptContextPrivate::callee() const
{
    if (arguments)
        return new QScriptValuePrivate(engine, arguments->Callee());

    Q_UNIMPLEMENTED();
    return new QScriptValuePrivate();
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptContextPrivate::activationObject() const
{
    if (!parent)
        return new QScriptValuePrivate(engine, engine->globalObject());
    Q_ASSERT(!context.IsEmpty());
    return new QScriptValuePrivate(engine, context->GetExtensionObject());
}

inline QScriptValueList QScriptContextPrivate::scopeChain() const
{
    QScriptValueList list;
    for (int i = 0; i < scopes.size(); ++i) {
        v8::Handle<v8::Object> object = scopes.at(i)->GetExtensionObject();
        list.append(QScriptValuePrivate::get(new QScriptValuePrivate(engine, object)));
    }

    if (!context.IsEmpty())
        list.append(QScriptValuePrivate::get(activationObject()));

    if (!inheritedScope.IsEmpty()) {
        v8::Handle<v8::Context> current = inheritedScope;
        do {
            v8::Handle<v8::Object> object = current->GetExtensionObject();
            list.append(QScriptValuePrivate::get(new QScriptValuePrivate(engine, object)));
            current = current->GetPrevious();
        } while (!current.IsEmpty());
    }

    if (parentContext()) {
        // Implicit global context
        list.append(QScriptValuePrivate::get(new QScriptValuePrivate(engine, engine->globalObject())));
    }

    return list;
}

inline void QScriptContextPrivate::pushScope(QScriptValuePrivate *object)
{
    v8::Handle<v8::Object> objectHandle(v8::Object::Cast(*object->asV8Value(engine)));
    v8::Handle<v8::Context> scopeContext = v8::Context::NewScopeContext(objectHandle);
    scopes.append(v8::Persistent<v8::Context>::New(scopeContext));
    scopeContext->Enter();
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptContextPrivate::popScope()
{
    if (scopes.isEmpty()) {
        // In the old back-end, this would pop the activation object
        // from the scope chain.
        Q_UNIMPLEMENTED();
        return new QScriptValuePrivate();
    }
    v8::Persistent<v8::Context> scopeContext = scopes.takeFirst();
    v8::Handle<v8::Object> object = scopeContext->GetExtensionObject();
    scopeContext->Exit();
    scopeContext.Dispose();
    return new QScriptValuePrivate(engine, object);
}

inline void QScriptContextPrivate::setInheritedScope(v8::Handle<v8::Context> object)
{
    Q_ASSERT(inheritedScope.IsEmpty());
    inheritedScope = v8::Persistent<v8::Context>::New(object);
}

QT_END_NAMESPACE

#endif
