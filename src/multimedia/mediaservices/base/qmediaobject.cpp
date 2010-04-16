/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMediaservics module of the Qt Toolkit.
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

#include "qmediaobject_p.h"

#include <QtMediaServices/qmediaservice.h>
#include <QtMediaServices/qmetadatacontrol.h>


QT_BEGIN_NAMESPACE

void QMediaObjectPrivate::_q_notify()
{
    Q_Q(QMediaObject);

    const QMetaObject* m = q->metaObject();

    foreach (int pi, notifyProperties) {
        QMetaProperty p = m->property(pi);
        p.notifySignal().invoke(
            q, QGenericArgument(QMetaType::typeName(p.userType()), p.read(q).data()));
    }
}


/*!
    \class QMediaObject
    \preliminary
    \brief The QMediaObject class provides a common base for multimedia objects.
    \since 4.7

    \ingroup multimedia

    QMediaObject derived classes provide access to the functionality of a
    QMediaService.  Each media object hosts a QMediaService and uses the
    QMediaControl interfaces implemented by the service to implement its
    API.  Most media objects when constructed will request a new
    QMediaService instance from a QMediaServiceProvider, but some like
    QMediaRecorder will share a service with another object.

    QMediaObject itself provides an API for accessing a media service's \l {metaData()}{meta-data} and a means of connecting other media objects,
    and peripheral classes like QVideoWidget and QMediaPlaylist.

    \sa QMediaService, QMediaControl
*/

/*!
    Destroys a media object.
*/

QMediaObject::~QMediaObject()
{
    delete d_ptr;
}

/*!
    Returns the service availability error state.
*/

QtMediaServices::AvailabilityError QMediaObject::availabilityError() const
{
    return QtMediaServices::ServiceMissingError;
}

/*!
    Returns true if the service is available for use.
*/

bool QMediaObject::isAvailable() const
{
    return false;
}

/*!
    Returns the media service that provides the functionality of a multimedia object.
*/

QMediaService* QMediaObject::service() const
{
    return d_func()->service;
}

int QMediaObject::notifyInterval() const
{
    return d_func()->notifyTimer->interval();
}

void QMediaObject::setNotifyInterval(int milliSeconds)
{
    Q_D(QMediaObject);

    if (d->notifyTimer->interval() != milliSeconds) {
        d->notifyTimer->setInterval(milliSeconds);

        emit notifyIntervalChanged(milliSeconds);
    }
}

/*!
  \internal
*/
void QMediaObject::bind(QObject*)
{
}

/*!
  \internal
*/
void QMediaObject::unbind(QObject*)
{
}


/*!
    Constructs a media object which uses the functionality provided by a media \a service.

    The \a parent is passed to QObject.

    This class is meant as a base class for Multimedia objects so this
    constructor is protected.
*/

QMediaObject::QMediaObject(QObject *parent, QMediaService *service):
    QObject(parent),
    d_ptr(new QMediaObjectPrivate)

{
    Q_D(QMediaObject);

    d->q_ptr = this;

    d->notifyTimer = new QTimer(this);
    d->notifyTimer->setInterval(1000);
    connect(d->notifyTimer, SIGNAL(timeout()), SLOT(_q_notify()));

    d->service = service;

    setupMetaData();
}

/*!
    \internal
*/

QMediaObject::QMediaObject(QMediaObjectPrivate &dd, QObject *parent,
                                            QMediaService *service):
    QObject(parent),
    d_ptr(&dd)
{
    Q_D(QMediaObject);
    d->q_ptr = this;

    d->notifyTimer = new QTimer(this);
    d->notifyTimer->setInterval(1000);
    connect(d->notifyTimer, SIGNAL(timeout()), SLOT(_q_notify()));

    d->service = service;

    setupMetaData();
}

/*!
    Watch the property \a name. The property's notify signal will be emitted
    once every notifyInterval milliseconds.

    \sa notifyInterval
*/

void QMediaObject::addPropertyWatch(QByteArray const &name)
{
    Q_D(QMediaObject);

    const QMetaObject* m = metaObject();

    int index = m->indexOfProperty(name.constData());

    if (index != -1 && m->property(index).hasNotifySignal()) {
        d->notifyProperties.insert(index);

        if (!d->notifyTimer->isActive())
            d->notifyTimer->start();
    }
}

/*!
    Remove property \a name from the list of properties whose changes are
    regularly signaled.

    \sa notifyInterval
*/

void QMediaObject::removePropertyWatch(QByteArray const &name)
{
    Q_D(QMediaObject);

    int index = metaObject()->indexOfProperty(name.constData());

    if (index != -1) {
        d->notifyProperties.remove(index);

        if (d->notifyProperties.isEmpty())
            d->notifyTimer->stop();
    }
}

