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

#include "qmlexpression.h"
#include "qmlexpression_p.h"

#include "qmlengine_p.h"
#include "qmlcontext_p.h"
#include "qmlrewrite_p.h"
#include "qmlcompiler_p.h"

#include <QtCore/qdebug.h>
#include <QtScript/qscriptprogram.h>

#include <private/qscriptdeclarativeclass_p.h>

Q_DECLARE_METATYPE(QList<QObject *>);

QT_BEGIN_NAMESPACE

bool QmlDelayedError::addError(QmlEnginePrivate *e)
{
    if (!e || prevError) return false;

    if (e->inProgressCreations == 0) return false; // Not in construction

    prevError = &e->erroredBindings;
    nextError = e->erroredBindings;
    e->erroredBindings = this;
    if (nextError) nextError->prevError = &nextError;

    return true;
}

QmlExpressionData::QmlExpressionData()
: expressionFunctionValid(false), expressionRewritten(false), me(0), 
  trackChange(true), isShared(false), line(-1), guardList(0), guardListLength(0)
{
}

QmlExpressionData::~QmlExpressionData()
{
    if (guardList) { delete [] guardList; guardList = 0; }
}

QmlExpressionPrivate::QmlExpressionPrivate()
: data(new QmlExpressionData)
{
    data->q = this;
}

QmlExpressionPrivate::QmlExpressionPrivate(QmlExpressionData *d)
: data(d)
{
    data->q = this;
}

QmlExpressionPrivate::~QmlExpressionPrivate()
{
    if (data) { data->q = 0; data->release(); data = 0; }
}

void QmlExpressionPrivate::init(QmlContext *ctxt, const QString &expr, 
                                QObject *me)
{
    data->expression = expr;

    data->QmlAbstractExpression::setContext(ctxt);
    data->me = me;
}

void QmlExpressionPrivate::init(QmlContext *ctxt, void *expr, QmlRefCount *rc, 
                                QObject *me, const QString &url, int lineNumber)
{
    data->url = url;
    data->line = lineNumber;

    quint32 *exprData = (quint32 *)expr;
    QmlCompiledData *dd = (QmlCompiledData *)rc;

    data->expressionRewritten = true;
    data->expression = QString::fromRawData((QChar *)(exprData + 2), exprData[1]);

    int progIdx = *(exprData);
    bool isShared = progIdx & 0x80000000;
    progIdx &= 0x7FFFFFFF;

    QmlEngine *engine = ctxt->engine();
    QmlEnginePrivate *ep = QmlEnginePrivate::get(engine);
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    if (isShared) {

        if (!dd->cachedClosures.at(progIdx)) {
            QScriptContext *scriptContext = QScriptDeclarativeClass::pushCleanContext(scriptEngine);
            scriptContext->pushScope(ep->contextClass->newSharedContext());
            dd->cachedClosures[progIdx] = new QScriptValue(scriptEngine->evaluate(data->expression, data->url, data->line));
            scriptEngine->popContext();
        }

        data->expressionFunction = *dd->cachedClosures.at(progIdx);
        data->isShared = true;
        data->expressionFunctionValid = true;

    } else {

#if !defined(Q_OS_SYMBIAN) //XXX Why doesn't this work?
        if (!dd->cachedPrograms.at(progIdx)) {
            dd->cachedPrograms[progIdx] =
                new QScriptProgram(data->expression, data->url, data->line);
        }

        data->expressionFunction = evalInObjectScope(ctxt, me, *dd->cachedPrograms.at(progIdx));
#else
        data->expressionFunction = evalInObjectScope(ctxt, me, data->expression);
#endif

        data->expressionFunctionValid = true;
    }

    data->QmlAbstractExpression::setContext(ctxt);
    data->me = me;
}

QScriptValue QmlExpressionPrivate::evalInObjectScope(QmlContext *context, QObject *object, 
                                                     const QString &program)
{
    QmlEnginePrivate *ep = QmlEnginePrivate::get(context->engine());
    QScriptContext *scriptContext = QScriptDeclarativeClass::pushCleanContext(&ep->scriptEngine);
    scriptContext->pushScope(ep->contextClass->newContext(context, object));
    QScriptValue rv = ep->scriptEngine.evaluate(program);
    ep->scriptEngine.popContext();
    return rv;
}

QScriptValue QmlExpressionPrivate::evalInObjectScope(QmlContext *context, QObject *object, 
                                                     const QScriptProgram &program)
{
    QmlEnginePrivate *ep = QmlEnginePrivate::get(context->engine());
    QScriptContext *scriptContext = QScriptDeclarativeClass::pushCleanContext(&ep->scriptEngine);
    scriptContext->pushScope(ep->contextClass->newContext(context, object));
    QScriptValue rv = ep->scriptEngine.evaluate(program);
    ep->scriptEngine.popContext();
    return rv;
}

