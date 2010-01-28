/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#import <QTKit/QTDataReference.h>
#import <QTKit/QTMovie.h>

#include "qt7backend.h"

#include "qt7playersession.h"
#include "qt7playercontrol.h"
#include "qt7videooutputcontrol.h"

#include <QtMultimedia/qmediaplaylistnavigator.h>

#include <CoreFoundation/CoreFoundation.h>

#include <QtCore/qurl.h>
#include <QtCore/qdebug.h>


QT_BEGIN_NAMESPACE

@interface QTMovieObserver : NSObject
{
@private
    QT7PlayerSession *m_session;
    QTMovie *m_movie;
}

- (QTMovieObserver *) initWithPlayerSession:(QT7PlayerSession*)session;
- (void) setMovie:(QTMovie *)movie;
- (void) processEOS:(NSNotification *)notification;
@end

@implementation QTMovieObserver

- (QTMovieObserver *) initWithPlayerSession:(QT7PlayerSession*)session
{
    if (!(self = [super init]))
        return nil;

    self->m_session = session;
    return self;
}

- (void) setMovie:(QTMovie *)movie
{
    if (m_movie == movie)
        return;

    if (m_movie) {
        [[NSNotificationCenter defaultCenter] removeObserver:self];
        [m_movie release];
    }

    m_movie = movie;

    if (movie) {
        [[NSNotificationCenter defaultCenter] addObserver: self selector:
            @selector(processEOS:) name: QTMovieDidEndNotification object: m_movie];

        [movie retain];
    }
}

- (void) processEOS:(NSNotification *)notification
{
    Q_UNUSED(notification);
    m_session->processEOS();
}

@end

static CFStringRef qString2CFStringRef(const QString &string)
{
    return CFStringCreateWithCharacters(0, reinterpret_cast<const UniChar *>(string.unicode()),
                                        string.length());
}

QT7PlayerSession::QT7PlayerSession(QObject *parent)
   : QObject(parent)
   , m_QTMovie(0)
   , m_state(QMediaPlayer::StoppedState)
   , m_mediaStatus(QMediaPlayer::NoMedia)
   , m_mediaStream(0)
   , m_videoOutput(0)
   , m_muted(false)
   , m_volume(100)
   , m_rate(1.0)
{
    m_movieObserver = [[QTMovieObserver alloc] initWithPlayerSession:this];
}

QT7PlayerSession::~QT7PlayerSession()
{
    [(QTMovieObserver*)m_movieObserver setMovie:nil];
    [(QTMovieObserver*)m_movieObserver release];
}

void *QT7PlayerSession::movie() const
{
    return m_QTMovie;
}

void QT7PlayerSession::setVideoOutput(QT7VideoOutput *output)
{
    if (m_videoOutput == output)
        return;

    qDebug() << "set output" << output;

    if (m_videoOutput) {
        m_videoOutput->setEnabled(false);
        m_videoOutput->setMovie(0);
    }

    m_videoOutput = output;

    if (m_videoOutput) {
        m_videoOutput->setEnabled(m_QTMovie != 0);
        m_videoOutput->setMovie(m_QTMovie);
    }
}


qint64 QT7PlayerSession::position() const
{
    if (!m_QTMovie || m_state == QMediaPlayer::PausedState)
        return m_currentTime;

    AutoReleasePool pool;

    QTTime qtTime = [(QTMovie*)m_QTMovie currentTime];
    quint64 t = static_cast<quint64>(float(qtTime.timeValue) / float(qtTime.timeScale) * 1000.0f);
    m_currentTime = t;

    return m_currentTime;
}

qint64 QT7PlayerSession::duration() const
{
    if (!m_QTMovie)
        return 0;

    AutoReleasePool pool;

    QTTime qtTime = [(QTMovie*)m_QTMovie duration];

    return static_cast<quint64>(float(qtTime.timeValue) / float(qtTime.timeScale) * 1000.0f);
}

QMediaPlayer::State QT7PlayerSession::state() const
{
    return m_state;
}

QMediaPlayer::MediaStatus QT7PlayerSession::mediaStatus() const
{
    return m_mediaStatus;
}

int QT7PlayerSession::bufferStatus() const
{
    return 100;
}

int QT7PlayerSession::volume() const
{
    return m_volume;
}

bool QT7PlayerSession::isMuted() const
{
    return m_muted;
}

bool QT7PlayerSession::isSeekable() const
{
    return true;
}

qreal QT7PlayerSession::playbackRate() const
{
    return m_rate;
}

void QT7PlayerSession::setPlaybackRate(qreal rate)
{
    if (qFuzzyCompare(m_rate, rate))
        return;

    m_rate = rate;

    if (m_QTMovie && m_state == QMediaPlayer::PlayingState) {        
        float preferredRate = [[(QTMovie*)m_QTMovie attributeForKey:@"QTMoviePreferredRateAttribute"] floatValue];
        [(QTMovie*)m_QTMovie setRate:preferredRate*m_rate];
    }
}

