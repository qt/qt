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

#include "qmlcompositetypemanager_p.h"

#include "qmlcompositetypedata_p.h"
#include "qmlscriptparser_p.h"
#include "qmlengine.h"
#include "qmlengine_p.h"
#include "qmlcomponent.h"
#include "qmlcomponent_p.h"
#include "qmlcompiler_p.h"

#include <QtNetwork/qnetworkreply.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>

QT_BEGIN_NAMESPACE

#if (QT_VERSION < QT_VERSION_CHECK(4, 7, 0))
inline uint qHash(const QUrl &uri)
{
    return qHash(uri.toEncoded(QUrl::FormattingOption(0x100)));
}
#endif

QmlCompositeTypeData::QmlCompositeTypeData()
: status(Invalid), errorType(NoError), component(0), compiledComponent(0)
{
}

QmlCompositeTypeData::~QmlCompositeTypeData()
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

QmlCompositeTypeResource::QmlCompositeTypeResource()
{
}

QmlCompositeTypeResource::~QmlCompositeTypeResource()
{
    for (int ii = 0; ii < dependants.count(); ++ii)
        dependants.at(ii)->release();
}

void QmlCompositeTypeData::addWaiter(QmlComponentPrivate *p)
{
    waiters << p;
}

void QmlCompositeTypeData::remWaiter(QmlComponentPrivate *p)
{
    waiters.removeAll(p);
}

QmlComponent *QmlCompositeTypeData::toComponent(QmlEngine *engine)
{
    if (!component) {

        QmlCompiledData *cc = toCompiledComponent(engine);
        if (cc) {
            component = new QmlComponent(engine, cc, -1, -1, 0);
            cc->release();
        } else {
            component = new QmlComponent(engine, 0);
            component->d_func()->url = imports.baseUrl();
            component->d_func()->state.errors = errors;
        }

    }

    return component;
}

QmlCompiledData *
QmlCompositeTypeData::toCompiledComponent(QmlEngine *engine)
{
    if (status == Complete && !compiledComponent) {

        compiledComponent = new QmlCompiledData(engine);
        compiledComponent->url = imports.baseUrl();
        compiledComponent->name = compiledComponent->url.toString();

        QmlCompiler compiler;
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

QmlCompositeTypeData::TypeReference::TypeReference()
: type(0), unit(0)
{
}

QmlCompositeTypeManager::QmlCompositeTypeManager(QmlEngine *e)
: engine(e)
{
}

QmlCompositeTypeManager::~QmlCompositeTypeManager()
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

QmlCompositeTypeData *QmlCompositeTypeManager::get(const QUrl &url)
{
    QmlCompositeTypeData *unit = components.value(url);

    if (!unit) {
        unit = new QmlCompositeTypeData;
        unit->status = QmlCompositeTypeData::Waiting;
        unit->progress = 0.0;
        unit->imports.setBaseUrl(url);
        components.insert(url, unit);

        loadSource(unit);
    }

    unit->addref();
    return unit;
}

QmlCompositeTypeData *
QmlCompositeTypeManager::getImmediate(const QByteArray &data, const QUrl &url)
{
    QmlCompositeTypeData *unit = new QmlCompositeTypeData;
    unit->status = QmlCompositeTypeData::Waiting;
    unit->imports.setBaseUrl(url);
    setData(unit, data, url);
    return unit;
}

void QmlCompositeTypeManager::clearCache()
{
    for (Components::Iterator iter = components.begin(); iter != components.end();) {
        if ((*iter)->status != QmlCompositeTypeData::Waiting) {
            (*iter)->release();
            iter = components.erase(iter);
        } else {
            ++iter;
        }
    }

    for (Resources::Iterator iter = resources.begin(); iter != resources.end();) {
        if ((*iter)->status != QmlCompositeTypeResource::Waiting) {
            (*iter)->release();
            iter = resources.erase(iter);
        } else {
            ++iter;
        }
    }
}

void QmlCompositeTypeManager::replyFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());

    QmlCompositeTypeData *unit = components.value(reply->url());
    Q_ASSERT(unit);

    if (reply->error() != QNetworkReply::NoError) {
        QString errorDescription;
        // ### - Fill in error
        errorDescription = QLatin1String("Network error for URL ") + 
                      reply->url().toString();

        unit->status = QmlCompositeTypeData::Error;
        // ### FIXME
        QmlError error;
        error.setDescription(errorDescription);
        unit->errorType = QmlCompositeTypeData::AccessError;
        unit->errors << error;
        doComplete(unit);

    } else {
        QByteArray data = reply->readAll();

        setData(unit, data, reply->url());
    }

    reply->deleteLater();
}