/*!
    \class QmlExpression
    \brief The QmlExpression class evaluates ECMAScript in a QML context.
*/

/*!
    Create an invalid QmlExpression.

    As the expression will not have an associated QmlContext, this will be a
    null expression object and its value will always be an invalid QVariant.
 */
QmlExpression::QmlExpression()
: QObject(*new QmlExpressionPrivate, 0)
{
}

/*!  \internal */
QmlExpression::QmlExpression(QmlContext *ctxt, void *expr,
                             QmlRefCount *rc, QObject *me, 
                             const QString &url, int lineNumber,
                             QmlExpressionPrivate &dd)
: QObject(dd, 0)
{
    Q_D(QmlExpression);
    d->init(ctxt, expr, rc, me, url, lineNumber);
}

/*!
    Create a QmlExpression object.

    The \a expression ECMAScript will be executed in the \a ctxt QmlContext.
    If specified, the \a scope object's properties will also be in scope during
    the expression's execution.
*/
QmlExpression::QmlExpression(QmlContext *ctxt, const QString &expression,
                             QObject *scope)
: QObject(*new QmlExpressionPrivate, 0)
{
    Q_D(QmlExpression);
    d->init(ctxt, expression, scope);
}

/*!  \internal */
QmlExpression::QmlExpression(QmlContext *ctxt, const QString &expression,
                             QObject *scope, QmlExpressionPrivate &dd)
: QObject(dd, 0)
{
    Q_D(QmlExpression);
    d->init(ctxt, expression, scope);
}

/*!
    Destroy the QmlExpression instance.
*/
QmlExpression::~QmlExpression()
{
}

/*!
    Returns the QmlEngine this expression is associated with, or 0 if there
    is no association or the QmlEngine has been destroyed.
*/
QmlEngine *QmlExpression::engine() const
{
    Q_D(const QmlExpression);
    return d->data->context()?d->data->context()->engine():0;
}

/*!
    Returns the QmlContext this expression is associated with, or 0 if there
    is no association or the QmlContext has been destroyed.
*/
QmlContext *QmlExpression::context() const
{
    Q_D(const QmlExpression);
    return d->data->context();
}

/*!
    Returns the expression string.
*/
QString QmlExpression::expression() const
{
    Q_D(const QmlExpression);
    return d->data->expression;
}

/*!
    Clear the expression.
*/
void QmlExpression::clearExpression()
{
    setExpression(QString());
}

/*!
    Set the expression to \a expression.
*/
void QmlExpression::setExpression(const QString &expression)
{
    Q_D(QmlExpression);

    d->clearGuards();

    d->data->expression = expression;
    d->data->expressionFunctionValid = false;
    d->data->expressionRewritten = false;
    d->data->expressionFunction = QScriptValue();
}

void QmlExpressionPrivate::exceptionToError(QScriptEngine *scriptEngine, 
                                            QmlError &error)
{
    if (scriptEngine->hasUncaughtException() && 
        scriptEngine->uncaughtException().isError()) {

        QString fileName;
        int lineNumber = scriptEngine->uncaughtExceptionLineNumber();

        QScriptValue exception = scriptEngine->uncaughtException();
        QLatin1String fileNameProp("fileName");

        if (!exception.property(fileNameProp).toString().isEmpty()){
            fileName = exception.property(fileNameProp).toString();
        } else {
            fileName = QLatin1String("<Unknown File>");
        }

        error.setUrl(QUrl(fileName));
        error.setLine(lineNumber);
        error.setColumn(-1);
        error.setDescription(exception.toString());
    } else {
        error = QmlError();
    }
}

