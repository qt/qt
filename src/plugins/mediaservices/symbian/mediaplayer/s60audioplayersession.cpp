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

#include "s60audioplayersession.h"
#include <QtCore/qdebug.h>
#include <QtCore/qvariant.h>

#include <AudioOutput.h>
#include <MAudioOutputObserver.h>

QT_BEGIN_NAMESPACE

S60AudioPlayerSession::S60AudioPlayerSession(QObject *parent)
    : S60MediaPlayerSession(parent)
    , m_player(0)
    , m_audioEndpoint("Default")
{
#ifndef HAS_NO_AUDIOROUTING
   m_audioOutput = 0;
#endif
    QT_TRAP_THROWING(m_player = CAudioPlayer::NewL(*this, 0, EMdaPriorityPreferenceNone));
    m_player->RegisterForAudioLoadingNotification(*this);
}

S60AudioPlayerSession::~S60AudioPlayerSession()
{
#if !defined(HAS_NO_AUDIOROUTING)
    if (m_audioOutput)
        m_audioOutput->UnregisterObserver(*this);
    delete m_audioOutput;
#endif
    m_player->Close();
    delete m_player;
}

void S60AudioPlayerSession::doLoadL(const TDesC &path)
{
#ifndef HAS_NO_AUDIOROUTING
    // m_audioOutput needs to be reinitialized after MapcInitComplete
    if (m_audioOutput)
        m_audioOutput->UnregisterObserver(*this);
    delete m_audioOutput;
    m_audioOutput = NULL;
#endif

    m_player->OpenFileL(path);
}

qint64 S60AudioPlayerSession::doGetDurationL() const
{
    return m_player->Duration().Int64() / qint64(1000);
}

qint64 S60AudioPlayerSession::doGetPositionL() const
{
    TTimeIntervalMicroSeconds ms = 0;
    m_player->GetPosition(ms);
    return ms.Int64() / qint64(1000);
}

bool S60AudioPlayerSession::isVideoAvailable() const
{
    return false;
}
bool S60AudioPlayerSession::isAudioAvailable() const
{
    return true; // this is a bit happy scenario, but we do emit error that we can't play
}

void S60AudioPlayerSession::MaloLoadingStarted()
{
    buffering();
}

void S60AudioPlayerSession::MaloLoadingComplete()
{
    buffered();
}

void S60AudioPlayerSession::doPlay()
{
// For some reason loading progress callbalck are not called on emulator
#ifdef __WINSCW__
    buffering();
#endif
    m_player->Play();
#ifdef __WINSCW__
    buffered();
#endif

}

void S60AudioPlayerSession::doPauseL()
{
    m_player->Pause();
}

void S60AudioPlayerSession::doStop()
{
    m_player->Stop();
}

void S60AudioPlayerSession::doSetVolumeL(int volume)
{
    m_player->SetVolume((volume / 100.0) * m_player->MaxVolume());
}

void S60AudioPlayerSession::doSetPositionL(qint64 microSeconds)
{
    m_player->SetPosition(TTimeIntervalMicroSeconds(microSeconds));
}

void S60AudioPlayerSession::updateMetaDataEntriesL()
{
    metaDataEntries().clear();
    int numberOfMetaDataEntries = 0;

    m_player->GetNumberOfMetaDataEntries(numberOfMetaDataEntries);

    for (int i = 0; i < numberOfMetaDataEntries; i++) {
        CMMFMetaDataEntry *entry = NULL;
        entry = m_player->GetMetaDataEntryL(i);
        metaDataEntries().insert(QString::fromUtf16(entry->Name().Ptr(), entry->Name().Length()), QString::fromUtf16(entry->Value().Ptr(), entry->Value().Length()));
        delete entry;
    }
    emit metaDataChanged();
}

int S60AudioPlayerSession::doGetBufferStatusL() const
{
    int progress = 0;
    m_player->GetAudioLoadingProgressL(progress);
    return progress;
}

void S60AudioPlayerSession::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
{
    Q_UNUSED(aDuration);
    setError(aError);
#ifndef HAS_NO_AUDIOROUTING
    TRAPD(err,
        m_audioOutput = CAudioOutput::NewL(*m_player);
        m_audioOutput->RegisterObserverL(*this);
    );
    setActiveEndpoint(m_audioEndpoint);
    setError(err);
#endif
    loaded();
}

void S60AudioPlayerSession::MapcPlayComplete(TInt aError)
{
    setError(aError);
    endOfMedia();
}

void S60AudioPlayerSession::doSetAudioEndpoint(const QString& audioEndpoint)
{
    m_audioEndpoint = audioEndpoint;
}

QString S60AudioPlayerSession::activeEndpoint() const
{
    QString outputName = QString("Default");
#if !defined(HAS_NO_AUDIOROUTING)
    if (m_audioOutput) {
        CAudioOutput::TAudioOutputPreference output = m_audioOutput->AudioOutput();
        outputName = qStringFromTAudioOutputPreference(output);
    }
#endif
    return outputName;
}

QString S60AudioPlayerSession::defaultEndpoint() const
{
    QString outputName = QString("Default");
#if !defined(HAS_NO_AUDIOROUTING)
    if (m_audioOutput) {
        CAudioOutput::TAudioOutputPreference output = m_audioOutput->DefaultAudioOutput();
        outputName = qStringFromTAudioOutputPreference(output);
    }
#endif
    return outputName;
}

void S60AudioPlayerSession::setActiveEndpoint(const QString& name)
{
#if !defined(HAS_NO_AUDIOROUTING)
    CAudioOutput::TAudioOutputPreference output = CAudioOutput::ENoPreference;

    if (name == QString("Default"))
        output = CAudioOutput::ENoPreference;
    else if (name == QString("All"))
        output = CAudioOutput::EAll;
    else if (name == QString("None"))
        output = CAudioOutput::ENoOutput;
    else if (name == QString("Earphone"))
        output = CAudioOutput::EPrivate;
    else if (name == QString("Speaker"))
        output = CAudioOutput::EPublic;
    if (m_audioOutput) {
        TRAPD(err, m_audioOutput->SetAudioOutputL(output));
        setError(err);

        if (m_audioEndpoint != name) {
            m_audioEndpoint = name;
            emit activeEndpointChanged(name);
        }
    }
#endif
}

#if !defined(HAS_NO_AUDIOROUTING)
void S60AudioPlayerSession::DefaultAudioOutputChanged(CAudioOutput& aAudioOutput,
                                        CAudioOutput::TAudioOutputPreference aNewDefault)
{
    // Emit already implemented in setActiveEndpoint function
    Q_UNUSED(aAudioOutput)
    Q_UNUSED(aNewDefault)
}

QString S60AudioPlayerSession::qStringFromTAudioOutputPreference(CAudioOutput::TAudioOutputPreference output) const
{
    if (output == CAudioOutput::ENoPreference)
        return QString("Default");
    else if (output == CAudioOutput::EAll)
        return QString("All");
    else if (output == CAudioOutput::ENoOutput)
        return QString("None");
    else if (output == CAudioOutput::EPrivate)
        return QString("Earphone");
    else if (output == CAudioOutput::EPublic)
        return QString("Speaker");
    return QString("Default");
}
#endif
QT_END_NAMESPACE