void QmlCompositeTypeManager::resourceReplyFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());

    QmlCompositeTypeResource *resource = resources.value(reply->url());
    Q_ASSERT(resource);

    if (reply->error() != QNetworkReply::NoError) {

        resource->status = QmlCompositeTypeResource::Error;

    } else {

        resource->status = QmlCompositeTypeResource::Complete;
        resource->data = reply->readAll();

    }

    doComplete(resource);
    reply->deleteLater();
}

static QString toLocalFileOrQrc(const QUrl& url)
{
    QString r = url.toLocalFile();
    if (r.isEmpty() && url.scheme() == QLatin1String("qrc"))
        r = QLatin1Char(':') + url.path();
    return r;
}

void QmlCompositeTypeManager::loadResource(QmlCompositeTypeResource *resource)
{
    QUrl url(resource->url);

    QString lf = toLocalFileOrQrc(url);
    if (!lf.isEmpty()) {

        QFile file(lf);
        if (file.open(QFile::ReadOnly)) {
            resource->data = file.readAll();
            resource->status = QmlCompositeTypeResource::Complete;
        } else {
            resource->status = QmlCompositeTypeResource::Error;
        }

    } else {

        QNetworkReply *reply = 
            engine->networkAccessManager()->get(QNetworkRequest(url));
        QObject::connect(reply, SIGNAL(finished()),
                         this, SLOT(resourceReplyFinished()));

    }
}

void QmlCompositeTypeManager::loadSource(QmlCompositeTypeData *unit)
{
    QUrl url(unit->imports.baseUrl());

    QString lf = toLocalFileOrQrc(url);
    if (!lf.isEmpty()) {

        QFile file(lf);
        if (file.open(QFile::ReadOnly)) {
            QByteArray data = file.readAll();
            setData(unit, data, url);
        } else {
            QString errorDescription;
            // ### - Fill in error
            errorDescription = QLatin1String("File error for URL ") + url.toString();
            unit->status = QmlCompositeTypeData::Error;
            // ### FIXME
            QmlError error;
            error.setDescription(errorDescription);
            unit->errorType = QmlCompositeTypeData::AccessError;
            unit->errors << error;
            doComplete(unit);
        }

    } else {
        QNetworkReply *reply = 
            engine->networkAccessManager()->get(QNetworkRequest(url));
        QObject::connect(reply, SIGNAL(finished()),
                         this, SLOT(replyFinished()));
        QObject::connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
                         this, SLOT(requestProgress(qint64,qint64)));
    }
}

void QmlCompositeTypeManager::requestProgress(qint64 received, qint64 total)
{
    if (total <= 0)
        return;
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());

    QmlCompositeTypeData *unit = components.value(reply->url());
    Q_ASSERT(unit);

    unit->progress = qreal(received)/total;

    foreach (QmlComponentPrivate *comp, unit->waiters)
        comp->updateProgress(unit->progress);
}

void QmlCompositeTypeManager::setData(QmlCompositeTypeData *unit, 
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
        unit->status = QmlCompositeTypeData::Error;
        unit->errorType = QmlCompositeTypeData::GeneralError;
        doComplete(unit);
    }
}

void QmlCompositeTypeManager::doComplete(QmlCompositeTypeData *unit)
{
    for (int ii = 0; ii < unit->dependants.count(); ++ii) {
        checkComplete(unit->dependants.at(ii));
        unit->dependants.at(ii)->release();
    }
    unit->dependants.clear();

    while(!unit->waiters.isEmpty()) {
        QmlComponentPrivate *p = unit->waiters.takeFirst();
        p->typeDataReady();
    }
}

