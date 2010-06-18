/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "private/qdeclarativecompositetypemanager_p.h"

#include "private/qdeclarativecompositetypedata_p.h"
#include "private/qdeclarativescriptparser_p.h"
#include "qdeclarativeengine.h"
#include "private/qdeclarativeengine_p.h"
#include "qdeclarativecomponent.h"
#include "private/qdeclarativecomponent_p.h"
#include "private/qdeclarativecompiler_p.h"

#include <QtNetwork/qnetworkreply.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>

QT_BEGIN_NAMESPACE

QDeclarativeCompositeTypeData::QDeclarativeCompositeTypeData()
: status(Invalid), errorType(NoError), component(0), compiledComponent(0)
{
}

QDeclarativeCompositeTypeData::~QDeclarativeCompositeTypeData()
{
    for (int ii = 0; ii < dependants.count(); ++ii)
        dependants.at(ii)->release();

    for (int ii = 0; ii < resources.count(); ++ii)
        resources.at(ii)->release();

    if (compiledComponent)
        compiledComponent->release();

    if (component) 
        delete component;
}

QDeclarativeCompositeTypeResource::QDeclarativeCompositeTypeResource()
{
}

QDeclarativeCompositeTypeResource::~QDeclarativeCompositeTypeResource()
{
    for (int ii = 0; ii < dependants.count(); ++ii)
        dependants.at(ii)->release();
}

void QDeclarativeCompositeTypeData::addWaiter(QDeclarativeComponentPrivate *p)
{
    waiters << p;
}

void QDeclarativeCompositeTypeData::remWaiter(QDeclarativeComponentPrivate *p)
{
    waiters.removeAll(p);
}

QDeclarativeComponent *QDeclarativeCompositeTypeData::toComponent(QDeclarativeEngine *engine)
{
    if (!component) {

        QDeclarativeCompiledData *cc = toCompiledComponent(engine);
        if (cc) {
            component = new QDeclarativeComponent(engine, cc, -1, -1, 0);
            cc->release();
        } else {
            component = new QDeclarativeComponent(engine, 0);
            component->d_func()->url = imports.baseUrl();
            component->d_func()->state.errors = errors;
        }

    }

    return component;
}

QDeclarativeCompiledData *
QDeclarativeCompositeTypeData::toCompiledComponent(QDeclarativeEngine *engine)
{
    if (status == Complete && !compiledComponent) {

        // Build script imports
        foreach (const QDeclarativeScriptParser::Import &import, data.imports()) {
            if (import.type == QDeclarativeScriptParser::Import::Script) {
                QString url = imports.baseUrl().resolved(QUrl(import.uri)).toString();

                ScriptReference ref;
                ref.qualifier = import.qualifier;

                for (int ii = 0; ii < resources.count(); ++ii) {
                    if (resources.at(ii)->url == url) {
                        ref.resource = resources.at(ii);
                        break;
                    }
                }

                Q_ASSERT(ref.resource);

                scripts << ref;
            }
        }

        compiledComponent = new QDeclarativeCompiledData(engine);
        compiledComponent->url = imports.baseUrl();
        compiledComponent->name = compiledComponent->url.toString();

        QDeclarativeCompiler compiler;
        if (!compiler.compile(engine, this, compiledComponent)) {
            status = Error;
            errors = compiler.errors();
            compiledComponent->release();
            compiledComponent = 0;
        }

        // Data is no longer needed once we have a compiled component
        data.clear();
    }

    if (compiledComponent)
        compiledComponent->addref();

    return compiledComponent;
}

QDeclarativeCompositeTypeData::TypeReference::TypeReference()
: type(0), unit(0)
{
}

QDeclarativeCompositeTypeData::ScriptReference::ScriptReference()
: resource(0)
{
}

QDeclarativeCompositeTypeManager::QDeclarativeCompositeTypeManager(QDeclarativeEngine *e)
: engine(e), redirectCount(0)
{
}