/*!
    \property QMediaObject::notifyInterval

    The interval at which notifiable properties will update.

    The interval is expressed in milliseconds, the default value is 1000.

    \sa addPropertyWatch(), removePropertyWatch()
*/

/*!
    \fn void QMediaObject::notifyIntervalChanged(int milliseconds)

    Signal a change in the notify interval period to \a milliseconds.
*/

/*!
    \property QMediaObject::metaDataAvailable
    \brief whether access to a media object's meta-data is available.

    If this is true there is meta-data available, otherwise there is no meta-data available.
*/

bool QMediaObject::isMetaDataAvailable() const
{
    Q_D(const QMediaObject);

    return d->metaDataControl
            ? d->metaDataControl->isMetaDataAvailable()
            : false;
}

/*!
    \fn QMediaObject::metaDataAvailableChanged(bool available)

    Signals that the \a available state of a media object's meta-data has changed.
*/

/*!
    \property QMediaObject::metaDataWritable
    \brief whether a media object's meta-data is writable.

    If this is true the meta-data is writable, otherwise the meta-data is read-only.
*/

bool QMediaObject::isMetaDataWritable() const
{
    Q_D(const QMediaObject);

    return d->metaDataControl
            ? d->metaDataControl->isWritable()
            : false;
}

/*!
    \fn QMediaObject::metaDataWritableChanged(bool writable)

    Signals that the \a writable state of a media object's meta-data has changed.
*/

/*!
    Returns the value associated with a meta-data \a key.
*/
QVariant QMediaObject::metaData(QtMediaServices::MetaData key) const
{
    Q_D(const QMediaObject);

    return d->metaDataControl
            ? d->metaDataControl->metaData(key)
            : QVariant();
}

/*!
    Sets a \a value for a meta-data \a key.
*/
void QMediaObject::setMetaData(QtMediaServices::MetaData key, const QVariant &value)
{
    Q_D(QMediaObject);

    if (d->metaDataControl)
        d->metaDataControl->setMetaData(key, value);
}

/*!
    Returns a list of keys there is meta-data available for.
*/
QList<QtMediaServices::MetaData> QMediaObject::availableMetaData() const
{
    Q_D(const QMediaObject);

    return d->metaDataControl
            ? d->metaDataControl->availableMetaData()
            : QList<QtMediaServices::MetaData>();
}

/*!
    \fn QMediaObject::metaDataChanged()

    Signals that a media object's meta-data has changed.
*/

/*!
    Returns the value associated with a meta-data \a key.

    The naming and type of extended meta-data is not standardized, so the values and meaning
    of keys may vary between backends.
*/
QVariant QMediaObject::extendedMetaData(const QString &key) const
{
    Q_D(const QMediaObject);

    return d->metaDataControl
            ? d->metaDataControl->extendedMetaData(key)
            : QVariant();
}

/*!
    Sets a \a value for a meta-data \a key.

    The naming and type of extended meta-data is not standardized, so the values and meaning
    of keys may vary between backends.
*/
void QMediaObject::setExtendedMetaData(const QString &key, const QVariant &value)
{
    Q_D(QMediaObject);

    if (d->metaDataControl)
        d->metaDataControl->setExtendedMetaData(key, value);
}

/*!
    Returns a list of keys there is extended meta-data available for.
*/
QStringList QMediaObject::availableExtendedMetaData() const
{
    Q_D(const QMediaObject);

    return d->metaDataControl
            ? d->metaDataControl->availableExtendedMetaData()
            : QStringList();
}


void QMediaObject::setupMetaData()
{
    Q_D(QMediaObject);

    if (d->service != 0) {
        d->metaDataControl =
            qobject_cast<QMetaDataControl*>(d->service->control(QMetaDataControl_iid));

        if (d->metaDataControl) {
            connect(d->metaDataControl, SIGNAL(metaDataChanged()), SIGNAL(metaDataChanged()));
            connect(d->metaDataControl,
                    SIGNAL(metaDataAvailableChanged(bool)),
                    SIGNAL(metaDataAvailableChanged(bool)));
            connect(d->metaDataControl,
                    SIGNAL(writableChanged(bool)),
                    SIGNAL(metaDataWritableChanged(bool)));
        }
    }
}

/*!
    \fn QMediaObject::availabilityChanged(bool available)

    Signal emitted when the availability state has changed to \a available
*/


#include "moc_qmediaobject.cpp"

QT_END_NAMESPACE

