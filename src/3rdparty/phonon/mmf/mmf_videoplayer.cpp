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

#include <QApplication>    // for QApplication::activeWindow
#include <QUrl>
#include <QTimer>
#include <QWidget>

#include <coemain.h>    // for CCoeEnv
#include <coecntrl.h>

#include "mmf_videoplayer.h"
#include "utils.h"

#ifndef QT_NO_DEBUG
#include "objectdump.h"
#endif

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::VideoPlayer
  \internal
*/

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::VideoPlayer::VideoPlayer(MediaObject *parent, const AbstractPlayer *player)
        :   AbstractMediaPlayer(parent, player)
        ,   m_wsSession(CCoeEnv::Static()->WsSession())
        ,   m_screenDevice(*CCoeEnv::Static()->ScreenDevice())
        ,   m_window(0)
        ,   m_totalTime(0)
        ,   m_pendingChanges(false)
        ,   m_dsaActive(false)
{
    construct();
}

void MMF::VideoPlayer::construct()
{
    TRACE_CONTEXT(VideoPlayer::VideoPlayer, EVideoApi);
    TRACE_ENTRY_0();

    getVideoWindow();

    const TInt priority = 0;
    const TMdaPriorityPreference preference = EMdaPriorityPreferenceNone;

    CVideoPlayerUtility *player = 0;
    QT_TRAP_THROWING(player = CVideoPlayerUtility::NewL
            (
                 *this,
                 priority, preference,
                 m_wsSession, m_screenDevice,
                 *m_window,
                 m_videoRect, m_videoRect
            )
        );
    m_player.reset(player);

    // CVideoPlayerUtility::NewL starts DSA
    m_dsaActive = true;

    m_player->RegisterForVideoLoadingNotification(*this);

    TRACE_EXIT_0();
}

MMF::VideoPlayer::~VideoPlayer()
{
    TRACE_CONTEXT(VideoPlayer::~VideoPlayer, EVideoApi);
    TRACE_ENTRY_0();

    // QObject destructor removes all signal-slot connections involving this
    // object, so we do not need to disconnect from m_videoOutput here.

    TRACE_EXIT_0();
}

CVideoPlayerUtility* MMF::VideoPlayer::nativePlayer() const
{
    return m_player.data();
}

//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------

void MMF::VideoPlayer::doPlay()
{
    TRACE_CONTEXT(VideoPlayer::doPlay, EVideoApi);

    applyPendingChanges();

    m_player->Play();
}

void MMF::VideoPlayer::doPause()
{
    TRACE_CONTEXT(VideoPlayer::doPause, EVideoApi);

    TRAPD(err, m_player->PauseL());
    if (KErrNone != err && state() != ErrorState) {
        TRACE("PauseL error %d", err);
        setError(tr("Pause failed"), err);
    }
}

void MMF::VideoPlayer::doStop()
{
    m_player->Stop();
}

void MMF::VideoPlayer::doSeek(qint64 ms)
{
    TRACE_CONTEXT(VideoPlayer::doSeek, EVideoApi);

    TRAPD(err, m_player->SetPositionL(TTimeIntervalMicroSeconds(ms * 1000)));

    if(KErrNone != err)
        setError(tr("Seek failed"), err);
}

int MMF::VideoPlayer::setDeviceVolume(int mmfVolume)
{
    TRAPD(err, m_player->SetVolumeL(mmfVolume));
    return err;
}

int MMF::VideoPlayer::openFile(RFile& file)
{
    TRAPD(err, m_player->OpenFileL(file));
    return err;
}

int MMF::VideoPlayer::openUrl(const QString& url)
{
    TRAPD(err, m_player->OpenUrlL(qt_QString2TPtrC(url)));
    return err;
}

int MMF::VideoPlayer::bufferStatus() const
{
    int result = 0;
    TRAP_IGNORE(m_player->GetVideoLoadingProgressL(result));
    return result;
}

void MMF::VideoPlayer::close()
{
    m_player->Close();
}

bool MMF::VideoPlayer::hasVideo() const
{
    return true;
}

