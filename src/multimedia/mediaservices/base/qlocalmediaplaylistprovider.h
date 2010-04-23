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

#ifndef QLOCALMEDIAPAYLISTPROVIDER_H
#define QLOCALMEDIAPAYLISTPROVIDER_H

#include <QtMediaServices/qmediaplaylistprovider.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Multimedia)


class QLocalMediaPlaylistProviderPrivate;
class Q_MEDIASERVICES_EXPORT QLocalMediaPlaylistProvider : public QMediaPlaylistProvider
{
    Q_OBJECT

public:
    QLocalMediaPlaylistProvider(QObject *parent=0);
    virtual ~QLocalMediaPlaylistProvider();

    virtual int mediaCount() const;
    virtual QMediaContent media(int pos) const;

    virtual bool isReadOnly() const;

    virtual bool addMedia(const QMediaContent &content);
    virtual bool addMedia(const QList<QMediaContent> &items);
    virtual bool insertMedia(int pos, const QMediaContent &content);
    virtual bool insertMedia(int pos, const QList<QMediaContent> &items);
    virtual bool removeMedia(int pos);
    virtual bool removeMedia(int start, int end);
    virtual bool clear();

public Q_SLOTS:
    virtual void shuffle();

private:
    Q_DECLARE_PRIVATE(QLocalMediaPlaylistProvider)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QLOCALMEDIAPAYLISTSOURCE_H
