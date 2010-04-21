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

#include <QtMediaServices/qmediaplaylistioplugin.h>

QT_BEGIN_NAMESPACE

/*!
    \class QMediaPlaylistReader
    \preliminary
    \since 4.7
    \brief The QMediaPlaylistReader class provides an interface for reading a playlist file.

    \sa QMediaPlaylistIOPlugin
*/

/*!
    Destroys a media playlist reader.
*/
QMediaPlaylistReader::~QMediaPlaylistReader()
{
}

/*!
    \fn QMediaPlaylistReader::atEnd() const

    Identifies if a playlist reader has reached the end of its input.

    Returns true if the reader has reached the end; and false otherwise.
*/

/*!
    \fn QMediaPlaylistReader::readItem()

    Reads an item of media from a playlist file.

    Returns the read media, or a null QMediaContent if no more media is available.
*/

/*!
    \fn QMediaPlaylistReader::close()

    Closes a playlist reader's input device.
*/

/*!
    \class QMediaPlaylistWriter
    \preliminary
    \since 4.7
    \brief The QMediaPlaylistWriter class provides an interface for writing a playlist file.

    \sa QMediaPlaylistIOPlugin
*/

/*!
    Destroys a media playlist writer.
*/
QMediaPlaylistWriter::~QMediaPlaylistWriter()
{
}

/*!
    \fn QMediaPlaylistWriter::writeItem(const QMediaContent &media)

    Writes an item of \a media to a playlist file.

    Returns true if the media was written succesfully; and false otherwise.
*/

/*!
    \fn QMediaPlaylistWriter::close()

    Finalizes the writing of a playlist and closes the output device.
*/

/*!
    \class QMediaPlaylistIOPlugin
    \since 4.7
    \brief The QMediaPlaylistIOPlugin class provides an interface for media playlist I/O plug-ins.
*/

/*!
    Constructs a media playlist I/O plug-in with the given \a parent.
*/
QMediaPlaylistIOPlugin::QMediaPlaylistIOPlugin(QObject *parent)
    :QObject(parent)
{
}

/*!
    Destroys a media playlist I/O plug-in.
*/
QMediaPlaylistIOPlugin::~QMediaPlaylistIOPlugin()
{
}

/*!
    \fn QMediaPlaylistIOPlugin::canRead(QIODevice *device, const QByteArray &format) const

    Identifies if plug-in can read \a format data from an I/O \a device.

    Returns true if the data can be read; and false otherwise.
*/

/*!
    \fn QMediaPlaylistIOPlugin::canRead(const QUrl& location, const QByteArray &format) const

    Identifies if a plug-in can read \a format data from a URL \a location.

    Returns true if the data can be read; and false otherwise.
*/

/*!
    \fn QMediaPlaylistIOPlugin::canWrite(QIODevice *device, const QByteArray &format) const

    Identifies if a plug-in can write \a format data to an I/O \a device.

    Returns true if the data can be written; and false otherwise.
*/

/*!
    \fn QMediaPlaylistIOPlugin::keys() const

    Returns a list of format keys supported by a plug-in.
*/

/*!
    \fn QMediaPlaylistIOPlugin::createReader(QIODevice *device, const QByteArray &format)

    Returns a new QMediaPlaylistReader which reads \a format data from an I/O \a device.

    If the device is invalid or the format is unsupported this will return a null pointer.
*/

/*!
    \fn QMediaPlaylistIOPlugin::createReader(const QUrl& location, const QByteArray &format)

    Returns a new QMediaPlaylistReader which reads \a format data from a URL \a location.

    If the location or the format is unsupported this will return a null pointer.
*/

/*!
    \fn QMediaPlaylistIOPlugin::createWriter(QIODevice *device, const QByteArray &format)

    Returns a new QMediaPlaylistWriter which writes \a format data to an I/O \a device.

    If the device is invalid or the format is unsupported this will return a null pointer.
*/

QT_END_NAMESPACE

#include "moc_qmediaplaylistioplugin.cpp"

