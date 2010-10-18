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

#include "qscriptcontext.h"
#include "qscriptcontext_p.h"
#include "qscriptengine.h"
#include "qscriptisolate_p.h"

#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

/*!
  \since 4.3
  \class QScriptContext

  \brief The QScriptContext class represents a Qt Script function invocation.

  \ingroup script
  \mainclass

  A QScriptContext provides access to the `this' object and arguments
  passed to a script function. You typically want to access this
  information when you're writing a native (C++) function (see
  QScriptEngine::newFunction()) that will be called from script
  code. For example, when the script code

  \snippet doc/src/snippets/code/src_script_qscriptcontext.cpp 0

  is evaluated, a QScriptContext will be created, and the context will
  carry the arguments as QScriptValues; in this particular case, the
  arguments will be one QScriptValue containing the number 20.5, a second
  QScriptValue containing the string \c{"hello"}, and a third QScriptValue
  containing a Qt Script object.

  Use argumentCount() to get the number of arguments passed to the
  function, and argument() to get an argument at a certain index. The
  argumentsObject() function returns a Qt Script array object
  containing all the arguments; you can use the QScriptValueIterator
  to iterate over its elements, or pass the array on as arguments to
  another script function using QScriptValue::call().

  Use thisObject() to get the `this' object associated with the function call,
  and setThisObject() to set the `this' object. If you are implementing a
  native "instance method", you typically fetch the thisObject() and access
  one or more of its properties:

  \snippet doc/src/snippets/code/src_script_qscriptcontext.cpp 1

  Use isCalledAsConstructor() to determine if the function was called
  as a constructor (e.g. \c{"new foo()"} (as constructor) or just
  \c{"foo()"}).  When a function is called as a constructor, the
  thisObject() contains the newly constructed object that the function
  is expected to initialize.

  Use throwValue() or throwError() to throw an exception.

  Use callee() to obtain the QScriptValue that represents the function being
  called. This can for example be used to call the function recursively.

  Use parentContext() to get a pointer to the context that precedes
  this context in the activation stack. This is mostly useful for
  debugging purposes (e.g. when constructing some form of backtrace).

  The activationObject() function returns the object that is used to
  hold the local variables associated with this function call. You can
  replace the activation object by calling setActivationObject(). A
  typical usage of these functions is when you want script code to be
  evaluated in the context of the parent context, e.g. to implement an
  include() function:

  \snippet doc/src/snippets/code/src_script_qscriptcontext.cpp 2

  Use backtrace() to get a human-readable backtrace associated with
  this context. This can be useful for debugging purposes when
  implementing native functions. The toString() function provides a
  string representation of the context. (QScriptContextInfo provides
  more detailed debugging-related information about the
  QScriptContext.)

  Use engine() to obtain a pointer to the QScriptEngine that this context
  resides in.

  \sa QScriptContextInfo, QScriptEngine::newFunction(), QScriptable
*/

/*!
    \enum QScriptContext::ExecutionState

    This enum specifies the frameution state of the context.

    \value NormalState The context is in a normal state.

    \value ExceptionState The context is in an exceptional state.
*/

/*!
    \enum QScriptContext::Error

    This enum specifies types of error.

    \value ReferenceError A reference error.

    \value SyntaxError A syntax error.

    \value TypeError A type error.

    \value RangeError A range error.

    \value URIError A URI error.

    \value UnknownError An unknown error.
*/

/*!
  \internal
*/
QScriptContext::QScriptContext()
    : d_ptr(static_cast<QScriptContextPrivate *>(this))
{
}

/*!
  Throws an exception with the given \a value.
  Returns the value thrown (the same as the argument).

  \sa throwError(), state()
*/
QScriptValue QScriptContext::throwValue(const QScriptValue &value)
{
    Q_UNUSED(value);
    Q_UNIMPLEMENTED();
    return QScriptValue();
}

