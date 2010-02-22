/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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

#include <QtMultimedia/qaudioengineplugin.h>
#include <QtMultimedia/qaudioengine.h>

#include <qstringlist.h>
#include <qiodevice.h>
#include <qbytearray.h>
#include <qdebug.h>

#include "symbianaudiodeviceinfo.h"
#include "symbianaudioinput.h"
#include "symbianaudiooutput.h"

QT_BEGIN_NAMESPACE

class SymbianAudioPlugin : public QAudioEnginePlugin
{
public:
    SymbianAudioPlugin(QObject *parent = 0);
    ~SymbianAudioPlugin();

    QStringList keys() const;

    QList<QByteArray> availableDevices(QAudio::Mode) const;
    QAbstractAudioInput* createInput(const QByteArray& device,
                             const QAudioFormat& format = QAudioFormat());
    QAbstractAudioOutput* createOutput(const QByteArray& device,
                              const QAudioFormat& format = QAudioFormat());
    QAbstractAudioDeviceInfo* createDeviceInfo(const QByteArray& device,
                                               QAudio::Mode mode);
};

SymbianAudioPlugin::SymbianAudioPlugin(QObject *parent)
    :   QAudioEnginePlugin(parent)
{

}

SymbianAudioPlugin::~SymbianAudioPlugin()
{

}

QStringList SymbianAudioPlugin::keys() const
{
    QStringList keys(QLatin1String("default"));
    keys << QLatin1String("default");
    return keys;
}

QList<QByteArray> SymbianAudioPlugin::availableDevices(QAudio::Mode mode) const
{
    Q_UNUSED(mode)
    QList<QByteArray> devices;
    devices.append("default");
    return devices;
}

QAbstractAudioInput* SymbianAudioPlugin::createInput(
                         const QByteArray &device, const QAudioFormat &format)
{
    return new SymbianAudioInput(device, format);
}

QAbstractAudioOutput* SymbianAudioPlugin::createOutput(
                          const QByteArray &device, const QAudioFormat &format)
{
    return new SymbianAudioOutput(device, format);
}

QAbstractAudioDeviceInfo* SymbianAudioPlugin::createDeviceInfo(
                              const QByteArray& device, QAudio::Mode mode)
{
    return new SymbianAudioDeviceInfo(device, mode);
}

Q_EXPORT_STATIC_PLUGIN(SymbianAudioPlugin)
Q_EXPORT_PLUGIN2(qaudio, SymbianAudioPlugin)

QT_END_NAMESPACE