QDeclarativeCompositeTypeManager::~QDeclarativeCompositeTypeManager()
{
    for (Components::Iterator iter = components.begin(); iter != components.end();) {
        (*iter)->release();
        iter = components.erase(iter);
    }
    for (Resources::Iterator iter = resources.begin(); iter != resources.end();) {
        (*iter)->release();
        iter = resources.erase(iter);
    }
}

QDeclarativeCompositeTypeData *QDeclarativeCompositeTypeManager::get(const QUrl &url)
{
    Redirects::Iterator redir = redirects.find(url);
    if (redir != redirects.end())
        return get(*redir);

    QDeclarativeCompositeTypeData *unit = components.value(url);

    if (!unit) {
        unit = new QDeclarativeCompositeTypeData;
        unit->status = QDeclarativeCompositeTypeData::Waiting;
        unit->progress = 0.0;
        unit->imports.setBaseUrl(url);
        components.insert(url, unit);

        loadSource(unit);
    }

    unit->addref();
    return unit;
}

QDeclarativeCompositeTypeData *
QDeclarativeCompositeTypeManager::getImmediate(const QByteArray &data, const QUrl &url)
{
    QDeclarativeCompositeTypeData *unit = new QDeclarativeCompositeTypeData;
    unit->status = QDeclarativeCompositeTypeData::Waiting;
    unit->imports.setBaseUrl(url);
    setData(unit, data, url);
    return unit;
}

void QDeclarativeCompositeTypeManager::clearCache()
{
    for (Components::Iterator iter = components.begin(); iter != components.end();) {
        if ((*iter)->status != QDeclarativeCompositeTypeData::Waiting) {
            (*iter)->release();
            iter = components.erase(iter);
        } else {
            ++iter;
        }
    }

    for (Resources::Iterator iter = resources.begin(); iter != resources.end();) {
        if ((*iter)->status != QDeclarativeCompositeTypeResource::Waiting) {
            (*iter)->release();
            iter = resources.erase(iter);
        } else {
            ++iter;
        }
    }
}

#define TYPEMANAGER_MAXIMUM_REDIRECT_RECURSION 16

void QDeclarativeCompositeTypeManager::replyFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());

    QDeclarativeCompositeTypeData *unit = components.value(reply->url());
    Q_ASSERT(unit);

    redirectCount++;
    if (redirectCount < TYPEMANAGER_MAXIMUM_REDIRECT_RECURSION) {
        QVariant redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (redirect.isValid()) {
            QUrl url = reply->url().resolved(redirect.toUrl());
            redirects.insert(reply->url(),url);
            unit->imports.setBaseUrl(url);
            components.remove(reply->url());
            components.insert(url, unit);
            reply->deleteLater();
            reply = engine->networkAccessManager()->get(QNetworkRequest(url));
            QObject::connect(reply, SIGNAL(finished()),
                             this, SLOT(replyFinished()));
            QObject::connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
                             this, SLOT(requestProgress(qint64,qint64)));
            return;
        }
    }
    redirectCount = 0;

    if (reply->error() != QNetworkReply::NoError) {
        QString errorDescription;
        // ### - Fill in error
        errorDescription = QLatin1String("Network error for URL ") + 
                      reply->url().toString();

        unit->status = QDeclarativeCompositeTypeData::Error;
        // ### FIXME
        QDeclarativeError error;
        error.setDescription(errorDescription);
        unit->errorType = QDeclarativeCompositeTypeData::AccessError;
        unit->errors << error;
        doComplete(unit);

    } else {
        QByteArray data = reply->readAll();

        setData(unit, data, reply->url());
    }

    reply->deleteLater();
}

