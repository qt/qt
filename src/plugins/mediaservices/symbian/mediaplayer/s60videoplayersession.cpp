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

#include "s60videoplayersession.h"
#include "s60videowidget.h"
#include "s60mediaplayerservice.h"
#include "s60videooverlay.h"

#include <QtCore/qdebug.h>
#include <QtGui/qwidget.h>
#include <QtCore/qtimer.h>
#include <QApplication>

#include <coecntrl.h>
#include <coemain.h>    // For CCoeEnv
#include <w32std.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>

#include <AudioOutput.h>
#include <MAudioOutputObserver.h>

QT_BEGIN_NAMESPACE

S60VideoPlayerSession::S60VideoPlayerSession(QMediaService *service)
    : S60MediaPlayerSession(service)
    , m_player(0)
    , m_rect(0, 0, 0, 0)
    , m_output(QVideoOutputControl::NoOutput)
    , m_windowId(0)
    , m_dsaActive(false)
    , m_dsaStopped(false)
    , m_wsSession(CCoeEnv::Static()->WsSession())
    , m_screenDevice(*CCoeEnv::Static()->ScreenDevice())
    , m_window(0)
    , m_service(*service)
    , m_aspectRatioMode(Qt::KeepAspectRatio)
    , m_originalSize(1, 1)
    , m_audioOutput(0)
    , m_audioEndpoint("Default")
{
    resetNativeHandles();
    QT_TRAP_THROWING(m_player = CVideoPlayerUtility::NewL(
        *this,
        0,
        EMdaPriorityPreferenceNone,
        m_wsSession,
        m_screenDevice,
        *m_window,
        m_rect,
        m_rect));
    m_dsaActive = true;
    m_player->RegisterForVideoLoadingNotification(*this);
}

S60VideoPlayerSession::~S60VideoPlayerSession()
{
#if !defined(HAS_NO_AUDIOROUTING_IN_VIDEOPLAYER)
    if (m_audioOutput)
        m_audioOutput->UnregisterObserver(*this);
    delete m_audioOutput;
#endif
    m_player->Close();
    delete m_player;
}

void S60VideoPlayerSession::doLoadL(const TDesC &path)
{
    // m_audioOutput needs to be reinitialized after MapcInitComplete
    if (m_audioOutput)
        m_audioOutput->UnregisterObserver(*this);
    delete m_audioOutput;
    m_audioOutput = NULL;

    m_player->OpenFileL(path);
}

void S60VideoPlayerSession::doLoadUrlL(const TDesC &path)
{
    // m_audioOutput needs to be reinitialized after MapcInitComplete
    if (m_audioOutput)
        m_audioOutput->UnregisterObserver(*this);
    delete m_audioOutput;
    m_audioOutput = NULL;

    m_player->OpenUrlL(path);
}

int S60VideoPlayerSession::doGetBufferStatusL() const
{
    int progress = 0;
    m_player->GetVideoLoadingProgressL(progress);
    return progress;
}

qint64 S60VideoPlayerSession::doGetDurationL() const
{
    return m_player->DurationL().Int64() / qint64(1000);
}

void S60VideoPlayerSession::setVideoRenderer(QObject *videoOutput)
{
    Q_UNUSED(videoOutput)
    QVideoOutputControl *videoControl = qobject_cast<QVideoOutputControl *>(m_service.control(QVideoOutputControl_iid));

    //Render changes
    if (m_output != videoControl->output()) {

        if (m_output == QVideoOutputControl::WidgetOutput) {
            S60VideoWidgetControl *widgetControl = qobject_cast<S60VideoWidgetControl *>(m_service.control(QVideoWidgetControl_iid));
            disconnect(widgetControl, SIGNAL(widgetUpdated()), this, SLOT(resetVideoDisplay()));
            disconnect(widgetControl, SIGNAL(beginVideoWindowNativePaint()), this, SLOT(suspendDirectScreenAccess()));
            disconnect(widgetControl, SIGNAL(endVideoWindowNativePaint()), this, SLOT(resumeDirectScreenAccess()));
            disconnect(this, SIGNAL(stateChanged(QMediaPlayer::State)), widgetControl, SLOT(videoStateChanged(QMediaPlayer::State)));
        }

        if (videoControl->output() == QVideoOutputControl::WidgetOutput) {
            S60VideoWidgetControl *widgetControl = qobject_cast<S60VideoWidgetControl *>(m_service.control(QVideoWidgetControl_iid));
            connect(widgetControl, SIGNAL(widgetUpdated()), this, SLOT(resetVideoDisplay()));
            connect(widgetControl, SIGNAL(beginVideoWindowNativePaint()), this, SLOT(suspendDirectScreenAccess()));
            connect(widgetControl, SIGNAL(endVideoWindowNativePaint()), this, SLOT(resumeDirectScreenAccess()));
            connect(this, SIGNAL(stateChanged(QMediaPlayer::State)), widgetControl, SLOT(videoStateChanged(QMediaPlayer::State)));
        }

        m_output = videoControl->output();
        resetVideoDisplay();
    }
}