qint64 MMF::VideoPlayer::currentTime() const
{
    TRACE_CONTEXT(VideoPlayer::currentTime, EVideoApi);

    TTimeIntervalMicroSeconds us;
    TRAPD(err, us = m_player->PositionL())

    qint64 result = 0;

    if (KErrNone == err) {
        result = toMilliSeconds(us);
    } else {
        TRACE("PositionL error %d", err);

        // If we don't cast away constness here, we simply have to ignore
        // the error.
        const_cast<VideoPlayer*>(this)->setError(tr("Getting position failed"), err);
    }

    return result;
}

qint64 MMF::VideoPlayer::totalTime() const
{
    return m_totalTime;
}


//-----------------------------------------------------------------------------
// MVideoPlayerUtilityObserver callbacks
//-----------------------------------------------------------------------------

void MMF::VideoPlayer::MvpuoOpenComplete(TInt aError)
{
    TRACE_CONTEXT(VideoPlayer::MvpuoOpenComplete, EVideoApi);
    TRACE_ENTRY("state %d error %d", state(), aError);

    __ASSERT_ALWAYS(LoadingState == state(), Utils::panic(InvalidStatePanic));

    if (KErrNone == aError)
        m_player->Prepare();
    else
        setError(tr("Opening clip failed"), aError);

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::MvpuoPrepareComplete(TInt aError)
{
    TRACE_CONTEXT(VideoPlayer::MvpuoPrepareComplete, EVideoApi);
    TRACE_ENTRY("state %d error %d", state(), aError);

    __ASSERT_ALWAYS(LoadingState == state(), Utils::panic(InvalidStatePanic));

    TRAPD(err, doPrepareCompleteL(aError));

    if (KErrNone == err) {
        maxVolumeChanged(m_player->MaxVolume());

        if (m_videoOutput)
            m_videoOutput->setVideoSize(m_videoFrameSize);

        updateVideoRect();
        applyPendingChanges();

        emit totalTimeChanged(totalTime());
        changeState(StoppedState);
    } else {
        setError(tr("Buffering clip failed"), err);
    }

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::doPrepareCompleteL(TInt aError)
{
    User::LeaveIfError(aError);

    // Get frame size
    TSize size;
    m_player->VideoFrameSizeL(size);
    m_videoFrameSize = QSize(size.iWidth, size.iHeight);

    // Get duration
    m_totalTime = toMilliSeconds(m_player->DurationL());
}


void MMF::VideoPlayer::MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError)
{
    TRACE_CONTEXT(VideoPlayer::MvpuoFrameReady, EVideoApi);
    TRACE_ENTRY("state %d error %d", state(), aError);

    Q_UNUSED(aFrame);
    Q_UNUSED(aError);   // suppress warnings in release builds

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::MvpuoPlayComplete(TInt aError)
{
    TRACE_CONTEXT(VideoPlayer::MvpuoPlayComplete, EVideoApi)
    TRACE_ENTRY("state %d error %d", state(), aError);

    // Call base class function which handles end of playback for both
    // audio and video clips.
    playbackComplete(aError);

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::MvpuoEvent(const TMMFEvent &aEvent)
{
    TRACE_CONTEXT(VideoPlayer::MvpuoEvent, EVideoApi);
    TRACE_ENTRY("state %d", state());

    Q_UNUSED(aEvent);

    TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// MVideoLoadingObserver callbacks
//-----------------------------------------------------------------------------

void MMF::VideoPlayer::MvloLoadingStarted()
{
    bufferingStarted();
}

void MMF::VideoPlayer::MvloLoadingComplete()
{
    bufferingComplete();
}


//-----------------------------------------------------------------------------
// Video window updates
//-----------------------------------------------------------------------------

void MMF::VideoPlayer::getVideoWindow()
{
    TRACE_CONTEXT(VideoPlayer::getVideoWindow, EVideoInternal);
    TRACE_ENTRY_0();

    if(m_videoOutput) {
        // Dump information to log, only in debug builds
        m_videoOutput->dump();

        initVideoOutput();
        videoWindowChanged();
    } else
        // Top-level window
        m_window = QApplication::activeWindow()->effectiveWinId()->DrawableWindow();

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::videoOutputChanged()
{
    TRACE_CONTEXT(VideoPlayer::videoOutputChanged, EVideoInternal);
    TRACE_ENTRY_0();

    if (m_videoOutput) {
        initVideoOutput();
        videoWindowChanged();
    }

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::initVideoOutput()
{
    m_videoOutput->winId();
    m_videoOutput->setVideoSize(m_videoFrameSize);

    bool connected = connect(
        m_videoOutput, SIGNAL(videoWindowChanged()),
        this, SLOT(videoWindowChanged())
    );
    Q_ASSERT(connected);

    connected = connect(
        m_videoOutput, SIGNAL(beginVideoWindowNativePaint()),
        this, SLOT(suspendDirectScreenAccess())
    );
    Q_ASSERT(connected);

    connected = connect(
        m_videoOutput, SIGNAL(endVideoWindowNativePaint()),
        this, SLOT(resumeDirectScreenAccess())
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
}

void MMF::VideoPlayer::videoWindowChanged()
{
    TRACE_CONTEXT(VideoPlayer::videoOutputRegionChanged, EVideoInternal);
    TRACE_ENTRY("state %d", state());

    m_window = m_videoOutput->videoWindow();
    updateVideoRect();

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::suspendDirectScreenAccess()
{
    m_dsaWasActive = stopDirectScreenAccess();
}

void MMF::VideoPlayer::resumeDirectScreenAccess()
{
    if(m_dsaWasActive) {
        startDirectScreenAccess();
        m_dsaWasActive = false;
    }
}

void MMF::VideoPlayer::startDirectScreenAccess()
{
    if(!m_dsaActive) {
        TRAPD(err, m_player->StartDirectScreenAccessL());
        if(KErrNone == err)
            m_dsaActive = true;
        else
            setError(tr("Video display error"), err);
    }
}

bool MMF::VideoPlayer::stopDirectScreenAccess()
{
    const bool dsaWasActive = m_dsaActive;
    if(m_dsaActive) {
        TRAPD(err, m_player->StopDirectScreenAccessL());
        if(KErrNone == err)
            m_dsaActive = false;
        else
            setError(tr("Video display error"), err);
    }
    return dsaWasActive;
}

// Helper function for aspect ratio / scale mode handling
QSize scaleToAspect(const QSize& srcRect, int aspectWidth, int aspectHeight)
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

void MMF::VideoPlayer::updateVideoRect()
{
    QRect videoRect;
    QRect windowRect = m_videoOutput->videoWindowRect();

    // Clip to physical window size
    // This is due to a defect in the layout when running on S60 3.2, which
    // results in the rectangle of the video widget extending outside the
    // screen in certain circumstances.  These include the initial startup
    // of the mediaplayer demo in portrait mode.  When this rectangle is
    // passed to the CVideoPlayerUtility, no video is rendered.
    const TSize screenSize = m_screenDevice.SizeInPixels();
    const QRect screenRect(0, 0, screenSize.iWidth, screenSize.iHeight);
    windowRect = windowRect.intersected(screenRect);

    const QSize windowSize = windowRect.size();

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

    switch(m_videoOutput->scaleMode()) {
    case Phonon::VideoWidget::ScaleAndCrop:
        if(videoHeight < windowHeight) {
            videoWidth *= windowToVideoHeightRatio;
            videoHeight = windowHeight;
        }
        break;
    case Phonon::VideoWidget::FitInView:
    default:
        if(videoHeight > windowHeight) {
            videoWidth *= windowToVideoHeightRatio;
            videoHeight = windowHeight;
        }
        break;
    }

    // Calculate scale factors
    m_scaleWidth = 100.0f * videoWidth / m_videoFrameSize.width();
    m_scaleHeight = 100.0f * videoHeight / m_videoFrameSize.height();

    m_videoRect = qt_QRect2TRect(windowRect);

    if (state() == LoadingState)
        m_pendingChanges = true;
    else {
        applyVideoWindowChange();
        m_pendingChanges = false;
    }
}

void MMF::VideoPlayer::aspectRatioChanged()
{
    TRACE_CONTEXT(VideoPlayer::aspectRatioChanged, EVideoInternal);
    TRACE_ENTRY("state %d aspectRatio %d", state());

    updateVideoRect();

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::scaleModeChanged()
{
    TRACE_CONTEXT(VideoPlayer::scaleModeChanged, EVideoInternal);
    TRACE_ENTRY("state %d", state());

    updateVideoRect();

    TRACE_EXIT_0();
}

#ifndef QT_NO_DEBUG

// The following code is for debugging problems related to video visibility.  It allows
// the VideoPlayer instance to query the window server in order to determine the
// DSA drawing region for the video window.

class CDummyAO : public CActive
{
public:
    CDummyAO() : CActive(CActive::EPriorityStandard) { CActiveScheduler::Add(this); }
    void RunL() { }
    void DoCancel() { }
    TRequestStatus& Status() { return iStatus; }
    void SetActive() { CActive::SetActive(); }
};

void getDsaRegion(RWsSession &session, const RWindowBase &window)
{
    RDirectScreenAccess dsa(session);
    TInt err = dsa.Construct();
    CDummyAO ao;
    RRegion* region;
    err = dsa.Request(region, ao.Status(), window);
    ao.SetActive();
    dsa.Close();
    ao.Cancel();
    if (region) {
        qDebug() << "Phonon::MMF::getDsaRegion count" << region->Count();
        for (int i=0; i<region->Count(); ++i) {
            const TRect& rect = region->RectangleList()[i];
            qDebug() << "Phonon::MMF::getDsaRegion rect"
                << rect.iTl.iX << rect.iTl.iY << rect.iBr.iX << rect.iBr.iY;
        }
        region->Close();
    }
}

#endif // _DEBUG

void MMF::VideoPlayer::applyPendingChanges()
{
    if(m_pendingChanges)
        applyVideoWindowChange();

    m_pendingChanges = false;
}

void MMF::VideoPlayer::applyVideoWindowChange()
{
    TRACE_CONTEXT(VideoPlayer::applyVideoWindowChange, EVideoInternal);
    TRACE_ENTRY_0();

#ifndef QT_NO_DEBUG
    getDsaRegion(m_wsSession, *m_window);
#endif

    static const TBool antialias = ETrue;

    TRAPD(err, m_player->SetScaleFactorL(m_scaleWidth, m_scaleHeight, antialias));
    if(KErrNone != err) {
        TRACE("SetScaleFactorL (1) err %d", err);
        setError(tr("Video display error"), err);
    }

    if(KErrNone == err) {
        TRAP(err,
            m_player->SetDisplayWindowL
                (
                    m_wsSession, m_screenDevice,
                    *m_window,
                    m_videoRect, m_videoRect
                )
            );

        if (KErrNone != err) {
            TRACE("SetDisplayWindowL err %d", err);
            setError(tr("Video display error"), err);
        } else {
            m_dsaActive = true;
            TRAP(err, m_player->SetScaleFactorL(m_scaleWidth, m_scaleHeight, antialias));
            if(KErrNone != err) {
                TRACE("SetScaleFactorL (2) err %d", err);
                setError(tr("Video display error"), err);
            }
        }
    }

    TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// Metadata
//-----------------------------------------------------------------------------

int MMF::VideoPlayer::numberOfMetaDataEntries() const
{
    int numberOfEntries = 0;
    TRAP_IGNORE(numberOfEntries = m_player->NumberOfMetaDataEntriesL());
    return numberOfEntries;
}

QPair<QString, QString> MMF::VideoPlayer::metaDataEntry(int index) const
{
    CMMFMetaDataEntry *entry = 0;
    QT_TRAP_THROWING(entry = m_player->MetaDataEntryL(index));
    return QPair<QString, QString>(qt_TDesC2QString(entry->Name()), qt_TDesC2QString(entry->Value()));
}

QT_END_NAMESPACE