void QDeclarativeCompositeTypeManager::resourceReplyFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());

    QDeclarativeCompositeTypeResource *resource = resources.value(reply->url());
    Q_ASSERT(resource);

    redirectCount++;
    if (redirectCount < TYPEMANAGER_MAXIMUM_REDIRECT_RECURSION) {
        QVariant redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (redirect.isValid()) {
            QUrl url = reply->url().resolved(redirect.toUrl());
            redirects.insert(reply->url(),url);
            resource->url = url.toString();
            resources.remove(reply->url());
            resources.insert(url, resource);
            reply->deleteLater();
            reply = engine->networkAccessManager()->get(QNetworkRequest(url));
            QObject::connect(reply, SIGNAL(finished()),
                             this, SLOT(resourceReplyFinished()));
            return;
        }
    }
    redirectCount = 0;

    if (reply->error() != QNetworkReply::NoError) {

        resource->status = QDeclarativeCompositeTypeResource::Error;

    } else {

        resource->status = QDeclarativeCompositeTypeResource::Complete;
        resource->data = reply->readAll();

    }

    doComplete(resource);
    reply->deleteLater();
}

void QDeclarativeCompositeTypeManager::loadResource(QDeclarativeCompositeTypeResource *resource)
{
    QUrl url(resource->url);

    QString lf = QDeclarativeEnginePrivate::urlToLocalFileOrQrc(url);
    if (!lf.isEmpty()) {

        QFile file(lf);
        if (file.open(QFile::ReadOnly)) {
            resource->data = file.readAll();
            resource->status = QDeclarativeCompositeTypeResource::Complete;
        } else {
            resource->status = QDeclarativeCompositeTypeResource::Error;
        }
    } else if (url.scheme().isEmpty()) {

        // We can't open this, so just declare as an error
        resource->status = QDeclarativeCompositeTypeResource::Error;
    } else {

        QNetworkReply *reply = 
            engine->networkAccessManager()->get(QNetworkRequest(url));
        QObject::connect(reply, SIGNAL(finished()),
                         this, SLOT(resourceReplyFinished()));

    }
}

void QDeclarativeCompositeTypeManager::loadSource(QDeclarativeCompositeTypeData *unit)
{
    QUrl url(unit->imports.baseUrl());

    QString lf = QDeclarativeEnginePrivate::urlToLocalFileOrQrc(url);
    if (!lf.isEmpty()) {

        QFile file(lf);
        if (file.open(QFile::ReadOnly)) {
            QByteArray data = file.readAll();
            setData(unit, data, url);
            return; // success
        }
    } else if (!url.scheme().isEmpty()) {
        QNetworkReply *reply = 
            engine->networkAccessManager()->get(QNetworkRequest(url));
        QObject::connect(reply, SIGNAL(finished()),
                         this, SLOT(replyFinished()));
        QObject::connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
                         this, SLOT(requestProgress(qint64,qint64)));
        return; // waiting
    }

    // error happened
    QString errorDescription;
    // ### - Fill in error
    errorDescription = QLatin1String("File error for URL ") + url.toString();
    unit->status = QDeclarativeCompositeTypeData::Error;
    // ### FIXME
    QDeclarativeError error;
    error.setDescription(errorDescription);
    unit->errorType = QDeclarativeCompositeTypeData::AccessError;
    unit->errors << error;
    doComplete(unit);
}

void QDeclarativeCompositeTypeManager::requestProgress(qint64 received, qint64 total)
{
    if (total <= 0)
        return;
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());

    QDeclarativeCompositeTypeData *unit = components.value(reply->url());
    Q_ASSERT(unit);

    unit->progress = qreal(received)/total;

    foreach (QDeclarativeComponentPrivate *comp, unit->waiters)
        comp->updateProgress(unit->progress);
}

void QDeclarativeCompositeTypeManager::setData(QDeclarativeCompositeTypeData *unit, 
                                     const QByteArray &data,
                                     const QUrl &url)
{
    bool ok = true;
    if (!unit->data.parse(data, url)) {
        ok = false;
        unit->errors << unit->data.errors();
    }

    if (ok) {
        compile(unit);
    } else {
        unit->status = QDeclarativeCompositeTypeData::Error;
        unit->errorType = QDeclarativeCompositeTypeData::GeneralError;
        doComplete(unit);
    }
}

void QDeclarativeCompositeTypeManager::doComplete(QDeclarativeCompositeTypeData *unit)
{
    for (int ii = 0; ii < unit->dependants.count(); ++ii) {
        checkComplete(unit->dependants.at(ii));
        unit->dependants.at(ii)->release();
    }
    unit->dependants.clear();

    while(!unit->waiters.isEmpty()) {
        QDeclarativeComponentPrivate *p = unit->waiters.takeFirst();
        p->typeDataReady();
    }
}

