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

#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>
#include <QtGui/qwidget.h>

#include "qgstreamerplayerservice.h"
#include "qgstreamerplayercontrol.h"
#include "qgstreamerplayersession.h"
#include "qgstreamermetadataprovider.h"
#include "qgstreamervideooutputcontrol.h"

#include "qgstreamervideooverlay.h"
#include "qgstreamervideorenderer.h"

#include "qgstreamervideowidget.h"
//#include "qgstreamerstreamscontrol.h"

#include <qmediaplaylistnavigator.h>
#include <qmediaplaylist.h>


QT_BEGIN_NAMESPACE


QGstreamerPlayerService::QGstreamerPlayerService(QObject *parent):
     QMediaService(parent),
     m_videoRenderer(0),
     m_videoWindow(0),
     m_videoWidget(0)
{
    m_session = new QGstreamerPlayerSession(this);
    m_control = new QGstreamerPlayerControl(m_session, this);
    m_metaData = new QGstreamerMetaDataProvider(m_session, this);
    m_videoOutput = new QGstreamerVideoOutputControl(this);
//    m_streamsControl = new QGstreamerStreamsControl(m_session,this);

    connect(m_videoOutput, SIGNAL(outputChanged(QVideoOutputControl::Output)),
            this, SLOT(videoOutputChanged(QVideoOutputControl::Output)));
    m_videoRenderer = new QGstreamerVideoRenderer(this);

#ifdef Q_WS_X11
    m_videoWindow = new QGstreamerVideoOverlay(this);
    m_videoWidget = new QGstreamerVideoWidgetControl(this);
#endif

    QList<QVideoOutputControl::Output> outputs;

    if (m_videoRenderer)
        outputs << QVideoOutputControl::RendererOutput;
    if (m_videoWidget)
        outputs << QVideoOutputControl::WidgetOutput;
    if (m_videoWindow)
        outputs << QVideoOutputControl::WindowOutput;

    m_videoOutput->setAvailableOutputs(outputs);
}

QGstreamerPlayerService::~QGstreamerPlayerService()
{
}

QMediaControl *QGstreamerPlayerService::control(const char *name) const
{
    if (qstrcmp(name,QMediaPlayerControl_iid) == 0)
        return m_control;

    if (qstrcmp(name,QMetaDataControl_iid) == 0)
        return m_metaData;

//    if (qstrcmp(name,QMediaStreamsControl_iid) == 0)
//        return m_streamsControl;

    if (qstrcmp(name, QVideoOutputControl_iid) == 0)
        return m_videoOutput;

    if (qstrcmp(name, QVideoWidgetControl_iid) == 0)
        return m_videoWidget;

    if (qstrcmp(name, QVideoRendererControl_iid) == 0)
        return m_videoRenderer;

    if (qstrcmp(name, QVideoWindowControl_iid) == 0)
        return m_videoWindow;

    return 0;
}

void QGstreamerPlayerService::videoOutputChanged(QVideoOutputControl::Output output)
{
    switch (output) {
    case QVideoOutputControl::NoOutput:
        m_control->setVideoOutput(0);
        break;
    case QVideoOutputControl::RendererOutput:
        m_control->setVideoOutput(m_videoRenderer);
        break;
    case QVideoOutputControl::WindowOutput:
        m_control->setVideoOutput(m_videoWindow);
        break;
    case QVideoOutputControl::WidgetOutput:
        m_control->setVideoOutput(m_videoWidget);
        break;
    default:
        qWarning("Invalid video output selection");
        break;
    }
}

QT_END_NAMESPACE
