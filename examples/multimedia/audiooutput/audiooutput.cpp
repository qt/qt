/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include <QDebug>
#include <QVBoxLayout>

#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QtCore/qmath.h>
#include <QtCore/qendian.h>
#include "audiooutput.h"

#define SECONDS     1
#define FREQ        600
#define SYSTEM_FREQ 44100

Generator::Generator(const QAudioFormat &format,
                     qint64 durationUs,
                     int frequency,
                     QObject *parent)
    :   QIODevice(parent)
    ,   pos(0)
{
    generateData(format, durationUs, frequency);
}

Generator::~Generator()
{

}

void Generator::start()
{
    open(QIODevice::ReadOnly);
}

void Generator::stop()
{
    pos = 0;
    close();
}

void Generator::generateData(const QAudioFormat &format, qint64 durationUs, int frequency)
{
    const int channelBytes = format.sampleSize() / 8;
    const int sampleBytes = format.channels() * channelBytes;

    qint64 length = (format.frequency() * format.channels() * (format.sampleSize() / 8))
                        * durationUs / 100000;

    Q_ASSERT(length % sampleBytes == 0);
    Q_UNUSED(sampleBytes) // suppress warning in release builds

    buffer.resize(length);
    unsigned char *ptr = reinterpret_cast<unsigned char *>(buffer.data());
    int sampleIndex = 0;

    while (length) {
        const qreal x = qSin(2 * M_PI * frequency * qreal(sampleIndex % format.frequency()) / format.frequency());
        for (int i=0; i<format.channels(); ++i) {
            if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::UnSignedInt) {
                const quint8 value = static_cast<quint8>((1.0 + x) / 2 * 255);
                *reinterpret_cast<quint8*>(ptr) = value;
            } else if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::SignedInt) {
                const qint8 value = static_cast<qint8>(x * 127);
                *reinterpret_cast<quint8*>(ptr) = value;
            } else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::UnSignedInt) {
                quint16 value = static_cast<quint16>((1.0 + x) / 2 * 65535);
                if (format.byteOrder() == QAudioFormat::LittleEndian)
                    qToLittleEndian<quint16>(value, ptr);
                else
                    qToBigEndian<quint16>(value, ptr);
            } else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::SignedInt) {
                qint16 value = static_cast<qint16>(x * 32767);
                if (format.byteOrder() == QAudioFormat::LittleEndian)
                    qToLittleEndian<qint16>(value, ptr);
                else
                    qToBigEndian<qint16>(value, ptr);
            }

            ptr += channelBytes;
            length -= channelBytes;
        }
        ++sampleIndex;
    }
}

qint64 Generator::readData(char *data, qint64 len)
{
    qint64 total = 0;
    while (len - total) {
        const qint64 chunk = qMin((buffer.size() - pos), len - total);
        memcpy(data, buffer.constData() + pos, chunk);
        pos = (pos + chunk) % buffer.size();
        total += chunk;
    }
    return total;
}

qint64 Generator::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 Generator::bytesAvailable() const
{
    return buffer.size() + QIODevice::bytesAvailable();
}

AudioTest::AudioTest()
{
    QWidget *window = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    deviceBox = new QComboBox(this);
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
        deviceBox->addItem(deviceInfo.deviceName(), qVariantFromValue(deviceInfo));
    connect(deviceBox,SIGNAL(activated(int)),SLOT(deviceChanged(int)));
    layout->addWidget(deviceBox);

    button = new QPushButton(this);
    button->setText(tr("Click for Push Mode"));
    connect(button,SIGNAL(clicked()),SLOT(toggle()));
    layout->addWidget(button);

    button2 = new QPushButton(this);
    button2->setText(tr("Click To Suspend"));
    connect(button2,SIGNAL(clicked()),SLOT(togglePlay()));
    layout->addWidget(button2);

    window->setLayout(layout);
    setCentralWidget(window);
    window->show();

    buffer = new char[BUFFER_SIZE];

    pullMode = true;

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),SLOT(writeMore()));

    settings.setFrequency(SYSTEM_FREQ);
    settings.setChannels(1);
    settings.setSampleSize(16);
    settings.setCodec("audio/pcm");
    settings.setByteOrder(QAudioFormat::LittleEndian);
    settings.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(settings)) {
        qWarning()<<"default format not supported try to use nearest";
        settings = info.nearestFormat(settings);
    }

    gen = new Generator(settings, SECONDS*1000000, FREQ, this);
    gen->start();

    audioOutput = new QAudioOutput(settings,this);
    connect(audioOutput,SIGNAL(notify()),SLOT(status()));
    connect(audioOutput,SIGNAL(stateChanged(QAudio::State)),SLOT(state(QAudio::State)));

    audioOutput->start(gen);
}

AudioTest::~AudioTest()
{
    delete [] buffer;
}

void AudioTest::deviceChanged(int idx)
{
    timer->stop();
    gen->stop();
    audioOutput->stop();
    audioOutput->disconnect(this);
    delete audioOutput;

    device = deviceBox->itemData(idx).value<QAudioDeviceInfo>();
    audioOutput = new QAudioOutput(device,settings,this);
    connect(audioOutput,SIGNAL(notify()),SLOT(status()));
    connect(audioOutput,SIGNAL(stateChanged(QAudio::State)),SLOT(state(QAudio::State)));
    gen->start();
    audioOutput->start(gen);
}

void AudioTest::status()
{
    qWarning() << "byteFree = " << audioOutput->bytesFree() << " bytes, elapsedUSecs = " << audioOutput->elapsedUSecs() << ", processedUSecs = " << audioOutput->processedUSecs();
}

void AudioTest::writeMore()
{
    if (!audioOutput)
        return;

    if (audioOutput->state() == QAudio::StoppedState)
        return;

    int    l;
    int    out;

    int chunks = audioOutput->bytesFree()/audioOutput->periodSize();
    while(chunks) {
       l = gen->read(buffer,audioOutput->periodSize());
       if (l > 0)
           out = output->write(buffer,l);
       if (l != audioOutput->periodSize())
	   break;
       chunks--;
    }
}

void AudioTest::toggle()
{
    // Change between pull and push modes

    timer->stop();
    audioOutput->stop();

    if (pullMode) {
        button->setText("Click for Pull Mode");
        output = audioOutput->start();
        pullMode = false;
        timer->start(20);
    } else {
        button->setText("Click for Push Mode");
        pullMode = true;
        audioOutput->start(gen);
    }

    button2->setText("Click To Suspend");
}

void AudioTest::togglePlay()
{
    // toggle suspend/resume
    if (audioOutput->state() == QAudio::SuspendedState) {
        qWarning() << "status: Suspended, resume()";
        audioOutput->resume();
        button2->setText("Click To Suspend");
    } else if (audioOutput->state() == QAudio::ActiveState) {
        qWarning() << "status: Active, suspend()";
        audioOutput->suspend();
        button2->setText("Click To Resume");
    } else if (audioOutput->state() == QAudio::StoppedState) {
        qWarning() << "status: Stopped, resume()";
        audioOutput->resume();
        button2->setText("Click To Suspend");
    } else if (audioOutput->state() == QAudio::IdleState) {
        qWarning() << "status: IdleState";
    }
}

void AudioTest::state(QAudio::State state)
{
    qWarning() << " state=" << state;
}
