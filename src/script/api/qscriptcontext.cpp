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

#include <QtCore/qstringlist.h>
#include "qscriptcontextinfo.h"
#include "qscriptisolate_p.h"
#include "qscriptcontext.h"
#include "qscriptcontext_p.h"
#include "qscriptengine.h"
#include "qscriptvalue_p.h"
#include "qscript_impl_p.h"


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
    Q_D(QScriptContext);
    QScriptIsolate api(d->engine);
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> exception = QScriptValuePrivate::get(value)->asV8Value(d->engine);
    if (exception.IsEmpty())
        exception = v8::Undefined();
    d->engine->throwException(exception);
    return value;
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
    Q_D(QScriptContext);
    QScriptIsolate api(d->engine);
    v8::HandleScope handleScope;
    return d->engine->scriptValueFromInternal(d->throwError(error, text));
}

/*!
  \overload

  Throws an error with the given \a text.
  Returns the created error object.

  \sa throwValue(), state()
*/
QScriptValue QScriptContext::throwError(const QString &text)
{
    Q_D(QScriptContext);
    QScriptIsolate api(d->engine);
    v8::HandleScope handleScope;
    return d->engine->scriptValueFromInternal(d->throwError(UnknownError, text));
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
    Q_D(const QScriptContext);
    QScriptIsolate api(d->engine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(d->argumentsObject());
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
    if (d_ptr->isNativeFunction())
        return d_ptr->arguments->IsConstructCall();
    if (d_ptr->isJSFrame()) {
        QScriptIsolate api(d_ptr->engine);
        return d_ptr->frame->IsConstructor();
    }
    return false;
}

/*!
  Returns the parent context of this QScriptContext.
*/
QScriptContext *QScriptContext::parentContext() const
{
    if (!d_ptr->previous && d_ptr->parent && d_ptr == d_ptr->engine->currentContext()) {
        QScriptIsolate api(d_ptr->engine);
        v8::HandleScope handleScope;
        v8::Handle<v8::StackTrace> stacktrace = v8::StackTrace::CurrentStackTrace(QScriptContextPrivate::stackTraceLimit, v8::StackTrace::kDetailed);
        //build a linked list of QScriptContextPrivate for the js frames
        QScriptContextPrivate **tail = &d_ptr->previous;
        for (int i = 0; i < stacktrace->GetFrameCount(); ++i) {
            v8::Local<v8::StackFrame> fr = stacktrace->GetFrame(i);
            *tail = new QScriptContextPrivate(d_ptr->parent, fr);
            tail = &((*tail)->previous);
        }
    }
    if (d_ptr->previous)
        return d_ptr->previous;
    if (d_ptr->isJSFrame())
        return 0; //skip all the native contexts. They are unfortunately hidden by V8, we reached the end of the stack already.
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
    Q_D(const QScriptContext);
    QScriptIsolate api(d->engine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(d->activationObject());
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
    Q_D(QScriptContext);
    QScriptIsolate api(d->engine);
    v8::HandleScope handleScope;
    d->setActivationObject(QScriptValuePrivate::get(activation));
}

/*!
  Returns the `this' object associated with this QScriptContext.
*/
QScriptValue QScriptContext::thisObject() const
{
    Q_D(const QScriptContext);
    QScriptIsolate api(d->engine);
    v8::HandleScope handleScope;
    return d->engine->scriptValueFromInternal(d->thisObject());
}

/*!
  Sets the `this' object associated with this QScriptContext to be
  \a thisObject.

  If \a thisObject is not an object, this function does nothing.
*/
void QScriptContext::setThisObject(const QScriptValue &thisObject)
{
    Q_D(QScriptContext);
    QScriptIsolate api(d->engine);
    v8::HandleScope handleScope;
    d->setThisObject(QScriptValuePrivate::get(thisObject));
}

/*!
  Returns the frameution state of this QScriptContext.
*/
QScriptContext::ExecutionState QScriptContext::state() const
{
    Q_D(const QScriptContext);
    QScriptEnginePrivate *engine = d->engine;
    if (engine) {
        QScriptIsolate api(engine, QScriptIsolate::NotNullEngine);
        return engine->hasUncaughtException() ? QScriptContext::ExceptionState : QScriptContext::NormalState;
    }
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
    QStringList result;
    const QScriptContext *ctx = this;
    while (ctx) {
        result.append(ctx->toString());
        ctx = ctx->parentContext();
    }
    return result;
}

/*!
  \since 4.4

  Returns a string representation of this context.
  This is useful for debugging.

  \sa backtrace()
*/
QString QScriptContext::toString() const
{
    QScriptContextInfo info(this);
    QString result;

    QString functionName = info.functionName();
    if (functionName.isEmpty()) {
        if (d_ptr->isGlobalContext() || (!d_ptr->previous && d_ptr->isJSFrame())) {
            result.append(QLatin1String("<global>"));
        } else if (!d_ptr->isJSFrame()) {
                result.append(QLatin1String("<native>"));
        } else {
            QScriptIsolate api(d_ptr->engine);
            if (d_ptr->frame->IsEval()) {
                result.append(QLatin1String("<eval>"));
            } else {
                result.append(QLatin1String("<anonymous>"));
            }
        }
    } else {
        result.append(functionName);
    }

    QStringList parameterNames = info.functionParameterNames();
    result.append(QLatin1Char('('));
    for (int i = 0; i < argumentCount(); ++i) {
        if (i > 0)
            result.append(QLatin1String(", "));
        if (i < parameterNames.count()) {
            result.append(parameterNames.at(i));
            result.append(QLatin1String(" = "));
        }
        QScriptValue arg = argument(i);
        if (arg.isString())
            result.append(QLatin1Char('\''));
        result.append(arg.toString());
        if (arg.isString())
            result.append(QLatin1Char('\''));
    }
    result.append(QLatin1Char(')'));

    QString fileName = info.fileName();
    int lineNumber = info.lineNumber();
    result.append(QLatin1String(" at "));
    if (!fileName.isEmpty()) {
        result.append(fileName);
        result.append(QLatin1Char(':'));
    }
    result.append(QString::number(lineNumber));
    return result;
}

/*!
  \internal
  \since 4.5

  Returns the scope chain of this QScriptContext.
*/
QScriptValueList QScriptContext::scopeChain() const
{
    Q_D(const QScriptContext);
    QScriptIsolate api(d->engine);
    v8::HandleScope handleScope;
    return d->scopeChain();
}

/*!
  \internal
  \since 4.5

  Adds the given \a object to the front of this context's scope chain.

  If \a object is not an object, this function does nothing.
*/
void QScriptContext::pushScope(const QScriptValue &object)
{
    Q_D(QScriptContext);
    Q_ASSERT(this == d->engine->currentContext());
    QScriptIsolate api(d->engine);
    v8::HandleScope handleScope;
    QScriptValuePrivate *object_p = QScriptValuePrivate::get(object);
    if (!object_p->isObject())
        return;
    d->pushScope(object_p);
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
    return QScriptValuePrivate::get(d->popScope());
}

v8::Handle<v8::Value> QScriptContextPrivate::argumentsPropertyGetter(v8::Local<v8::String> property, const v8::AccessorInfo &info)
{
    v8::Local<v8::Object> self = info.Holder();
    QScriptContextPrivate *ctx = static_cast<QScriptContextPrivate *>(v8::External::Unwrap(info.Data()));

    QScriptSharedDataPointer<QScriptValuePrivate> argsObject(ctx->argumentsObject());
    self->ForceSet(property, *argsObject);
    ctx->hasArgumentGetter = false;
    return *argsObject;
}

QT_END_NAMESPACE
