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

#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcontext.h>
#include <private/qobject_p.h>
#include <QtCore/qfile.h>
#include <QtCore/qdebug.h>
#include <QtScript/qscriptvalue.h>
#include <QtScript/qscriptcontext.h>
#include <QtScript/qscriptengine.h>
#include <private/qmlnullablevalue_p.h>
#include <private/qmlengine_p.h>
#include <private/qmlcontext_p.h>
#include "qmlscript.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QtDeclarative/qmlinfo.h>
#include <private/qfxperf_p.h>

QT_BEGIN_NAMESPACE

class QmlScriptPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlScript);

public:
    QmlScriptPrivate() : reply(0) {}

    void addScriptToEngine(const QString &, const QString &source=QString());

    QString script;
    QNetworkReply *reply;
    QUrl url;
};

/*!
    \qmlclass Script QmlScript
    \brief The Script element adds JavaScript snippets.
    \ingroup group_utility

    QmlScript is used to add convenient JavaScript "glue" methods to
    your Qt Declarative application or component. While you can have any JavaScript code
    within a QmlScript, it is best to limit yourself to defining functions.

    \qml
    Script {
        function debugMyComponent() {
            print(text.text);
            print(otherinterestingitem.property);
        }
    }
    MouseRegion { onClicked: debugMyComponent() }
    \endqml

    \note QmlScript executes JavaScript as soon as it is specified.
    When defining a component, this may be before the execution context is
    fully specified.  As a result some properties or items may not be
    accessible. By limiting your JavaScript to defining functions that are
    only executed later once the context is fully defined, this problem is
    avoided.
*/

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Script,QmlScript)
QmlScript::QmlScript(QObject *parent) : QObject(*(new QmlScriptPrivate), parent)
{
}

/*!
    \qmlproperty string Script::script
    \default
    JavaScript code to execute.
*/
/*!
    \property QmlScript::script
    \brief a script snippet.
*/
QString QmlScript::script() const
{
    Q_D(const QmlScript);
    return d->script;
}

void QmlScript::setScript(const QString &script)
{
    Q_D(QmlScript);
    d->script = script;
    d->addScriptToEngine(d->script);
}

/*!
    \qmlproperty string Script::source

    Setting this property causes the Script element to read JavaScript code from
    the file specified.
 */
/*!
    \property QmlScript::source
    \brief the path to a script file.
*/
QUrl QmlScript::source() const
{
    Q_D(const QmlScript);
    return d->url;
}

void QmlScript::setSource(const QUrl &source)
{
    Q_D(QmlScript);
    if (d->url == source)
        return;
    d->url = qmlContext(this)->resolvedUrl(source);
    
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
    if (d->url.scheme() == QLatin1String("file")) {
        QFile file(d->url.toLocalFile());
        file.open(QIODevice::ReadOnly);
        QByteArray ba = file.readAll();
        d->addScriptToEngine(QString::fromUtf8(ba), file.fileName());
    } else
#endif
    {
        QNetworkRequest req(d->url);
        req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        d->reply = qmlEngine(this)->networkAccessManager()->get(req);
        QObject::connect(d->reply, SIGNAL(finished()),
                         this, SLOT(replyFinished()));
    }
}

void QmlScript::replyFinished()
{
    Q_D(QmlScript);
    if (!d->reply->error()) {
        QByteArray ba = d->reply->readAll();
        d->addScriptToEngine(QString::fromUtf8(ba), d->url.toString());
    }
    d->reply->deleteLater();
    d->reply = 0;
}

void QmlScriptPrivate::addScriptToEngine(const QString &script, const QString &source)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::AddScript> pt;
#endif
    Q_Q(QmlScript);
    QmlEngine *engine = qmlEngine(q);
    QmlContext *context = qmlContext(q);
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    QScriptContext *currentContext = scriptEngine->currentContext();
    QScriptValueList oldScopeChain = currentContext->scopeChain();
    QScriptValue oldact = currentContext->activationObject();

    for (int i = 0; i < oldScopeChain.size(); ++i) {
        currentContext->popScope();
    }
    for (int i = context->d_func()->scopeChain.size() - 1; i > -1; --i) {
        currentContext->pushScope(context->d_func()->scopeChain.at(i));
    }

    currentContext->setActivationObject(context->d_func()->scopeChain.at(0));

    QScriptValue val = scriptEngine->evaluate(script, source);
    if (scriptEngine->hasUncaughtException()) {
        if (scriptEngine->uncaughtException().isError()){
            QScriptValue exception = scriptEngine->uncaughtException();
            if (!exception.property(QLatin1String("fileName")).toString().isEmpty()){
                qWarning() << exception.property(QLatin1String("fileName")).toString() 
                           << scriptEngine->uncaughtExceptionLineNumber()
                           << exception.toString();

            } else {
                qmlInfo(q) << exception.toString();
            }
        }
    }

    currentContext->setActivationObject(oldact);

    for (int i = 0; i < context->d_func()->scopeChain.size(); ++i)
        currentContext->popScope();

    for (int i = oldScopeChain.size() - 1; i > -1; --i)
        currentContext->pushScope(oldScopeChain.at(i));
}

QT_END_NAMESPACE
