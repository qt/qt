/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
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

#include <QtCore/qdebug.h>
#include <QtMultimedia/qaudioengine.h>
#include <QtMultimedia/qaudioengineplugin.h>
#include <private/qfactoryloader_p.h>
#include "qaudiodevicefactory_p.h"
#include "qaudiodeviceid_p.h"

#if defined(Q_OS_WIN)
#include "qaudiodeviceinfo_win32_p.h"
#include "qaudiooutput_win32_p.h"
#include "qaudioinput_win32_p.h"
#elif defined(Q_OS_MAC)
#include "qaudiodeviceinfo_mac_p.h"
#include "qaudiooutput_mac_p.h"
#include "qaudioinput_mac_p.h"
#elif defined(HAS_ALSA)
#include "qaudiodeviceinfo_alsa_p.h"
#include "qaudiooutput_alsa_p.h"
#include "qaudioinput_alsa_p.h"
#endif

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
        (QAudioEngineFactoryInterface_iid, QLatin1String("/audio"), Qt::CaseInsensitive))


class QNullDeviceInfo : public QAbstractAudioDeviceInfo
{
public:
    QAudioFormat preferredFormat() const { qWarning()<<"using null deviceinfo, none available"; return QAudioFormat(); }
    bool isFormatSupported(const QAudioFormat& ) const { return false; }
    QAudioFormat nearestFormat(const QAudioFormat& ) const { return QAudioFormat(); }
    QString deviceName() const { return QString(); }
    QStringList codecList() { return QStringList(); }
    QList<int> frequencyList()  { return QList<int>(); }
    QList<int> channelsList() { return QList<int>(); }
    QList<int> sampleSizeList() { return QList<int>(); }
    QList<QAudioFormat::Endian> byteOrderList() { return QList<QAudioFormat::Endian>(); }
    QList<QAudioFormat::SampleType> sampleTypeList() { return QList<QAudioFormat::SampleType>(); }
};

class QNullInputDevice : public QAbstractAudioInput
{
public:
    QIODevice* start(QIODevice* ) { qWarning()<<"using null input device, none available"; return 0; }
    void stop() {}
    void reset() {}
    void suspend() {}
    void resume() {}
    int bytesReady() const { return 0; }
    int periodSize() const { return 0; }
    void setBufferSize(int ) {}
    int bufferSize() const  { return 0; }
    void setNotifyInterval(int ) {}
    int notifyInterval() const { return 0; }
    qint64 totalTime() const { return 0; }
    qint64 clock() const { return 0; }
    QAudio::Error error() const { return QAudio::OpenError; }
    QAudio::State state() const { return QAudio::StopState; }
    QAudioFormat format() const { return QAudioFormat(); }
};

class QNullOutputDevice : public QAbstractAudioOutput
{
public:
    QIODevice* start(QIODevice* ) { qWarning()<<"using null output device, none available"; return 0; }
    void stop() {}
    void reset() {}
    void suspend() {}
    void resume() {}
    int bytesFree() const { return 0; }
    int periodSize() const { return 0; }
    void setBufferSize(int ) {}
    int bufferSize() const  { return 0; }
    void setNotifyInterval(int ) {}
    int notifyInterval() const { return 0; }
    qint64 totalTime() const { return 0; }
    qint64 clock() const { return 0; }
    QAudio::Error error() const { return QAudio::OpenError; }
    QAudio::State state() const { return QAudio::StopState; }
    QAudioFormat format() const { return QAudioFormat(); }
};

QList<QAudioDeviceId> QAudioDeviceFactory::deviceList(QAudio::Mode mode)
{
    QList<QAudioDeviceId> devices;
#if (defined(Q_OS_WIN) || defined(Q_OS_MAC) || defined(HAS_ALSA))
    foreach (const QByteArray &handle, QAudioDeviceInfoPrivate::deviceList(mode))
        devices += createDeviceId(QLatin1String("builtin"), mode, handle);
#endif
    QFactoryLoader* l = loader();

    foreach (QString const& key, l->keys()) {
        QAudioEngineFactoryInterface* plugin = qobject_cast<QAudioEngineFactoryInterface*>(l->instance(key));
        if (plugin) {
            foreach (QByteArray const& handle, plugin->deviceList(mode))
                devices += createDeviceId(key, mode, handle);
        }

        delete plugin;
    }

    return devices;
}

