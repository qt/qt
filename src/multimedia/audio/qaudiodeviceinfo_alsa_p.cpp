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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qaudiodeviceinfo_alsa_p.h"

QT_BEGIN_NAMESPACE

QAudioDeviceInfoPrivate::QAudioDeviceInfoPrivate(QByteArray dev, QAudio::Mode mode)
{
    handle = 0;

    device = QLatin1String(dev);
    this->mode = mode;
}

QAudioDeviceInfoPrivate::~QAudioDeviceInfoPrivate()
{
    close();
}

bool QAudioDeviceInfoPrivate::isFormatSupported(const QAudioFormat& format) const
{
    return testSettings(format);
}

QAudioFormat QAudioDeviceInfoPrivate::preferredFormat() const
{
    QAudioFormat nearest;
    if(mode == QAudio::AudioOutput) {
        nearest.setFrequency(44100);
        nearest.setChannels(2);
        nearest.setByteOrder(QAudioFormat::LittleEndian);
        nearest.setSampleType(QAudioFormat::SignedInt);
        nearest.setSampleSize(16);
        nearest.setCodec(tr("audio/pcm"));
    } else {
        nearest.setFrequency(8000);
        nearest.setChannels(1);
        nearest.setSampleType(QAudioFormat::SignedInt);
        nearest.setSampleSize(8);
        nearest.setCodec(tr("audio/pcm"));
    }
    return nearest;
}

QAudioFormat QAudioDeviceInfoPrivate::nearestFormat(const QAudioFormat& format) const
{
    if(testSettings(format))
        return format;
    else
        return preferredFormat();
}

QString QAudioDeviceInfoPrivate::deviceName() const
{
    return device;
}

QStringList QAudioDeviceInfoPrivate::codecList()
{
    updateLists();
    return codecz;
}

QList<int> QAudioDeviceInfoPrivate::frequencyList()
{
    updateLists();
    return freqz;
}

QList<int> QAudioDeviceInfoPrivate::channelsList()
{
    updateLists();
    return channelz;
}

QList<int> QAudioDeviceInfoPrivate::sampleSizeList()
{
    updateLists();
    return sizez;
}

QList<QAudioFormat::Endian> QAudioDeviceInfoPrivate::byteOrderList()
{
    updateLists();
    return byteOrderz;
}

QList<QAudioFormat::SampleType> QAudioDeviceInfoPrivate::sampleTypeList()
{
    updateLists();
    return typez;
}

bool QAudioDeviceInfoPrivate::open()
{
    int err = 0;
    QString dev = device;
    if(!dev.contains(tr("default"))) {
        int idx = snd_card_get_index(dev.toLocal8Bit().constData());
        dev = QString(tr("hw:%1,0")).arg(idx);
    }
    if(mode == QAudio::AudioOutput) {
        err=snd_pcm_open( &handle,dev.toLocal8Bit().constData(),SND_PCM_STREAM_PLAYBACK,0);
    } else {
        err=snd_pcm_open( &handle,dev.toLocal8Bit().constData(),SND_PCM_STREAM_CAPTURE,0);
    }
    if(err < 0) {
        handle = 0;
        return false;
    }
    return true;
}

void QAudioDeviceInfoPrivate::close()
{
    if(handle)
        snd_pcm_close(handle);
    handle = 0;
}

