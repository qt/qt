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

#include <QtCore/qurl.h>

#include <QtMediaServices/qmediaplaylistprovider.h>
#include "qmediaplaylistprovider_p.h"


QT_BEGIN_NAMESPACE

/*!
    \class QMediaPlaylistProvider
    \preliminary
    \since 4.7
    \brief The QMediaPlaylistProvider class provides an abstract list of media.

    \sa QMediaPlaylist
*/

/*!
    Constructs a playlist provider with the given \a parent.
*/
QMediaPlaylistProvider::QMediaPlaylistProvider(QObject *parent)
    :QObject(parent), d_ptr(new QMediaPlaylistProviderPrivate)
{
}

/*!
    \internal
*/
QMediaPlaylistProvider::QMediaPlaylistProvider(QMediaPlaylistProviderPrivate &dd, QObject *parent)
    :QObject(parent), d_ptr(&dd)
{
}

/*!
    Destroys a playlist provider.
*/
QMediaPlaylistProvider::~QMediaPlaylistProvider()
{
    delete d_ptr;
}

/*!
    \fn QMediaPlaylistProvider::mediaCount() const;

    Returns the size of playlist.
*/

/*!
    \fn QMediaPlaylistProvider::media(int index) const;

    Returns the media at \a index in the playlist.

    If the index is invalid this will return a null media content.
*/


/*!
    Loads a playlist from from a URL \a location. If no playlist \a format is specified the loader
    will inspect the URL or probe the headers to guess the format.

    New items are appended to playlist.

    Returns true if the provider supports the format and loading from the locations URL protocol,
    otherwise this will return false.
*/
bool QMediaPlaylistProvider::load(const QUrl &location, const char *format)
{
    Q_UNUSED(location);
    Q_UNUSED(format);
    return false;
}

/*!
    Loads a playlist from from an I/O \a device. If no playlist \a format is specified the loader
    will probe the headers to guess the format.

    New items are appended to playlist.

    Returns true if the provider supports the format and loading from an I/O device, otherwise this
    will return false.
*/
bool QMediaPlaylistProvider::load(QIODevice * device, const char *format)
{
    Q_UNUSED(device);
    Q_UNUSED(format);
    return false;
}

/*!
    Saves the contents of a playlist to a URL \a location.  If no playlist \a format is specified
    the writer will inspect the URL to guess the format.

    Returns true if the playlist was saved succesfully; and false otherwise.
  */
bool QMediaPlaylistProvider::save(const QUrl &location, const char *format)
{
    Q_UNUSED(location);
    Q_UNUSED(format);
    return false;
}

/*!
    Saves the contents of a playlist to an I/O \a device in the specified \a format.

    Returns true if the playlist was saved succesfully; and false otherwise.
*/
bool QMediaPlaylistProvider::save(QIODevice * device, const char *format)
{
    Q_UNUSED(device);
    Q_UNUSED(format);
    return false;
}

/*!
    Returns true if a playlist is read-only; otherwise returns false.
*/
bool QMediaPlaylistProvider::isReadOnly() const
{
    return true;
}

/*!
    Append \a media to a playlist.

    Returns true if the media was appended; and false otherwise.
*/
bool QMediaPlaylistProvider::addMedia(const QMediaContent &media)
{
    Q_UNUSED(media);
    return false;
}

/*!
    Append multiple media \a items to a playlist.

    Returns true if the media items were appended; and false otherwise.
*/
bool QMediaPlaylistProvider::addMedia(const QList<QMediaContent> &items)
{
    foreach(const QMediaContent &item, items) {
        if (!addMedia(item))
            return false;
    }

    return true;
}

/*!
    Inserts \a media into a playlist at \a position.

    Returns true if the media was inserted; and false otherwise.
*/
bool QMediaPlaylistProvider::insertMedia(int position, const QMediaContent &media)
{
    Q_UNUSED(position);
    Q_UNUSED(media);
    return false;
}

/*!
    Inserts multiple media \a items into a playlist at \a position.

    Returns true if the media \a items were inserted; and false otherwise.
*/
bool QMediaPlaylistProvider::insertMedia(int position, const QList<QMediaContent> &items)
{
    for (int i=0; i<items.count(); i++) {
        if (!insertMedia(position+i,items.at(i)))
            return false;
    }

    return true;
}


/*!
    Removes the media at \a position from a playlist.

    Returns true if the media was removed; and false otherwise.
*/
bool QMediaPlaylistProvider::removeMedia(int position)
{
    Q_UNUSED(position);
    return false;
}

/*!
    Removes the media between the given \a start and \a end positions from a playlist.

    Returns true if the media was removed; and false otherwise.
  */
bool QMediaPlaylistProvider::removeMedia(int start, int end)
{
    for (int pos=start; pos<=end; pos++) {
        if (!removeMedia(pos))
            return false;
    }

    return true;
}

/*!
    Removes all media from a playlist.

    Returns true if the media was removed; and false otherwise.
*/
bool QMediaPlaylistProvider::clear()
{
    return removeMedia(0, mediaCount()-1);
}

/*!
    Shuffles the contents of a playlist.
*/
void QMediaPlaylistProvider::shuffle()
{
}

/*!
    \fn void QMediaPlaylistProvider::mediaAboutToBeInserted(int start, int end);

    Signals that new media is about to be inserted into a playlist between the \a start and \a end
    positions.
*/

/*!
    \fn void QMediaPlaylistProvider::mediaInserted(int start, int end);

    Signals that new media has been inserted into a playlist between the \a start and \a end
    positions.
*/

/*!
    \fn void QMediaPlaylistProvider::mediaAboutToBeRemoved(int start, int end);

    Signals that media is about to be removed from a playlist between the \a start and \a end
    positions.
*/

/*!
    \fn void QMediaPlaylistProvider::mediaRemoved(int start, int end);

    Signals that media has been removed from a playlist between the \a start and \a end positions.
*/

/*!
    \fn void QMediaPlaylistProvider::mediaChanged(int start, int end);

    Signals that media in playlist between the \a start and \a end positions inclusive has changed.
*/

/*!
    \fn void QMediaPlaylistProvider::loaded()

    Signals that a load() finished successfully.
*/

/*!
    \fn void QMediaPlaylistProvider::loadFailed(QMediaPlaylist::Error error, const QString& errorMessage)

    Signals that a load failed() due to an \a error.  The \a errorMessage provides more information.
*/

#include "moc_qmediaplaylistprovider.cpp"

QT_END_NAMESPACE