void QmlCompositeTypeManager::doComplete(QmlCompositeTypeResource *resource)
{
    for (int ii = 0; ii < resource->dependants.count(); ++ii) {
        checkComplete(resource->dependants.at(ii));
        resource->dependants.at(ii)->release();
    }
    resource->dependants.clear();
}

void QmlCompositeTypeManager::checkComplete(QmlCompositeTypeData *unit)
{
    if (unit->status != QmlCompositeTypeData::Waiting
     && unit->status != QmlCompositeTypeData::WaitingResources)
        return;

    int waiting = 0;
    for (int ii = 0; ii < unit->resources.count(); ++ii) {
        QmlCompositeTypeResource *r = unit->resources.at(ii);

        if (!r)
            continue;

        if (r->status == QmlCompositeTypeResource::Error) {
            unit->status = QmlCompositeTypeData::Error;
            QmlError error;
            error.setUrl(unit->imports.baseUrl());
            error.setDescription(tr("Resource %1 unavailable").arg(r->url));
            unit->errors << error;
            doComplete(unit);
            return;
        } else if (r->status == QmlCompositeTypeResource::Waiting) {
            waiting++;
        }
    }

    if (waiting == 0) {
        if (unit->status == QmlCompositeTypeData::WaitingResources) {
            waiting += resolveTypes(unit);
            if (unit->status != QmlCompositeTypeData::Error) {
                if (waiting)
                    unit->status = QmlCompositeTypeData::Waiting;
            } else {
                return;
            }
        } else {
            for (int ii = 0; ii < unit->types.count(); ++ii) {
                QmlCompositeTypeData *u = unit->types.at(ii).unit;

                if (!u)
                    continue;

                if (u->status == QmlCompositeTypeData::Error) {
                    unit->status = QmlCompositeTypeData::Error;
                    unit->errors = u->errors;
                    doComplete(unit);
                    return;
                } else if (u->status == QmlCompositeTypeData::Waiting) {
                    waiting++;
                }
            }
        }
    }

    if (!waiting) {
        unit->status = QmlCompositeTypeData::Complete;
        doComplete(unit);
    }
}

