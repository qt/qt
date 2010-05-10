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

#ifndef S60AUDIOPLAYERSESSION_H
#define S60AUDIOPLAYERSESSION_H

#include "s60mediaplayersession.h"

#include <mdaaudiosampleplayer.h>
typedef CMdaAudioPlayerUtility CAudioPlayer;
typedef MMdaAudioPlayerCallback MAudioPlayerObserver;

#ifndef HAS_NO_AUDIOROUTING
#include <AudioOutput.h>
#include <MAudioOutputObserver.h>
#endif

QT_BEGIN_NAMESPACE

class S60AudioPlayerSession : public S60MediaPlayerSession
                            , public MAudioPlayerObserver
                            , public MAudioLoadingObserver
#ifndef HAS_NO_AUDIOROUTING
                            , public MAudioOutputObserver
#endif
{
    Q_OBJECT

public:
    S60AudioPlayerSession(QObject *parent);
    ~S60AudioPlayerSession();

    //From S60MediaPlayerSession
    bool isVideoAvailable() const;
    bool isAudioAvailable() const;

    // From MAudioLoadingObserver
    void MaloLoadingStarted();
    void MaloLoadingComplete();

#ifndef HAS_NO_AUDIOROUTING
    // From MAudioOutputObserver
    void DefaultAudioOutputChanged( CAudioOutput& aAudioOutput,
        CAudioOutput::TAudioOutputPreference aNewDefault );
#endif
public:
    // From S60MediaPlayerAudioEndpointSelector
    QString activeEndpoint() const;
    QString defaultEndpoint() const;
public Q_SLOTS:
    void setActiveEndpoint(const QString& name);
Q_SIGNALS:
    void activeEndpointChanged(const QString & name);

protected:
    //From S60MediaPlayerSession
    void doLoadL(const TDesC &path);
    void doLoadUrlL(const TDesC &path){Q_UNUSED(path)/*empty implementation*/}
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

private:
    void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    void MapcPlayComplete(TInt aError);
#ifndef HAS_NO_AUDIOROUTING
    QString qStringFromTAudioOutputPreference(CAudioOutput::TAudioOutputPreference output) const;
#endif
private:
    CAudioPlayer *m_player;
#ifndef HAS_NO_AUDIOROUTING
    CAudioOutput *m_audioOutput;
#endif
    QString m_audioEndpoint;
};

QT_END_NAMESPACE

#endif