QVariant QmlExpressionPrivate::evalQtScript(QObject *secondaryScope, bool *isUndefined)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QmlPerfTimer<QmlPerf::BindValueQt> perfqt;
#endif

    QmlExpressionData *data = this->data;
    QmlContextPrivate *ctxtPriv = data->context()->d_func();
    QmlEngine *engine = data->context()->engine();
    QmlEnginePrivate *ep = QmlEnginePrivate::get(engine);

    if (secondaryScope)
       ctxtPriv->defaultObjects.append(secondaryScope);

    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    if (!data->expressionFunctionValid) {

        QScriptContext *scriptContext = QScriptDeclarativeClass::pushCleanContext(scriptEngine);
        scriptContext->pushScope(ep->contextClass->newContext(data->context(), data->me));

        if (data->expressionRewritten) {
            data->expressionFunction = scriptEngine->evaluate(data->expression, 
                                                              data->url, data->line);
        } else {
            QmlRewrite::RewriteBinding rewriteBinding;

            bool ok = true;
            const QString code = rewriteBinding(data->expression, &ok);
            if (!ok) {
                scriptEngine->popContext();
                return QVariant();
            }
            data->expressionFunction = scriptEngine->evaluate(code, data->url, data->line);
        }

        scriptEngine->popContext();
        data->expressionFunctionValid = true;
    }

    QmlContext *oldSharedContext = 0;
    QObject *oldSharedScope = 0;
    if (data->isShared) {
        oldSharedContext = ep->sharedContext;
        oldSharedScope = ep->sharedScope;
        ep->sharedContext = data->context();
        ep->sharedScope = data->me;
    }

    QScriptValue svalue = data->expressionFunction.call();

    if (data->isShared) {
        ep->sharedContext = oldSharedContext;
        ep->sharedScope = oldSharedScope;
    }

    if (isUndefined)
        *isUndefined = svalue.isUndefined() || scriptEngine->hasUncaughtException();

    // Handle exception
    if (scriptEngine->hasUncaughtException()) {
       exceptionToError(scriptEngine, data->error);
       scriptEngine->clearExceptions();
       return QVariant();
    } else {
        data->error = QmlError();
    }

    if (secondaryScope) {
        QObject *last = ctxtPriv->defaultObjects.takeLast();
        Q_ASSERT(last == secondaryScope);
        Q_UNUSED(last);
    }

    QVariant rv;

    if (svalue.isArray()) {
        int length = svalue.property(QLatin1String("length")).toInt32();
        if (length && svalue.property(0).isObject()) {
            QList<QObject *> list;
            for (int ii = 0; ii < length; ++ii) {
                QScriptValue arrayItem = svalue.property(ii);
                QObject *d = arrayItem.toQObject();
                list << d;
            }
            rv = QVariant::fromValue(list);
        }
    } else if (svalue.isObject() &&
               ep->objectClass->scriptClass(svalue) == ep->objectClass) {
        QObject *o = svalue.toQObject();
        int type = QMetaType::QObjectStar;
        // If the object is null, we extract the predicted type.  While this isn't
        // 100% reliable, in many cases it gives us better error messages if we
        // assign this null-object to an incompatible property
        if (!o) type = ep->objectClass->objectType(svalue);

        return QVariant(type, &o);
    }

    if (rv.isNull())
        rv = svalue.toVariant();

    return rv;
}

QVariant QmlExpressionPrivate::value(QObject *secondaryScope, bool *isUndefined)
{
    Q_Q(QmlExpression);

    QVariant rv;
    if (!q->engine()) {
        qWarning("QmlExpression: Attempted to evaluate an expression in an invalid context");
        return rv;
    }

    if (data->expression.isEmpty())
        return rv;

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QmlPerfTimer<QmlPerf::BindValue> perf;
#endif

    QmlEnginePrivate *ep = QmlEnginePrivate::get(q->engine());

    QmlExpression *lastCurrentExpression = ep->currentExpression;
    bool lastCaptureProperties = ep->captureProperties;
    QPODVector<QmlEnginePrivate::CapturedProperty> lastCapturedProperties;
    ep->capturedProperties.copyAndClear(lastCapturedProperties);

    ep->currentExpression = q;
    ep->captureProperties = data->trackChange;

    // This object might be deleted during the eval
    QmlExpressionData *localData = data;
    localData->addref();

    rv = evalQtScript(secondaryScope, isUndefined);

    ep->currentExpression = lastCurrentExpression;
    ep->captureProperties = lastCaptureProperties;

    // Check if we were deleted
    if (localData->q) {
        if ((!data->trackChange || !ep->capturedProperties.count()) && data->guardList) {
            clearGuards();
        } else if(data->trackChange) {
            updateGuards(ep->capturedProperties);
        }
    }

    localData->release();

    lastCapturedProperties.copyAndClear(ep->capturedProperties);

    return rv;
}

/*!
    Returns the value of the expression, or an invalid QVariant if the
    expression is invalid or has an error.

    \a isUndefined is set to true if the expression resulted in an
    undefined value.

    \sa hasError(), error()
*/
QVariant QmlExpression::value(bool *isUndefined)
{
    Q_D(QmlExpression);
    return d->value(0, isUndefined);
}

/*!
    Returns true if the expression results in a constant value.
    QmlExpression::value() must have been invoked at least once before the
    return from this method is valid.
 */