QAudioDeviceId QAudioDeviceFactory::defaultInputDevice()
{
    QAudioEngineFactoryInterface* plugin = qobject_cast<QAudioEngineFactoryInterface*>(loader()->instance(QLatin1String("default")));

    if (plugin) {
        QList<QByteArray> list = plugin->deviceList(QAudio::AudioInput);
        if (list.size() > 0)
            return createDeviceId(QLatin1String("default"), QAudio::AudioInput, list.at(0));
    }
#if (defined(Q_OS_WIN) || defined(Q_OS_MAC) || defined(HAS_ALSA))
    return createDeviceId(QLatin1String("builtin"), QAudio::AudioInput, QAudioDeviceInfoPrivate::defaultInputDevice());
#endif
    return QAudioDeviceId();
}

QAudioDeviceId QAudioDeviceFactory::defaultOutputDevice()
{
    QAudioEngineFactoryInterface* plugin = qobject_cast<QAudioEngineFactoryInterface*>(loader()->instance(QLatin1String("default")));

    if (plugin) {
        QList<QByteArray> list = plugin->deviceList(QAudio::AudioOutput);
        if (list.size() > 0)
            return createDeviceId(QLatin1String("default"), QAudio::AudioOutput, list.at(0));
    }
#if (defined(Q_OS_WIN) || defined(Q_OS_MAC) || defined(HAS_ALSA))
    return createDeviceId(QLatin1String("builtin"), QAudio::AudioOutput, QAudioDeviceInfoPrivate::defaultOutputDevice());
#endif
    return QAudioDeviceId();
}

QAbstractAudioDeviceInfo* QAudioDeviceFactory::audioDeviceInfo(QAudioDeviceId const& id)
{
    if (id.isNull())
        return new QNullDeviceInfo();
#if (defined(Q_OS_WIN) || defined(Q_OS_MAC) || defined(HAS_ALSA))
    if (id.d->key == QLatin1String("builtin"))
        return new QAudioDeviceInfoPrivate(id.d->handle, QAudio::Mode(id.d->mode));
#endif
    QAudioEngineFactoryInterface* plugin = qobject_cast<QAudioEngineFactoryInterface*>(loader()->instance(id.d->key));

    if (plugin)
        return plugin->createDeviceInfo(id.d->handle, QAudio::Mode(id.d->mode));

    return new QNullDeviceInfo();
}

QAbstractAudioInput* QAudioDeviceFactory::createDefaultInputDevice(QAudioFormat const &format)
{
    return createInputDevice(defaultInputDevice(), format);
}

QAbstractAudioOutput* QAudioDeviceFactory::createDefaultOutputDevice(QAudioFormat const &format)
{
    return createOutputDevice(defaultOutputDevice(), format);
}

QAbstractAudioInput* QAudioDeviceFactory::createInputDevice(QAudioDeviceId const& id, QAudioFormat const &format)
{
    if (id.isNull())
        return new QNullInputDevice();
#if (defined(Q_OS_WIN) || defined(Q_OS_MAC) || defined(HAS_ALSA))
    if (id.d->key == QLatin1String("builtin")) {
        if(!defaultInputDevice().isNull())
            return new QAudioInputPrivate(id.d->handle, format);
        else
            return new QNullInputDevice();
    }
#endif
    QAudioEngineFactoryInterface* plugin = qobject_cast<QAudioEngineFactoryInterface*>(loader()->instance((id.d->key)));

    if (plugin)
        return plugin->createInput(id.d->handle, format);

    return new QNullInputDevice();
}

QAbstractAudioOutput* QAudioDeviceFactory::createOutputDevice(QAudioDeviceId const& id, QAudioFormat const &format)
{
    if (id.isNull())
        return new QNullOutputDevice();
#if (defined(Q_OS_WIN) || defined(Q_OS_MAC) || defined(HAS_ALSA))
    if (id.d->key == QLatin1String("builtin")) {
        if(!defaultOutputDevice().isNull())
            return new QAudioOutputPrivate(id.d->handle, format);
        else
            return new QNullOutputDevice();
    }
#endif
    QAudioEngineFactoryInterface* plugin = qobject_cast<QAudioEngineFactoryInterface*>(loader()->instance(id.d->key));

    if (plugin)
        return plugin->createOutput(id.d->handle, format);

    return new QNullOutputDevice();
}

QAudioDeviceId QAudioDeviceFactory::createDeviceId(QString const& key, int mode, QByteArray const& handle)
{
    return QAudioDeviceId(new QAudioDeviceIdPrivate(key, mode, handle));
}

QT_END_NAMESPACE

