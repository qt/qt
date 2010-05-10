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

#ifndef S60VIDEOPLAYERSESSION_H
#define S60VIDEOPLAYERSESSION_H

#include "s60mediaplayersession.h"
#include "s60mediaplayeraudioendpointselector.h"
#include <videoplayer.h>
#include <QtGui/qwidget.h>
#include <qvideowidget.h>

#include <AudioOutput.h>
#include <MAudioOutputObserver.h>

QT_BEGIN_NAMESPACE

class QTimer;

class S60VideoPlayerSession : public S60MediaPlayerSession,
                              public MVideoPlayerUtilityObserver,
                              public MVideoLoadingObserver,
                              public MAudioOutputObserver
{
    Q_OBJECT

public:
    S60VideoPlayerSession(QMediaService *service);
    ~S60VideoPlayerSession();

    //From S60MediaPlayerSession
    bool isVideoAvailable() const;
    bool isAudioAvailable() const;
    void setVideoRenderer(QObject *renderer);

    //From MVideoLoadingObserver
    void MvloLoadingStarted();
    void MvloLoadingComplete();

    // From MAudioOutputObserver
    void DefaultAudioOutputChanged(CAudioOutput& aAudioOutput,
        CAudioOutput::TAudioOutputPreference aNewDefault);

public:
    // From S60MediaPlayerAudioEndpointSelector
    QString activeEndpoint() const;
    QString defaultEndpoint() const;
public Q_SLOTS:
    void setActiveEndpoint(const QString& name);
Q_SIGNALS:
     void activeEndpointChanged(const QString &name);

protected:
    //From S60MediaPlayerSession
    void doLoadL(const TDesC &path);
    void doLoadUrlL(const TDesC &path);
    void doPlay();
    void doStop();
    void doPauseL();
    void doSetVolumeL(int volume);
    qint64 doGetPositionL() const;
    void doSetPositionL(qint64 microSeconds);
    void updateMetaDataEntriesL();
    int doGetBufferStatusL() const;
    qint64 doGetDurationL() const;
    void doSetAudioEndpoint(const QString& audioEndpoint);

private slots:
    void resetVideoDisplay();
    void suspendDirectScreenAccess();
    void resumeDirectScreenAccess();

private:
    bool resetNativeHandles();
    QPair<qreal, qreal> scaleFactor();
    void startDirectScreenAccess();
    bool stopDirectScreenAccess();
    QString qStringFromTAudioOutputPreference(CAudioOutput::TAudioOutputPreference output) const;


    // From MVideoPlayerUtilityObserver
    void MvpuoOpenComplete(TInt aError);
    void MvpuoPrepareComplete(TInt aError);
    void MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError);
    void MvpuoPlayComplete(TInt aError);
    void MvpuoEvent(const TMMFEvent &aEvent);

private:
    // Qwn
    CVideoPlayerUtility *m_player;
    TRect m_rect;
    QVideoOutputControl::Output m_output;
    WId m_windowId;
    bool m_dsaActive;
    bool m_dsaStopped;

    //Reference
    RWsSession &m_wsSession;
    CWsScreenDevice &m_screenDevice;
    RWindowBase *m_window;
    QMediaService &m_service;
    Qt::AspectRatioMode m_aspectRatioMode;
    QSize m_originalSize;
    CAudioOutput *m_audioOutput;
    QString m_audioEndpoint;
};

QT_END_NAMESPACE

#endif
