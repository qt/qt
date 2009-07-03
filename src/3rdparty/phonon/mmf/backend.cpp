/*  This file is part of the KDE project.

Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or 3 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <QStringList>
#include <QtPlugin>

#include "backend.h"
#include "audiooutput.h"
#include "mediaobject.h"

using namespace Phonon;
using namespace Phonon::MMF;

Backend::Backend(QObject *parent)
{
    setParent(parent);

    setProperty("identifier",     QLatin1String("mmf"));
    setProperty("backendName",    QLatin1String("MMF"));
    setProperty("backendComment", QLatin1String("MMF Backend"));
    setProperty("backendVersion", QLatin1String("0.1"));
    setProperty("backendWebsite", QLatin1String("http://www.qtsoftware.com/"));
}

QObject *Backend::createObject(BackendInterface::Class c, QObject *parent, const QList<QVariant> &)
{
    switch(c)
    {
        case AudioOutputClass:
            return new AudioOutput(this, parent);
        case MediaObjectClass:
            return new MediaObject(parent);
        case VolumeFaderEffectClass:
        /* Fallthrough. */
        case VisualizationClass:
        /* Fallthrough. */
        case VideoDataOutputClass:
        /* Fallthrough. */
        case EffectClass:
        /* Fallthrough. */
        case VideoWidgetClass:
            return 0;
    }

    Q_ASSERT_X(false, Q_FUNC_INFO,
               "This line should never be reached.");
    return 0;
}

QList<int> Backend::objectDescriptionIndexes(ObjectDescriptionType) const
{
    return QList<int>();
}

QHash<QByteArray, QVariant> Backend::objectDescriptionProperties(ObjectDescriptionType, int) const
{
    return QHash<QByteArray, QVariant>();
}

bool Backend::startConnectionChange(QSet<QObject *>)
{
    return false;
}

bool Backend::connectNodes(QObject *source, QObject *target)
{
    MediaObject *const mo = qobject_cast<MediaObject *>(source);
    AudioOutput *const ao = qobject_cast<AudioOutput *>(target);

    if(!mo || !ao)
        return false;

    ao->setMediaObject(mo);

    return true;
}

bool Backend::disconnectNodes(QObject *, QObject *)
{
    return true;
}

bool Backend::endConnectionChange(QSet<QObject *>)
{
    return true;
}

QStringList Backend::availableMimeTypes() const
{
    return QStringList();
}

Q_EXPORT_PLUGIN2(phonon_mmf, Phonon::MMF::Backend);

