/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMediaServices module of the Qt Toolkit.
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

#ifndef QMEDIAPLAYLISTPROVIDER_H
#define QMEDIAPLAYLISTPROVIDER_H

#include <QtCore/qobject.h>

#include <QtMediaServices/qmediacontent.h>
#include <QtMediaServices/qmediaplaylist.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Multimedia)

class QString;


class QMediaPlaylistProviderPrivate;
class Q_MEDIASERVICES_EXPORT QMediaPlaylistProvider : public QObject
{
    Q_OBJECT

public:
    QMediaPlaylistProvider(QObject *parent=0);
    virtual ~QMediaPlaylistProvider();

    virtual bool load(const QUrl &location, const char *format = 0);
    virtual bool load(QIODevice * device, const char *format = 0);
    virtual bool save(const QUrl &location, const char *format = 0);
    virtual bool save(QIODevice * device, const char *format);

    virtual int mediaCount() const = 0;
    virtual QMediaContent media(int index) const = 0;

    virtual bool isReadOnly() const;

    virtual bool addMedia(const QMediaContent &content);
    virtual bool addMedia(const QList<QMediaContent> &contentList);
    virtual bool insertMedia(int index, const QMediaContent &content);
    virtual bool insertMedia(int index, const QList<QMediaContent> &content);
    virtual bool removeMedia(int pos);
    virtual bool removeMedia(int start, int end);
    virtual bool clear();

public Q_SLOTS:
    virtual void shuffle();

Q_SIGNALS:
    void mediaAboutToBeInserted(int start, int end);
    void mediaInserted(int start, int end);

    void mediaAboutToBeRemoved(int start, int end);
    void mediaRemoved(int start, int end);

    void mediaChanged(int start, int end);

    void loaded();
    void loadFailed(QMediaPlaylist::Error, const QString& errorMessage);

protected:
    QMediaPlaylistProviderPrivate *d_ptr;
    QMediaPlaylistProvider(QMediaPlaylistProviderPrivate &dd, QObject *parent);

private:
    Q_DECLARE_PRIVATE(QMediaPlaylistProvider)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMEDIAPLAYLISTPROVIDER_H
