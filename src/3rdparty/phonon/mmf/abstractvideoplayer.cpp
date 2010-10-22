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

#include <QDir>
#include <QUrl>
#include <QTimer>
#include <QWidget>

#include <coemain.h>    // for CCoeEnv

#include "abstractvideoplayer.h"
#include "utils.h"

#ifndef QT_NO_DEBUG
#include "objectdump.h"
#endif

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::AbstractVideoPlayer
  \internal
*/

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::AbstractVideoPlayer::AbstractVideoPlayer(MediaObject *parent, const AbstractPlayer *player)
    :   AbstractMediaPlayer(parent, player)
    ,   m_wsSession(CCoeEnv::Static()->WsSession())
    ,   m_screenDevice(*CCoeEnv::Static()->ScreenDevice())
    ,   m_window(0)
    ,   m_scaleWidth(1.0)
    ,   m_scaleHeight(1.0)
    ,   m_totalTime(0)
{

}

void MMF::AbstractVideoPlayer::construct()
{
    TRACE_CONTEXT(AbstractVideoPlayer::AbstractVideoPlayer, EVideoApi);
    TRACE_ENTRY_0();

    if (m_videoOutput) {
        initVideoOutput();
        m_window = m_videoOutput->videoWindow();
    }

    createPlayer();

    m_player->RegisterForVideoLoadingNotification(*this);

    TRACE_EXIT_0();
}

MMF::AbstractVideoPlayer::~AbstractVideoPlayer()
{
    TRACE_CONTEXT(AbstractVideoPlayer::~AbstractVideoPlayer, EVideoApi);
    TRACE_ENTRY_0();

    // QObject destructor removes all signal-slot connections involving this
    // object, so we do not need to disconnect from m_videoOutput here.

    TRACE_EXIT_0();
}

CVideoPlayerUtility* MMF::AbstractVideoPlayer::nativePlayer() const
{
    return m_player.data();
}

//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------

void MMF::AbstractVideoPlayer::doPlay()
{
    TRACE_CONTEXT(AbstractVideoPlayer::doPlay, EVideoApi);

    handlePendingParametersChanged();

    m_player->Play();
}

void MMF::AbstractVideoPlayer::doPause()
{
    TRACE_CONTEXT(AbstractVideoPlayer::doPause, EVideoApi);

    TRAPD(err, m_player->PauseL());
    if (KErrNone != err && state() != ErrorState) {
        TRACE("PauseL error %d", err);
        setError(tr("Pause failed"), err);
    }
}

void MMF::AbstractVideoPlayer::doStop()
{
    m_player->Stop();
}

void MMF::AbstractVideoPlayer::doSeek(qint64 ms)
{
    TRACE_CONTEXT(AbstractVideoPlayer::doSeek, EVideoApi);

    TRAPD(err, m_player->SetPositionL(TTimeIntervalMicroSeconds(ms * 1000)));

    if (KErrNone != err)
        setError(tr("Seek failed"), err);
}

int MMF::AbstractVideoPlayer::setDeviceVolume(int mmfVolume)
{
    TRAPD(err, m_player->SetVolumeL(mmfVolume));
    return err;
}

int MMF::AbstractVideoPlayer::openFile(const QString &fileName)
{
    const QHBufC nativeFileName(QDir::toNativeSeparators(fileName));
    TRAPD(err, m_player->OpenFileL(*nativeFileName));
    return err;
}

int MMF::AbstractVideoPlayer::openFile(RFile &file)
{
    TRAPD(err, m_player->OpenFileL(file));
    return err;
}

int MMF::AbstractVideoPlayer::openUrl(const QString &url)
{
    TRAPD(err, m_player->OpenUrlL(qt_QString2TPtrC(url)));
    return err;
}

int MMF::AbstractVideoPlayer::openDescriptor(const TDesC8 &des)
{
    TRAPD(err, m_player->OpenDesL(des));
    return err;
}

int MMF::AbstractVideoPlayer::bufferStatus() const
{
    int result = 0;
    TRAP_IGNORE(m_player->GetVideoLoadingProgressL(result));
    return result;
}

void MMF::AbstractVideoPlayer::doClose()
{
    m_player->Close();
}

bool MMF::AbstractVideoPlayer::hasVideo() const
{
    return true;
}

qint64 MMF::AbstractVideoPlayer::getCurrentTime() const
{
    TRACE_CONTEXT(AbstractVideoPlayer::getCurrentTime, EVideoApi);

    TTimeIntervalMicroSeconds us;
    TRAPD(err, us = m_player->PositionL())

    qint64 result = 0;

    if (KErrNone == err) {
        result = toMilliSeconds(us);
    } else {
        TRACE("PositionL error %d", err);

        // If we don't cast away constness here, we simply have to ignore
        // the error.
        const_cast<AbstractVideoPlayer*>(this)->setError(tr("Getting position failed"), err);
    }

    return result;
}

qint64 MMF::AbstractVideoPlayer::totalTime() const
{
    return m_totalTime;
}


//-----------------------------------------------------------------------------
// Public slots
//-----------------------------------------------------------------------------

void MMF::AbstractVideoPlayer::videoWindowChanged()
{
    TRACE_CONTEXT(AbstractVideoPlayer::videoWindowChanged, EVideoInternal);
    TRACE_ENTRY("state %d", state());

    m_window = m_videoOutput ? m_videoOutput->videoWindow() : 0;

    if (m_videoOutput)
        m_videoOutput->dump();

    handleVideoWindowChanged();

    TRACE_EXIT_0();
}

void MMF::AbstractVideoPlayer::aspectRatioChanged()
{
    TRACE_CONTEXT(AbstractVideoPlayer::aspectRatioChanged, EVideoInternal);
    TRACE_ENTRY("state %d aspectRatio %d", state());

    if (m_videoOutput)
        updateScaleFactors(m_videoOutput->videoWindowSize());

    TRACE_EXIT_0();
}

void MMF::AbstractVideoPlayer::scaleModeChanged()
{
    TRACE_CONTEXT(AbstractVideoPlayer::scaleModeChanged, EVideoInternal);
    TRACE_ENTRY("state %d", state());

    if (m_videoOutput)
        updateScaleFactors(m_videoOutput->videoWindowSize());

    TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// MVideoPlayerUtilityObserver callbacks
//-----------------------------------------------------------------------------

void MMF::AbstractVideoPlayer::MvpuoOpenComplete(TInt aError)
{
    TRACE_CONTEXT(AbstractVideoPlayer::MvpuoOpenComplete, EVideoApi);
    TRACE_ENTRY("state %d error %d", state(), aError);

    __ASSERT_ALWAYS(LoadingState == state() ||
                    progressiveDownloadStalled() && BufferingState == state(),
                    Utils::panic(InvalidStatePanic));

    if (KErrNone == aError)
        m_player->Prepare();
    else
        setError(tr("Opening clip failed"), aError);

    TRACE_EXIT_0();
}

void MMF::AbstractVideoPlayer::MvpuoPrepareComplete(TInt aError)
{
    TRACE_CONTEXT(AbstractVideoPlayer::MvpuoPrepareComplete, EVideoApi);
    TRACE_ENTRY("state %d error %d", state(), aError);

    __ASSERT_ALWAYS(LoadingState == state() ||
                    progressiveDownloadStalled() && BufferingState == state(),
                    Utils::panic(InvalidStatePanic));

    TRAPD(err, getVideoClipParametersL(aError));

    if (KErrNone == err) {
        if (m_videoOutput)
            m_videoOutput->dump();

        maxVolumeChanged(m_player->MaxVolume());

        if (m_videoOutput)
            m_videoOutput->setVideoSize(m_videoFrameSize);

        prepareCompleted();
        handlePendingParametersChanged();

        emit totalTimeChanged(totalTime());
    }

    loadingComplete(aError);

    TRACE_EXIT_0();
}

void MMF::AbstractVideoPlayer::getVideoClipParametersL(TInt aError)
{
    User::LeaveIfError(aError);

    // Get frame size
    TSize size;
    m_player->VideoFrameSizeL(size);
    m_videoFrameSize = QSize(size.iWidth, size.iHeight);

    // Get duration
    m_totalTime = toMilliSeconds(m_player->DurationL());
}


void MMF::AbstractVideoPlayer::MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError)
{
    TRACE_CONTEXT(AbstractVideoPlayer::MvpuoFrameReady, EVideoApi);
    TRACE_ENTRY("state %d error %d", state(), aError);

    Q_UNUSED(aFrame);
    Q_UNUSED(aError);   // suppress warnings in release builds

    TRACE_EXIT_0();
}

void MMF::AbstractVideoPlayer::MvpuoPlayComplete(TInt aError)
{
    TRACE_CONTEXT(AbstractVideoPlayer::MvpuoPlayComplete, EVideoApi)
    TRACE_ENTRY("state %d error %d", state(), aError);

    // Call base class function which handles end of playback for both
    // audio and video clips.
    playbackComplete(aError);

    TRACE_EXIT_0();
}

