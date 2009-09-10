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

#include <QUrl>
#include <QTimer>
#include <QWidget>

#include <coemain.h>    // For CCoeEnv
#include <coecntrl.h>

#include "videoplayer.h"
#include "utils.h"

#ifdef _DEBUG
#include "objectdump/objectdump.h"
#endif

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::VideoPlayer::VideoPlayer()
        :   m_wsSession(NULL)
        ,   m_screenDevice(NULL)
        ,   m_window(NULL)
        ,   m_totalTime(0)
        ,   m_mmfOutputChangePending(false)
{
    construct();
}

MMF::VideoPlayer::VideoPlayer(const AbstractPlayer& player)
        :   AbstractMediaPlayer(player)
        ,   m_wsSession(NULL)
        ,   m_screenDevice(NULL)
        ,   m_window(NULL)
        ,   m_totalTime(0)
        ,   m_mmfOutputChangePending(false)
{
    construct();
}

void MMF::VideoPlayer::construct()
{
    TRACE_CONTEXT(VideoPlayer::VideoPlayer, EVideoApi);
    TRACE_ENTRY_0();

    if (!m_videoOutput) {
        m_dummyVideoOutput.reset(new VideoOutput(NULL));
    }

    videoOutput().setObserver(this);

    const TInt priority = 0;
    const TMdaPriorityPreference preference = EMdaPriorityPreferenceNone;

    getNativeWindowSystemHandles();

    // TODO: is this the correct way to handle errors which occur when
    // creating a Symbian object in the constructor of a Qt object?
    
    // TODO: check whether videoOutput is visible?  If not, then the 
    // corresponding window will not be active, meaning that the 
    // clipping region will be set to empty and the video will not be
    // visible.  If this is the case, we should set m_mmfOutputChangePending
    // and respond to future showEvents from the videoOutput widget.
    
    TRAPD(err,
          m_player.reset(CVideoPlayerUtility::NewL
                     (
                         *this,
                         priority, preference,
                         *m_wsSession, *m_screenDevice,
                         *m_window,
                         m_windowRect, m_clipRect
                     ))
         );

    if (KErrNone != err) {
        changeState(ErrorState);
    }

    TRACE_EXIT_0();
}

MMF::VideoPlayer::~VideoPlayer()
{
    TRACE_CONTEXT(VideoPlayer::~VideoPlayer, EVideoApi);
    TRACE_ENTRY_0();

    TRACE_EXIT_0();
}

//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------

void MMF::VideoPlayer::doPlay()
{
    TRACE_CONTEXT(VideoPlayer::doPlay, EVideoApi);
    
    // See comment in updateMmfOutput
    if(m_mmfOutputChangePending) {
        TRACE_0("MMF output change pending - pushing now");
        updateMmfOutput();
    }
    
    m_player->Play();
}