void QDeclarativeCompositeTypeManager::doComplete(QDeclarativeCompositeTypeResource *resource)
{
    for (int ii = 0; ii < resource->dependants.count(); ++ii) {
        checkComplete(resource->dependants.at(ii));
        resource->dependants.at(ii)->release();
    }
    resource->dependants.clear();
}

void QDeclarativeCompositeTypeManager::checkComplete(QDeclarativeCompositeTypeData *unit)
{
    if (unit->status != QDeclarativeCompositeTypeData::Waiting
     && unit->status != QDeclarativeCompositeTypeData::WaitingResources)
        return;

    int waiting = 0;
    for (int ii = 0; ii < unit->resources.count(); ++ii) {
        QDeclarativeCompositeTypeResource *r = unit->resources.at(ii);

        if (!r)
            continue;

        if (r->status == QDeclarativeCompositeTypeResource::Error) {
            unit->status = QDeclarativeCompositeTypeData::Error;
            QDeclarativeError error;
            error.setUrl(unit->imports.baseUrl());
            error.setDescription(tr("Resource %1 unavailable").arg(r->url));
            unit->errors << error;
            doComplete(unit);
            return;
        } else if (r->status == QDeclarativeCompositeTypeResource::Waiting) {
            waiting++;
        }
    }

    if (waiting == 0) {
        if (unit->status == QDeclarativeCompositeTypeData::WaitingResources) {
            waiting += resolveTypes(unit);
            if (unit->status != QDeclarativeCompositeTypeData::Error) {
                if (waiting)
                    unit->status = QDeclarativeCompositeTypeData::Waiting;
            } else {
                return;
            }
        } else {
            for (int ii = 0; ii < unit->types.count(); ++ii) {
                QDeclarativeCompositeTypeData *u = unit->types.at(ii).unit;

                if (!u)
                    continue;

                if (u->status == QDeclarativeCompositeTypeData::Error) {
                    unit->status = QDeclarativeCompositeTypeData::Error;
                    unit->errors = u->errors;
                    doComplete(unit);
                    return;
                } else if (u->status == QDeclarativeCompositeTypeData::Waiting
                        || u->status == QDeclarativeCompositeTypeData::WaitingResources)
                {
                    waiting++;
                }
            }
        }
    }

    if (!waiting) {
        unit->status = QDeclarativeCompositeTypeData::Complete;
        doComplete(unit);
    }
}

