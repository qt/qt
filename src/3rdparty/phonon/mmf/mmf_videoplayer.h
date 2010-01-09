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

#ifndef PHONON_MMF_VIDEOPLAYER_H
#define PHONON_MMF_VIDEOPLAYER_H

#include <videoplayer.h> // from epoc32/include

#include "abstractmediaplayer.h"
#include "videooutput.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace MMF
{
/**
 * @short Wrapper over MMF video client utility
 *
 * See
 * <a href="http://wiki.forum.nokia.com/index.php/How_to_play_a_video_file_using_CVideoPlayerUtility">How to
 * play a video file using CVideoPlayerUtility</a>
 */
class VideoPlayer : public AbstractMediaPlayer
                  , public MVideoPlayerUtilityObserver
                  , public MVideoLoadingObserver
{
    Q_OBJECT

public:
    VideoPlayer(MediaObject *parent = 0, const AbstractPlayer *player = 0);
    virtual ~VideoPlayer();

    typedef CVideoPlayerUtility NativePlayer;
    NativePlayer *nativePlayer() const;

    // AbstractPlayer
    virtual void doPlay();
    virtual void doPause();
    virtual void doStop();
    virtual void doSeek(qint64 milliseconds);
    virtual int setDeviceVolume(int mmfVolume);
    virtual int openFile(RFile& file);
    virtual int openUrl(const QString& url);
    virtual int bufferStatus() const;
    virtual void close();

    // MediaObjectInterface
    virtual bool hasVideo() const;
    virtual qint64 currentTime() const;
    virtual qint64 totalTime() const;

    // AbstractPlayer
    virtual void videoOutputChanged();

    // AbstractMediaPlayer
    virtual int numberOfMetaDataEntries() const;
    virtual QPair<QString, QString> metaDataEntry(int index) const;

public Q_SLOTS:
    void videoWindowChanged();
    void aspectRatioChanged();
    void scaleModeChanged();
    void suspendDirectScreenAccess();
    void resumeDirectScreenAccess();

private:
    void construct();

    void doPrepareCompleteL(TInt aError);

    void getVideoWindow();
    void initVideoOutput();
    void updateVideoRect();

    void applyPendingChanges();
    void applyVideoWindowChange();

    void startDirectScreenAccess();
    bool stopDirectScreenAccess();

private:
    // MVideoPlayerUtilityObserver
    virtual void MvpuoOpenComplete(TInt aError);
    virtual void MvpuoPrepareComplete(TInt aError);
    virtual void MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError);
    virtual void MvpuoPlayComplete(TInt aError);
    virtual void MvpuoEvent(const TMMFEvent &aEvent);

    // MVideoLoadingObserver
    virtual void MvloLoadingStarted();
    virtual void MvloLoadingComplete();

private:
    QScopedPointer<NativePlayer>        m_player;

    // Not owned
    RWsSession&                         m_wsSession;
    CWsScreenDevice&                    m_screenDevice;
    RWindowBase*                        m_window;

    /* Extent of the video display - will be clipped to m_windowRect */
    TRect                               m_videoRect;

    TReal32                             m_scaleWidth;
    TReal32                             m_scaleHeight;

    QSize                               m_videoFrameSize;
    qint64                              m_totalTime;

    bool                                m_pendingChanges;
    bool                                m_dsaActive;
    bool                                m_dsaWasActive;

};

}
}

QT_END_NAMESPACE

#endif