/*!
  Throws an \a error with the given \a text.
  Returns the created error object.

  The \a text will be stored in the \c{message} property of the error
  object.

  The error object will be initialized to contain information about
  the location where the error occurred; specifically, it will have
  properties \c{lineNumber}, \c{fileName} and \c{stack}. These
  properties are described in \l {QtScript Extensions to ECMAScript}.

  \sa throwValue(), state()
*/
QScriptValue QScriptContext::throwError(Error error, const QString &text)
{
    Q_UNUSED(error);
    Q_UNUSED(text);
    Q_UNIMPLEMENTED();
    return QScriptValue();
}

/*!
  \overload

  Throws an error with the given \a text.
  Returns the created error object.

  \sa throwValue(), state()
*/
QScriptValue QScriptContext::throwError(const QString &text)
{
    Q_UNUSED(text);
    Q_UNIMPLEMENTED();
    return QScriptValue();
}

/*!
  Destroys this QScriptContext.
*/
QScriptContext::~QScriptContext()
{
}

/*!
  Returns the QScriptEngine that this QScriptContext belongs to.
*/
QScriptEngine *QScriptContext::engine() const
{
    Q_D(const QScriptContext);
    return QScriptEnginePrivate::get(d->engine);
}

/*!
  Returns the function argument at the given \a index.

  If \a index >= argumentCount(), a QScriptValue of
  the primitive type Undefined is returned.

  \sa argumentCount()
*/
QScriptValue QScriptContext::argument(int index) const
{
    Q_D(const QScriptContext);
    QScriptIsolate api(d->engine);
    return QScriptValuePrivate::get(d->argument(index));
}

