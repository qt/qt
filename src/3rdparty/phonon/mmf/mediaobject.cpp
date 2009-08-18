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

#include "audioplayer.h"
#include "mediaobject.h"
#include "utils.h"
#include "videoplayer.h"


using namespace Phonon;
using namespace Phonon::MMF;

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::MediaObject::MediaObject(QObject *parent) : QObject::QObject(parent)
{
    TRACE_CONTEXT(MediaObject::MediaObject, EAudioApi);
    TRACE_ENTRY_0();

    Q_UNUSED(parent);

    TInt err = m_recognizer.Connect();
    err = m_fileServer.Connect();
    // TODO: handle this error
    
    // This must be called in order to be able to share file handles with
    // the recognizer server (see fileMediaType function).
    err = m_fileServer.ShareProtected();
    // TODO: handle this error

    m_tickTimer = new QTimer(this);
    connect(m_tickTimer, SIGNAL(timeout()), this, SLOT(tick()));

    TRACE_EXIT_0();
}

MMF::MediaObject::~MediaObject()
{
    TRACE_CONTEXT(MediaObject::~MediaObject, EAudioApi);
    TRACE_ENTRY_0();

    delete m_tickTimer;

    m_file.Close();
    m_fileServer.Close();
    m_recognizer.Close();

    TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// Recognizer
//-----------------------------------------------------------------------------

const TInt KMimePrefixLength = 6; // either "audio/" or "video/"
_LIT(KMimePrefixAudio, "audio/");
_LIT(KMimePrefixVideo, "video/");

MMF::MediaObject::MediaType MMF::MediaObject::mimeTypeToMediaType(const TDesC& mimeType)
{
	MediaType result = MediaTypeUnknown;

	if(mimeType.Left(KMimePrefixLength).Compare(KMimePrefixAudio) == 0)
		{
			result = MediaTypeAudio;
		}
		else if(mimeType.Left(KMimePrefixLength).Compare(KMimePrefixVideo) == 0)
		{
			result = MediaTypeVideo;
		}

	return result;
}


MMF::MediaObject::MediaType MMF::MediaObject::fileMediaType
	(const QString& fileName)
{
	MediaType result = MediaTypeUnknown;
	
	QHBufC fileNameSymbian = Utils::symbianFilename(fileName);
	
	m_file.Close();
	TInt err = m_file.Open(m_fileServer, *fileNameSymbian, EFileRead|EFileShareReadersOnly);

	if(KErrNone == err)
	{
		TDataRecognitionResult recognizerResult;
		err = m_recognizer.RecognizeData(m_file, recognizerResult);
		if(KErrNone == err)
		{
			const TPtrC mimeType = recognizerResult.iDataType.Des();
			result = mimeTypeToMediaType(mimeType);
		}
	}
	return result;
}


//-----------------------------------------------------------------------------
// MediaObjectInterface
//-----------------------------------------------------------------------------

void MMF::MediaObject::play()
{
	if(!m_player.isNull())
	{
		m_player->play();
	}
}

void MMF::MediaObject::pause()
{
	if(!m_player.isNull())
	{
		m_player->pause();
	}
}

void MMF::MediaObject::stop()
{
	if(!m_player.isNull())
	{
		m_player->stop();
	}
}

void MMF::MediaObject::seek(qint64 ms)
{
	if(!m_player.isNull())
	{
		m_player->seek(ms);
	}   
}

qint32 MMF::MediaObject::tickInterval() const
{
	qint32 result = 0;
	if(!m_player.isNull())
	{
		result = m_player->tickInterval();
	}
	return result;
}

void MMF::MediaObject::setTickInterval(qint32 interval)
{
	if(!m_player.isNull())
	{
		m_player->setTickInterval(interval);
	}
}

bool MMF::MediaObject::hasVideo() const
{
	bool result = false;
	if(!m_player.isNull())
	{
		result = m_player->hasVideo();
	}
	return result;
}

bool MMF::MediaObject::isSeekable() const
{
	bool result = false;
	if(!m_player.isNull())
	{
		result = m_player->isSeekable();
	}
	return result;
}

Phonon::State MMF::MediaObject::state() const
{
	Phonon::State result = Phonon::StoppedState;
	if(!m_player.isNull())
	{
		result = m_player->state();
	}
	return result;
}

qint64 MMF::MediaObject::currentTime() const
{
	qint64 result = 0;
	if(!m_player.isNull())
	{
		result = m_player->currentTime();
	}
	return result;
}

QString MMF::MediaObject::errorString() const
{
	QString result;
	if(!m_player.isNull())
	{
		result = m_player->errorString();
	}
	return result;
}

Phonon::ErrorType MMF::MediaObject::errorType() const
{
	Phonon::ErrorType result = Phonon::NoError;
	if(!m_player.isNull())
	{
		result = m_player->errorType();
	}
	return result;
}

qint64 MMF::MediaObject::totalTime() const
{
	qint64 result = 0;
	if(!m_player.isNull())
	{
		result = m_player->totalTime();
	}
	return result;
}

MediaSource MMF::MediaObject::source() const
{
	MediaSource result;
	if(!m_player.isNull())
	{
		result = m_player->source();
	}
	return result;
}

void MMF::MediaObject::setSource(const MediaSource &source)
{
    loadPlayer(source);
    if(!m_player.isNull())
    {
		//m_player->setSource(source);
    
		// This is a hack to work around KErrInUse from MMF client utility
		// OpenFileL calls
		m_player->setFileSource(source, m_file);
    }
}

void MMF::MediaObject::loadPlayer(const MediaSource &source)
{
	TRACE_CONTEXT(AudioPlayer::loadPlayer, EAudioApi);
    //TRACE_ENTRY("state %d source.type %d", m_state, source.type());
	// TODO: log state
	TRACE_ENTRY("source.type %d", source.type());
	
    // Destroy old player object
	if(!m_player.isNull())
	{
		disconnect(m_player.data(), 0, this, 0);
		m_player.reset(NULL);
	}
	
	MediaType mediaType = MediaTypeUnknown;
	
	// Determine media type
	switch(source.type())
	{
		case MediaSource::LocalFile:
			mediaType = fileMediaType(source.fileName());
			break;
			
		case MediaSource::Url:
			// TODO: support detection of media type from HTTP streams
			TRACE_0("Network streaming not supported yet");
			
		/*
		 * TODO: handle error
		 * 
			m_error = NormalError;
			changeState(ErrorState);
			break;
		 */
			
		case MediaSource::Invalid:
		case MediaSource::Disc:
		case MediaSource::Stream:
			TRACE_0("Unsupported media type");
		/*
		 * TODO: handle error
		 * 
			m_error = NormalError;
			changeState(ErrorState);
		 */
			break;
			
		case MediaSource::Empty:
			TRACE_EXIT_0();
			return;
	}
	
	switch(mediaType)
	{
		case MediaTypeUnknown:
			TRACE_0("Media type could not be determined");
		/*
		 * TODO: handle error
		 * 
			m_error = NormalError;
			changeState(ErrorState);
		 */
			break;
			
		case MediaTypeAudio:
			m_player.reset(new AudioPlayer());
			break;
			
		case MediaTypeVideo:
			m_player.reset(new VideoPlayer());
			break;
	}
		
    if(!m_player.isNull())
    {
		connect(m_player.data(), SIGNAL(totalTimeChanged()), SIGNAL(totalTimeChanged()));
		connect(m_player.data(), SIGNAL(stateChanged(Phonon::State, Phonon::State)), SIGNAL(stateChanged(Phonon::State, Phonon::State)));
		connect(m_player.data(), SIGNAL(finished()), SIGNAL(finished()));
		connect(m_player.data(), SIGNAL(tick(qint64)), SIGNAL(tick(qint64)));
    }
}

void MMF::MediaObject::setNextSource(const MediaSource &source)
{
	if(!m_player.isNull())
	{
		m_player->setNextSource(source);
	}
}

qint32 MMF::MediaObject::prefinishMark() const
{
	qint32 result = 0;
	if(!m_player.isNull())
	{
		result = m_player->prefinishMark();
	}
	return result;
}

void MMF::MediaObject::setPrefinishMark(qint32 mark)
{
	if(!m_player.isNull())
	{
		m_player->setPrefinishMark(mark);
	}
}

qint32 MMF::MediaObject::transitionTime() const
{
	qint32 result = 0;
	if(!m_player.isNull())
	{
		result = m_player->transitionTime();
	}
	return result;
}

void MMF::MediaObject::setTransitionTime(qint32 time)
{
	if(!m_player.isNull())
	{
		m_player->setTransitionTime(time);
	}
}

//-----------------------------------------------------------------------------
// Volume
//-----------------------------------------------------------------------------

qreal MMF::MediaObject::volume() const
{
	qreal result = 0.0;
	if(!m_player.isNull())
	{
		m_player->volume();
	}
	return result;
}

bool MMF::MediaObject::setVolume(qreal volume)
{
	bool result = false;
	if(!m_player.isNull())
	{
		result = m_player->setVolume(volume);
	}
	return result;
}

void MMF::MediaObject::setAudioOutput(AudioOutput* audioOutput)
{
	if(!m_player.isNull())
	{
		m_player->setAudioOutput(audioOutput);
	}
}