void MMF::AbstractVideoPlayer::MvpuoEvent(const TMMFEvent &aEvent)
{
    TRACE_CONTEXT(AbstractVideoPlayer::MvpuoEvent, EVideoApi);
    TRACE_ENTRY("state %d", state());

    Q_UNUSED(aEvent);

    TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// MVideoLoadingObserver callbacks
//-----------------------------------------------------------------------------

void MMF::AbstractVideoPlayer::MvloLoadingStarted()
{
    bufferingStarted();
}

void MMF::AbstractVideoPlayer::MvloLoadingComplete()
{
    bufferingComplete();
}


//-----------------------------------------------------------------------------
// Video window updates
//-----------------------------------------------------------------------------

void MMF::AbstractVideoPlayer::videoOutputChanged()
{
    TRACE_CONTEXT(AbstractVideoPlayer::videoOutputChanged, EVideoInternal);
    TRACE_ENTRY_0();

    if (m_videoOutput)
        initVideoOutput();

    videoWindowChanged();

    TRACE_EXIT_0();
}

void MMF::AbstractVideoPlayer::initVideoOutput()
{
    Q_ASSERT(m_videoOutput);

    bool connected = connect(
        m_videoOutput, SIGNAL(videoWindowChanged()),
        this, SLOT(videoWindowChanged())
    );
    Q_ASSERT(connected);

    connected = connect(
        m_videoOutput, SIGNAL(aspectRatioChanged()),
        this, SLOT(aspectRatioChanged())
    );
    Q_ASSERT(connected);

    connected = connect(
        m_videoOutput, SIGNAL(scaleModeChanged()),
        this, SLOT(scaleModeChanged())
    );
    Q_ASSERT(connected);

    // Suppress warnings in release builds
    Q_UNUSED(connected);

    m_videoOutput->setVideoSize(m_videoFrameSize);
}

// Helper function for aspect ratio / scale mode handling
QSize scaleToAspect(const QSize &srcRect, int aspectWidth, int aspectHeight)
{
    const qreal aspectRatio = qreal(aspectWidth) / aspectHeight;

    int width = srcRect.width();
    int height = srcRect.width() / aspectRatio;
    if (height > srcRect.height()){
        height = srcRect.height();
        width = srcRect.height() * aspectRatio;
    }
    return QSize(width, height);
}

void MMF::AbstractVideoPlayer::updateScaleFactors(const QSize &windowSize, bool apply)
{
    Q_ASSERT(m_videoOutput);

    if (m_videoFrameSize.isValid()) {
        QRect videoRect;

        // Calculate size of smallest rect which contains video frame size
        // and conforms to aspect ratio
        switch (m_videoOutput->aspectRatio()) {
        case Phonon::VideoWidget::AspectRatioAuto:
            videoRect.setSize(m_videoFrameSize);
            break;

        case Phonon::VideoWidget::AspectRatioWidget:
            videoRect.setSize(windowSize);
            break;

        case Phonon::VideoWidget::AspectRatio4_3:
            videoRect.setSize(scaleToAspect(m_videoFrameSize, 4, 3));
            break;

        case Phonon::VideoWidget::AspectRatio16_9:
            videoRect.setSize(scaleToAspect(m_videoFrameSize, 16, 9));
            break;
        }

        // Scale to fill the window width
        const int windowWidth = windowSize.width();
        const int windowHeight = windowSize.height();
        const qreal windowScaleFactor = qreal(windowWidth) / videoRect.width();
        int videoWidth = windowWidth;
        int videoHeight = videoRect.height() * windowScaleFactor;

        const qreal windowToVideoHeightRatio = qreal(windowHeight) / videoHeight;

        switch (m_videoOutput->scaleMode()) {
        case Phonon::VideoWidget::ScaleAndCrop:
            if (videoHeight < windowHeight) {
                videoWidth *= windowToVideoHeightRatio;
                videoHeight = windowHeight;
            }
            break;
        case Phonon::VideoWidget::FitInView:
        default:
            if (videoHeight > windowHeight) {
                videoWidth *= windowToVideoHeightRatio;
                videoHeight = windowHeight;
            }
            break;
        }

        // Calculate scale factors
        m_scaleWidth = 100.0f * videoWidth / m_videoFrameSize.width();
        m_scaleHeight = 100.0f * videoHeight / m_videoFrameSize.height();

        if (apply)
            parametersChanged(ScaleFactors);
    }
}

void MMF::AbstractVideoPlayer::parametersChanged(VideoParameters parameters)
{
    if (state() == LoadingState || progressiveDownloadStalled() && BufferingState == state())
        m_pendingChanges |= parameters;
    else
        handleParametersChanged(parameters);
}

void MMF::AbstractVideoPlayer::handlePendingParametersChanged()
{
    if (m_pendingChanges)
        handleParametersChanged(m_pendingChanges);
    m_pendingChanges = 0;
}


//-----------------------------------------------------------------------------
// Metadata
//-----------------------------------------------------------------------------

int MMF::AbstractVideoPlayer::numberOfMetaDataEntries() const
{
    int numberOfEntries = 0;
    TRAP_IGNORE(numberOfEntries = m_player->NumberOfMetaDataEntriesL());
    return numberOfEntries;
}

QPair<QString, QString> MMF::AbstractVideoPlayer::metaDataEntry(int index) const
{
    CMMFMetaDataEntry *entry = 0;
    QT_TRAP_THROWING(entry = m_player->MetaDataEntryL(index));
    return QPair<QString, QString>(qt_TDesC2QString(entry->Name()), qt_TDesC2QString(entry->Value()));
}

QT_END_NAMESPACE

