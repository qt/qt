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

#include <private/qmlcompositetypemanager_p.h>
#include <private/qmlxmlparser_p.h>
#include <private/qmlcompiledcomponent_p.h>
#include <QtDeclarative/qmlengine.h>
#include <QtNetwork/qnetworkreply.h>
#include <private/qmlengine_p.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlcomponent_p.h>

QmlCompositeTypeManager::TypeData::TypeData()
: status(Invalid), component(0), compiledComponent(0)
{
}

QmlCompositeTypeManager::TypeData::~TypeData()
{
    for(int ii = 0; ii < dependants.count(); ++ii)
        dependants.at(ii)->release();

    if(compiledComponent)
        compiledComponent->release();

    if(component) 
        delete component;
}

void QmlCompositeTypeManager::TypeData::addWaiter(QmlComponentPrivate *p)
{
    waiters << p;
}

void QmlCompositeTypeManager::TypeData::remWaiter(QmlComponentPrivate *p)
{
    waiters.removeAll(p);
}

QmlComponent *QmlCompositeTypeManager::TypeData::toComponent(QmlEngine *engine)
{
    if(!component) {

        QmlCompiledComponent *cc = toCompiledComponent(engine);
        if(cc) {
            component = new QmlComponent(engine, cc, -1, -1, 0);
        } else {
            component = new QmlComponent(engine, 0);
        }

    }

    return component;
}

QmlCompiledComponent *
QmlCompositeTypeManager::TypeData::toCompiledComponent(QmlEngine *engine)
{
    if(status == Complete && !compiledComponent) {

        compiledComponent = new QmlCompiledComponent;
        compiledComponent->url = QUrl(url);
        compiledComponent->name = url.toLatin1(); // ###

        QmlCompiler compiler;
        if(!compiler.compile(engine, this, compiledComponent)) {
            status = Error;
            errorDescription = compiler.errorDescription() + 
                               QLatin1String("@") +
                               url + QLatin1String(":") + 
                               QString::number(compiler.errorLine());
            compiledComponent->release();
            compiledComponent = 0;
        }

        // Data is no longer needed once we have a compiled component
        data.clear();
    }

    if(compiledComponent)
        compiledComponent->addref();

    return compiledComponent;
}

QmlCompositeTypeManager::TypeData::TypeReference::TypeReference()
: type(0), unit(0), parser(0) 
{
}

QmlCompositeTypeManager::QmlCompositeTypeManager(QmlEngine *e)
: engine(e)
{
}

QmlCompositeTypeManager::TypeData *QmlCompositeTypeManager::get(const QUrl &url)
{
    TypeData *unit = components.value(url.toString());

    if(!unit) {
        unit = new TypeData;
        unit->status = TypeData::Waiting;
        unit->url = url.toString();
        components.insert(url.toString(), unit);

        loadSource(unit);
    }

    unit->addref();
    return unit;
}

QmlCompositeTypeManager::TypeData *
QmlCompositeTypeManager::getImmediate(const QByteArray &data, const QUrl &url)
{
    TypeData *unit = new TypeData;
    unit->status = TypeData::Waiting;
    unit->url = url.toString();
    setData(unit, data, url);
    return unit;
}

