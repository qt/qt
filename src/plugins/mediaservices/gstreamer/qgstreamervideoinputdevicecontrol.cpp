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

#include "qgstreamervideoinputdevicecontrol.h"

#include <QtGui/QIcon>
#include <QtCore/QDir>
#include <QtCore/QDebug>

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


QT_BEGIN_NAMESPACE

QGstreamerVideoInputDeviceControl::QGstreamerVideoInputDeviceControl(QObject *parent)
    :QVideoDeviceControl(parent), m_selectedDevice(0)
{
    update();
}

QGstreamerVideoInputDeviceControl::~QGstreamerVideoInputDeviceControl()
{
}

int QGstreamerVideoInputDeviceControl::deviceCount() const
{
    return m_names.size();
}

QString QGstreamerVideoInputDeviceControl::deviceName(int index) const
{
    return m_names[index];
}

QString QGstreamerVideoInputDeviceControl::deviceDescription(int index) const
{
    return m_descriptions[index];
}

QIcon QGstreamerVideoInputDeviceControl::deviceIcon(int index) const
{
    Q_UNUSED(index);
    return QIcon();
}

int QGstreamerVideoInputDeviceControl::defaultDevice() const
{
    return 0;
}

int QGstreamerVideoInputDeviceControl::selectedDevice() const
{
    return m_selectedDevice;
}


void QGstreamerVideoInputDeviceControl::setSelectedDevice(int index)
{
    if (index != m_selectedDevice) {
        m_selectedDevice = index;
        emit selectedDeviceChanged(index);
        emit selectedDeviceChanged(deviceName(index));
    }
}


void QGstreamerVideoInputDeviceControl::update()
{
    m_names.clear();
    m_descriptions.clear();

#ifdef QMEDIA_GSTREAMER_CAPTURE
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

            m_names.append(entryInfo.filePath());
            m_descriptions.append(name);
        }
        ::close(fd);
    }
#endif
}

QT_END_NAMESPACE