int QDeclarativeCompositeTypeManager::resolveTypes(QDeclarativeCompositeTypeData *unit)
{
    // not called until all resources are loaded (they include import URLs)
    int waiting = 0;

    QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(engine);
    QDeclarativeImportDatabase &importDatabase = ep->importDatabase;

    // For local urls, add an implicit import "." as first (most overridden) lookup. 
    // This will also trigger the loading of the qmldir and the import of any native 
    // types from available plugins.
    {
        QDeclarativeDirComponents qmldircomponentsnetwork;
        if (QDeclarativeCompositeTypeResource *resource
            = resources.value(unit->imports.baseUrl().resolved(QUrl(QLatin1String("./qmldir"))))) {
            QDeclarativeDirParser parser;
            parser.setSource(QString::fromUtf8(resource->data));
            parser.parse();
            qmldircomponentsnetwork = parser.components();
        }

        importDatabase.addToImport(&unit->imports, qmldircomponentsnetwork, QLatin1String("."),
                                   QString(), -1, -1, QDeclarativeScriptParser::Import::File,
                                   0); // error ignored (just means no fallback)
    }


    foreach (QDeclarativeScriptParser::Import imp, unit->data.imports()) {
        QDeclarativeDirComponents qmldircomponentsnetwork;
        if (imp.type == QDeclarativeScriptParser::Import::Script)
            continue;

        if (imp.type == QDeclarativeScriptParser::Import::File && imp.qualifier.isEmpty()) {
            QString importUrl = unit->imports.baseUrl().resolved(QUrl(imp.uri + QLatin1String("/qmldir"))).toString();
            for (int ii = 0; ii < unit->resources.count(); ++ii) {
                if (unit->resources.at(ii)->url == importUrl) {
                    QDeclarativeDirParser parser;
                    parser.setSource(QString::fromUtf8(unit->resources.at(ii)->data));
                    parser.parse();
                    qmldircomponentsnetwork = parser.components();
                    break;
                }
            }
        }


        int vmaj = -1;
        int vmin = -1;
        if (!imp.version.isEmpty()) {
            int dot = imp.version.indexOf(QLatin1Char('.'));
            if (dot < 0) {
                vmaj = imp.version.toInt();
                vmin = 0;
            } else {
                vmaj = imp.version.left(dot).toInt();
                vmin = imp.version.mid(dot+1).toInt();
            }
        }

        QString errorString;
        if (!importDatabase.addToImport(&unit->imports, qmldircomponentsnetwork, imp.uri, imp.qualifier, 
                                        vmaj, vmin, imp.type, &errorString)) {
            QDeclarativeError error;
            error.setUrl(unit->imports.baseUrl());
            error.setDescription(errorString);
            error.setLine(imp.location.start.line);
            error.setColumn(imp.location.start.column);
            unit->status = QDeclarativeCompositeTypeData::Error;
            unit->errorType = QDeclarativeCompositeTypeData::GeneralError;
            unit->errors << error;
            doComplete(unit);
            return 0;
        }
    }


    QList<QDeclarativeScriptParser::TypeReference*> types = unit->data.referencedTypes();

    for (int ii = 0; ii < types.count(); ++ii) {
        QDeclarativeScriptParser::TypeReference *parserRef = types.at(ii);
        QByteArray typeName = parserRef->name.toUtf8();

        QDeclarativeCompositeTypeData::TypeReference ref;

        QUrl url;
        int majorVersion;
        int minorVersion;
        QDeclarativeImportedNamespace *typeNamespace = 0;
        QString errorString;
        if (!importDatabase.resolveType(unit->imports, typeName, &ref.type, &url, &majorVersion, &minorVersion, 
                                        &typeNamespace, &errorString) || typeNamespace) {
            // Known to not be a type:
            //  - known to be a namespace (Namespace {})
            //  - type with unknown namespace (UnknownNamespace.SomeType {})
            QDeclarativeError error;
            error.setUrl(unit->imports.baseUrl());
            QString userTypeName = QString::fromUtf8(typeName);
            userTypeName.replace(QLatin1Char('/'),QLatin1Char('.'));
            if (typeNamespace)
                error.setDescription(tr("Namespace %1 cannot be used as a type").arg(userTypeName));
            else
                error.setDescription(tr("%1 %2").arg(userTypeName).arg(errorString));

            if (!parserRef->refObjects.isEmpty()) {
                QDeclarativeParser::Object *obj = parserRef->refObjects.first();
                error.setLine(obj->location.start.line);
                error.setColumn(obj->location.start.column);
            }
            unit->status = QDeclarativeCompositeTypeData::Error;
            unit->errorType = QDeclarativeCompositeTypeData::GeneralError;
            unit->errors << error;
            doComplete(unit);
            return 0;
        }

        if (ref.type) {
            foreach (QDeclarativeParser::Object *obj, parserRef->refObjects) {
               // store namespace for DOM
               obj->majorVersion = majorVersion;
               obj->minorVersion = minorVersion;
            }
            unit->types << ref;
            continue;
        }

        Redirects::Iterator redir = redirects.find(url);
        if (redir != redirects.end())
            url = *redir;

        QDeclarativeCompositeTypeData *urlUnit = components.value(url);

        if (!urlUnit) {
            urlUnit = new QDeclarativeCompositeTypeData;
            urlUnit->status = QDeclarativeCompositeTypeData::Waiting;
            urlUnit->imports.setBaseUrl(url);
            components.insert(url, urlUnit);

            loadSource(urlUnit);
        }

        ref.unit = urlUnit;
        switch(urlUnit->status) {
        case QDeclarativeCompositeTypeData::Invalid:
        case QDeclarativeCompositeTypeData::Error:
            unit->status = QDeclarativeCompositeTypeData::Error;
            {
                QDeclarativeError error;
                error.setUrl(unit->imports.baseUrl());
                error.setDescription(tr("Type %1 unavailable").arg(QString::fromUtf8(typeName)));
                if (!parserRef->refObjects.isEmpty()) {
                    QDeclarativeParser::Object *obj = parserRef->refObjects.first();
                    error.setLine(obj->location.start.line);
                    error.setColumn(obj->location.start.column);
                }
                unit->errors << error;
            }
            if (urlUnit->errorType != QDeclarativeCompositeTypeData::AccessError) 
                unit->errors << urlUnit->errors;
            doComplete(unit);
            return 0;

        case QDeclarativeCompositeTypeData::Complete:
            break;

        case QDeclarativeCompositeTypeData::Waiting:
        case QDeclarativeCompositeTypeData::WaitingResources:
            unit->addref();
            ref.unit->dependants << unit;
            waiting++;
            break;
        }

        unit->types << ref;
    }
    return waiting;
}

