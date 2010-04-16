/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMediaServices  module of the Qt Toolkit.
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

#include <QtMediaServices/qlocalmediaplaylistprovider.h>
#include "qmediaplaylistprovider_p.h"
#include <QtMediaServices/qmediacontent.h>


QT_BEGIN_NAMESPACE

class QLocalMediaPlaylistProviderPrivate: public QMediaPlaylistProviderPrivate
{
public:
    QList<QMediaContent> resources;
};

QLocalMediaPlaylistProvider::QLocalMediaPlaylistProvider(QObject *parent)
    :QMediaPlaylistProvider(*new QLocalMediaPlaylistProviderPrivate, parent)
{
}

QLocalMediaPlaylistProvider::~QLocalMediaPlaylistProvider()
{
}

bool QLocalMediaPlaylistProvider::isReadOnly() const
{
    return false;
}

int QLocalMediaPlaylistProvider::mediaCount() const
{
    return d_func()->resources.size();
}

QMediaContent QLocalMediaPlaylistProvider::media(int pos) const
{
    return d_func()->resources.value(pos);
}

bool QLocalMediaPlaylistProvider::addMedia(const QMediaContent &content)
{
    Q_D(QLocalMediaPlaylistProvider);

    int pos = d->resources.count();

    emit mediaAboutToBeInserted(pos, pos);
    d->resources.append(content);
    emit mediaInserted(pos, pos);

    return true;
}

bool QLocalMediaPlaylistProvider::addMedia(const QList<QMediaContent> &items)
{
    Q_D(QLocalMediaPlaylistProvider);

    if (items.isEmpty())
        return true;

    int pos = d->resources.count();
    int end = pos+items.count()-1;

    emit mediaAboutToBeInserted(pos, end);
    d->resources.append(items);
    emit mediaInserted(pos, end);

    return true;
}


bool QLocalMediaPlaylistProvider::insertMedia(int pos, const QMediaContent &content)
{
    Q_D(QLocalMediaPlaylistProvider);

    emit mediaAboutToBeInserted(pos, pos);
    d->resources.insert(pos, content);
    emit mediaInserted(pos,pos);

    return true;
}

bool QLocalMediaPlaylistProvider::insertMedia(int pos, const QList<QMediaContent> &items)
{
    Q_D(QLocalMediaPlaylistProvider);

    if (items.isEmpty())
        return true;

    const int last = pos+items.count()-1;

    emit mediaAboutToBeInserted(pos, last);
    for (int i=0; i<items.count(); i++)
        d->resources.insert(pos+i, items.at(i));
    emit mediaInserted(pos, last);

    return true;
}

bool QLocalMediaPlaylistProvider::removeMedia(int fromPos, int toPos)
{
    Q_D(QLocalMediaPlaylistProvider);

    Q_ASSERT(fromPos >= 0);
    Q_ASSERT(fromPos <= toPos);
    Q_ASSERT(toPos < mediaCount());

    emit mediaAboutToBeRemoved(fromPos, toPos);
    d->resources.erase(d->resources.begin()+fromPos, d->resources.begin()+toPos+1);
    emit mediaRemoved(fromPos, toPos);

    return true;
}

bool QLocalMediaPlaylistProvider::removeMedia(int pos)
{
    Q_D(QLocalMediaPlaylistProvider);

    emit mediaAboutToBeRemoved(pos, pos);
    d->resources.removeAt(pos);
    emit mediaRemoved(pos, pos);

    return true;
}

bool QLocalMediaPlaylistProvider::clear()
{
    Q_D(QLocalMediaPlaylistProvider);
    if (!d->resources.isEmpty()) {
        int lastPos = mediaCount()-1;
        emit mediaAboutToBeRemoved(0, lastPos);
        d->resources.clear();
        emit mediaRemoved(0, lastPos);
    }

    return true;
}

void QLocalMediaPlaylistProvider::shuffle()
{
    Q_D(QLocalMediaPlaylistProvider);
    if (!d->resources.isEmpty()) {
        QList<QMediaContent> resources;

        while (!d->resources.isEmpty()) {
            resources.append(d->resources.takeAt(qrand() % d->resources.size()));
        }

        d->resources = resources;
        emit mediaChanged(0, mediaCount()-1);
    }

}

#include "moc_qlocalmediaplaylistprovider.cpp"

QT_END_NAMESPACE

