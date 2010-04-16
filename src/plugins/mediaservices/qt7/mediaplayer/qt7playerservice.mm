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

#include "qt7backend.h"
#include "qt7playerservice.h"
#include "qt7playercontrol.h"
#include "qt7playersession.h"
#include "qt7videooutputcontrol.h"
#include "qt7movieviewoutput.h"
#include "qt7movieviewrenderer.h"
#include "qt7movierenderer.h"
#include "qt7movievideowidget.h"
#include "qt7playermetadata.h"

#include <QtMediaServices/qmediaplaylistnavigator.h>
#include <QtMediaServices/qmediaplaylist.h>

QT_BEGIN_NAMESPACE

QT7PlayerService::QT7PlayerService(QObject *parent):
    QMediaService(parent)
{
    m_session = new QT7PlayerSession(this);

    m_control = new QT7PlayerControl(this);
    m_control->setSession(m_session);

    m_playerMetaDataControl = new QT7PlayerMetaDataControl(m_session, this);
    connect(m_control, SIGNAL(mediaChanged(QMediaContent)), m_playerMetaDataControl, SLOT(updateTags()));

    m_videoOutputControl = new QT7VideoOutputControl(this);

    m_videoWidnowControl = 0;
    m_videoWidgetControl = 0;
    m_videoRendererControl = 0;

#if defined(QT_MAC_USE_COCOA)
    m_videoWidnowControl = new QT7MovieViewOutput(this);
    m_videoOutputControl->enableOutput(QVideoOutputControl::WindowOutput);
//    qDebug() << "Using cocoa";
#endif

#ifdef QUICKTIME_C_API_AVAILABLE
    m_videoRendererControl = new QT7MovieRenderer(this);
    m_videoOutputControl->enableOutput(QVideoOutputControl::RendererOutput);

    m_videoWidgetControl = new QT7MovieVideoWidget(this);
    m_videoOutputControl->enableOutput(QVideoOutputControl::WidgetOutput);
//    qDebug() << "QuickTime C API is available";
#else
    m_videoRendererControl = new QT7MovieViewRenderer(this);
    m_videoOutputControl->enableOutput(QVideoOutputControl::RendererOutput);
//    qDebug() << "QuickTime C API is not available";
#endif


    connect(m_videoOutputControl, SIGNAL(videoOutputChanged(QVideoOutputControl::Output)),
            this, SLOT(updateVideoOutput()));
}

QT7PlayerService::~QT7PlayerService()
{
    m_session->setVideoOutput(0);
}

QMediaControl *QT7PlayerService::control(const char *name) const
{
    if (qstrcmp(name, QMediaPlayerControl_iid) == 0)
        return m_control;

    if (qstrcmp(name, QVideoOutputControl_iid) == 0)
        return m_videoOutputControl;

    if (qstrcmp(name, QVideoWindowControl_iid) == 0)
        return m_videoWidnowControl;

    if (qstrcmp(name, QVideoRendererControl_iid) == 0)
        return m_videoRendererControl;

    if (qstrcmp(name, QVideoWidgetControl_iid) == 0)
        return m_videoWidgetControl;

    if (qstrcmp(name, QMetaDataControl_iid) == 0)
        return m_playerMetaDataControl;

    return 0;
}

void QT7PlayerService::updateVideoOutput()
{
//    qDebug() << "QT7PlayerService::updateVideoOutput" << m_videoOutputControl->output();

    switch (m_videoOutputControl->output()) {
    case QVideoOutputControl::WindowOutput:
        m_session->setVideoOutput(m_videoWidnowControl);
        break;
    case QVideoOutputControl::RendererOutput:
        m_session->setVideoOutput(m_videoRendererControl);
        break;
    case QVideoOutputControl::WidgetOutput:
        m_session->setVideoOutput(m_videoWidgetControl);
        break;
    default:
        m_session->setVideoOutput(0);
    }
}

QT_END_NAMESPACE

#include "moc_qt7playerservice.cpp"
