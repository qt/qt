/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
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

#include <QtCore/qstring.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qdatastream.h>

#include <QtMultimedia/qaudiodeviceid.h>
#include "qaudiodeviceid_p.h"


QT_BEGIN_NAMESPACE

/*!
    \class QAudioDeviceId
    \brief The QAudioDeviceId class provides means for identifying a unique input or output device on a system.

    \inmodule QtMultimedia
    \ingroup multimedia
    \since 4.6

    \sa QAudioDeviceInfo, QAudioOutput, QAudioInput
*/

/*!
    Construct a new null QAudioDeviceId.
*/

QAudioDeviceId::QAudioDeviceId()
{
}

/*!
    Copy the QAudDeviceId referenced by \a other.
*/

QAudioDeviceId::QAudioDeviceId(const QAudioDeviceId &other):
    d(other.d)
{
}

/*!
    Destroy the QAudioDeviceId.
*/

QAudioDeviceId::~QAudioDeviceId()
{
}

/*!
    Make a copy of the \a other QAudioDeviceId.
*/

QAudioDeviceId& QAudioDeviceId::operator=(const QAudioDeviceId &other)
{
    d = other.d;
    return *this;
}

/*!
    Compare with the \a other QAudioDeviceId, return true if they are the same;
    otherwise false.
*/

bool QAudioDeviceId::operator==(const QAudioDeviceId &other) const
{
    return (d.constData() == 0 && other.d.constData() == 0) ||
            (d.constData() != 0 && other.d.constData() != 0 &&
             d->key == other.d->key && d->mode == other.d->mode && d->handle == other.d->handle);
}

/*!
    Compare with the \a other QAudioDeviceId, return false if they are the same;
    otherwise true.
*/

bool QAudioDeviceId::operator!=(const QAudioDeviceId &other) const
{
    return !(*this == other);
}

/*!
    Returns true if this is not a valid QAudioDeviceId; otherwise false.
*/

bool QAudioDeviceId::isNull() const
{
    return d.constData() == 0;
}

/*!
    \internal
*/

QAudioDeviceId::QAudioDeviceId(QAudioDeviceIdPrivate* data):
    d(data)
{
}

/*!
    \internal
*/

QAudioDeviceIdPrivate::QAudioDeviceIdPrivate(QString const& k, int m, QByteArray const& h):
    key(k), mode(m), handle(h)
{
}

#ifndef QT_NO_DATASTREAM
Q_MULTIMEDIA_EXPORT QDataStream &operator<<(QDataStream &s, const QAudioDeviceId &id)
{
    s << id.d->key << id.d->mode << id.d->handle;
    return s;
}

Q_MULTIMEDIA_EXPORT QDataStream &operator>>(QDataStream &s, QAudioDeviceId &id)
{
    QString key;
    int mode;
    QByteArray  handle;

    s >> key >> mode >> handle;
    id = QAudioDeviceId(new QAudioDeviceIdPrivate(key, mode, handle));

    return s;
}
#endif


QT_END_NAMESPACE