void QmlCompositeTypeManager::clearCache()
{
    for(Components::Iterator iter = components.begin(); iter != components.end();) {
        if((*iter)->status != TypeData::Waiting) {
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

    TypeData *unit = components.value(reply->url().toString());
    Q_ASSERT(unit);

    if(reply->error() != QNetworkReply::NoError) {

        QString errorDescription;
        // ### - Fill in error
        errorDescription = QLatin1String("Network error for URL ") + 
                      reply->url().toString();

        unit->status = TypeData::Error;
        unit->errorDescription = errorDescription;
        doComplete(unit);

    } else {
        QByteArray data = reply->readAll();

        setData(unit, data, reply->url());
    }

    reply->deleteLater();
}

void QmlCompositeTypeManager::loadSource(TypeData *unit)
{
    QUrl url(unit->url);

    if(url.scheme() == QLatin1String("file")) {

        QFile file(url.toLocalFile());
        if(file.open(QFile::ReadOnly)) {
            QByteArray data = file.readAll();
            setData(unit, data, url);
        } else {
            QString errorDescription;
            // ### - Fill in error
            errorDescription = QLatin1String("File error for URL ") + url.toString();
            unit->status = TypeData::Error;
            unit->errorDescription = errorDescription;
            doComplete(unit);
        }

    } else {
        QNetworkReply *reply = 
            engine->networkAccessManager()->get(QNetworkRequest(url));
        QObject::connect(reply, SIGNAL(finished()),
                         this, SLOT(replyFinished()));
    }
}

void QmlCompositeTypeManager::setData(TypeData *unit, const QByteArray &data,
                              const QUrl &url)
{
    if(!unit->data.parse(data, url)) {

        unit->status = TypeData::Error;
        unit->errorDescription = unit->data.errorDescription();
        doComplete(unit);

    } else {

        engine->addNameSpacePaths(unit->data.nameSpacePaths());
        compile(unit);

    }
}

void QmlCompositeTypeManager::doComplete(TypeData *unit)
{
    for(int ii = 0; ii < unit->dependants.count(); ++ii) {
        checkComplete(unit->dependants.at(ii));
        unit->dependants.at(ii)->release();
    }
    unit->dependants.clear();

    while(!unit->waiters.isEmpty()) {
        QmlComponentPrivate *p = unit->waiters.takeFirst();
        p->typeDataReady();
    }
}

void QmlCompositeTypeManager::checkComplete(TypeData *unit)
{
    if(unit->status != TypeData::Waiting)
        return;

    int waiting = 0;
    for(int ii = 0; ii < unit->types.count(); ++ii) {
        TypeData *u = unit->types.at(ii).unit;

        if(!u)
            continue;

        if(u->status == TypeData::Error) {
            unit->status = TypeData::Error;
            unit->errorDescription = u->errorDescription;
            doComplete(unit);
            return;
        } else if(u->status == TypeData::Waiting) {
            waiting++;
        }
    }
    if(!waiting) {
        unit->status = TypeData::Complete;
        doComplete(unit);
    }
}

void QmlCompositeTypeManager::compile(TypeData *unit)
{
    QStringList typeNames = unit->data.types();

    int waiting = 0;
    for(int ii = 0; ii < typeNames.count(); ++ii) {
        QByteArray type = typeNames.at(ii).toLatin1();

        TypeData::TypeReference ref;
        if (type == QByteArray("Property") ||
            type == QByteArray("Signal")) {
            unit->types << ref;
            continue;
        }

        QmlCustomParser *parser = 
            QmlMetaType::customParser(type);

        if(parser) {
            ref.parser = parser;
            unit->types << ref;
            continue;
        }

        ref.type = QmlMetaType::qmlType(type);
        if(ref.type) {
            ref.parser = parser;
            unit->types << ref;
            continue;
        }

        QUrl url = engine->componentUrl(QUrl(type + ".qml"), QUrl(unit->url));
        TypeData *urlUnit = components.value(url.toString());

        if(!urlUnit) {
            urlUnit = new TypeData;
            urlUnit->status = TypeData::Waiting;
            urlUnit->url = url.toString();
            components.insert(url.toString(), urlUnit);

            loadSource(urlUnit);
        }

        ref.unit = urlUnit;
        switch(urlUnit->status) {
        case TypeData::Invalid:
        case TypeData::Error:
            unit->status = TypeData::Error;
            unit->errorDescription = urlUnit->errorDescription;
            doComplete(unit);
            return;

        case TypeData::Complete:
            break;

        case TypeData::Waiting:
            unit->addref();
            ref.unit->dependants << unit;
            waiting++;
            break;
        }

        unit->types << ref;
    }

    if(waiting) {
        unit->status = TypeData::Waiting;
    } else {
        unit->status = TypeData::Complete;
        doComplete(unit);
    }
}
