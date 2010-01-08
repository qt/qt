/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite module of the Qt Toolkit.
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

#include "mediaobject.h"
#include "backend.h"

#include <QtCore>
#include <QtCore/QTimer>
#include <QtCore/QVector>
#include <QtCore/QFile>
#include <QtCore/QByteRef>
#include <QtCore/QStringList>
#include <QtCore/QEvent>
#include <QApplication>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Dummy
{

static const char* riffId = "RIFF";

MediaObject::MediaObject(Backend *backend, QObject *parent)
        : QObject(parent)
          , m_resumeState(false)
          , m_oldState(Phonon::LoadingState)
          , m_oldPos(0)
          , currentPos(0)
{
    Q_UNUSED(backend)

    m_error = Phonon::NoError;
    m_tickInterval = 100; // 100ms
    m_totalTime = 26000; // 26s
    m_prefinishMark = 0;
    m_transitionTime = 100; //100ms
    m_hasVideo = false;
    m_prefinishMarkReachedNotEmitted = true;
    m_aboutToFinishEmitted = false;
    m_pendingState = Phonon::LoadingState;
    m_state = Phonon::LoadingState;
    m_pendingState = Phonon::LoadingState;
    m_tickTimer = new QTimer(this);
    connect(m_tickTimer, SIGNAL(timeout()), SLOT(emitTick()));
}

MediaObject::~MediaObject()
{
    delete m_tickTimer;
}

QString stateString(const Phonon::State &state)
{
    switch (state) {
        case Phonon::LoadingState:
            return QString("LoadingState");
        case Phonon::StoppedState:
            return QString("StoppedState");
        case Phonon::PlayingState:
            return QString("PlayingState");
        case Phonon::BufferingState:
            return QString("BufferingState");
        case Phonon::PausedState:
            return QString("PausedState");
        case Phonon::ErrorState:
            return QString("ErrorState");
    }
    return QString();
}

void MediaObject::saveState()
{
    if (m_resumeState)
        return;

    if (m_pendingState == Phonon::PlayingState || m_pendingState == Phonon::PausedState) {
        m_resumeState = true;
        m_oldState = m_pendingState;
        m_oldPos = currentPos;
    }
}

void MediaObject::resumeState()
{
    if (m_resumeState)
        QMetaObject::invokeMethod(this, "setState", Qt::QueuedConnection, Q_ARG(State, m_oldState));
}

/**
 * !reimp
 */
State MediaObject::state() const
{
    return m_state;
}

/**
 * !reimp
 */
bool MediaObject::hasVideo() const
{
    return m_hasVideo;
}

/**
 * !reimp
 */
bool MediaObject::isSeekable() const
{
    return true;
}

/**
 * !reimp
 */
qint64 MediaObject::currentTime() const
{
    if (m_resumeState)
        return m_oldPos;

    switch (state()) {
    case Phonon::PausedState:
    case Phonon::BufferingState:
    case Phonon::PlayingState:
        return currentPos;
    case Phonon::StoppedState:
    case Phonon::LoadingState:
        return 0;
    case Phonon::ErrorState:
        break;
    }
    return -1;
}

/**
 * !reimp
 */
qint32 MediaObject::tickInterval() const
{
    return m_tickInterval;
}

/**
 * !reimp
 */
void MediaObject::setTickInterval(qint32 newTickInterval)
{
    m_tickInterval = newTickInterval;
    if (m_tickInterval <= 0) {
        m_tickTimer->setInterval(100);
    } else
        m_tickTimer->setInterval(newTickInterval);
}

/**
 * !reimp
 */
void MediaObject::play()
{
    if(m_state == Phonon::PlayingState)
        return;
    if(m_state == Phonon::ErrorState)
        return;

    if(m_state != Phonon::PausedState)
        m_tickTimer->stop();

    m_prefinishMarkReachedNotEmitted = true;
    m_aboutToFinishEmitted = false;

    setState(Phonon::PlayingState);
    m_resumeState = false;
    m_tickTimer->start();
}

/**
 * !reimp
 */
QString MediaObject::errorString() const
{
    return m_errorString;
}

/**
 * !reimp
 */
Phonon::ErrorType MediaObject::errorType() const
{
    return m_error;
}

void MediaObject::setState(State newstate)
{
    if (m_state == newstate)
        return;

    switch (newstate) {
        case Phonon::PausedState:
            m_pendingState = Phonon::PausedState;
            emit stateChanged(newstate, m_state);
            m_state = newstate;
            break;
        case Phonon::StoppedState:
            m_pendingState = Phonon::StoppedState;
            emit stateChanged(newstate, m_state);
            m_state = newstate;
            break;
        case Phonon::PlayingState:
            m_pendingState = Phonon::PlayingState;
            emit stateChanged(newstate, m_state);
            m_state = newstate;
            break;
        case Phonon::ErrorState:
            emit stateChanged(newstate, m_state);
            m_state = newstate;
            break;
        case Phonon::BufferingState:
        case Phonon::LoadingState:
            emit stateChanged(newstate, m_state);
            m_state = newstate;
            break;
    }
}

qint64 MediaObject::totalTime() const
{
    return m_totalTime;
}

qint32 MediaObject::prefinishMark() const
{
    return m_prefinishMark;
}

qint32 MediaObject::transitionTime() const
{
    return m_transitionTime;
}

void MediaObject::setTransitionTime(qint32 time)
{
    m_transitionTime = time;
}

qint64 MediaObject::remainingTime() const
{
    if(currentTime() > totalTime())
        return 0;

    return totalTime() - currentTime();
}

MediaSource MediaObject::source() const
{
    return m_source;
}

void MediaObject::setNextSource(const MediaSource &source)
{
    if (source.type() == MediaSource::Invalid &&
        source.type() == MediaSource::Empty)
        return;
    m_nextSource = source;
}

/*
 * !reimp
 */
void MediaObject::setSource(const MediaSource &source)
{
    QMultiMap<QString, QString> ret;

    ret.insert(QLatin1String("ARTIST"), "Nokia Dude");
    ret.insert(QLatin1String("ALBUM"), "Sound of silence");
    ret.insert(QLatin1String("DATE"), "2009");

    m_error = Phonon::NoError;
    setState(Phonon::LoadingState);

    m_source = source;
    currentPos = 0;

    if((source.fileName().contains(".avi")) ||
       (source.fileName().contains(".mp4"))) {
        m_hasVideo = true;
        emit hasVideoChanged(m_hasVideo);
    }
    if(source.fileName().contains(".wav")) {
        QFile file(source.fileName());
        if (file.open(QIODevice::ReadOnly)) {
            int len = file.read((char*)&header, sizeof(CombinedHeader));
            if(len == sizeof(CombinedHeader)) {
                if(memcmp(&header.riff.descriptor.id, riffId, 4) != 0) {
                    // Not a valid wav file, to satisfy unit test for mediaobject
                    m_error = Phonon::FatalError;
                    //m_state = Phonon::ErrorState;
                    m_errorString = "Invalid wav file";
                    setState(Phonon::ErrorState);
                    file.close();
                    return;
                }
            }
            file.close();
        }
    }
    emit metaDataChanged(ret);
    emit currentSourceChanged(source);
    emit totalTimeChanged(m_totalTime);

    setState(Phonon::StoppedState);
}

void MediaObject::setPrefinishMark(qint32 newPrefinishMark)
{
    m_prefinishMark = newPrefinishMark;
    if (currentTime() < totalTime() - m_prefinishMark) // not about to finish
        m_prefinishMarkReachedNotEmitted = true;
}

void MediaObject::pause()
{
    if (state() != Phonon::PausedState)
        setState(Phonon::PausedState);
    m_resumeState = false;
    m_tickTimer->stop();
}

void MediaObject::stop()
{
    if (state() != Phonon::StoppedState) {
        if(m_state != Phonon::ErrorState) {
            setState(Phonon::StoppedState);
        }
        m_prefinishMarkReachedNotEmitted = true;
    }
    m_resumeState = false;
    m_tickTimer->stop();
}

void MediaObject::emitTick()
{
    if (m_resumeState) {
        return;
    }
    if(m_tickInterval > 0)
        currentPos += m_tickInterval;
    else
        currentPos += 100;

    qint64 currentTime = currentPos;
    qint64 totalTime = m_totalTime;

    if (m_tickInterval > 0 && currentTime != m_previousTickTime) {
        emit tick(currentTime);
        m_previousTickTime = currentTime;
    }
    if (m_state == Phonon::PlayingState) {
        if (currentTime >= totalTime - m_prefinishMark) {
            if (m_prefinishMarkReachedNotEmitted) {
                m_prefinishMarkReachedNotEmitted = false;
                emit prefinishMarkReached(totalTime - currentTime);
            }
        }
        // Prepare load of next source
        if (currentTime >= totalTime - 500) {
            if (!m_aboutToFinishEmitted) {
                m_aboutToFinishEmitted = true; // track is about to finish
                emit aboutToFinish();
            }
        }
        if(currentTime >= totalTime) {
            m_tickTimer->stop();
            if(m_nextSource.type() != MediaSource::Invalid &&
                    m_nextSource.type() != MediaSource::Empty) {
                setSource(m_nextSource);
                m_nextSource = MediaSource();
                m_pendingState = Phonon::PlayingState;
            } else {
                setState(Phonon::PausedState);
                currentPos = 0;
                emit finished();
            }
        }
    }
}

void MediaObject::seek(qint64 time)
{
    // We will assume no buffering in the object so this is not needed.
    currentPos = time;
}

} // ns Dummy
} // ns Phonon

QT_END_NAMESPACE

#include "moc_mediaobject.cpp"
