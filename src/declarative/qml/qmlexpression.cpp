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
#include "qmlcompiler_p.h"

Q_DECLARE_METATYPE(QList<QObject *>);

QT_BEGIN_NAMESPACE

QmlExpressionData::QmlExpressionData()
: expressionFunctionValid(false), expressionRewritten(false), me(0), 
  trackChange(true), line(-1), guardList(0), guardListLength(0)
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
                                QObject *me, const QUrl &url, int lineNumber)
{
    data->fileName = url.toString();
    data->line = lineNumber;

    quint32 *exprData = (quint32 *)expr;
    Q_ASSERT(*exprData == BasicScriptEngineData || 
             *exprData == PreTransformedQtScriptData);
    if (*exprData == BasicScriptEngineData) {
        data->sse.load((const char *)(exprData + 1), rc);
    } else {
        QmlCompiledData *dd = (QmlCompiledData *)rc;

        data->expressionRewritten = true;
        data->expression = QString::fromRawData((QChar *)(exprData + 3), exprData[2]);

        int progIdx = *(exprData + 1);
        QmlEngine *engine = ctxt->engine();
        QmlEnginePrivate *ep = QmlEnginePrivate::get(engine);
        QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
#if !defined(Q_OS_SYMBIAN) && !defined(Q_OS_WIN32) //XXX Why doesn't this work?
        if (!dd->programs.at(progIdx)) {
            dd->programs[progIdx] =
                new QScriptProgram(scriptEngine->compile(data->expression, data->fileName, data->line));
        }
#endif

        QScriptContext *scriptContext = scriptEngine->pushCleanContext();
        scriptContext->pushScope(ep->contextClass->newContext(ctxt, me));

#if !defined(Q_OS_SYMBIAN) && !defined(Q_OS_WIN32)
        data->expressionFunction = scriptEngine->evaluate(*dd->programs[progIdx]);
#else
        data->expressionFunction = scriptEngine->evaluate(data->expression);
#endif

        data->expressionFunctionValid = true;
        scriptEngine->popContext();
    }

    data->QmlAbstractExpression::setContext(ctxt);
    data->me = me;
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
                             const QUrl &url, int lineNumber,
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
    if (d->data->sse.isValid())
        return QString::fromUtf8(d->data->sse.expression());
    else
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

    d->data->sse.clear();
}

QVariant QmlExpressionPrivate::evalSSE()
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::BindValueSSE> perfsse;
#endif

    QVariant rv = data->sse.run(data->context(), data->me);

    return rv;
}

void QmlExpressionPrivate::printException(QScriptEngine *scriptEngine)
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

        qWarning().nospace() << qPrintable(fileName) << ":" << lineNumber << ": "
                             << qPrintable(exception.toString());
    }
}

QVariant QmlExpressionPrivate::evalQtScript(QObject *secondaryScope, bool *isUndefined)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::BindValueQt> perfqt;
#endif

    QmlContextPrivate *ctxtPriv = data->context()->d_func();
    QmlEngine *engine = data->context()->engine();
    QmlEnginePrivate *ep = QmlEnginePrivate::get(engine);

    if (secondaryScope)
       ctxtPriv->defaultObjects.insert(ctxtPriv->highPriorityCount, 
                                       secondaryScope);

    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    if (!data->expressionFunctionValid) {

        QScriptContext *scriptContext = scriptEngine->pushCleanContext();
        scriptContext->pushScope(ep->contextClass->newContext(data->context(), data->me));

        if (data->expressionRewritten) {
            data->expressionFunction = scriptEngine->evaluate(data->expression, 
                                                              data->fileName, data->line);
        } else {
            QmlRewrite::RewriteBinding rewriteBinding;

            const QString code = rewriteBinding(data->expression);
            data->expressionFunction = scriptEngine->evaluate(code, data->fileName, data->line);
        }

        scriptEngine->popContext();
        data->expressionFunctionValid = true;
    }

    QScriptValue svalue = data->expressionFunction.call();

    if (isUndefined)
        *isUndefined = svalue.isUndefined() || scriptEngine->hasUncaughtException();

    if (scriptEngine->hasUncaughtException()) {
       printException(scriptEngine);
       return QVariant();
    }

    if (secondaryScope)
        ctxtPriv->defaultObjects.removeAt(ctxtPriv->highPriorityCount);

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
    if (!q->engine() || (!data->sse.isValid() && data->expression.isEmpty()))
        return rv;

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::BindValue> perf;
#endif

    QmlEnginePrivate *ep = QmlEnginePrivate::get(q->engine());

    QmlExpression *lastCurrentExpression = ep->currentExpression;
    QPODVector<QmlEnginePrivate::CapturedProperty> lastCapturedProperties;
    ep->capturedProperties.copyAndClear(lastCapturedProperties);

    ep->currentExpression = q;

    // This object might be deleted during the eval
    QmlExpressionData *localData = data;
    localData->addref();

    if (data->sse.isValid()) {
        rv = evalSSE();
    } else {
        rv = evalQtScript(secondaryScope, isUndefined);
    }

    ep->currentExpression = lastCurrentExpression;

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
QUrl QmlExpression::sourceFile() const
{
    Q_D(const QmlExpression);
    return QUrl(d->data->fileName);
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
    Set the location of this expression to \a line of \a fileName. This information
    is used by the script engine.
*/
void QmlExpression::setSourceLocation(const QUrl &fileName, int line)
{
    Q_D(QmlExpression);
    d->data->fileName = fileName.toString();
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

    for (int ii = 0; ii < data->guardListLength; ++ii) {
        if (data->guardList[ii].data()) {
            QMetaObject::disconnect(data->guardList[ii].data(), 
                                    data->guardList[ii].notifyIndex, 
                                    q, notifyIdx);
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
            QMetaObject::disconnect(data->guardList[ii].data(), 
                                    data->guardList[ii].notifyIndex, 
                                    q, notifyIdx);
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
            QMetaObject::disconnect(data->guardList[ii].data(), 
                                    data->guardList[ii].notifyIndex, 
                                    q, notifyIdx);
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

bool QmlAbstractExpression::isValid() const
{
    return m_context != 0;
}

QT_END_NAMESPACE

