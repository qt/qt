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
#include "QtCore/qdebug.h"

Q_DECLARE_METATYPE(QList<QObject *>);

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(qmlDebugger, QML_DEBUGGER)

QmlExpressionPrivate::QmlExpressionPrivate(QmlExpression *b)
: q(b), ctxt(0), sseData(0), proxy(0), me(0), trackChange(false), line(-1), id(0), log(0)
{
}

QmlExpressionPrivate::QmlExpressionPrivate(QmlExpression *b, void *expr, QmlRefCount *rc)
: q(b), ctxt(0), sse((const char *)expr, rc), sseData(0), proxy(0), me(0), trackChange(true), line(-1), id(0), log(0)
{
}

QmlExpressionPrivate::QmlExpressionPrivate(QmlExpression *b, const QString &expr)
: q(b), ctxt(0), expression(expr), sseData(0), proxy(0), me(0), trackChange(true), line(-1), id(0), log(0)
{
}

QmlExpressionPrivate::~QmlExpressionPrivate()
{
    sse.deleteScriptState(sseData);
    sseData = 0;
    delete proxy;
    delete log;
}

/*!
    Create an invalid QmlExpression.

    As the expression will not have an associated QmlContext, this will be a 
    null expression object and its value will always be an invalid QVariant.
 */
QmlExpression::QmlExpression()
: d(new QmlExpressionPrivate(this))
{
}

/*! \internal */
QmlExpression::QmlExpression(QmlContext *ctxt, void *expr, 
                             QmlRefCount *rc, QObject *me)
: d(new QmlExpressionPrivate(this, expr, rc))
{
    d->ctxt = ctxt;
    if(ctxt && ctxt->engine())
        d->id = ctxt->engine()->d_func()->getUniqueId();
    if(ctxt)
        ctxt->d_func()->childExpressions.insert(this);
    d->me = me;
}

/*!
    Create a QmlExpression object.

    The \a expression ECMAScript will be executed in the \a ctxt QmlContext.
    If specified, the \a scope object's properties will also be in scope during
    the expression's execution.
*/
QmlExpression::QmlExpression(QmlContext *ctxt, const QString &expression, 
                             QObject *scope)
: d(new QmlExpressionPrivate(this, expression))
{
    d->ctxt = ctxt;
    if(ctxt && ctxt->engine())
        d->id = ctxt->engine()->d_func()->getUniqueId();
    if(ctxt)
        ctxt->d_func()->childExpressions.insert(this);
    d->me = scope;
}

/*!
    Destroy the QmlExpression instance.
*/
QmlExpression::~QmlExpression()
{
    if (d->ctxt)
        d->ctxt->d_func()->childExpressions.remove(this);
    delete d; d = 0;
}

/*!
    Returns the QmlEngine this expression is associated with, or 0 if there
    is no association or the QmlEngine has been destroyed.
*/
QmlEngine *QmlExpression::engine() const
{
    return d->ctxt?d->ctxt->engine():0;
}

/*!
    Returns the QmlContext this expression is associated with, or 0 if there
    is no association or the QmlContext has been destroyed.
*/
QmlContext *QmlExpression::context() const
{
    return d->ctxt;
}

/*!
    Returns the expression string.
*/
QString QmlExpression::expression() const
{
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
    if (d->sseData) {
        d->sse.deleteScriptState(d->sseData);
        d->sseData = 0;
    }

    delete d->proxy; d->proxy = 0;

    d->expression = expression;

    d->sse.clear();
}