int QmlCompositeTypeManager::resolveTypes(QmlCompositeTypeData *unit)
{
    // not called until all resources are loaded (they include import URLs)

    int waiting = 0;

    foreach (QmlScriptParser::Import imp, unit->data.imports()) {
        QString qmldir;
        if (imp.type == QmlScriptParser::Import::File && imp.qualifier.isEmpty()) {
            QString importUrl = unit->imports.baseUrl().resolved(QUrl(imp.uri + QLatin1String("/qmldir"))).toString();
            for (int ii = 0; ii < unit->resources.count(); ++ii) {
                if (unit->resources.at(ii)->url == importUrl) {
                    qmldir = QString::fromUtf8(unit->resources.at(ii)->data);
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

        if (!QmlEnginePrivate::get(engine)->
                addToImport(&unit->imports, qmldir, imp.uri, imp.qualifier, vmaj, vmin, imp.type))
        {
            QmlError error;
            error.setUrl(unit->imports.baseUrl());
            error.setDescription(tr("Import %1 unavailable").arg(imp.uri));
            unit->status = QmlCompositeTypeData::Error;
            unit->errorType = QmlCompositeTypeData::GeneralError;
            unit->errors << error;
            doComplete(unit);
            return 0;
        }
    }

    QList<QmlScriptParser::TypeReference*> types = unit->data.referencedTypes();

    for (int ii = 0; ii < types.count(); ++ii) {
        QmlScriptParser::TypeReference *parserRef = types.at(ii);
        QByteArray typeName = parserRef->name.toUtf8();

        QmlCompositeTypeData::TypeReference ref;

        QUrl url;
        int majorVersion;
        int minorVersion;
        QmlEnginePrivate::ImportedNamespace *typeNamespace = 0;
        if (!QmlEnginePrivate::get(engine)->resolveType(unit->imports, typeName, &ref.type, &url, &majorVersion, &minorVersion, &typeNamespace)
                || typeNamespace)
        {
            // Known to not be a type:
            //  - known to be a namespace (Namespace {})
            //  - type with unknown namespace (UnknownNamespace.SomeType {})
            QmlError error;
            error.setUrl(unit->imports.baseUrl());
            if (typeNamespace)
                error.setDescription(tr("Namespace %1 cannot be used as a type").arg(QString::fromUtf8(typeName)));
            else
                error.setDescription(tr("%1 is not a type").arg(QString::fromUtf8(typeName)));
            if (!parserRef->refObjects.isEmpty()) {
                QmlParser::Object *obj = parserRef->refObjects.first();
                error.setLine(obj->location.start.line);
                error.setColumn(obj->location.start.column);
            }
            unit->status = QmlCompositeTypeData::Error;
            unit->errorType = QmlCompositeTypeData::GeneralError;
            unit->errors << error;
            doComplete(unit);
            return 0;
        }

        if (ref.type) {
            foreach (QmlParser::Object *obj, parserRef->refObjects) {
               // store namespace for DOM
               obj->majorVersion = majorVersion;
               obj->minorVersion = minorVersion;
            }
            unit->types << ref;
            continue;
        }

        QmlCompositeTypeData *urlUnit = components.value(url);

        if (!urlUnit) {
            urlUnit = new QmlCompositeTypeData;
            urlUnit->status = QmlCompositeTypeData::Waiting;
            urlUnit->imports.setBaseUrl(url);
            components.insert(url, urlUnit);

            loadSource(urlUnit);
        }

        ref.unit = urlUnit;
        switch(urlUnit->status) {
        case QmlCompositeTypeData::Invalid:
        case QmlCompositeTypeData::Error:
            unit->status = QmlCompositeTypeData::Error;
            {
                QmlError error;
                error.setUrl(unit->imports.baseUrl());
                error.setDescription(tr("Type %1 unavailable").arg(QString::fromUtf8(typeName)));
                if (!parserRef->refObjects.isEmpty()) {
                    QmlParser::Object *obj = parserRef->refObjects.first();
                    error.setLine(obj->location.start.line);
                    error.setColumn(obj->location.start.column);
                }
                unit->errors << error;
            }
            if (urlUnit->errorType != QmlCompositeTypeData::AccessError) 
                unit->errors << urlUnit->errors;
            doComplete(unit);
            return 0;

        case QmlCompositeTypeData::Complete:
            break;

        case QmlCompositeTypeData::Waiting:
        case QmlCompositeTypeData::WaitingResources:
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
void QmlCompositeTypeManager::compile(QmlCompositeTypeData *unit)
{
    int waiting = 0;

    QList<QUrl> resourceList = unit->data.referencedResources();

    foreach (QmlScriptParser::Import imp, unit->data.imports()) {
        if (imp.type == QmlScriptParser::Import::File && imp.qualifier.isEmpty()) {
            QUrl importUrl = unit->imports.baseUrl().resolved(QUrl(imp.uri + QLatin1String("/qmldir")));
            if (toLocalFileOrQrc(importUrl).isEmpty()) {
                // Import requires remote qmldir
                resourceList.prepend(importUrl);
            }
        }
    }

    for (int ii = 0; ii < resourceList.count(); ++ii) {
        QUrl url = unit->imports.baseUrl().resolved(resourceList.at(ii));

        QmlCompositeTypeResource *resource = resources.value(url);

        if (!resource) {
            resource = new QmlCompositeTypeResource;
            resource->status = QmlCompositeTypeResource::Waiting;
            resource->url = url.toString();
            resources.insert(url, resource);

            loadResource(resource);
        }

        switch(resource->status) {
        case QmlCompositeTypeResource::Invalid:
        case QmlCompositeTypeResource::Error:
            unit->status = QmlCompositeTypeData::Error;
            {
                QmlError error;
                error.setUrl(unit->imports.baseUrl());
                error.setDescription(tr("Resource %1 unavailable").arg(resource->url));
                unit->errors << error;
            }
            doComplete(unit);
            return;

        case QmlCompositeTypeData::Complete:
            break;

        case QmlCompositeTypeData::Waiting:
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
        if (unit->status != QmlCompositeTypeData::Error) {
            if (!waiting) {
                unit->status = QmlCompositeTypeData::Complete;
                doComplete(unit);
            } else {
                unit->status = QmlCompositeTypeData::Waiting;
            }
        }
    } else {
        unit->status = QmlCompositeTypeData::WaitingResources;
    }
}

QT_END_NAMESPACE
