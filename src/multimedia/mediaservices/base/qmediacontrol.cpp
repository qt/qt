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

#include <QtCore/qmetaobject.h>
#include <QtCore/qtimer.h>

#include <QtMediaServices//qmediacontrol.h>
#include <QtMediaServices/private/qmediacontrol_p.h>



QT_BEGIN_NAMESPACE

/*!
    \class QMediaControl
    \ingroup multimedia-serv
    \since 4.7

    \preliminary
    \brief The QMediaControl class provides a base interface for media service controls.

    Media controls provide an interface to individual features provided by a media service.  Most
    services implement a principal control which exposes the core functionality of the service and
    a number optional controls which expose any additional functionality.

    A pointer to a control implemented by a media service can be obtained using the
    \l {QMediaService::control()}{control()} member of QMediaService.  If the service doesn't
    implement a control it will instead return a null pointer.

    \code
    QMediaPlayerControl *control = qobject_cast<QMediaPlayerControl *>(
            service->control("com.nokia.Qt.QMediaPlayerControl/1.0"));
    \endcode

    Alternatively if the IId of the control has been declared using Q_MEDIA_DECLARE_CONTROL
    the template version of QMediaService::control() can be used to request the service without
    explicitly passing the IId.

    \code
    QMediaPlayerControl *control = service->control<QMediaPlayerControl *>();
    \endcode

    Most application code will not interface directly with a media service's controls, instead the
    QMediaObject which owns the service acts as an intermeditary between one or more controls and
    the application.

    \sa QMediaService, QMediaObject
*/

/*!
    \macro Q_MEDIA_DECLARE_CONTROL(Class, IId)
    \relates QMediaControl

    The Q_MEDIA_DECLARE_CONTROL macro declares an \a IId for a \a Class that inherits from
    QMediaControl.

    Declaring an IId for a QMediaControl allows an instance of that control to be requested from
    QMediaService::control() without explicitly passing the IId.

    \code
    QMediaPlayerControl *control = service->control<QMediaPlayerControl *>();
    \endcode

    \sa QMediaService::control()
*/

/*!
    Destroys a media control.
*/

QMediaControl::~QMediaControl()
{
    delete d_ptr;
}

/*!
    Constructs a media control with the given \a parent.
*/

QMediaControl::QMediaControl(QObject *parent)
    : QObject(parent)
    , d_ptr(new QMediaControlPrivate)
{
    d_ptr->q_ptr = this;
}

/*!
    \internal
*/

QMediaControl::QMediaControl(QMediaControlPrivate &dd, QObject *parent)
    : QObject(parent)
    , d_ptr(&dd)

{
    d_ptr->q_ptr = this;
}

#include "moc_qmediacontrol.cpp"

QT_END_NAMESPACE

