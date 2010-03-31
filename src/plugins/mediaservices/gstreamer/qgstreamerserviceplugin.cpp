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

#include <QtCore/qstring.h>
#include <QtCore/qdebug.h>
#include <QtGui/QIcon>
#include <QtCore/QDir>

#include "qgstreamerserviceplugin.h"

#ifdef QMEDIA_GSTREAMER_PLAYER
#include "qgstreamerplayerservice.h"
#endif
#ifdef QMEDIA_GSTREAMER_CAPTURE
#include "qgstreamercaptureservice.h"
#endif
#include <QtMultimedia/qmediaserviceprovider.h>

#ifdef QMEDIA_GSTREAMER_CAPTURE
#include <linux/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#endif


QT_BEGIN_NAMESPACE


QStringList QGstreamerServicePlugin::keys() const
{
    return QStringList()
#ifdef QMEDIA_GSTREAMER_PLAYER
            << QLatin1String(Q_MEDIASERVICE_MEDIAPLAYER)
#endif
#ifdef QMEDIA_GSTREAMER_CAPTURE
            << QLatin1String(Q_MEDIASERVICE_AUDIOSOURCE)
            << QLatin1String(Q_MEDIASERVICE_CAMERA)
#endif
            ;
}

QMediaService* QGstreamerServicePlugin::create(const QString &key)
{
#ifdef QMEDIA_GSTREAMER_PLAYER
    if (key == QLatin1String(Q_MEDIASERVICE_MEDIAPLAYER))
        return new QGstreamerPlayerService;
#endif
#ifdef QMEDIA_GSTREAMER_CAPTURE
    if (key == QLatin1String(Q_MEDIASERVICE_AUDIOSOURCE))
        return new QGstreamerCaptureService(key);

    if (key == QLatin1String(Q_MEDIASERVICE_CAMERA))
        return new QGstreamerCaptureService(key);
#endif

    qWarning() << "GStreamer service plugin: unsupported service -" << key;
    return 0;
}

void QGstreamerServicePlugin::release(QMediaService *service)
{
    delete service;
}

QList<QByteArray> QGstreamerServicePlugin::devices(const QByteArray &service) const
{
#ifdef QMEDIA_GSTREAMER_CAPTURE
    if (service == Q_MEDIASERVICE_CAMERA) {
        if (m_cameraDevices.isEmpty())
            updateDevices();

        return m_cameraDevices;
    }
#endif

    return QList<QByteArray>();
}

QString QGstreamerServicePlugin::deviceDescription(const QByteArray &service, const QByteArray &device)
{
#ifdef QMEDIA_GSTREAMER_CAPTURE
    if (service == Q_MEDIASERVICE_CAMERA) {
        if (m_cameraDevices.isEmpty())
            updateDevices();

        for (int i=0; i<m_cameraDevices.count(); i++)
            if (m_cameraDevices[i] == device)
                return m_cameraDescriptions[i];
    }
#endif

    return QString();
}

void QGstreamerServicePlugin::updateDevices() const
{
#ifdef QMEDIA_GSTREAMER_CAPTURE
    m_cameraDevices.clear();
    m_cameraDescriptions.clear();

    QDir devDir("/dev");
    devDir.setFilter(QDir::System);

    QFileInfoList entries = devDir.entryInfoList(QStringList() << "video*");

    foreach( const QFileInfo &entryInfo, entries ) {
//        qDebug() << "Try" << entryInfo.filePath();

        int fd = ::open(entryInfo.filePath().toLatin1().constData(), O_RDWR );
        if (fd == -1)
            continue;

        bool isCamera = false;

        v4l2_input input;
        memset(&input, 0, sizeof(input));
        for (; ::ioctl(fd, VIDIOC_ENUMINPUT, &input) >= 0; ++input.index) {
            if(input.type == V4L2_INPUT_TYPE_CAMERA || input.type == 0) {
                isCamera = ::ioctl(fd, VIDIOC_S_INPUT, input.index) != 0;
                break;
            }
        }

        if (isCamera) {
            // find out its driver "name"
            QString name;
            struct v4l2_capability vcap;
            memset(&vcap, 0, sizeof(struct v4l2_capability));

            if (ioctl(fd, VIDIOC_QUERYCAP, &vcap) != 0)
                name = entryInfo.fileName();
            else
                name = QString((const char*)vcap.card);
//            qDebug() << "found camera: " << name;

            m_cameraDevices.append(entryInfo.filePath().toLocal8Bit());
            m_cameraDescriptions.append(name);
        }
        ::close(fd);
    }
#endif
}

Q_EXPORT_PLUGIN2(gstengine, QGstreamerServicePlugin);

QT_END_NAMESPACE
