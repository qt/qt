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

#include <QtMediaServices/qmetadatacontrol.h>
#include "qmediacontrol_p.h"


QT_BEGIN_NAMESPACE


/*!
    \class QMetaDataControl
    \ingroup multimedia-serv
    \since 4.7
    \preliminary
    \brief The QMetaDataControl class provides access to the meta-data of a
    QMediaService's media.

    If a QMediaService can provide read or write access to the meta-data of
    its current media it will implement QMetaDataControl.  This control
    provides functions for both retrieving and setting meta-data values.
    Meta-data may be addressed by the well defined keys in the
    QtMediaServices::MetaData enumeration using the metaData() functions, or by
    string keys using the extendedMetaData() functions.

    The functionality provided by this control is exposed to application
    code by the meta-data members of QMediaObject, and so meta-data access
    is potentially available in any of the media object classes.  Any media
    service may implement QMetaDataControl.

    The interface name of QMetaDataControl is \c com.nokia.Qt.QMetaDataControl/1.0 as
    defined in QMetaDataControl_iid.

    \sa QMediaService::control(), QMediaObject
*/

/*!
    \macro QMetaDataControl_iid

    \c com.nokia.Qt.QMetaDataControl/1.0

    Defines the interface name of the QMetaDataControl class.

    \relates QMetaDataControl
*/

/*!
    Construct a QMetaDataControl with \a parent. This class is meant as a base class
    for service specific meta data providers so this constructor is protected.
*/

QMetaDataControl::QMetaDataControl(QObject *parent):
    QMediaControl(*new QMediaControlPrivate, parent)
{
}

/*!
    Destroy the meta-data object.
*/

QMetaDataControl::~QMetaDataControl()
{
}

/*!
    \fn bool QMetaDataControl::isMetaDataAvailable() const

    Identifies if meta-data is available from a media service.

    Returns true if the meta-data is available and false otherwise.
*/

/*!
    \fn bool QMetaDataControl::isWritable() const

    Identifies if a media service's meta-data can be edited.

    Returns true if the meta-data is writable and false otherwise.
*/

/*!
    \fn QVariant QMetaDataControl::metaData(QtMediaServices::MetaData key) const

    Returns the meta-data for the given \a key.
*/

/*!
    \fn void QMetaDataControl::setMetaData(QtMediaServices::MetaData key, const QVariant &value)

    Sets the \a value of the meta-data element with the given \a key.
*/

/*!
    \fn QMetaDataControl::availableMetaData() const

    Returns a list of keys there is meta-data available for.
*/

/*!
    \fn QMetaDataControl::extendedMetaData(const QString &key) const

    Returns the metaData for an abitrary string \a key.

    The valid selection of keys for extended meta-data is determined by the provider and the meaning
    and type may differ between providers.
*/

/*!
    \fn QMetaDataControl::setExtendedMetaData(const QString &key, const QVariant &value)

    Change the value of the meta-data element with an abitrary string \a key to \a value.

    The valid selection of keys for extended meta-data is determined by the provider and the meaning
    and type may differ between providers.
*/

/*!
    \fn QMetaDataControl::availableExtendedMetaData() const

    Returns a list of keys there is extended meta-data available for.
*/


/*!
    \fn void QMetaDataControl::metaDataChanged()

    Signal the changes of meta-data.
*/

/*!
    \fn void QMetaDataControl::metaDataAvailableChanged(bool available)

    Signal the availability of meta-data has changed, \a available will
    be true if the multimedia object has meta-data.
*/

/*!
    \fn void QMetaDataControl::writableChanged(bool writable)

    Signal a change in the writable status of meta-data, \a writable will be
    true if meta-data elements can be added or adjusted.
*/

#include "moc_qmetadatacontrol.cpp"

QT_END_NAMESPACE