// ### Check ref counting in here
void QDeclarativeCompositeTypeManager::compile(QDeclarativeCompositeTypeData *unit)
{
    int waiting = 0;

    QList<QUrl> resourceList = unit->data.referencedResources();

    foreach (QDeclarativeScriptParser::Import imp, unit->data.imports()) {
        if (imp.type == QDeclarativeScriptParser::Import::File && imp.qualifier.isEmpty()) {
            QUrl importUrl = unit->imports.baseUrl().resolved(QUrl(imp.uri + QLatin1String("/qmldir")));
            if (QDeclarativeEnginePrivate::urlToLocalFileOrQrc(importUrl).isEmpty()) {
                // Import requires remote qmldir
                resourceList.prepend(importUrl);
            }
        }
    }

    QUrl importUrl;
    if (!unit->imports.baseUrl().scheme().isEmpty())
        importUrl = unit->imports.baseUrl().resolved(QUrl(QLatin1String("qmldir")));
    if (!importUrl.scheme().isEmpty() && QDeclarativeEnginePrivate::urlToLocalFileOrQrc(importUrl).isEmpty())
        resourceList.prepend(importUrl);

    for (int ii = 0; ii < resourceList.count(); ++ii) {
        QUrl url = unit->imports.baseUrl().resolved(resourceList.at(ii));

        QDeclarativeCompositeTypeResource *resource = resources.value(url);

        if (!resource) {
            resource = new QDeclarativeCompositeTypeResource;
            resource->status = QDeclarativeCompositeTypeResource::Waiting;
            resource->url = url.toString();
            resources.insert(url, resource);

            loadResource(resource);
        }

        switch(resource->status) {
        case QDeclarativeCompositeTypeResource::Invalid:
        case QDeclarativeCompositeTypeResource::Error:
            unit->status = QDeclarativeCompositeTypeData::Error;
            {
                QDeclarativeError error;
                error.setUrl(unit->imports.baseUrl());
                error.setDescription(tr("Resource %1 unavailable").arg(resource->url));
                unit->errors << error;
            }
            doComplete(unit);
            return;

        case QDeclarativeCompositeTypeData::Complete:
            break;

        case QDeclarativeCompositeTypeData::Waiting:
            unit->addref();
            resource->dependants << unit;
            waiting++;
            break;
        }

        resource->addref();
        unit->resources << resource;
    }

    if (waiting == 0) {
        waiting += resolveTypes(unit);
        if (unit->status != QDeclarativeCompositeTypeData::Error) {
            if (!waiting) {
                unit->status = QDeclarativeCompositeTypeData::Complete;
                doComplete(unit);
            } else {
                unit->status = QDeclarativeCompositeTypeData::Waiting;
            }
        }
    } else {
        unit->status = QDeclarativeCompositeTypeData::WaitingResources;
    }
}

QT_END_NAMESPACE
