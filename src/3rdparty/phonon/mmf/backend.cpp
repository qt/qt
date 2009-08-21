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
#include "utils.h"
#include "videowidget.h"

using namespace Phonon;
using namespace Phonon::MMF;

Backend::Backend(QObject *parent)	: QObject(parent)
{
	TRACE_CONTEXT(Backend::Backend, EBackend);
	TRACE_ENTRY_0();

    setProperty("identifier",     QLatin1String("phonon_mmf"));
    setProperty("backendName",    QLatin1String("MMF"));
    setProperty("backendComment", QLatin1String("Backend using Symbian Multimedia Framework (MMF)"));
    setProperty("backendVersion", QLatin1String("0.1"));
    setProperty("backendWebsite", QLatin1String("http://www.qtsoftware.com/"));
    
    TRACE_EXIT_0();
}

QObject *Backend::createObject(BackendInterface::Class c, QObject *parent, const QList<QVariant> &)
{
	TRACE_CONTEXT(Backend::createObject, EBackend);
	TRACE_ENTRY("class %d", c);

	QObject* result = NULL;

    switch(c)
    {
        case AudioOutputClass:
        	result = new AudioOutput(this, parent);
        	break;
        	
        case MediaObjectClass:
        	result =  new MediaObject(parent);
        	break;
        	
        case VolumeFaderEffectClass:
        case VisualizationClass:
        case VideoDataOutputClass:
        case EffectClass:
        	break;
        	
        case VideoWidgetClass:
        	result = new VideoWidget(qobject_cast<QWidget *>(parent));
        	break;
        	
        default:
        	TRACE_PANIC(InvalidBackendInterfaceClass);
    }

    TRACE_RETURN("0x%08x", result);
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
    return true;
}

bool Backend::connectNodes(QObject *source, QObject *target)
{
	TRACE_CONTEXT(Backend::connectNodes, EBackend);
	TRACE_ENTRY("source 0x%08x target 0x%08x", source, target);

	MediaObject *const mediaObject = qobject_cast<MediaObject *>(source);
    AudioOutput *const audioOutput = qobject_cast<AudioOutput *>(target);
    VideoWidget *const videoWidget = qobject_cast<VideoWidget *>(target);
    
    bool result = false;
    
    if(mediaObject and audioOutput)
    {
		TRACE("mediaObject 0x%08x -> audioOutput 0x%08x", mediaObject, audioOutput);
		audioOutput->setVolumeObserver(mediaObject);
		result = true;
    }
    
    if(mediaObject and videoWidget)
	{
		TRACE("mediaObject 0x%08x -> videoWidget 0x%08x", mediaObject, videoWidget);
		mediaObject->setVideoOutput(&videoWidget->videoOutput());
		result = true;
	}
    
    TRACE_RETURN("%d", result);
}

bool Backend::disconnectNodes(QObject *source, QObject *target)
{
	TRACE_CONTEXT(Backend::disconnectNodes, EBackend);
	TRACE_ENTRY("source 0x%08x target 0x%08x", source, target);

	MediaObject *const mediaObject = qobject_cast<MediaObject *>(source);
	AudioOutput *const audioOutput = qobject_cast<AudioOutput *>(target);
	VideoWidget *const videoWidget = qobject_cast<VideoWidget *>(target);
	
	bool result = true;
	
	// TODO: disconnection
	
	TRACE_RETURN("%d", result);
}

bool Backend::endConnectionChange(QSet<QObject *>)
{
    return true;
}

QStringList Backend::availableMimeTypes() const
{
	// TODO: query MMF for available MIME types

    return QStringList();
}

Q_EXPORT_PLUGIN2(phonon_mmf, Phonon::MMF::Backend);