/*!
  Returns the callee. The callee is the function object that this
  QScriptContext represents an invocation of.
*/
QScriptValue QScriptContext::callee() const
{
    Q_D(const QScriptContext);
    QScriptIsolate api(d->engine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(d->callee());
}

/*!
  Returns the arguments object of this QScriptContext.

  The arguments object has properties \c callee (equal to callee())
  and \c length (equal to argumentCount()), and properties \c 0, \c 1,
  ..., argumentCount() - 1 that provide access to the argument
  values. Initially, property \c P (0 <= \c P < argumentCount()) has
  the same value as argument(\c P). In the case when \c P is less
  than the number of formal parameters of the function, \c P shares
  its value with the corresponding property of the activation object
  (activationObject()). This means that changing this property changes
  the corresponding property of the activation object and vice versa.

  \sa argument(), activationObject()
*/
QScriptValue QScriptContext::argumentsObject() const
{
    Q_UNIMPLEMENTED();
    return QScriptValue();
}

/*!
  Returns true if the function was called as a constructor
  (e.g. \c{"new foo()"}); otherwise returns false.

  When a function is called as constructor, the thisObject()
  contains the newly constructed object to be initialized.

  \note This function is only guaranteed to work for a context
  corresponding to native functions.
*/
bool QScriptContext::isCalledAsConstructor() const
{
    if (d_ptr->arguments)
        return d_ptr->arguments->IsConstructCall();
    return false;
}

/*!
  Returns the parent context of this QScriptContext.
*/
QScriptContext *QScriptContext::parentContext() const
{
    return d_ptr->parent;
}

/*!
  Returns the number of arguments passed to the function
  in this invocation.

  Note that the argument count can be different from the
  formal number of arguments (the \c{length} property of
  callee()).

  \sa argument()
*/
int QScriptContext::argumentCount() const
{
    Q_D(const QScriptContext);
    return d->argumentCount();
}

/*!
  \internal
*/
QScriptValue QScriptContext::returnValue() const
{
    Q_UNIMPLEMENTED();
    return QScriptValue();
}

/*!
  \internal
*/
void QScriptContext::setReturnValue(const QScriptValue &result)
{
    Q_UNUSED(result);
    Q_UNIMPLEMENTED();
}

/*!
  Returns the activation object of this QScriptContext. The activation
  object provides access to the local variables associated with this
  context.

  \note The activation object might not be available if there is no
  active QScriptEngineAgent, as it might be optimized.

  \sa argument(), argumentsObject()
*/

QScriptValue QScriptContext::activationObject() const
{
    Q_UNIMPLEMENTED();
    return QScriptValue();
}

/*!
  Sets the activation object of this QScriptContext to be the given \a
  activation.

  If \a activation is not an object, this function does nothing.

  \note For a context corresponding to a JavaScript function, this is only
  guarenteed to work if there was an QScriptEngineAgent active on the
  engine while the function was evaluated.
*/
void QScriptContext::setActivationObject(const QScriptValue &activation)
{
    Q_UNUSED(activation);
    Q_UNIMPLEMENTED();
}

/*!
  Returns the `this' object associated with this QScriptContext.
*/
QScriptValue QScriptContext::thisObject() const
{
    Q_D(const QScriptContext);
    QScriptIsolate api(d->engine);
    return QScriptValuePrivate::get(d->thisObject());
}

/*!
  Sets the `this' object associated with this QScriptContext to be
  \a thisObject.

  If \a thisObject is not an object, this function does nothing.
*/
void QScriptContext::setThisObject(const QScriptValue &thisObject)
{
    Q_UNUSED(thisObject);
    Q_UNIMPLEMENTED();
}

/*!
  Returns the frameution state of this QScriptContext.
*/
QScriptContext::ExecutionState QScriptContext::state() const
{
    Q_UNIMPLEMENTED();
    return QScriptContext::NormalState;
}

/*!
  Returns a human-readable backtrace of this QScriptContext.

  Each line is of the form \c{<function-name>(<arguments>)@<file-name>:<line-number>}.

  To access individual pieces of debugging-related information (for
  example, to construct your own backtrace representation), use
  QScriptContextInfo.

  \sa QScriptEngine::uncaughtExceptionBacktrace(), QScriptContextInfo, toString()
*/
QStringList QScriptContext::backtrace() const
{
    Q_UNIMPLEMENTED();
    return QStringList();
}

/*!
  \since 4.4

  Returns a string representation of this context.
  This is useful for debugging.

  \sa backtrace()
*/
QString QScriptContext::toString() const
{
    Q_UNIMPLEMENTED();
    return QString();
}

/*!
  \internal
  \since 4.5

  Returns the scope chain of this QScriptContext.
*/
QScriptValueList QScriptContext::scopeChain() const
{
    Q_D(const QScriptContext);
    QScriptValueList list;
    for (int i = 0; i < d->v8Scopes.count(); i++) {
        list << QScriptValuePrivate::get(new QScriptValuePrivate(d->engine, d->v8Scopes.at(i).second));
    }
    return list;
}

static v8::Handle<v8::Value> QtScopeObjectNamedPropertyGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info)
{
    v8::HandleScope handleScope;
    v8::Local<v8::Array> scopeChain = v8::Local<v8::Array>::Cast(info.Data());
    v8::Local<v8::Object> scopeObject = v8::Local<v8::Object>::Cast(scopeChain->Get(0));
    v8::Local<v8::Object> nextInScope = v8::Local<v8::Object>::Cast(scopeChain->Get(1));
    v8::Local<v8::Value> result = scopeObject->Get(property);
    if (result.IsEmpty() || result->IsUndefined())
        return handleScope.Close(nextInScope->Get(property));
    return handleScope.Close(result);
}

static v8::Handle<v8::Value> QtScopeObjectNamedPropertySetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
{
    v8::HandleScope handleScope;
    v8::Local<v8::Array> scopeChain = v8::Local<v8::Array>::Cast(info.Data());
    v8::Local<v8::Object> scopeObject = v8::Local<v8::Object>::Cast(scopeChain->Get(0));
    v8::Local<v8::Object> nextInScope = v8::Local<v8::Object>::Cast(scopeChain->Get(1));

    if (scopeObject->Set(property, value) || nextInScope->Set(property, value))
        return handleScope.Close(value);
    return handleScope.Close(v8::Handle<v8::Value>());
}