void QT7PlayerSession::setPosition(qint64 pos)
{
    if ( !isSeekable() || pos == position())
        return;

    AutoReleasePool pool;

    pos = qMin(pos, duration());

    QTTime newQTTime = [(QTMovie*)m_QTMovie currentTime];
    newQTTime.timeValue = (pos / 1000.0f) * newQTTime.timeScale;
    [(QTMovie*)m_QTMovie setCurrentTime:newQTTime];
}

void QT7PlayerSession::play()
{
    m_state = QMediaPlayer::PlayingState;

    float preferredRate = [[(QTMovie*)m_QTMovie attributeForKey:@"QTMoviePreferredRateAttribute"] floatValue];
    [(QTMovie*)m_QTMovie setRate:preferredRate*m_rate];

    emit stateChanged(m_state);
}

void QT7PlayerSession::pause()
{
    m_state = QMediaPlayer::PausedState;

    [(QTMovie*)m_QTMovie setRate:0];

    emit stateChanged(m_state);
}

void QT7PlayerSession::stop()
{
    m_state = QMediaPlayer::StoppedState;

    [(QTMovie*)m_QTMovie setRate:0];
    setPosition(0);

    if (m_state == QMediaPlayer::StoppedState)
        emit stateChanged(m_state);
}

void QT7PlayerSession::setVolume(int volume)
{
    if (m_QTMovie) {
        m_volume = volume;
        [(QTMovie*)m_QTMovie setVolume:(volume/100.0f)];
    }
}

void QT7PlayerSession::setMuted(bool muted)
{
    if (m_muted != muted) {
        m_muted = muted;

        if (m_QTMovie)
            [(QTMovie*)m_QTMovie setMuted:m_muted];

        emit mutedChanged(muted);
    }
}

QMediaContent QT7PlayerSession::media() const
{
    return m_resources;
}

const QIODevice *QT7PlayerSession::mediaStream() const
{
    return m_mediaStream;
}

void QT7PlayerSession::setMedia(const QMediaContent &content, QIODevice *stream)
{
    AutoReleasePool pool;

    if (m_QTMovie) {
        [(QTMovieObserver*)m_movieObserver setMovie:nil];

        if (m_videoOutput) {
            m_videoOutput->setEnabled(false);
            m_videoOutput->setMovie(0);
        }

        [(QTMovie*)m_QTMovie release];
        m_QTMovie = 0;
    }

    m_resources = content;
    m_mediaStream = stream;

    QUrl url;

    if (!content.isNull())
        url = content.canonicalUrl();
    else
        return;

    NSError *err = 0;
    QTDataReference *dataRef = 0;

    if ( url.scheme() == "file" ) {
        NSString *nsFileName = (NSString *)qString2CFStringRef( url.toLocalFile() );
        dataRef = [QTDataReference dataReferenceWithReferenceToFile:nsFileName];
    } else {
        NSString *urlString = (NSString *)qString2CFStringRef( url.toString() );
        NSURL *url = [NSURL URLWithString: urlString];
        dataRef = [QTDataReference dataReferenceWithReferenceToURL:url];
    }

    NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
                dataRef, QTMovieDataReferenceAttribute,
                [NSNumber numberWithBool:YES], QTMovieOpenAsyncOKAttribute,
                [NSNumber numberWithBool:YES], QTMovieIsActiveAttribute,
                [NSNumber numberWithBool:YES], QTMovieResolveDataRefsAttribute,
                [NSNumber numberWithBool:YES], QTMovieDontInteractWithUserAttribute,
                nil];

    m_QTMovie = [[QTMovie movieWithAttributes:attr error:&err] retain];

    if (err) {
        [(QTMovie*)m_QTMovie release];
        m_QTMovie = 0;
        QString description = QString::fromUtf8([[err localizedDescription] UTF8String]);

        qDebug() << "QT7PlayerSession::setMedia error" << description;
        emit error(QMediaPlayer::FormatError, description );
    } else {
        [(QTMovieObserver*)m_movieObserver setMovie:(QTMovie*)m_QTMovie];

        if (m_videoOutput) {
            m_videoOutput->setEnabled(true);
            m_videoOutput->setMovie(m_QTMovie);
        }

        emit durationChanged(duration());
        emit audioAvailableChanged(isAudioAvailable());
        emit videoAvailableChanged(isVideoAvailable());

        [(QTMovie*)m_QTMovie setMuted:m_muted];
        setVolume(m_volume);
    }
}

bool QT7PlayerSession::isAudioAvailable() const
{
    if (!m_QTMovie)
        return false;

    AutoReleasePool pool;
    return [[(QTMovie*)m_QTMovie attributeForKey:@"QTMovieHasAudioAttribute"] boolValue] == YES;
}

bool QT7PlayerSession::isVideoAvailable() const
{
    if (!m_QTMovie)
        return false;

    AutoReleasePool pool;
    return [[(QTMovie*)m_QTMovie attributeForKey:@"QTMovieHasVideoAttribute"] boolValue] == YES;
}

void QT7PlayerSession::processEOS()
{    
    m_mediaStatus = QMediaPlayer::EndOfMedia;
    emit stateChanged(m_state = QMediaPlayer::StoppedState);
    emit mediaStatusChanged(m_mediaStatus);
}

#include "moc_qt7playersession.cpp"

QT_END_NAMESPACE

