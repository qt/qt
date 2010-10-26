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
#include <QFontDatabase>

#include <private/qobject_p.h>
#include <private/qdeclarativeengine_p.h>
#include <qdeclarativeinfo.h>

QT_BEGIN_NAMESPACE

#define FONTLOADER_MAXIMUM_REDIRECT_RECURSION 16

class QDeclarativeFontObject : public QObject
{
Q_OBJECT

public:
    QDeclarativeFontObject(int _id);

    void download(const QUrl &url, QNetworkAccessManager *manager);

Q_SIGNALS:
    void fontDownloaded(const QString&, QDeclarativeFontLoader::Status);

private Q_SLOTS:
    void replyFinished();

public:
    int id;

private:
    QNetworkReply *reply;
    int redirectCount;

    Q_DISABLE_COPY(QDeclarativeFontObject)
};

QDeclarativeFontObject::QDeclarativeFontObject(int _id = -1)
    : QObject(0), id(_id), reply(0), redirectCount(0) {}


void QDeclarativeFontObject::download(const QUrl &url, QNetworkAccessManager *manager)
{
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    reply = manager->get(req);
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
}

void QDeclarativeFontObject::replyFinished()
{
    if (reply) {
        redirectCount++;
        if (redirectCount < FONTLOADER_MAXIMUM_REDIRECT_RECURSION) {
            QVariant redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
            if (redirect.isValid()) {
                QUrl url = reply->url().resolved(redirect.toUrl());
                QNetworkAccessManager *manager = reply->manager();
                reply->deleteLater();
                reply = 0;
                download(url, manager);
                return;
            }
        }
        redirectCount = 0;

        if (!reply->error()) {
            id = QFontDatabase::addApplicationFontFromData(reply->readAll());
            if (id != -1)
                emit fontDownloaded(QFontDatabase::applicationFontFamilies(id).at(0), QDeclarativeFontLoader::Ready);
            else
                emit fontDownloaded(QString(), QDeclarativeFontLoader::Error);
        } else {
            emit fontDownloaded(QString(), QDeclarativeFontLoader::Error);
        }
        reply->deleteLater();
        reply = 0;
    }
}


class QDeclarativeFontLoaderPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeFontLoader)

public:
    QDeclarativeFontLoaderPrivate() : status(QDeclarativeFontLoader::Null) {}

    QUrl url;
    QString name;
    QDeclarativeFontLoader::Status status;
    static QHash<QUrl, QDeclarativeFontObject*> fonts;
};

QHash<QUrl, QDeclarativeFontObject*> QDeclarativeFontLoaderPrivate::fonts;

/*!
    \qmlclass FontLoader QDeclarativeFontLoader
  \ingroup qml-utility-elements
    \since 4.7
    \brief The FontLoader element allows fonts to be loaded by name or URL.

    The FontLoader element is used to load fonts by name or URL. 
    
    The \l status indicates when the font has been loaded, which is useful 
    for fonts loaded from remote sources.

    For example:
    \qml
    import QtQuick 1.0

    Column { 
        FontLoader { id: fixedFont; name: "Courier" }
        FontLoader { id: webFont; source: "http://www.mysite.com/myfont.ttf" }

        Text { text: "Fixed-size font"; font.family: fixedFont.name }
        Text { text: "Fancy font"; font.family: webFont.name }
    }
    \endqml

    \sa {declarative/text/fonts}{Fonts example}
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
    emit sourceChanged();

#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
    QString localFile = QDeclarativeEnginePrivate::urlToLocalFileOrQrc(d->url);
    if (!localFile.isEmpty()) {
        if (!d->fonts.contains(d->url)) {
            int id = QFontDatabase::addApplicationFont(localFile);
            if (id != -1) {
                updateFontInfo(QFontDatabase::applicationFontFamilies(id).at(0), Ready);
                QDeclarativeFontObject *fo = new QDeclarativeFontObject(id);
                d->fonts[d->url] = fo;
            } else {
                updateFontInfo(QString(), Error);
            }
        } else {
            updateFontInfo(QFontDatabase::applicationFontFamilies(d->fonts[d->url]->id).at(0), Ready);
        }
    } else
#endif
    {
        if (!d->fonts.contains(d->url)) {
            QDeclarativeFontObject *fo = new QDeclarativeFontObject;
            d->fonts[d->url] = fo;
            fo->download(d->url, qmlEngine(this)->networkAccessManager());
            d->status = Loading;
            emit statusChanged();
            QObject::connect(fo, SIGNAL(fontDownloaded(QString,QDeclarativeFontLoader::Status)),
                this, SLOT(updateFontInfo(QString,QDeclarativeFontLoader::Status)));
        } else {
            QDeclarativeFontObject *fo = d->fonts[d->url];
            if (fo->id == -1) {
                d->status = Loading;
                emit statusChanged();
                QObject::connect(fo, SIGNAL(fontDownloaded(QString,QDeclarativeFontLoader::Status)),
                    this, SLOT(updateFontInfo(QString,QDeclarativeFontLoader::Status)));
            }
            else
                updateFontInfo(QFontDatabase::applicationFontFamilies(fo->id).at(0), Ready);
        }
    }
}

void QDeclarativeFontLoader::updateFontInfo(const QString& name, QDeclarativeFontLoader::Status status)
{
    Q_D(QDeclarativeFontLoader);

    if (name != d->name) {
        d->name = name;
        emit nameChanged();
    }
    if (status != d->status) {
        if (status == Error)
            qmlInfo(this) << "Cannot load font: \"" << d->url.toString() << "\"";
        d->status = status;
        emit statusChanged();
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
    if (d->name == name)
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

    Use this status to provide an update or respond to the status change in some way.
    For example, you could:

    \e {Trigger a state change:}
    \qml 
        State { name: 'loaded'; when: loader.status = FontLoader.Ready }
    \endqml

    \e {Implement an \c onStatusChanged signal handler:}
    \qml 
        FontLoader {
            id: loader
            onStatusChanged: if (loader.status == FontLoader.Ready) console.log('Loaded')
        }
    \endqml

    \e {Bind to the status value:}
    \qml
        Text { text: loader.status != FontLoader.Ready ? 'Not Loaded' : 'Loaded' }
    \endqml
*/
QDeclarativeFontLoader::Status QDeclarativeFontLoader::status() const
{
    Q_D(const QDeclarativeFontLoader);
    return d->status;
}

QT_END_NAMESPACE

#include <qdeclarativefontloader.moc>
