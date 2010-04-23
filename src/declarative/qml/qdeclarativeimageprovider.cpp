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

#include "qdeclarativeimageprovider.h"

QT_BEGIN_NAMESPACE

/*!
    \class QDeclarativeImageProvider
    \since 4.7
    \brief The QDeclarativeImageProvider class provides an interface for threaded image requests in QML.

    QDeclarativeImageProvider can be used by a QDeclarativeEngine to provide images to QML asynchronously.
    The image request will be run in a low priority thread, allowing potentially costly image
    loading to be done in the background, without affecting the performance of the UI.

    See the QDeclarativeEngine::addImageProvider() documentation for an
    example of how a custom QDeclarativeImageProvider can be constructed and used.

    \note the request() method may be called by multiple threads, so ensure the
    implementation of this method is reentrant.

    \sa QDeclarativeEngine::addImageProvider()
*/

/*!
  Destroys the image provider.
 */
QDeclarativeImageProvider::~QDeclarativeImageProvider()
{
}

/*!
    \fn QImage QDeclarativeImageProvider::request(const QString &id, QSize *size, const QSize& requestedSize)

    Implement this method to return the image with \a id.

    If \a requestedSize is a valid size, the image returned should be of that size.

    In all cases, \a size must be set to the original size of the image.

    \note this method may be called by multiple threads, so ensure the
    implementation of this method is reentrant.
*/

QT_END_NAMESPACE