/*!
    Called by QmlExpression each time the expression value changes from the
    last time it was evaluated.  The expression must have been evaluated at 
    least once (by calling QmlExpression::value()) before this callback will
    be made.

    The default implementation does nothing.
*/
void QmlExpression::valueChanged()
{
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

    QScriptEngine *scriptEngine = engine->scriptEngine();
    QScriptValueList oldScopeChain = 
        scriptEngine->currentContext()->scopeChain();

    for (int i = 0; i < oldScopeChain.size(); ++i)
        scriptEngine->currentContext()->popScope();
    for (int i = ctxtPriv->scopeChain.size() - 1; i > -1; --i) 
        scriptEngine->currentContext()->pushScope(ctxtPriv->scopeChain.at(i));
    
    QScriptValue svalue = 
        scriptEngine->evaluate(expression, fileName.toString(), line);

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
    QVariant rv;
    if (!d->ctxt || !engine() || (!d->sse.isValid() && d->expression.isEmpty()))
        return rv;

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::BindValue> perf;
#endif

    QmlBasicScript::CacheState cacheState = QmlBasicScript::Reset;

    QmlEnginePrivate *ep = engine()->d_func();
    QmlExpression *lastCurrentExpression = ep->currentExpression;
    ep->currentExpression = this;

    if (d->sse.isValid()) {
        rv = d->evalSSE(cacheState);
    } else {
        rv = d->evalQtScript();
    }

    ep->currentExpression = lastCurrentExpression;

    if (cacheState != QmlBasicScript::NoChange) {
        if (cacheState != QmlBasicScript::Incremental && d->proxy) {
            delete d->proxy;
            d->proxy = 0;
        }

        if (trackChange() && ep->capturedProperties.count()) {
            if (!d->proxy)
                d->proxy = new QmlExpressionBindProxy(this);

            static int changedIndex = -1;
            if (changedIndex == -1)
                changedIndex = QmlExpressionBindProxy::staticMetaObject.indexOfSlot("changed()");

            if(qmlDebugger()) {
                QmlExpressionLog log;
                log.setTime(engine()->d_func()->getUniqueId());
                log.setExpression(expression());
                log.setResult(rv);

                for (int ii = 0; ii < ep->capturedProperties.count(); ++ii) {
                    const QmlEnginePrivate::CapturedProperty &prop =
                        ep->capturedProperties.at(ii);

                    if (prop.notifyIndex != -1) {
                        QMetaObject::connect(prop.object, prop.notifyIndex,
                                             d->proxy, changedIndex);
                    } else {
                        const QMetaObject *metaObj = prop.object->metaObject();
                        QMetaProperty metaProp = 
                            metaObj->property(prop.coreIndex);

                        QString warn = QLatin1String("Expression depends on non-NOTIFYable property: ") + 
                                       QLatin1String(metaObj->className()) + 
                                       QLatin1String("::") + 
                                       QLatin1String(metaProp.name());
                        log.addWarning(warn);
                    }
                }
                d->addLog(log);

            } else {
                bool outputWarningHeader = false;
                for (int ii = 0; ii < ep->capturedProperties.count(); ++ii) {
                    const QmlEnginePrivate::CapturedProperty &prop =
                        ep->capturedProperties.at(ii);

                    if (prop.notifyIndex != -1) {
                        QMetaObject::connect(prop.object, prop.notifyIndex,
                                             d->proxy, changedIndex);
                    } else {
                        if (!outputWarningHeader) {
                            outputWarningHeader = true;
                            qWarning() << "QmlExpression: Expression" << expression() << "depends on non-NOTIFYable properties:";
                        }

                        const QMetaObject *metaObj = prop.object->metaObject();
                        QMetaProperty metaProp = 
                            metaObj->property(prop.coreIndex);

                        qWarning().nospace() << "    " << metaObj->className() 
                                             << "::" << metaProp.name();
                    }
                }
            }
        } else {
            QmlExpressionLog log;
            log.setTime(engine()->d_func()->getUniqueId());
            log.setExpression(expression());
            log.setResult(rv);
            d->addLog(log);
        }

    } else {
        if(qmlDebugger()) {
            QmlExpressionLog log;
            log.setTime(engine()->d_func()->getUniqueId());
            log.setExpression(expression());
            log.setResult(rv);
            d->addLog(log);
        }
    }

    ep->capturedProperties.clear();

    return rv;
}

/*!
    Returns true if the expression results in a constant value.
    QmlExpression::value() must have been invoked at least once before the
    return from this method is valid.
 */
bool QmlExpression::isConstant() const
{
    return d->proxy == 0;
}

/*!
    Returns true if the changes are tracked in the expression's value.
*/
bool QmlExpression::trackChange() const
{
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
    d->trackChange = trackChange;
}