bool S60VideoPlayerSession::resetNativeHandles()
{
    QVideoOutputControl* videoControl = qobject_cast<QVideoOutputControl *>(m_service.control(QVideoOutputControl_iid));
    WId newId = 0;
    TRect newRect = TRect(0,0,0,0);
    Qt::AspectRatioMode aspectRatioMode = Qt::KeepAspectRatio;

    if (videoControl->output() == QVideoOutputControl::WidgetOutput) {
        S60VideoWidgetControl* widgetControl = qobject_cast<S60VideoWidgetControl *>(m_service.control(QVideoWidgetControl_iid));
        QWidget *videoWidget = widgetControl->videoWidget();
        newId = widgetControl->videoWidgetWId();
        newRect = QRect2TRect(QRect(videoWidget->mapToGlobal(videoWidget->pos()), videoWidget->size()));
        aspectRatioMode = widgetControl->aspectRatioMode();
    } else if (videoControl->output() == QVideoOutputControl::WindowOutput) {
        S60VideoOverlay* windowControl = qobject_cast<S60VideoOverlay *>(m_service.control(QVideoWindowControl_iid));
        newId = windowControl->winId();
        newRect = TRect( newId->DrawableWindow()->AbsPosition(), newId->DrawableWindow()->Size());
    } else {
        if (QApplication::activeWindow())
            newId = QApplication::activeWindow()->effectiveWinId();

        if (!newId && QApplication::allWidgets().count())
            newId = QApplication::allWidgets().at(0)->effectiveWinId();

        Q_ASSERT(newId != 0);
    }

    if (newRect == m_rect &&  newId == m_windowId && aspectRatioMode == m_aspectRatioMode)
        return false;

    if (newId) {
        m_rect = newRect;
        m_windowId = newId;
        m_window = m_windowId->DrawableWindow();
        m_aspectRatioMode = aspectRatioMode;
        return true;
    }
    return false;
}

bool S60VideoPlayerSession::isVideoAvailable() const
{
#ifdef PRE_S60_50_PLATFORM
    return true; // this is not support in pre 5th platforms
#else
    if (m_player)
        return m_player->VideoEnabledL();
    else
        return false;
#endif
}

bool S60VideoPlayerSession::isAudioAvailable() const
{
    if (m_player)
        return m_player->AudioEnabledL();
    else
        return false;
}

void S60VideoPlayerSession::doPlay()
{
    m_player->Play();
}

void S60VideoPlayerSession::doPauseL()
{
    m_player->PauseL();
}

void S60VideoPlayerSession::doStop()
{
    m_player->Stop();
}

qint64 S60VideoPlayerSession::doGetPositionL() const
{
    return m_player->PositionL().Int64() / qint64(1000);
}

void S60VideoPlayerSession::doSetPositionL(qint64 microSeconds)
{
    m_player->SetPositionL(TTimeIntervalMicroSeconds(microSeconds));
}

void S60VideoPlayerSession::doSetVolumeL(int volume)
{
    m_player->SetVolumeL((volume / 100.0)* m_player->MaxVolume());
}

QPair<qreal, qreal> S60VideoPlayerSession::scaleFactor()
{
    QSize scaled = m_originalSize;
    if (m_aspectRatioMode == Qt::IgnoreAspectRatio)
        scaled.scale(TRect2QRect(m_rect).size(), Qt::IgnoreAspectRatio);
    else if(m_aspectRatioMode == Qt::KeepAspectRatio)
        scaled.scale(TRect2QRect(m_rect).size(), Qt::KeepAspectRatio);

    qreal width = qreal(scaled.width()) / qreal(m_originalSize.width()) * qreal(100);
    qreal height = qreal(scaled.height()) / qreal(m_originalSize.height()) * qreal(100);

    return QPair<qreal, qreal>(width, height);
}

void S60VideoPlayerSession::startDirectScreenAccess()
{
    if(m_dsaActive)
        return;

    TRAPD(err, m_player->StartDirectScreenAccessL());
    if(err == KErrNone)
        m_dsaActive = true;
    setError(err);
}

bool S60VideoPlayerSession::stopDirectScreenAccess()
{
    if(!m_dsaActive)
        return false;

    TRAPD(err, m_player->StopDirectScreenAccessL());
    if(err == KErrNone)
        m_dsaActive = false;

    setError(err);
    return true;
}

void S60VideoPlayerSession::MvpuoOpenComplete(TInt aError)
{
    setError(aError);
    m_player->Prepare();
}