bool QmlExpression::isConstant() const
{
    Q_D(const QmlExpression);
    return !d->data->guardList;
}

/*!
    Returns true if the changes are tracked in the expression's value.
*/
bool QmlExpression::trackChange() const
{
    Q_D(const QmlExpression);
    return d->data->trackChange;
}

/*!
    Set whether changes are tracked in the expression's value to \a trackChange.

    If true, the QmlExpression will monitor properties involved in the
    expression's evaluation, and call QmlExpression::valueChanged() if they have
    changed.  This allows an application to ensure that any value associated
    with the result of the expression remains up to date.

    If false, the QmlExpression will not montitor properties involved in the
    expression's evaluation, and QmlExpression::valueChanged() will never be
    called.  This is more efficient if an application wants a "one off"
    evaluation of the expression.

    By default, trackChange is true.
*/
void QmlExpression::setTrackChange(bool trackChange)
{
    Q_D(QmlExpression);
    d->data->trackChange = trackChange;
}

/*!
    Returns the source file URL for this expression.  The source location must
    have been previously set by calling setSourceLocation().
*/
QString QmlExpression::sourceFile() const
{
    Q_D(const QmlExpression);
    return d->data->url;
}

/*!
    Returns the source file line number for this expression.  The source location 
    must have been previously set by calling setSourceLocation().
*/
int QmlExpression::lineNumber() const
{
    Q_D(const QmlExpression);
    return d->data->line;
}

/*!
    Set the location of this expression to \a line of \a url. This information
    is used by the script engine.
*/
void QmlExpression::setSourceLocation(const QString &url, int line)
{
    Q_D(QmlExpression);
    d->data->url = url;
    d->data->line = line;
}

/*!
    Returns the expression's scope object, if provided, otherwise 0.

    In addition to data provided by the expression's QmlContext, the scope
    object's properties are also in scope during the expression's evaluation.
*/
QObject *QmlExpression::scopeObject() const
{
    Q_D(const QmlExpression);
    return d->data->me;
}

/*!
    Returns true if the last call to value() resulted in an error, 
    otherwise false.
    
    \sa error(), clearError()
*/
bool QmlExpression::hasError() const
{
    Q_D(const QmlExpression);
    return d->data->error.isValid();
}

/*!
    Clear any expression errors.  Calls to hasError() following this will
    return false.

    \sa hasError(), error()
*/
void QmlExpression::clearError()
{
    Q_D(QmlExpression);
    d->data->error = QmlError();
}

/*!
    Return any error from the last call to value().  If there was no error,
    this returns an invalid QmlError instance.

    \sa hasError(), clearError()
*/

QmlError QmlExpression::error() const
{
    Q_D(const QmlExpression);
    return d->data->error;
}

/*! \internal */
void QmlExpression::__q_notify()
{
    emitValueChanged();
}

void QmlExpressionPrivate::clearGuards()
{
    Q_Q(QmlExpression);

    static int notifyIdx = -1;
    if (notifyIdx == -1) 
        notifyIdx = 
            QmlExpression::staticMetaObject.indexOfMethod("__q_notify()");

    for (int ii = 0; ii < data->guardListLength; ++ii) {
        if (data->guardList[ii].data()) {
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 2))
            QMetaObject::disconnectOne(data->guardList[ii].data(), 
                                       data->guardList[ii].notifyIndex, 
                                       q, notifyIdx);
#else
            // QTBUG-6781
            QMetaObject::disconnect(data->guardList[ii].data(), 
                                    data->guardList[ii].notifyIndex, 
                                    q, notifyIdx);
#endif
        }
    }

    delete [] data->guardList; data->guardList = 0; 
    data->guardListLength = 0;
}

