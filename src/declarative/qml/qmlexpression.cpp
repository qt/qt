/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmlexpression.h"
#include "qmlexpression_p.h"
#include "qmlengine_p.h"
#include "qmlcontext_p.h"
#include "qmlrewrite_p.h"
#include "QtCore/qdebug.h"

Q_DECLARE_METATYPE(QList<QObject *>);

QT_BEGIN_NAMESPACE

QmlExpressionPrivate::QmlExpressionPrivate()
: nextExpression(0), prevExpression(0), ctxt(0), expressionFunctionValid(false), expressionRewritten(false), sseData(0), me(0), trackChange(true), line(-1), guardList(0), guardListLength(0)
{
}

void QmlExpressionPrivate::init(QmlContext *ctxt, const QString &expr, 
                                QObject *me)
{
    Q_Q(QmlExpression);

    expression = expr;

    this->ctxt = ctxt;
    if (ctxt) {
        QmlContextPrivate *cp = ctxt->d_func();
        nextExpression = cp->expressions;
        if (nextExpression) nextExpression->prevExpression = &nextExpression;
        prevExpression = &cp->expressions;
        cp->expressions = this;
    }
    this->me = me;
}

void QmlExpressionPrivate::init(QmlContext *ctxt, void *expr, QmlRefCount *rc, 
                                QObject *me)
{
    Q_Q(QmlExpression);

    quint32 *data = (quint32 *)expr;
    Q_ASSERT(*data == BasicScriptEngineData || 
             *data == PreTransformedQtScriptData);
    if (*data == BasicScriptEngineData) {
        sse.load((const char *)(data + 1), rc);
    } else {
        expression = QString::fromRawData((QChar *)(data + 2), data[1]);
        expressionRewritten = true;
    }

    this->ctxt = ctxt;
    if (ctxt) {
        QmlContextPrivate *cp = ctxt->d_func();
        nextExpression = cp->expressions;
        if (nextExpression) nextExpression->prevExpression = &nextExpression;
        prevExpression = &cp->expressions;
        cp->expressions = this;
    }
    this->me = me;
}

QmlExpressionPrivate::~QmlExpressionPrivate()
{
    sse.deleteScriptState(sseData);
    sseData = 0;
    if (guardList) { delete [] guardList; guardList = 0; }
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
                             QmlExpressionPrivate &dd)
: QObject(dd, 0)
{
    Q_D(QmlExpression);
    d->init(ctxt, expr, rc, me);
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
    Q_D(QmlExpression);
    if (d->prevExpression) {
        *(d->prevExpression) = d->nextExpression;
        if (d->nextExpression) 
            d->nextExpression->prevExpression = d->prevExpression;
    }
}

/*!
    Returns the QmlEngine this expression is associated with, or 0 if there
    is no association or the QmlEngine has been destroyed.
*/
QmlEngine *QmlExpression::engine() const
{
    Q_D(const QmlExpression);
    return d->ctxt?d->ctxt->engine():0;
}

/*!
    Returns the QmlContext this expression is associated with, or 0 if there
    is no association or the QmlContext has been destroyed.
*/
QmlContext *QmlExpression::context() const
{
    Q_D(const QmlExpression);
    return d->ctxt;
}

/*!
    Returns the expression string.
*/
QString QmlExpression::expression() const
{
    Q_D(const QmlExpression);
    if (d->sse.isValid())
        return QLatin1String(d->sse.expression());
    else
        return d->expression;
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
    if (d->sseData) {
        d->sse.deleteScriptState(d->sseData);
        d->sseData = 0;
    }

    d->clearGuards();

    d->expression = expression;
    d->expressionFunctionValid = false;
    d->expressionRewritten = false;
    d->expressionFunction = QScriptValue();

    d->sse.clear();
}

QVariant QmlExpressionPrivate::evalSSE(QmlBasicScript::CacheState &cacheState)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::BindValueSSE> perfsse;
#endif

    QmlContextPrivate *ctxtPriv = ctxt->d_func();
    if (me)
        ctxtPriv->defaultObjects.insert(ctxtPriv->highPriorityCount , me);

    if (!sseData)
        sseData = sse.newScriptState();
    QVariant rv = sse.run(ctxt, sseData, &cacheState);

    if (me)
        ctxtPriv->defaultObjects.removeAt(ctxtPriv->highPriorityCount);

    return rv;
}

