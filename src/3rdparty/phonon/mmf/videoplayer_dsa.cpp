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

#include <coecntrl.h> // for CCoeControl

#include <QApplication>    // for QApplication::activeWindow
#include <QtCore/private/qcore_symbian_p.h> // for qt_TRect2QRect

#include "utils.h"
#include "videooutput_dsa.h"
#include "videoplayer_dsa.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

// Two-phase constructor idiom is used because construct() calls virtual
// functions and therefore cannot be called from the AbstractVideoPlayer
// C++ constructor.
DsaVideoPlayer* DsaVideoPlayer::create(MediaObject *parent,
                                       const AbstractPlayer *player)
{
    QScopedPointer<DsaVideoPlayer> self(new DsaVideoPlayer(parent, player));
    self->construct();
    return self.take();
}

DsaVideoPlayer::DsaVideoPlayer(MediaObject *parent, const AbstractPlayer *player)
    :   AbstractVideoPlayer(parent, player)
    ,   m_dsaActive(false)
    ,   m_dsaWasActive(false)
{

}

DsaVideoPlayer::~DsaVideoPlayer()
{

}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

void MMF::DsaVideoPlayer::videoWindowScreenRectChanged()
{
    Q_ASSERT(m_videoOutput);

    QRect windowRect = static_cast<DsaVideoOutput *>(m_videoOutput)->videoWindowScreenRect();

    // Clip to physical window size
    // This is due to a defect in the layout when running on S60 3.2, which
    // results in the rectangle of the video widget extending outside the
    // screen in certain circumstances.  These include the initial startup
    // of the mediaplayer demo in portrait mode.  When this rectangle is
    // passed to the CVideoPlayerUtility, no video is rendered.
    const TSize screenSize = m_screenDevice.SizeInPixels();
    const QRect screenRect(0, 0, screenSize.iWidth, screenSize.iHeight);
    windowRect = windowRect.intersected(screenRect);

    // Recalculate scale factors.  Pass 'false' as second parameter in order to
    // suppress application of the change to the player - this is done at the end
    // of the function.
    updateScaleFactors(windowRect.size(), false);

    m_videoScreenRect = qt_QRect2TRect(windowRect);

    parametersChanged(WindowScreenRect | ScaleFactors);
}

void MMF::DsaVideoPlayer::suspendDirectScreenAccess()
{
    m_dsaWasActive = stopDirectScreenAccess();
}

void MMF::DsaVideoPlayer::resumeDirectScreenAccess()
{
    if (m_dsaWasActive) {
        startDirectScreenAccess();
        m_dsaWasActive = false;
    }
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void MMF::DsaVideoPlayer::createPlayer()
{
    // A window handle must be provided in order to construct
    // CVideoPlayerUtility.  If no VideoOutput has yet been connected to this
    // player, we temporarily use the top-level application window handle.
    // No video ever gets rendered into this window; SetDisplayWindowL is
    // always called before rendering actually begins.
    if (!m_window)
        m_window = QApplication::activeWindow()->effectiveWinId()->DrawableWindow();

    const TInt priority = 0;
    const TMdaPriorityPreference preference = EMdaPriorityPreferenceNone;

    CVideoPlayerUtility *player = 0;
    QT_TRAP_THROWING(player = CVideoPlayerUtility::NewL(*this,
                                     priority, preference,
                                     m_wsSession, m_screenDevice,
                                     *m_window,
                                     m_videoScreenRect, m_videoScreenRect));
    m_player.reset(player);

    // CVideoPlayerUtility::NewL starts DSA
    m_dsaActive = true;
}

void MMF::DsaVideoPlayer::initVideoOutput()
{
    Q_ASSERT(m_videoOutput);

    bool connected = connect(
        m_videoOutput, SIGNAL(videoWindowScreenRectChanged()),
        this, SLOT(videoWindowScreenRectChanged())
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

    // Suppress warnings in release builds
    Q_UNUSED(connected);

    AbstractVideoPlayer::initVideoOutput();
}

void MMF::DsaVideoPlayer::prepareCompleted()
{
    if (m_videoOutput)
        videoWindowScreenRectChanged();
}

void MMF::DsaVideoPlayer::handleVideoWindowChanged()
{
    if (!m_window) {
        if (QWidget *window = QApplication::activeWindow())
            m_window = window->effectiveWinId()->DrawableWindow();
        else
            m_window = 0;
        m_videoScreenRect = TRect();
    }

    parametersChanged(WindowHandle | WindowScreenRect);
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
    // Dump complete window tree
    session.LogCommand(RWsSession::ELoggingStatusDump);

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

#endif // QT_NO_DEBUG

void MMF::DsaVideoPlayer::handleParametersChanged(VideoParameters parameters)
{
    TRACE_CONTEXT(DsaVideoPlayer::handleParametersChanged, EVideoInternal);
    TRACE_ENTRY("parameters 0x%x", parameters.operator int());

    if (!m_window)
        return;

#ifndef QT_NO_DEBUG
    getDsaRegion(m_wsSession, *m_window);
#endif

    if (m_player) {
        static const TBool antialias = ETrue;
        int err = KErrNone;

        if (parameters & ScaleFactors) {
            TRAP(err, m_player->SetScaleFactorL(m_scaleWidth, m_scaleHeight,
                                                antialias));
            if(KErrNone != err) {
                TRACE("SetScaleFactorL (1) err %d", err);
                setError(tr("Video display error"), err);
            }
        }

        if (KErrNone == err) {
            if (parameters & WindowHandle || parameters & WindowScreenRect) {
                TRAP(err,
                    m_player->SetDisplayWindowL(m_wsSession, m_screenDevice,
                                                *m_window,
                                                m_videoScreenRect,
                                                m_videoScreenRect));
            }

            if (KErrNone != err) {
                TRACE("SetDisplayWindowL err %d", err);
                setError(tr("Video display error"), err);
            } else {
                m_dsaActive = true;
                if (parameters & ScaleFactors) {
                    TRAP(err, m_player->SetScaleFactorL(m_scaleWidth, m_scaleHeight,
                                                        antialias));
                    if (KErrNone != err) {
                        TRACE("SetScaleFactorL (2) err %d", err);
                        setError(tr("Video display error"), err);
                    }
                }
            }
        }
    }

    TRACE_EXIT_0();
}

void MMF::DsaVideoPlayer::startDirectScreenAccess()
{
    TRACE_CONTEXT(DsaVideoPlayer::startDirectScreenAccess, EVideoInternal);
    TRACE_ENTRY("dsaActive %d", m_dsaActive);

    int err = KErrNone;

    if (!m_dsaActive) {
        TRAP(err, m_player->StartDirectScreenAccessL());
        if (KErrNone == err)
            m_dsaActive = true;
        else
            setError(tr("Video display error"), err);
    }

    if (m_videoOutput)
        m_videoOutput->dump();

    TRACE_EXIT("error %d", err);
}

bool MMF::DsaVideoPlayer::stopDirectScreenAccess()
{
    TRACE_CONTEXT(DsaVideoPlayer::stopDirectScreenAccess, EVideoInternal);
    TRACE_ENTRY("dsaActive %d", m_dsaActive);

    int err = KErrNone;

    const bool dsaWasActive = m_dsaActive;
    if (m_dsaActive) {
        TRAP(err, m_player->StopDirectScreenAccessL());
        if (KErrNone == err)
            m_dsaActive = false;
        else
            setError(tr("Video display error"), err);
    }

    TRACE_EXIT("error %d", err);

    return dsaWasActive;
}

QT_END_NAMESPACE

