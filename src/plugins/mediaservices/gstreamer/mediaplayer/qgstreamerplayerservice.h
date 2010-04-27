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

#ifndef QGSTREAMERPLAYERSERVICE_H
#define QGSTREAMERPLAYERSERVICE_H

#include <QtCore/qobject.h>
#include <QtCore/qiodevice.h>

#include <QtMediaServices/qmediaservice.h>

#include "qgstreamervideooutputcontrol.h"


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QMediaMetaData;
class QMediaPlayerControl;
class QMediaPlaylist;
class QMediaPlaylistNavigator;
class QGstreamerMetaData;
class QGstreamerPlayerControl;
class QGstreamerPlayerSession;
class QGstreamerMetaDataProvider;
class QGstreamerStreamsControl;
class QGstreamerVideoRenderer;
class QGstreamerVideoOverlay;
class QGstreamerVideoWidgetControl;


class QGstreamerPlayerService : public QMediaService
{
    Q_OBJECT
public:
    QGstreamerPlayerService(QObject *parent = 0);
    ~QGstreamerPlayerService();

    //void setVideoOutput(QObject *output);

    QMediaControl *control(const char *name) const;

private slots:
    void videoOutputChanged(QVideoOutputControl::Output output);

private:
    QGstreamerPlayerControl *m_control;
    QGstreamerPlayerSession *m_session;
    QGstreamerMetaDataProvider *m_metaData;
    QGstreamerVideoOutputControl *m_videoOutput;
    QGstreamerStreamsControl *m_streamsControl;

    QGstreamerVideoRenderer *m_videoRenderer;
    QGstreamerVideoOverlay *m_videoWindow;
    QGstreamerVideoWidgetControl *m_videoWidget;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
