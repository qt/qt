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

#include "private/qobject_p.h"
#include "qmlfontfamily.h"
#include <QUrl>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QmlContext>
#include <QtDeclarative/qmlengine.h>
#include <QFontDatabase>

QT_BEGIN_NAMESPACE

class QmlFontFamilyPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlFontFamily);

public:
    QmlFontFamilyPrivate() : reply(0), status(QmlFontFamily::Null) {}

    void addFontToDatabase(const QByteArray &);

    QUrl url;
    QString name;
    QNetworkReply *reply;
    QmlFontFamily::Status status;
};

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,FontFamily,QmlFontFamily)

/*!
    \qmlclass FontFamily QmlFontFamily
    \ingroup group_utility
    \brief This item allows using fonts by name or url.

    Example:
    \code
    FontFamily { id: FixedFont; name: "Courier" }
    FontFamily { id: WebFont; source: "http://www.mysite.com/myfont.ttf" }

    Text { text: "Fixed-size font"; font.family: FixedFont.name }
    Text { text: "Fancy font"; font.family: WebFont.name }
    \endcode
*/
QmlFontFamily::QmlFontFamily(QObject *parent)
    : QObject(*(new QmlFontFamilyPrivate), parent)
{
}

QmlFontFamily::~QmlFontFamily()
{
}

/*!
    \qmlproperty url FontFamily::source
    The url of the font to load.
*/
QUrl QmlFontFamily::source() const
{
    Q_D(const QmlFontFamily);
    return d->url;
}

void QmlFontFamily::setSource(const QUrl &url)
{
    Q_D(QmlFontFamily);
    if (url == d->url)
        return;
    d->url = qmlContext(this)->resolvedUrl(url);

    d->status = Loading;
    emit statusChanged();
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
    if (d->url.scheme() == QLatin1String("file")) {
        QFile file(d->url.toLocalFile());
        file.open(QIODevice::ReadOnly);
        QByteArray ba = file.readAll();
        d->addFontToDatabase(ba);
    } else
#endif
    {
        QNetworkRequest req(d->url);
        req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        d->reply = qmlEngine(this)->networkAccessManager()->get(req);
        QObject::connect(d->reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    }
}

/*!
    \qmlproperty string FontFamily::name

    This property holds the name of the font family.
    It is set automatically when a font is loaded using the \c url property.

    Use this to set the \c font.family property of a \c Text item.

    Example:
    \qml
    FontFamily { id: WebFont; source: "http://www.mysite.com/myfont.ttf" }
    Text { text: "Fancy font"; font.family: WebFont.name }
    \endqml
*/
QString QmlFontFamily::name() const
{
    Q_D(const QmlFontFamily);
    return d->name;
}

void QmlFontFamily::setName(const QString &name)
{
    Q_D(QmlFontFamily);
    if (d->name == name )
        return;
    d->name = name;
    emit nameChanged();
}

/*!
    \qmlproperty enum FontFamily::status

    This property holds the status of font loading.  It can be one of:
    \list
    \o Null - no font has been set
    \o Ready - the font has been loaded
    \o Loading - the font is currently being loaded
    \o Error - an error occurred while loading the font
    \endlist
*/
QmlFontFamily::Status QmlFontFamily::status() const
{
    Q_D(const QmlFontFamily);
    return d->status;
}

void QmlFontFamily::replyFinished()
{
    Q_D(QmlFontFamily);
    if (!d->reply->error()) {
        QByteArray ba = d->reply->readAll();
        d->addFontToDatabase(ba);
    } else {
	d->status = Error;
        emit statusChanged();
    }
    d->reply->deleteLater();
    d->reply = 0;
}

void QmlFontFamilyPrivate::addFontToDatabase(const QByteArray &ba)
{
    Q_Q(QmlFontFamily);

    int id = QFontDatabase::addApplicationFontFromData(ba);
    if (id != -1) {
        name = QFontDatabase::applicationFontFamilies(id).at(0);
        emit q->nameChanged();
        status = QmlFontFamily::Ready;
    } else {
        status = QmlFontFamily::Error;
        qWarning() << "Cannot load font: " << name << url;
    }
    emit q->statusChanged();
}

QT_END_NAMESPACE
