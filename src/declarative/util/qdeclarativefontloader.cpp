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

#include "private/qdeclarativefontloader_p.h"

#include <qdeclarativecontext.h>
#include <qdeclarativeengine.h>

#include <QStringList>
#include <QUrl>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QFontDatabase>

#include <private/qobject_p.h>
#include <private/qdeclarativeengine_p.h>
#include <qdeclarativeinfo.h>

QT_BEGIN_NAMESPACE

class QDeclarativeFontLoaderPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeFontLoader)

public:
    QDeclarativeFontLoaderPrivate() : reply(0), status(QDeclarativeFontLoader::Null), redirectCount(0) {}

    void addFontToDatabase(const QByteArray &);

    QUrl url;
    QString name;
    QNetworkReply *reply;
    QDeclarativeFontLoader::Status status;
    int redirectCount;
};



/*!
    \qmlclass FontLoader QDeclarativeFontLoader
    \since 4.7
    \brief This item allows using fonts by name or url.

    Example:
    \qml
    FontLoader { id: fixedFont; name: "Courier" }
    FontLoader { id: webFont; source: "http://www.mysite.com/myfont.ttf" }

    Text { text: "Fixed-size font"; font.family: fixedFont.name }
    Text { text: "Fancy font"; font.family: webFont.name }
    \endqml
*/
QDeclarativeFontLoader::QDeclarativeFontLoader(QObject *parent)
    : QObject(*(new QDeclarativeFontLoaderPrivate), parent)
{
}

QDeclarativeFontLoader::~QDeclarativeFontLoader()
{
}

/*!
    \qmlproperty url FontLoader::source
    The url of the font to load.
*/
QUrl QDeclarativeFontLoader::source() const
{
    Q_D(const QDeclarativeFontLoader);
    return d->url;
}

void QDeclarativeFontLoader::setSource(const QUrl &url)
{
    Q_D(QDeclarativeFontLoader);
    if (url == d->url)
        return;
    d->url = qmlContext(this)->resolvedUrl(url);

    d->status = Loading;
    emit statusChanged();
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
    QString lf = QDeclarativeEnginePrivate::urlToLocalFileOrQrc(d->url);
    if (!lf.isEmpty()) {
        int id = QFontDatabase::addApplicationFont(lf);
        if (id != -1) {
            d->name = QFontDatabase::applicationFontFamilies(id).at(0);
            emit nameChanged();
            d->status = QDeclarativeFontLoader::Ready;
        } else {
            d->status = QDeclarativeFontLoader::Error;
            qmlInfo(this) << "Cannot load font: \"" << url.toString() << "\"";
        }
        emit statusChanged();
    } else
#endif
    {
        QNetworkRequest req(d->url);
        req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
        d->reply = qmlEngine(this)->networkAccessManager()->get(req);
        QObject::connect(d->reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    }
}

/*!
    \qmlproperty string FontLoader::name

    This property holds the name of the font family.
    It is set automatically when a font is loaded using the \c url property.

    Use this to set the \c font.family property of a \c Text item.

    Example:
    \qml
    FontLoader { id: webFont; source: "http://www.mysite.com/myfont.ttf" }
    Text { text: "Fancy font"; font.family: webFont.name }
    \endqml
*/
QString QDeclarativeFontLoader::name() const
{
    Q_D(const QDeclarativeFontLoader);
    return d->name;
}

void QDeclarativeFontLoader::setName(const QString &name)
{
    Q_D(QDeclarativeFontLoader);
    if (d->name == name )
        return;
    d->name = name;
    emit nameChanged();
    d->status = Ready;
    emit statusChanged();
}

/*!
    \qmlproperty enumeration FontLoader::status

    This property holds the status of font loading.  It can be one of:
    \list
    \o FontLoader.Null - no font has been set
    \o FontLoader.Ready - the font has been loaded
    \o FontLoader.Loading - the font is currently being loaded
    \o FontLoader.Error - an error occurred while loading the font
    \endlist

    Note that a change in the status property does not cause anything to happen
    (although it reflects what has happened to the font loader internally). If you wish
    to react to the change in status you need to do it yourself, for example in one
    of the following ways:
    \list
    \o Create a state, so that a state change occurs, e.g. State{name: 'loaded'; when: loader.status = FontLoader.Ready;}
    \o Do something inside the onStatusChanged signal handler, e.g. FontLoader{id: loader; onStatusChanged: if(loader.status == FontLoader.Ready) console.log('Loaded');}
    \o Bind to the status variable somewhere, e.g. Text{text: if(loader.status!=FontLoader.Ready){'Not Loaded';}else{'Loaded';}}
    \endlist
*/
QDeclarativeFontLoader::Status QDeclarativeFontLoader::status() const
{
    Q_D(const QDeclarativeFontLoader);
    return d->status;
}

#define FONTLOADER_MAXIMUM_REDIRECT_RECURSION 16

void QDeclarativeFontLoader::replyFinished()
{
    Q_D(QDeclarativeFontLoader);
    if (d->reply) {
        d->redirectCount++;
        if (d->redirectCount < FONTLOADER_MAXIMUM_REDIRECT_RECURSION) {
            QVariant redirect = d->reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
            if (redirect.isValid()) {
                QUrl url = d->reply->url().resolved(redirect.toUrl());
                d->reply->deleteLater();
                d->reply = 0;
                setSource(url);
                return;
            }
        }
        d->redirectCount=0;

        if (!d->reply->error()) {
            QByteArray ba = d->reply->readAll();
            d->addFontToDatabase(ba);
        } else {
            d->status = Error;
            qmlInfo(this) << "Cannot load font: \"" << d->reply->url().toString() << "\"";
            emit statusChanged();
        }
        d->reply->deleteLater();
        d->reply = 0;
    }
}

void QDeclarativeFontLoaderPrivate::addFontToDatabase(const QByteArray &ba)
{
    Q_Q(QDeclarativeFontLoader);

    int id = QFontDatabase::addApplicationFontFromData(ba);
    if (id != -1) {
        name = QFontDatabase::applicationFontFamilies(id).at(0);
        emit q->nameChanged();
        status = QDeclarativeFontLoader::Ready;
    } else {
        status = QDeclarativeFontLoader::Error;
        qmlInfo(q) << "Cannot load font: \"" << url.toString() << "\"";
    }
    emit q->statusChanged();
}

QT_END_NAMESPACE
