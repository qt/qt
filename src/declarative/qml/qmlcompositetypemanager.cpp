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

#include <private/qmlcompositetypedata_p.h>
#include <private/qmlcompositetypemanager_p.h>
#include <private/qmlscriptparser_p.h>
#include <QtDeclarative/qmlengine.h>
#include <QtNetwork/qnetworkreply.h>
#include <private/qmlengine_p.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlcomponent_p.h>
#include <private/qmlcompiler_p.h>

QT_BEGIN_NAMESPACE

QmlCompositeTypeData::QmlCompositeTypeData()
: status(Invalid), errorType(NoError), component(0), compiledComponent(0)
{
}

QmlCompositeTypeData::~QmlCompositeTypeData()
{
    for (int ii = 0; ii < dependants.count(); ++ii)
        dependants.at(ii)->release();

    if (compiledComponent)
        compiledComponent->release();

    if (component) 
        delete component;
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
        } else {
            component = new QmlComponent(engine, 0);
            component->d_func()->url = imports.baseUrl();
            component->d_func()->errors = errors;
        }

    }

    return component;
}

QmlCompiledData *
QmlCompositeTypeData::toCompiledComponent(QmlEngine *engine)
{
    if (status == Complete && !compiledComponent) {

        compiledComponent = new QmlCompiledData;
        compiledComponent->url = imports.baseUrl();
        compiledComponent->name = compiledComponent->url.toString().toLatin1(); // ###

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

QmlCompositeTypeData *QmlCompositeTypeManager::get(const QUrl &url)
{
    QmlCompositeTypeData *unit = components.value(url.toString());

    if (!unit) {
        unit = new QmlCompositeTypeData;
        unit->status = QmlCompositeTypeData::Waiting;
        unit->imports.setBaseUrl(url);
        components.insert(url.toString(), unit);

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
}


void QmlCompositeTypeManager::replyFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());

    QmlCompositeTypeData *unit = components.value(reply->url().toString());
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

void QmlCompositeTypeManager::loadSource(QmlCompositeTypeData *unit)
{
    QUrl url(unit->imports.baseUrl());

    if (url.scheme() == QLatin1String("file")) {

        QFile file(url.toLocalFile());
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
    }
}

void QmlCompositeTypeManager::setData(QmlCompositeTypeData *unit, 
                                     const QByteArray &data,
                                     const QUrl &url)
{
    bool ok = true;
    if (!unit->data.parse(data, url)) {
        ok = false;
        unit->errors << unit->data.errors();
    } else {
        foreach (QmlScriptParser::Import imp, unit->data.imports()) {
            if (!QmlEnginePrivate::get(engine)->addToImport(&unit->imports, imp.uri, imp.qualifier, imp.version, imp.type)) {
                QmlError error;
                error.setUrl(url);
                error.setDescription(tr("Import %1 unavailable").arg(imp.uri));
                unit->errors << error;
                ok = false;
            }
        }
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

void QmlCompositeTypeManager::checkComplete(QmlCompositeTypeData *unit)
{
    if (unit->status != QmlCompositeTypeData::Waiting)
        return;

    int waiting = 0;
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
    if (!waiting) {
        unit->status = QmlCompositeTypeData::Complete;
        doComplete(unit);
    }
}

void QmlCompositeTypeManager::compile(QmlCompositeTypeData *unit)
{
    QStringList typeNames = unit->data.types();

    int waiting = 0;
    for (int ii = 0; ii < typeNames.count(); ++ii) {
        QByteArray type = typeNames.at(ii).toLatin1();

        QmlCompositeTypeData::TypeReference ref;
        if (type == QByteArray("Property") ||
            type == QByteArray("Signal")) {
            unit->types << ref;
            continue;
        }

        QUrl url;
        if (!QmlEnginePrivate::get(engine)->resolveType(unit->imports, type, &ref.type, &url)) {
            // XXX could produce error message here.
        }

        if (ref.type) {
            unit->types << ref;
            continue;
        }

        QmlCompositeTypeData *urlUnit = components.value(url.toString());

        if (!urlUnit) {
            urlUnit = new QmlCompositeTypeData;
            urlUnit->status = QmlCompositeTypeData::Waiting;
            urlUnit->imports.setBaseUrl(url);
            components.insert(url.toString(), urlUnit);

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
                error.setDescription(tr("Type %1 unavailable").arg(QLatin1String(type)));
                unit->errors << error;
            }
            if (urlUnit->errorType != QmlCompositeTypeData::AccessError) 
                unit->errors << urlUnit->errors;
            doComplete(unit);
            return;

        case QmlCompositeTypeData::Complete:
            break;

        case QmlCompositeTypeData::Waiting:
            unit->addref();
            ref.unit->dependants << unit;
            waiting++;
            break;
        }

        unit->types << ref;
    }

    if (waiting) {
        unit->status = QmlCompositeTypeData::Waiting;
    } else {
        unit->status = QmlCompositeTypeData::Complete;
        doComplete(unit);
    }
}

QT_END_NAMESPACE
