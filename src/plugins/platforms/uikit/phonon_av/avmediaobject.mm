/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "avmediaobject.h"
#include "avaudiooutput.h"

#include <QtCore/QUrl>
#import <Foundation/NSString.h>
#import <Foundation/NSURL.h>
#import <AVFoundation/AVAudioPlayer.h>

@interface AudioPlayerDelegate : NSObject <AVAudioPlayerDelegate> {
    AVMediaObject *mediaObject;
}

- (id)initWithMediaObject:(AVMediaObject *)obj;
- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag;

@end

@implementation AudioPlayerDelegate

- (id)initWithMediaObject:(AVMediaObject *)obj
{
    if ((self = [self init])) {
        mediaObject = obj;
    }
    return self;
}

- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag
{
    Q_UNUSED(flag)
    Q_UNUSED(player)
    mediaObject->handlePlayerFinished();
}

@end

class AVMediaObjectPrivate {
public:
    AVMediaObjectPrivate(AVMediaObject *parent)
        : player(0)
    {
        delegate = [[AudioPlayerDelegate alloc] initWithMediaObject:parent];
    }
    ~AVMediaObjectPrivate()
    {
        [delegate release];
    }

    AVAudioPlayer *player;
    AudioPlayerDelegate *delegate;
};

AVMediaObject::AVMediaObject(QObject *parent)
    : QObject(parent),
      m_tickInterval(0),
      m_state(Phonon::LoadingState),
      m_errorType(Phonon::NoError),
      m_output(0),
      d(new AVMediaObjectPrivate(this))
{
}

AVMediaObject::~AVMediaObject()
{
    if (m_state == Phonon::PlayingState)
        stop();
    [d->player release];
    delete d;
}

void AVMediaObject::changeState(Phonon::State state)
{
    if (m_state == state)
        return;
    Phonon::State oldState = m_state;
    m_state = state;
    emit stateChanged(m_state, oldState);
}

bool AVMediaObject::checkPlayer() const
{
    if (!d->player) {
        m_errorType = Phonon::NormalError;
        m_errorString = tr("Media source has not been set.");
        return false;
    }
    return true;
}

void AVMediaObject::play()
{
    if (!checkPlayer())
        return;
    if (![d->player play]) {
        m_errorType = Phonon::NormalError;
        m_errorString = tr("Failed to play media source.");
        return;
    }
    changeState(Phonon::PlayingState);
}

void AVMediaObject::pause()
{
    if (!checkPlayer())
        return;
    [d->player pause];
    changeState(Phonon::PausedState);
}

void AVMediaObject::stop()
{
    if (!checkPlayer())
        return;
    [d->player stop];
    d->player.currentTime = 0;
    changeState(Phonon::StoppedState);
}

void AVMediaObject::seek(qint64 milliseconds)
{
    if (!checkPlayer())
        return;
    d->player.currentTime = milliseconds/1000.;
}

qint32 AVMediaObject::tickInterval() const
{
    return m_tickInterval;
}

void AVMediaObject::setTickInterval(qint32 newTickInterval)
{
    m_tickInterval = newTickInterval;
    //TODO
}

bool AVMediaObject::hasVideo() const
{
    return false;
}

bool AVMediaObject::isSeekable() const
{
    return true;
}

qint64 AVMediaObject::currentTime() const
{
    if (!checkPlayer())
        return 0;
    return (qint64)(d->player.currentTime * 1000);
}

Phonon::State AVMediaObject::state() const
{
    return m_state;
}

QString AVMediaObject::errorString() const
{
    return m_errorString;
}

Phonon::ErrorType AVMediaObject::errorType() const
{
    return m_errorType;
}

qint64 AVMediaObject::totalTime() const
{
    if (!checkPlayer())
        return 0;
    return d->player.duration;
}

Phonon::MediaSource AVMediaObject::source() const
{
    return m_mediaSource;
}

void AVMediaObject::setSource(const Phonon::MediaSource &source)
{
    if (d->player) {
        stop();
        [d->player release];
        d->player = 0;
    }
    m_mediaSource = source;
    NSString *urlString = [NSString stringWithCString:source.url().toEncoded().constData()
            encoding:NSASCIIStringEncoding];
    NSURL *url = [NSURL URLWithString:urlString];
    d->player = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:NULL];
    if (!d->player) {
        m_errorString = tr("Failed to create player for '%1'").arg(source.url().toString());
        changeState(Phonon::ErrorState);
        return;
    }
    d->player.delegate = d->delegate;
    [d->player prepareToPlay];
    changeState(Phonon::StoppedState);
    emit currentSourceChanged(m_mediaSource);
    emit totalTimeChanged((qint64)(d->player.duration * 1000));
}

void AVMediaObject::setNextSource(const Phonon::MediaSource &source)
{
    setSource(source);
}

qint32 AVMediaObject::prefinishMark() const
{
    // not implemented
    return 0;
}

void AVMediaObject::setPrefinishMark(qint32)
{
    // not implemented
}

qint32 AVMediaObject::transitionTime() const
{
    // not implemented
    return 0;
}

void AVMediaObject::setTransitionTime(qint32)
{
    // not implemented
}

void AVMediaObject::setAudioOutput(AVAudioOutput *audioOutput)
{
    if (m_output) {
        disconnect(m_output, SIGNAL(volumeChanged(qreal)), this, SLOT(setVolume(qreal)));
    }
    m_output = audioOutput;
    if (m_output) {
        connect(m_output, SIGNAL(volumeChanged(qreal)), this, SLOT(setVolume(qreal)));
        setVolume(m_output->volume());
    }
}

void AVMediaObject::setVolume(qreal newVolume)
{
    if (!d->player) // do nothing, will be set when player is created
        return;
    [d->player setVolume:qMin((float)1.0, (float)newVolume)];
}

void AVMediaObject::handlePlayerFinished()
{
    emit aboutToFinish();
    changeState(Phonon::StoppedState);
    emit finished();
}