void MMF::VideoPlayer::doPause()
{
    TRACE_CONTEXT(VideoPlayer::doPause, EVideoApi);

    TRAPD(err, m_player->PauseL());
    if (KErrNone != err) {
        TRACE("PauseL error %d", err);
        setError(NormalError);
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

    if (KErrNone != err) {
        TRACE("SetPositionL error %d", err);
        setError(NormalError);
    }
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
        const_cast<VideoPlayer*>(this)->setError(NormalError);
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
        setError(NormalError);

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

        videoOutput().setFrameSize(m_frameSize);

        // See comment in updateMmfOutput
        if(m_mmfOutputChangePending) {
            TRACE_0("MMF output change pending - pushing now");
            updateMmfOutput();
        }       
        
        emit totalTimeChanged(totalTime());
        changeState(StoppedState);
    } else {
        setError(NormalError);
    }

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::doPrepareCompleteL(TInt aError)
{
    User::LeaveIfError(aError);

    // Get frame size
    TSize size;
    m_player->VideoFrameSizeL(size);
    m_frameSize = QSize(size.iWidth, size.iHeight);

    // Get duration
    m_totalTime = toMilliSeconds(m_player->DurationL());
}


void MMF::VideoPlayer::MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError)
{
    TRACE_CONTEXT(VideoPlayer::MvpuoFrameReady, EVideoApi);
    TRACE_ENTRY("state %d error %d", state(), aError);

    // TODO
    Q_UNUSED(aFrame);
    Q_UNUSED(aError);   // suppress warnings in release builds

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::MvpuoPlayComplete(TInt aError)
{
    TRACE_CONTEXT(VideoPlayer::MvpuoPlayComplete, EVideoApi)
    TRACE_ENTRY("state %d error %d", state(), aError);

    // TODO
    Q_UNUSED(aError);   // suppress warnings in release builds

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::MvpuoEvent(const TMMFEvent &aEvent)
{
    TRACE_CONTEXT(VideoPlayer::MvpuoEvent, EVideoApi);
    TRACE_ENTRY("state %d", state());

    // TODO
    Q_UNUSED(aEvent);

    TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// VideoOutputObserver
//-----------------------------------------------------------------------------

void MMF::VideoPlayer::videoOutputRegionChanged()
{
    TRACE_CONTEXT(VideoPlayer::videoOutputRegionChanged, EVideoInternal);
    TRACE_ENTRY("state %d", state());

    getNativeWindowSystemHandles();

    // See comment in updateMmfOutput
    if(state() == LoadingState)
        m_mmfOutputChangePending = true;
    else
        updateMmfOutput();
        
    TRACE_EXIT_0();
}

void MMF::VideoPlayer::updateMmfOutput()
{
    TRACE_CONTEXT(VideoPlayer::updateMmfOutput, EVideoInternal);
    TRACE_ENTRY_0();
    
    // Calling SetDisplayWindowL is a no-op unless the MMF controller has 
    // been loaded, so we shouldn't do it.  Instead, the
    // m_mmfOutputChangePending flag is used to record the fact that we
    // need to call SetDisplayWindowL, and this is checked in 
    // MvpuoPrepareComplete, at which point the MMF controller has been
    // loaded.
    
    // TODO: check whether videoOutput is visible?  If not, then the 
    // corresponding window will not be active, meaning that the 
    // clipping region will be set to empty and the video will not be
    // visible.  If this is the case, we should set m_mmfOutputChangePending
    // and respond to future showEvents from the videoOutput widget.
    
    getNativeWindowSystemHandles();

    TRAPD(err,
          m_player->SetDisplayWindowL
          (
              *m_wsSession, *m_screenDevice,
              *m_window,
              m_windowRect, m_clipRect
          )
         );

    if (KErrNone != err) {
        TRACE("SetDisplayWindowL error %d", err);
        setError(NormalError);
    }
    
    m_mmfOutputChangePending = false;
    
    TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

VideoOutput& MMF::VideoPlayer::videoOutput()
{
    TRACE_CONTEXT(VideoPlayer::videoOutput, EVideoInternal);
    TRACE("videoOutput 0x%08x dummy 0x%08x", m_videoOutput, m_dummyVideoOutput.data());

    return m_videoOutput ? *m_videoOutput : *m_dummyVideoOutput;
}

void MMF::VideoPlayer::videoOutputChanged()
{
    TRACE_CONTEXT(VideoPlayer::videoOutputChanged, EVideoInternal);
    TRACE_ENTRY_0();

    // Lazily construct a dummy output if needed here
    if (!m_videoOutput and m_dummyVideoOutput.isNull()) {
        m_dummyVideoOutput.reset(new VideoOutput(NULL));
    }

    videoOutput().setObserver(this);

    videoOutput().setFrameSize(m_frameSize);

    videoOutputRegionChanged();

    TRACE_EXIT_0();
}

void MMF::VideoPlayer::getNativeWindowSystemHandles()
{
    TRACE_CONTEXT(VideoPlayer::getNativeWindowSystemHandles, EVideoInternal);
    TRACE_ENTRY_0();

    VideoOutput& output = videoOutput();
    CCoeControl* const control = output.winId();
    
    CCoeEnv* const coeEnv = control->ControlEnv();
    m_wsSession = &(coeEnv->WsSession());
    m_screenDevice = coeEnv->ScreenDevice();
    m_window = control->DrawableWindow();
    
#ifdef _DEBUG
    QScopedPointer<ObjectDump::QDumper> dumper(new ObjectDump::QDumper);
    dumper->setPrefix("Phonon::MMF"); // to aid searchability of logs
    ObjectDump::addDefaultAnnotators(*dumper);
    TRACE_0("Dumping VideoOutput:");
    dumper->dumpObject(output);
#endif

#ifdef PHONON_MMF_HARD_CODE_VIDEO_RECT
    // HACK: why isn't control->Rect updated following a call to
    // updateGeometry on the parent widget?
    m_windowRect = TRect(0, 100, 320, 250);
    m_clipRect = m_windowRect;
#else    
    m_windowRect = TRect(
        control->DrawableWindow()->AbsPosition(),
        control->DrawableWindow()->Size());
    
    m_clipRect = m_windowRect;
#endif
    
#ifdef PHONON_MMF_HARD_CODE_VIDEO_RECT_TO_EMPTY
    m_windowRect = TRect(0, 0, 0, 0);
    m_clipRect = m_windowRect;
#endif
    
    TRACE("windowRect            %d %d - %d %d",
        m_windowRect.iTl.iX, m_windowRect.iTl.iY,
        m_windowRect.iBr.iX, m_windowRect.iBr.iY);
    TRACE("clipRect              %d %d - %d %d",
        m_clipRect.iTl.iX, m_clipRect.iTl.iY,
        m_clipRect.iBr.iX, m_clipRect.iBr.iY);
    
    TRACE_EXIT_0();
}


QT_END_NAMESPACE