void QmlExpressionPrivate::updateGuards(const QPODVector<QmlEnginePrivate::CapturedProperty> &properties)
{
    //clearGuards();
    Q_Q(QmlExpression);

    static int notifyIdx = -1;
    if (notifyIdx == -1) 
        notifyIdx = 
            QmlExpression::staticMetaObject.indexOfMethod("__q_notify()");

    QmlExpressionData::SignalGuard *newGuardList = 0;
    
    if (properties.count() != data->guardListLength)
        newGuardList = new QmlExpressionData::SignalGuard[properties.count()];

    bool outputWarningHeader = false;
    int hit = 0;
    for (int ii = 0; ii < properties.count(); ++ii) {
        const QmlEnginePrivate::CapturedProperty &property = properties.at(ii);

        bool needGuard = true;
        if (ii >= data->guardListLength) {
            // New guard
        } else if(data->guardList[ii].data() == property.object && 
                  data->guardList[ii].notifyIndex == property.notifyIndex) {
            // Cache hit
            if (!data->guardList[ii].isDuplicate || 
                (data->guardList[ii].isDuplicate && hit == ii)) {
                needGuard = false;
                ++hit;
            }
        } else if(data->guardList[ii].data() && !data->guardList[ii].isDuplicate) {
            // Cache miss
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 2))
            QMetaObject::disconnectOne(data->guardList[ii].data(), 
                                       data->guardList[ii].notifyIndex, 
                                       q, notifyIdx);
#else
            // QTBUG-6781
            QMetaObject::disconnect(data->guardList[ii].data(), 
                                    data->guardList[ii].notifyIndex, 
                                    q, notifyIdx);
#endif
        } 
        /* else {
            // Cache miss, but nothing to do
        } */

        if (needGuard) {
            if (!newGuardList) {
                newGuardList = new QmlExpressionData::SignalGuard[properties.count()];
                for (int jj = 0; jj < ii; ++jj)
                    newGuardList[jj] = data->guardList[jj];
            }

            if (property.notifyIndex != -1) {
                bool existing = false;
                for (int jj = 0; !existing && jj < ii; ++jj) 
                    existing = newGuardList[jj].data() == property.object &&
                        newGuardList[jj].notifyIndex == property.notifyIndex;

                newGuardList[ii] = property.object;
                newGuardList[ii].notifyIndex = property.notifyIndex;
                if (existing)
                    newGuardList[ii].isDuplicate = true;
                else
                    QMetaObject::connect(property.object, property.notifyIndex,
                                         q, notifyIdx);
            } else {
                if (!outputWarningHeader) {
                    outputWarningHeader = true;
                    qWarning() << "QmlExpression: Expression" << q->expression()
                               << "depends on non-NOTIFYable properties:";
                }

                const QMetaObject *metaObj = property.object->metaObject();
                QMetaProperty metaProp = metaObj->property(property.coreIndex);

                qWarning().nospace() << "    " << metaObj->className()
                                     << "::" << metaProp.name();
            }
        } else if (newGuardList) {
            newGuardList[ii] = data->guardList[ii];
        }
    }

    for (int ii = properties.count(); ii < data->guardListLength; ++ii) {
        if (data->guardList[ii].data() && !data->guardList[ii].isDuplicate) {
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 2))
            QMetaObject::disconnectOne(data->guardList[ii].data(), 
                                       data->guardList[ii].notifyIndex, 
                                       q, notifyIdx);
#else
            // QTBUG-6781
            QMetaObject::disconnect(data->guardList[ii].data(), 
                                    data->guardList[ii].notifyIndex, 
                                    q, notifyIdx);
#endif
        }
    }

    if (newGuardList) {
        if (data->guardList) delete [] data->guardList;
        data->guardList = newGuardList;
        data->guardListLength = properties.count();
    }
}

/*!
    \fn void QmlExpression::valueChanged()

    Emitted each time the expression value changes from the last time it was
    evaluated.  The expression must have been evaluated at least once (by
    calling QmlExpression::value()) before this signal will be emitted.
*/

/*!
    Subclasses can capture the emission of the valueChanged() signal by overriding
    this function. They can choose whether to then call valueChanged().
*/
void QmlExpression::emitValueChanged()
{
    emit valueChanged();
}

QmlAbstractExpression::QmlAbstractExpression()
: m_context(0), m_prevExpression(0), m_nextExpression(0)
{
}

QmlAbstractExpression::~QmlAbstractExpression()
{
    if (m_prevExpression) {
        *m_prevExpression = m_nextExpression;
        if (m_nextExpression) 
            m_nextExpression->m_prevExpression = m_prevExpression;
    }
}

QmlContext *QmlAbstractExpression::context() const
{
    return m_context;
}

void QmlAbstractExpression::setContext(QmlContext *context)
{
    if (m_prevExpression) {
        *m_prevExpression = m_nextExpression;
        if (m_nextExpression) 
            m_nextExpression->m_prevExpression = m_prevExpression;
        m_prevExpression = 0;
        m_nextExpression = 0;
    }

    m_context = context;

    if (m_context) {
        QmlContextPrivate *cp = 
            static_cast<QmlContextPrivate *>(QObjectPrivate::get(m_context));
        m_nextExpression = cp->expressions;
        if (m_nextExpression) 
            m_nextExpression->m_prevExpression = &m_nextExpression;
        m_prevExpression = &cp->expressions;
        cp->expressions = this;
    }
}

void QmlAbstractExpression::refresh()
{
}

bool QmlAbstractExpression::isValid() const
{
    return m_context != 0;
}

QT_END_NAMESPACE