bool QAudioDeviceInfoPrivate::testSettings(const QAudioFormat& format) const
{
    // Set nearest to closest settings that do work.
    // See if what is in settings will work (return value).

    int err = 0;
    snd_pcm_t* handle;
    snd_pcm_hw_params_t *params;
    QString dev = device;

    // open()
    if(!dev.contains(tr("default"))) {
        int idx = snd_card_get_index(dev.toLocal8Bit().constData());
        dev = QString(tr("hw:%1,0")).arg(idx);
    }
    if(mode == QAudio::AudioOutput) {
        err=snd_pcm_open( &handle,dev.toLocal8Bit().constData(),SND_PCM_STREAM_PLAYBACK,0);
    } else {
        err=snd_pcm_open( &handle,dev.toLocal8Bit().constData(),SND_PCM_STREAM_CAPTURE,0);
    }
    if(err < 0) {
        handle = 0;
        return false;
    }

    bool testChannel = false;
    bool testCodec = false;
    bool testFreq = false;
    bool testType = false;
    bool testSize = false;

    int  dir = 0;

    snd_pcm_nonblock( handle, 0 );
    snd_pcm_hw_params_alloca( &params );
    snd_pcm_hw_params_any( handle, params );

    // For now, just accept only audio/pcm codec
    if(!format.codec().startsWith(tr("audio/pcm"))) {
        err=-1;
    } else
        testCodec = true;

    if(err>=0 && format.channels() != -1) {
        err = snd_pcm_hw_params_test_channels(handle,params,format.channels());
        if(err>=0)
            err = snd_pcm_hw_params_set_channels(handle,params,format.channels());
        if(err>=0)
            testChannel = true;
    }

    if(err>=0 && format.frequency() != -1) {
        err = snd_pcm_hw_params_test_rate(handle,params,format.frequency(),0);
        if(err>=0)
            err = snd_pcm_hw_params_set_rate(handle,params,format.frequency(),dir);
        if(err>=0)
            testFreq = true;
    }

    if((err>=0 && format.sampleSize() != -1) &&
            (format.sampleType() != QAudioFormat::Unknown)) {
        switch(format.sampleSize()) {
            case 8:
                if(format.sampleType() == QAudioFormat::SignedInt)
                    err = snd_pcm_hw_params_set_format(handle,params,SND_PCM_FORMAT_S8);
                else if(format.sampleType() == QAudioFormat::UnSignedInt)
                    err = snd_pcm_hw_params_set_format(handle,params,SND_PCM_FORMAT_U8);
                break;
            case 16:
                if(format.sampleType() == QAudioFormat::SignedInt) {
                    if(format.byteOrder() == QAudioFormat::LittleEndian)
                        err = snd_pcm_hw_params_set_format(handle,params,SND_PCM_FORMAT_S16_LE);
                    else if(format.byteOrder() == QAudioFormat::BigEndian)
                        err = snd_pcm_hw_params_set_format(handle,params,SND_PCM_FORMAT_S16_BE);
                } else if(format.sampleType() == QAudioFormat::UnSignedInt) {
                    if(format.byteOrder() == QAudioFormat::LittleEndian)
                        err = snd_pcm_hw_params_set_format(handle,params,SND_PCM_FORMAT_U16_LE);
                    else if(format.byteOrder() == QAudioFormat::BigEndian)
                        err = snd_pcm_hw_params_set_format(handle,params,SND_PCM_FORMAT_U16_BE);
                }
                break;
            case 32:
                if(format.sampleType() == QAudioFormat::SignedInt) {
                    if(format.byteOrder() == QAudioFormat::LittleEndian)
                        err = snd_pcm_hw_params_set_format(handle,params,SND_PCM_FORMAT_S32_LE);
                    else if(format.byteOrder() == QAudioFormat::BigEndian)
                        err = snd_pcm_hw_params_set_format(handle,params,SND_PCM_FORMAT_S32_BE);
                } else if(format.sampleType() == QAudioFormat::UnSignedInt) {
                    if(format.byteOrder() == QAudioFormat::LittleEndian)
                        err = snd_pcm_hw_params_set_format(handle,params,SND_PCM_FORMAT_U32_LE);
                    else if(format.byteOrder() == QAudioFormat::BigEndian)
                        err = snd_pcm_hw_params_set_format(handle,params,SND_PCM_FORMAT_U32_BE);
                }
        }
        if(err>=0) {
            testSize = true;
            testType = true;
        }
    }
    if(err>=0)
        err = snd_pcm_hw_params(handle, params);

    if(err == 0) {
        // settings work
        // close()
        if(handle)
            snd_pcm_close(handle);
        return true;
    }
    if(handle)
        snd_pcm_close(handle);

    return false;
}

void QAudioDeviceInfoPrivate::updateLists()
{
    // redo all lists based on current settings
    freqz.clear();
    channelz.clear();
    sizez.clear();
    byteOrderz.clear();
    typez.clear();
    codecz.clear();

    if(!handle)
        open();

    if(!handle)
        return;

    for(int i=0; i<(int)MAX_SAMPLE_RATES; i++) {
        //if(snd_pcm_hw_params_test_rate(handle, params, SAMPLE_RATES[i], dir) == 0)
        freqz.append(SAMPLE_RATES[i]);
    }
    channelz.append(1);
    channelz.append(2);
    sizez.append(8);
    sizez.append(16);
    sizez.append(32);
    byteOrderz.append(QAudioFormat::LittleEndian);
    byteOrderz.append(QAudioFormat::BigEndian);
    typez.append(QAudioFormat::SignedInt);
    typez.append(QAudioFormat::UnSignedInt);
    typez.append(QAudioFormat::Float);
    codecz.append(tr("audio/pcm"));
    close();
}

QList<QByteArray> QAudioDeviceInfoPrivate::deviceList(QAudio::Mode mode)
{
    QAudio::Mode _m;
    QList<QByteArray> devices;
    QByteArray filter;
    QString dir;

    // Create a list of all current audio devices that support mode
    void **hints, **n;
    char *name, *descr, *io;

    if(snd_device_name_hint(-1, "pcm", &hints) < 0) {
        qWarning()<<"no alsa devices available";
        return devices;
    }
    n = hints;

    while (*n != NULL) {
        _m = QAudio::AudioOutput;
        name = snd_device_name_get_hint(*n, "NAME");
        descr = snd_device_name_get_hint(*n, "DESC");
        io = snd_device_name_get_hint(*n, "IOID");
        dir = QString::fromUtf8(io);
        if((name != NULL) && (descr != NULL) && ((io == NULL) || (dir.length() ==filter.length()))) {
            if(dir.length() == 5)
                _m = QAudio::AudioInput;
            if(io == NULL)
                _m = mode;

            QString str = tr(name);

            if(str.contains(tr("default"))) {
                int pos = str.indexOf(tr("="),0);
                devices.append(str.mid(pos+1).toLocal8Bit().constData());
            }
        }
        if(name != NULL)
            free(name);
        if(descr != NULL)
            free(descr);
        if(io != NULL)
            free(io);
        n++;
    }
    snd_device_name_free_hint(hints);

    if(devices.size() > 0) {
        devices.append("default");
        if(mode == QAudio::AudioInput) {
            filter.append("Input");
        } else {
            filter.append("Output");
        }
    }

    return devices;
}

QByteArray QAudioDeviceInfoPrivate::defaultInputDevice()
{
    QList<QByteArray> devices = deviceList(QAudio::AudioInput);
    if(devices.size() == 0)
        return QByteArray();

    return QByteArray("default");
}

QByteArray QAudioDeviceInfoPrivate::defaultOutputDevice()
{
    QList<QByteArray> devices = deviceList(QAudio::AudioOutput);
    if(devices.size() == 0)
        return QByteArray();

    return QByteArray("default");
}

QT_END_NAMESPACE
