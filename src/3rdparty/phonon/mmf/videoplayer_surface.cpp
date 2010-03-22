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

#include <videoplayer2.h>

#include <QtCore/private/qcore_symbian_p.h> // for qt_QRect2TRect

#include "utils.h"
#include "videooutput_surface.h"
#include "videoplayer_surface.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

// Two-phase constructor idiom is used because construct() calls virtual
// functions and therefore cannot be called from the AbstractVideoPlayer
// C++ constructor.
SurfaceVideoPlayer* SurfaceVideoPlayer::create(MediaObject *parent,
                                       const AbstractPlayer *player)
{
    QScopedPointer<SurfaceVideoPlayer> self(new SurfaceVideoPlayer(parent, player));
    self->construct();
    return self.take();
}

SurfaceVideoPlayer::SurfaceVideoPlayer(MediaObject *parent, const AbstractPlayer *player)
    :   AbstractVideoPlayer(parent, player)
    ,   m_displayWindow(0)
{

}

SurfaceVideoPlayer::~SurfaceVideoPlayer()
{

}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

void MMF::SurfaceVideoPlayer::videoWindowSizeChanged()
{
    updateScaleFactors(m_videoOutput->videoWindowSize());
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void MMF::SurfaceVideoPlayer::createPlayer()
{
    const TInt priority = 0;
    const TMdaPriorityPreference preference = EMdaPriorityPreferenceNone;

    CVideoPlayerUtility2 *player = 0;
    QT_TRAP_THROWING(player = CVideoPlayerUtility2::NewL(*this,
                                         priority, preference));
    m_player.reset(player);
}

void MMF::SurfaceVideoPlayer::initVideoOutput()
{
    bool connected = connect(
        m_videoOutput, SIGNAL(videoWindowSizeChanged()),
        this, SLOT(videoWindowSizeChanged())
    );
    Q_ASSERT(connected);

    // Suppress warnings in release builds
    Q_UNUSED(connected);

    AbstractVideoPlayer::initVideoOutput();
}

void MMF::SurfaceVideoPlayer::prepareCompleted()
{
    videoWindowSizeChanged();
}

void MMF::SurfaceVideoPlayer::handleVideoWindowChanged()
{
    parametersChanged(WindowHandle);
}

void MMF::SurfaceVideoPlayer::handleParametersChanged(VideoParameters parameters)
{
    CVideoPlayerUtility2 *player = static_cast<CVideoPlayerUtility2 *>(m_player.data());

    int err = KErrNone;

    TRect rect;

    if (m_videoOutput) {
        m_videoOutput->dump();
        const QSize size = m_videoOutput->videoWindowSize();
        rect.SetSize(TSize(size.width(), size.height()));
    }

    if (parameters & WindowHandle) {
        if (m_displayWindow)
            player->RemoveDisplayWindow(*m_displayWindow);

        RWindow *window = static_cast<RWindow *>(m_window);
        if (window) {
            window->SetBackgroundColor(TRgb(0, 0, 0, 255));
            TRAP(err, player->AddDisplayWindowL(m_wsSession, m_screenDevice, *window, rect, rect));
            if (KErrNone != err) {
                setError(tr("Video display error"), err);
                window = 0;
            }
        }
        m_displayWindow = window;
    }

    if (KErrNone == err) {
        if (parameters & ScaleFactors) {
            Q_ASSERT(m_displayWindow);
            TRAP(err, player->SetVideoExtentL(*m_displayWindow, rect));
            if (KErrNone == err)
                TRAP(err, player->SetWindowClipRectL(*m_displayWindow, rect));
            if (KErrNone == err)
                TRAP(err, player->SetScaleFactorL(*m_displayWindow, m_scaleWidth, m_scaleHeight));
            if (KErrNone != err)
                setError(tr("Video display error"), err);
        }
    }
}

QT_END_NAMESPACE