static v8::Handle<v8::Integer> QtScopeObjectNamedPropertyQuery(v8::Local<v8::String> property, const v8::AccessorInfo& info)
{
    v8::HandleScope handleScope;

    // FIXME how we can access attributes of a property?
    Q_UNIMPLEMENTED();
    // always intercepts
    return handleScope.Close(v8::Integer::New(v8::None));
}

static v8::Handle<v8::Boolean> QtScopeObjectNamedPropertyDeleter(v8::Local<v8::String> property, const v8::AccessorInfo& info)
{
    v8::HandleScope handleScope;
    v8::Local<v8::Array> scopeChain = v8::Local<v8::Array>::Cast(info.Data());
    v8::Local<v8::Object> scopeObject = v8::Local<v8::Object>::Cast(scopeChain->Get(0));

    bool result = scopeObject->Delete(property);
    // always intercepts
    if (result)
        return handleScope.Close(v8::True());
    return handleScope.Close(v8::False());
}

static v8::Handle<v8::Array> QtScopeObjectNamedPropertyEnumeration(const v8::AccessorInfo& info)
{
    v8::HandleScope handleScope;
    v8::Local<v8::Object> nextInScope = v8::Local<v8::Object>::Cast(info.Data());
    // FIXME it should return own properties plus all from nextInScope?
    Q_UNIMPLEMENTED();
    return v8::Handle<v8::Array>();
    return handleScope.Close(info.This()->GetPropertyNames());
}

static v8::Handle<v8::Value> QtScopeObjectIndexedPropertyGetter(uint32_t index, const v8::AccessorInfo& info)
{
    v8::HandleScope handleScope;
    v8::Local<v8::Array> scopeChain = v8::Local<v8::Array>::Cast(info.Data());
    v8::Local<v8::Object> scopeObject = v8::Local<v8::Object>::Cast(scopeChain->Get(0));
    v8::Local<v8::Object> nextInScope = v8::Local<v8::Object>::Cast(scopeChain->Get(1));
    v8::Local<v8::Value> result = scopeObject->Get(index);
    if (result.IsEmpty())
        return handleScope.Close(nextInScope->Get(index));
    return handleScope.Close(result);
}