QVariant QmlExpressionPrivate::evalQtScript()
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::BindValueQt> perfqt;
#endif

    QmlContextPrivate *ctxtPriv = ctxt->d_func();
    QmlEngine *engine = ctxt->engine();

    if (me)
       ctxtPriv->defaultObjects.insert(ctxtPriv->highPriorityCount, me);

    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
    QScriptValueList oldScopeChain =
        scriptEngine->currentContext()->scopeChain();

    for (int i = 0; i < oldScopeChain.size(); ++i)
        scriptEngine->currentContext()->popScope();
    for (int i = ctxtPriv->scopeChain.size() - 1; i > -1; --i)
        scriptEngine->currentContext()->pushScope(ctxtPriv->scopeChain.at(i));

    if (!expressionFunctionValid) {

        if (expressionRewritten) {
            expressionFunction = scriptEngine->evaluate(expression, fileName, line);
        } else {
            QmlRewrite::RewriteBinding rewriteBinding;

            const QString code = rewriteBinding(expression);
            expressionFunction = scriptEngine->evaluate(code, fileName, line);
        }
        expressionFunctionValid = true;
    }

    QScriptValue svalue = expressionFunction.call();


    if (scriptEngine->hasUncaughtException()) {
        if (scriptEngine->uncaughtException().isError()){
            QScriptValue exception = scriptEngine->uncaughtException();
            QLatin1String fileNameProp("fileName");
            if (!exception.property(fileNameProp).toString().isEmpty()){
                    qWarning() << exception.property(fileNameProp).toString()
                               << scriptEngine->uncaughtExceptionLineNumber()
                               << exception.toString();
            } else {
                qWarning() << exception.toString();
            }
        }
    }

    if (me)
        ctxtPriv->defaultObjects.removeAt(ctxtPriv->highPriorityCount);

    QVariant rv;

    if (svalue.isArray()) {
        int length = svalue.property(QLatin1String("length")).toInt32();
        if (length && svalue.property(0).isObject()) {
            QList<QObject *> list;
            for (int ii = 0; ii < length; ++ii) {
                QScriptValue arrayItem = svalue.property(ii);
                QObject *d =
                    qvariant_cast<QObject *>(arrayItem.data().toVariant());
                if (d) {
                    list << d;
                } else {
                    list << 0;
                }
            }
            rv = QVariant::fromValue(list);
        }
    } else if (svalue.isObject() &&
               !svalue.isNumber() &&
               !svalue.isString() &&
               !svalue.isDate() &&
               !svalue.isError() &&
               !svalue.isFunction() &&
               !svalue.isNull() &&
               !svalue.isQMetaObject() &&
               !svalue.isQObject() &&
               !svalue.isRegExp()) {
        QScriptValue objValue = svalue.data();
        if (objValue.isValid()) {
            QVariant var = objValue.toVariant();
            if (var.userType() >= (int)QVariant::UserType &&
                QmlMetaType::isObject(var.userType()))
                rv = var;
        }
    }
    if (rv.isNull())
        rv = svalue.toVariant();

    for (int i = 0; i < ctxtPriv->scopeChain.size(); ++i)
        scriptEngine->currentContext()->popScope();
    for (int i = oldScopeChain.size() - 1; i > -1; --i)
        scriptEngine->currentContext()->pushScope(oldScopeChain.at(i));

    return rv;
}

/*!
    Returns the value of the expression, or an invalid QVariant if the
    expression is invalid or has an error.
*/
QVariant QmlExpression::value()
{
    Q_D(QmlExpression);

    QVariant rv;
    if (!d->ctxt || !engine() || (!d->sse.isValid() && d->expression.isEmpty()))
        return rv;

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::BindValue> perf;
#endif

    QmlBasicScript::CacheState cacheState = QmlBasicScript::Reset;

    QmlEnginePrivate *ep = QmlEnginePrivate::get(engine());

    QmlExpression *lastCurrentExpression = ep->currentExpression;
    QPODVector<QmlEnginePrivate::CapturedProperty> lastCapturedProperties;
    ep->capturedProperties.copyAndClear(lastCapturedProperties);

    ep->currentExpression = this;

    if (d->sse.isValid()) {
        rv = d->evalSSE(cacheState);
    } else {
        rv = d->evalQtScript();
    }

    ep->currentExpression = lastCurrentExpression;

    if ((!trackChange() || !ep->capturedProperties.count()) && d->guardList) {
        d->clearGuards();
    } else if(trackChange()) {
        d->updateGuards(ep->capturedProperties);
    }

    lastCapturedProperties.copyAndClear(ep->capturedProperties);

    return rv;
}

