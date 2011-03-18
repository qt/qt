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
#include "qscriptshareddata_p.h"
#include "qscriptvalue_p.h"
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

    inline QScriptContextPrivate(QScriptEnginePrivate *engine); // the global context (member of QScriptEnginePrivate)
    inline QScriptContextPrivate(QScriptEnginePrivate *engine, const v8::Arguments *args, v8::Handle<v8::Value> callee = v8::Handle<v8::Value>(), v8::Handle<v8::Object> customThisObject = v8::Handle<v8::Object>()); // native function context (on the stack)
    inline QScriptContextPrivate(QScriptEnginePrivate *engine, const v8::AccessorInfo *accessor); // native acessors (on the stack)
    inline QScriptContextPrivate(QScriptEnginePrivate *engine, v8::Handle<v8::Context> context); // from QScriptEngine::pushContext
    inline QScriptContextPrivate(QScriptContextPrivate *parent, v8::Handle<v8::StackFrame> frame); // internal, for js frame (allocated in parentContext())
    inline ~QScriptContextPrivate();

    inline bool isGlobalContext() const { return !parent; }
    inline bool isNativeFunction() const { return arguments; }
    inline bool isNativeAccessor() const { return accessorInfo; }
    inline bool isJSFrame() const { return !frame.IsEmpty(); }
    inline bool isPushedContext() const { return !context.IsEmpty() && !arguments && !accessorInfo; }

    inline QScriptPassPointer<QScriptValuePrivate> argument(int index) const;
    inline int argumentCount() const;
    inline QScriptPassPointer<QScriptValuePrivate> argumentsObject() const;
    v8::Handle<v8::Object> thisObject() const;
    inline void setThisObject(QScriptValuePrivate *);
    inline QScriptPassPointer<QScriptValuePrivate> callee() const;

    inline QScriptPassPointer<QScriptValuePrivate> activationObject() const;
    inline void setActivationObject(QScriptValuePrivate *);
    inline QScriptValueList scopeChain() const;
    inline void pushScope(QScriptValuePrivate *object);
    inline QScriptPassPointer<QScriptValuePrivate> popScope();
    inline QScriptPassPointer<QScriptValuePrivate> createArgumentsObject();
    inline void initializeArgumentsProperty();

    inline v8::Handle<v8::Value> throwError(Error error, const QString &text);

    QScriptContext* q_ptr;
    QScriptEnginePrivate *engine;
    const v8::Arguments *arguments;
    const v8::AccessorInfo *accessorInfo;
    v8::Persistent<v8::Context> context;
    QList<v8::Persistent<v8::Context> > scopes;
    v8::Persistent<v8::Context> inheritedScope;
    QScriptContextPrivate *parent; //the parent native frame as seen by the engine
    mutable QScriptContextPrivate *previous; //the previous js frame (lazily build)
    v8::Persistent<v8::StackFrame> frame; //only for js frames
    QScriptSharedDataPointer<QScriptValuePrivate> argsObject;
    v8::Persistent<v8::Object> m_thisObject;
    v8::Persistent<v8::Value> m_callee;
    bool hasArgumentGetter;

    static const int stackTraceLimit = 100;

private:
    static v8::Handle<v8::Value> argumentsPropertyGetter(v8::Local<v8::String> , const v8::AccessorInfo &);
    Q_DISABLE_COPY(QScriptContextPrivate)
};


QT_END_NAMESPACE

#endif