static v8::Handle<v8::Value> QtScopeObjectIndexedPropertySetter(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
{
    v8::HandleScope handleScope;
    v8::Local<v8::Array> scopeChain = v8::Local<v8::Array>::Cast(info.Data());
    v8::Local<v8::Object> scopeObject = v8::Local<v8::Object>::Cast(scopeChain->Get(0));
    v8::Local<v8::Object> nextInScope = v8::Local<v8::Object>::Cast(scopeChain->Get(1));

    if (scopeObject->Set(index, value) || nextInScope->Set(index, value))
        return handleScope.Close(value);
    return handleScope.Close(v8::Handle<v8::Value>());
}

static v8::Handle<v8::Integer> QtScopeObjectIndexedPropertyQuery(uint32_t index, const v8::AccessorInfo& info)
{
    v8::HandleScope handleScope;
    // FIXME how we can access attributes of a property?
    Q_UNIMPLEMENTED();
    // always intercepts
    return handleScope.Close(v8::Integer::New(v8::None));
}

static v8::Handle<v8::Boolean> QtScopeObjectIndexedPropertyDeleter(uint32_t index, const v8::AccessorInfo& info)
{
    v8::HandleScope handleScope;
    v8::Local<v8::Array> scopeChain = v8::Local<v8::Array>::Cast(info.Data());
    v8::Local<v8::Object> scopeObject = v8::Local<v8::Object>::Cast(scopeChain->Get(0));

    bool result = scopeObject->Delete(index);
    // always intercepts
    if (result)
        return handleScope.Close(v8::True());
    return handleScope.Close(v8::False());
}

static v8::Handle<v8::Array> QtScopeObjectIndexedPropertyEnumeration(const v8::AccessorInfo& info)
{
    Q_UNIMPLEMENTED();
    return v8::Handle<v8::Array>();
    v8::HandleScope handleScope;
    //v8::Local<v8::Object> nextInScope = v8::Local<v8::Object>::Cast(info.Data());
    // FIXME it should return own properties plus all from nextInScope?
    return handleScope.Close(info.This()->GetPropertyNames());
}

/*!
  \internal
  \since 4.5

  Adds the given \a object to the front of this context's scope chain.

  If \a object is not an object, this function does nothing.
*/
void QScriptContext::pushScope(const QScriptValue &value)
{
    Q_D(QScriptContext);
    QScriptIsolate api(d->engine);
    v8::HandleScope handleScope;
    QScriptValuePrivate *object = QScriptValuePrivate::get(value);
    if (!object->isObject())
        return;

    v8::Handle<v8::Value> securityToken = d->engine->securityToken();
    v8::Handle<v8::ObjectTemplate> scopeObjectTemplate = v8::ObjectTemplate::New();
    {   // Initialize scopeObjectTemplate
        v8::Handle<v8::Value> globalObject;
        if (d->v8Scopes.isEmpty()) {
            globalObject = d->engine->globalObject();
        } else {
            globalObject = d->v8Scopes.at(d->v8Scopes.count() - 1).first->Global();
        }
        v8::Handle<v8::Value> scopeObject = static_cast<v8::Handle<v8::Value> >(*object);
        v8::Handle<v8::Array> scopeChain = v8::Array::New(2);
        Q_ASSERT(!globalObject.IsEmpty());
        Q_ASSERT(!scopeObject.IsEmpty());
        Q_ASSERT(!scopeChain.IsEmpty());
        scopeChain->Set(0, scopeObject);
        scopeChain->Set(1, globalObject);
        scopeObjectTemplate->SetNamedPropertyHandler(QtScopeObjectNamedPropertyGetter,
                                                    QtScopeObjectNamedPropertySetter,
                                                    QtScopeObjectNamedPropertyQuery,
                                                    QtScopeObjectNamedPropertyDeleter,
                                                    QtScopeObjectNamedPropertyEnumeration,
                                                    scopeChain);
        scopeObjectTemplate->SetIndexedPropertyHandler(QtScopeObjectIndexedPropertyGetter,
                                                    QtScopeObjectIndexedPropertySetter,
                                                    QtScopeObjectIndexedPropertyQuery,
                                                    QtScopeObjectIndexedPropertyDeleter,
                                                    QtScopeObjectIndexedPropertyEnumeration,
                                                    scopeChain);
    }

    v8::Persistent< v8::Context > v8Context = v8::Context::New(/* ExtensionConfiguration */ 0, scopeObjectTemplate);
    d->v8Scopes.append(qMakePair(v8Context, v8::Persistent<v8::Value>::New(static_cast<v8::Handle<v8::Value> >(*object))));
    v8Context->SetSecurityToken(securityToken);
    v8Context->Enter();
}

/*!
  \internal
  \since 4.5

  Removes the front object from this context's scope chain, and
  returns the removed object.

  If the scope chain is already empty, this function returns an
  invalid QScriptValue.
*/
QScriptValue QScriptContext::popScope()
{
    Q_D(QScriptContext);
    QScriptIsolate api(d->engine);
    v8::HandleScope handleScope;
    if (!d->v8Scopes.count())
        return QScriptValue();

    // FIXME what would happen if setGlobalObject was called after pushScope?
    v8::Persistent<v8::Context> &v8Context = d->v8Scopes[d->v8Scopes.size() - 1].first;
    v8Context->Exit();
    v8Context.Dispose();
    v8::Persistent<v8::Value> &object = d->v8Scopes[d->v8Scopes.size() - 1].second;
    d->v8Scopes.resize(d->v8Scopes.size() - 1);

    // FIXME  we need more QSVP overloaded constructors, then we can skip redundant Dispose & New
    QScriptValuePrivate *result = new QScriptValuePrivate(d->engine, object);
    return QScriptValuePrivate::get(result);
}

QT_END_NAMESPACE