/*!
    Returns true if the expression results in a constant value.
    QmlExpression::value() must have been invoked at least once before the
    return from this method is valid.
 */
bool QmlExpression::isConstant() const
{
    Q_D(const QmlExpression);
    return !d->guardList;
}

/*!
    Returns true if the changes are tracked in the expression's value.
*/
bool QmlExpression::trackChange() const
{
    Q_D(const QmlExpression);
    return d->trackChange;
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
    d->trackChange = trackChange;
}

/*!
    Set the location of this expression to \a line of \a fileName. This information
    is used by the script engine.
*/
void QmlExpression::setSourceLocation(const QUrl &fileName, int line)
{
    Q_D(QmlExpression);
    d->fileName = fileName.toString();
    d->line = line;
}

/*!
    Returns the expression's scope object, if provided, otherwise 0.

    In addition to data provided by the expression's QmlContext, the scope
    object's properties are also in scope during the expression's evaluation.
*/
QObject *QmlExpression::scopeObject() const
{
    Q_D(const QmlExpression);
    return d->me;
}

/*! \internal */
void QmlExpression::__q_notify()
{
    valueChanged();
}

void QmlExpressionPrivate::clearGuards()
{
    Q_Q(QmlExpression);

    static int notifyIdx = -1;
    if (notifyIdx == -1) 
        notifyIdx = 
            QmlExpression::staticMetaObject.indexOfMethod("__q_notify()");

    for (int ii = 0; ii < guardListLength; ++ii) {
        if (guardList[ii].data()) {
            QMetaObject::disconnect(guardList[ii].data(), 
                                    guardList[ii].notifyIndex, 
                                    q, notifyIdx);
        }
    }

    delete [] guardList; guardList = 0; 
    guardListLength = 0;
}

void QmlExpressionPrivate::updateGuards(const QPODVector<QmlEnginePrivate::CapturedProperty> &properties)
{
    //clearGuards();
    Q_Q(QmlExpression);

    static int notifyIdx = -1;
    if (notifyIdx == -1) 
        notifyIdx = 
            QmlExpression::staticMetaObject.indexOfMethod("__q_notify()");

    SignalGuard *newGuardList = 0;
    
    if (properties.count() != guardListLength)
        newGuardList = new SignalGuard[properties.count()];

    bool outputWarningHeader = false;
    int hit = 0;
    for (int ii = 0; ii < properties.count(); ++ii) {
        const QmlEnginePrivate::CapturedProperty &property = properties.at(ii);

        bool needGuard = true;
        if (ii >= guardListLength) {
            // New guard
        } else if(guardList[ii].data() == property.object && 
                  guardList[ii].notifyIndex == property.notifyIndex) {
            // Cache hit
            if (!guardList[ii].isDuplicate || 
                (guardList[ii].isDuplicate && hit == ii)) {
                needGuard = false;
                ++hit;
            }
        } else if(guardList[ii].data() && !guardList[ii].isDuplicate) {
            // Cache miss
            QMetaObject::disconnect(guardList[ii].data(), 
                                    guardList[ii].notifyIndex, 
                                    q, notifyIdx);
        } 
        /* else {
            // Cache miss, but nothing to do
        } */

        if (needGuard) {
            if (!newGuardList) {
                newGuardList = new SignalGuard[properties.count()];
                for (int jj = 0; jj < ii; ++jj)
                    newGuardList[jj] = guardList[jj];
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
            newGuardList[ii] = guardList[ii];
        }
    }

    for (int ii = properties.count(); ii < guardListLength; ++ii) {
        if (guardList[ii].data() && !guardList[ii].isDuplicate) {
            QMetaObject::disconnect(guardList[ii].data(), 
                                    guardList[ii].notifyIndex, 
                                    q, notifyIdx);
        }
    }

    if (newGuardList) {
        if (guardList) delete [] guardList;
        guardList = newGuardList;
        guardListLength = properties.count();
    }
}

/*!
    \fn void QmlExpression::valueChanged()

    Emitted each time the expression value changes from the last time it was
    evaluated.  The expression must have been evaluated at least once (by
    calling QmlExpression::value()) before this signal will be emitted.
*/

QT_END_NAMESPACE