/*!
    Set the location of this expression to \a line of \a fileName. This information
    is used by the script engine.
*/
void QmlExpression::setSourceLocation(const QUrl &fileName, int line)
{
    d->fileName = fileName;
    d->line = line;
}

/*!
    Returns the expression's scope object, if provided, otherwise 0.

    In addition to data provided by the expression's QmlContext, the scope 
    object's properties are also in scope during the expression's evaluation.
*/
QObject *QmlExpression::scopeObject() const
{
    return d->me;
}

/*!
    \internal
*/
quint32 QmlExpression::id() const
{
    return d->id;
}

/*!
    \class QmlExpression
    \brief The QmlExpression class evaluates ECMAScript in a QML context.
*/

/*!
    \class QmlExpressionObject
    \brief The QmlExpressionObject class extends QmlExpression with signals and slots.

    To remain as lightweight as possible, QmlExpression does not inherit QObject
    and consequently cannot use signals or slots.  For the cases where this is
    more convenient in an application, QmlExpressionObject can be used instead.

    QmlExpressionObject behaves identically to QmlExpression, except that the
    QmlExpressionObject::value() method is a slot, and the 
    QmlExpressionObject::valueChanged() callback is a signal.
*/
/*!
    Create a QmlExpression with the specified \a parent.

    As the expression will not have an associated QmlContext, this will be a 
    null expression object and its value will always be an invalid QVariant.
*/
QmlExpressionObject::QmlExpressionObject(QObject *parent)
: QObject(parent)
{
}

/*!
    Create a QmlExpressionObject with the specified \a parent.

    The \a expression ECMAScript will be executed in the \a ctxt QmlContext.
    If specified, the \a scope object's properties will also be in scope during
    the expression's execution.
*/
QmlExpressionObject::QmlExpressionObject(QmlContext *ctxt, const QString &expression, QObject *scope, QObject *parent)
: QObject(parent), QmlExpression(ctxt, expression, scope)
{
}

/*!  \internal */
QmlExpressionObject::QmlExpressionObject(QmlContext *ctxt, void *d, QmlRefCount *rc, QObject *me)
: QmlExpression(ctxt, d, rc, me)
{
}

/*!
    Returns the value of the expression, or an invalid QVariant if the 
    expression is invalid or has an error.
*/
QVariant QmlExpressionObject::value()
{
    return QmlExpression::value();
}

/*!
    \fn void QmlExpressionObject::valueChanged()

    Emitted each time the expression value changes from the last time it was
    evaluated.  The expression must have been evaluated at least once (by 
    calling QmlExpressionObject::value()) before this signal will be emitted.  
*/

void QmlExpressionPrivate::addLog(const QmlExpressionLog &l)
{
    if (!log)
        log = new QList<QmlExpressionLog>();
    log->append(l);
}

QmlExpressionLog::QmlExpressionLog()
{
}

QmlExpressionLog::QmlExpressionLog(const QmlExpressionLog &o)
: m_time(o.m_time),
  m_expression(o.m_expression),
  m_result(o.m_result),
  m_warnings(o.m_warnings)
{
}

QmlExpressionLog::~QmlExpressionLog()
{
}

QmlExpressionLog &QmlExpressionLog::operator=(const QmlExpressionLog &o)
{
    m_time = o.m_time;
    m_expression = o.m_expression;
    m_result = o.m_result;
    m_warnings = o.m_warnings;
    return *this;
}

void QmlExpressionLog::setTime(quint32 time)
{
    m_time = time;
}

quint32 QmlExpressionLog::time() const
{
    return m_time;
}

QString QmlExpressionLog::expression() const
{
    return m_expression;
}

void QmlExpressionLog::setExpression(const QString &e)
{
    m_expression = e;
}

QStringList QmlExpressionLog::warnings() const
{
    return m_warnings;
}

void QmlExpressionLog::addWarning(const QString &w)
{
    m_warnings << w;
}

QVariant QmlExpressionLog::result() const
{
    return m_result;
}

void QmlExpressionLog::setResult(const QVariant &r)
{
    m_result = r;
}


QT_END_NAMESPACE