void S60VideoPlayerSession::MvpuoPrepareComplete(TInt aError)
{
    setError(aError);
    TRAPD(err,
        m_player->SetDisplayWindowL(m_wsSession,
                                    m_screenDevice,
                                    *m_window,
                                    m_rect,
                                    m_rect);
        TSize originalSize;
        m_player->VideoFrameSizeL(originalSize);
        m_originalSize = QSize(originalSize.iWidth, originalSize.iHeight);
        m_player->SetScaleFactorL(scaleFactor().first, scaleFactor().second, true));

    setError(err);
    m_dsaActive = true;
#if !defined(HAS_NO_AUDIOROUTING_IN_VIDEOPLAYER)
    TRAP(err,
        m_audioOutput = CAudioOutput::NewL(*m_player);
        m_audioOutput->RegisterObserverL(*this);
    );
    setActiveEndpoint(m_audioEndpoint);
    setError(err);
#endif
    loaded();
}

void S60VideoPlayerSession::MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError)
{
    Q_UNUSED(aFrame);
    Q_UNUSED(aError);
}

void S60VideoPlayerSession::MvpuoPlayComplete(TInt aError)
{
    setError(aError);
    endOfMedia();
}

void S60VideoPlayerSession::MvpuoEvent(const TMMFEvent &aEvent)
{
    Q_UNUSED(aEvent);
}

void S60VideoPlayerSession::updateMetaDataEntriesL()
{
    metaDataEntries().clear();
    int numberOfMetaDataEntries = 0;

    numberOfMetaDataEntries = m_player->NumberOfMetaDataEntriesL();

    for (int i = 0; i < numberOfMetaDataEntries; i++) {
        CMMFMetaDataEntry *entry = NULL;
        entry = m_player->MetaDataEntryL(i);
        metaDataEntries().insert(TDesC2QString(entry->Name()), TDesC2QString(entry->Value()));
        delete entry;
    }
    emit metaDataChanged();
}

void S60VideoPlayerSession::resetVideoDisplay()
{
    if (resetNativeHandles()) {
        TRAPD(err,
           m_player->SetDisplayWindowL(m_wsSession,
                                       m_screenDevice,
                                       *m_window,
                                       m_rect,
                                       m_rect));
        setError(err);
        if(    mediaStatus() == QMediaPlayer::LoadedMedia
            || mediaStatus() == QMediaPlayer::StalledMedia
            || mediaStatus() == QMediaPlayer::BufferingMedia
            || mediaStatus() == QMediaPlayer::BufferedMedia
            || mediaStatus() == QMediaPlayer::EndOfMedia) {
            TRAPD(err, m_player->SetScaleFactorL(scaleFactor().first, scaleFactor().second, true));
            setError(err);
        }
    }
}

void S60VideoPlayerSession::suspendDirectScreenAccess()
{
    m_dsaStopped = stopDirectScreenAccess();
}

void S60VideoPlayerSession::resumeDirectScreenAccess()
{
    if(!m_dsaStopped)
        return;

    startDirectScreenAccess();
    m_dsaStopped = false;
}

void S60VideoPlayerSession::MvloLoadingStarted()
{
    buffering();
}

void S60VideoPlayerSession::MvloLoadingComplete()
{
    buffered();
}

void S60VideoPlayerSession::doSetAudioEndpoint(const QString& audioEndpoint)
{
    m_audioEndpoint = audioEndpoint;
}

QString S60VideoPlayerSession::activeEndpoint() const
{
    QString outputName = QString("Default");
#if !defined(HAS_NO_AUDIOROUTING_IN_VIDEOPLAYER)
    if (m_audioOutput) {
        CAudioOutput::TAudioOutputPreference output = m_audioOutput->AudioOutput();
        outputName = qStringFromTAudioOutputPreference(output);
    }
#endif
    return outputName;
}

QString S60VideoPlayerSession::defaultEndpoint() const
{
    QString outputName = QString("Default");
#if !defined(HAS_NO_AUDIOROUTING_IN_VIDEOPLAYER)
    if (m_audioOutput) {
        CAudioOutput::TAudioOutputPreference output = m_audioOutput->DefaultAudioOutput();
        outputName = qStringFromTAudioOutputPreference(output);
    }
#endif
    return outputName;
}

void S60VideoPlayerSession::setActiveEndpoint(const QString& name)
{
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
#if !defined(HAS_NO_AUDIOROUTING_IN_VIDEOPLAYER)
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

void S60VideoPlayerSession::DefaultAudioOutputChanged( CAudioOutput& aAudioOutput,
                                        CAudioOutput::TAudioOutputPreference aNewDefault )
{
    // Emit already implemented in setActiveEndpoint function
    Q_UNUSED(aAudioOutput)
    Q_UNUSED(aNewDefault)
}

QString S60VideoPlayerSession::qStringFromTAudioOutputPreference(CAudioOutput::TAudioOutputPreference output) const
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

QT_END_NAMESPACE

