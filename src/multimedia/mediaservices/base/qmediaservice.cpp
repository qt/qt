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

#include <QtCore/qtimer.h>

#include <QtMediaServices/qmediaservice.h>
#include "qmediaservice_p.h"


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Multimedia)

/*!
    \class QMediaService
    \brief The QMediaService class provides a common base class for media
    service implementations.
    \ingroup multimedia-serv
    \preliminary
    \since 4.7

    Media services provide implementations of the functionality promised
    by media objects, and allow multiple providers to implement a QMediaObject.

    To provide the functionality of a QMediaObject media services implement
    QMediaControl interfaces.  Services typically implement one core media
    control which provides the core feature of a media object, and some
    number of additional controls which provide either optional features of
    the media object, or features of a secondary media object or peripheral
    object.

    A pointer to media service's QMediaControl implementation can be
    obtained by passing the control's interface name to the control() function.

    \code
    QMediaPlayerControl *control = qobject_cast<QMediaPlayerControl *>(
            service->control("com.nokia.Qt.QMediaPlayerControl/1.0"));
    \endcode

    Media objects can use services loaded dynamically from plug-ins or
    implemented statically within an applications.  Plug-in based services
    should also implement the QMediaServiceProviderPlugin interface.  Static
    services should implement the QMediaServiceProvider interface.

    \sa QMediaObject, QMediaControl, QMediaServiceProvider, QMediaServiceProviderPlugin
*/

/*!
    Construct a media service with the given \a parent. This class is meant as a
    base class for Multimedia services so this constructor is protected.
*/

QMediaService::QMediaService(QObject *parent)
    : QObject(parent)
    , d_ptr(new QMediaServicePrivate)
{
    d_ptr->q_ptr = this;
}

/*!
    \internal
*/
QMediaService::QMediaService(QMediaServicePrivate &dd, QObject *parent)
    : QObject(parent)
    , d_ptr(&dd)
{
    d_ptr->q_ptr = this;
}

/*!
    Destroys a media service.
*/

QMediaService::~QMediaService()
{
    delete d_ptr;
}

/*!
    \fn QMediaService::control(const char *interface) const

    Returns a pointer to the media control implementing \a interface.

    If the service does not implement the control a null pointer is returned instead.
*/

/*!
    \fn QMediaService::control() const

    Returns a pointer to the media control of type T implemented by a media service.

    If the service does not implment the control a null pointer is returned instead.
*/

#include "moc_qmediaservice.cpp"

QT_END_NAMESPACE

QT_END_HEADER

