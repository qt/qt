/*  This file is part of the KDE project
    Copyright (C) 2005 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Nokia Corporation
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "audiodataoutput.h"
#include "audiodataoutput_p.h"
#include "factory_p.h"

#define PHONON_CLASSNAME AudioDataOutput

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{

PHONON_HEIR_IMPL(AbstractAudioOutput)

PHONON_GETTER(int, dataSize, d->dataSize)
PHONON_GETTER(int, sampleRate, -1)
PHONON_SETTER(setDataSize, dataSize, int)

bool AudioDataOutputPrivate::aboutToDeleteBackendObject()
{
    Q_ASSERT(m_backendObject);
    pBACKEND_GET(int, dataSize, "dataSize");

    return AbstractAudioOutputPrivate::aboutToDeleteBackendObject();
}

void AudioDataOutputPrivate::setupBackendObject()
{
    Q_Q(AudioDataOutput);
    Q_ASSERT(m_backendObject);
    AbstractAudioOutputPrivate::setupBackendObject();

    // set up attributes
    pBACKEND_CALL1("setDataSize", int, dataSize);

    qRegisterMetaType<QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > >("QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> >");

    QObject::connect(m_backendObject,
            SIGNAL(dataReady(const QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > &)),
            q, SIGNAL(dataReady(const QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > &)));
    QObject::connect(m_backendObject, SIGNAL(endOfMedia(int)), q, SIGNAL(endOfMedia(int)));
}

} // namespace Phonon

QT_END_NAMESPACE
QT_END_HEADER

#undef PHONON_CLASSNAME
// vim: sw=4 ts=4 tw=80
