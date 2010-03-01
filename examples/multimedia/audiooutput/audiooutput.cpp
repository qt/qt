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

const QString AudioTest::PushModeLabel(tr("Enable push mode"));
const QString AudioTest::PullModeLabel(tr("Enable pull mode"));
const QString AudioTest::SuspendLabel(tr("Suspend playback"));
const QString AudioTest::ResumeLabel(tr("Resume playback"));

const int DurationSeconds = 1;
const int ToneFrequencyHz = 600;
const int DataFrequencyHz = 44100;
const int BufferSize      = 32768;


Generator::Generator(const QAudioFormat &format,
                     qint64 durationUs,
                     int frequency,
                     QObject *parent)
    :   QIODevice(parent)
    ,   m_pos(0)
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
    m_pos = 0;
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

    m_buffer.resize(length);
    unsigned char *ptr = reinterpret_cast<unsigned char *>(m_buffer.data());
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
        const qint64 chunk = qMin((m_buffer.size() - m_pos), len - total);
        memcpy(data, m_buffer.constData() + m_pos, chunk);
        m_pos = (m_pos + chunk) % m_buffer.size();
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
    return m_buffer.size() + QIODevice::bytesAvailable();
}

AudioTest::AudioTest()
    :   m_pullTimer(new QTimer(this))
    ,   m_modeButton(0)
    ,   m_suspendResumeButton(0)
    ,   m_deviceBox(0)
    ,   m_device(QAudioDeviceInfo::defaultOutputDevice())
    ,   m_generator(0)
    ,   m_audioOutput(0)
    ,   m_output(0)
    ,   m_buffer(BufferSize, 0)
{
    initializeWindow();
    initializeAudio();
}

void AudioTest::initializeWindow()
{
    QScopedPointer<QWidget> window(new QWidget);
    QScopedPointer<QVBoxLayout> layout(new QVBoxLayout);

    m_deviceBox = new QComboBox(this);
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
        m_deviceBox->addItem(deviceInfo.deviceName(), qVariantFromValue(deviceInfo));
    connect(m_deviceBox,SIGNAL(activated(int)),SLOT(deviceChanged(int)));
    layout->addWidget(m_deviceBox);

    m_modeButton = new QPushButton(this);
    m_modeButton->setText(PushModeLabel);
    connect(m_modeButton, SIGNAL(clicked()), SLOT(toggleMode()));
    layout->addWidget(m_modeButton);

    m_suspendResumeButton = new QPushButton(this);
    m_suspendResumeButton->setText(SuspendLabel);
    connect(m_suspendResumeButton, SIGNAL(clicked()), SLOT(toggleSuspendResume()));
    layout->addWidget(m_suspendResumeButton);

    window->setLayout(layout.data());
    layout.take(); // ownership transferred

    setCentralWidget(window.data());
    QWidget *const windowPtr = window.take(); // ownership transferred
    windowPtr->show();
}

void AudioTest::initializeAudio()
{
    connect(m_pullTimer, SIGNAL(timeout()), SLOT(pullTimerExpired()));

    m_pullMode = true;

    m_format.setFrequency(DataFrequencyHz);
    m_format.setChannels(1);
    m_format.setSampleSize(16);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(m_format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        m_format = info.nearestFormat(m_format);
    }

    m_generator = new Generator(m_format, DurationSeconds*1000000, ToneFrequencyHz, this);

    createAudioOutput();
}

void AudioTest::createAudioOutput()
{
    delete m_audioOutput;
    m_audioOutput = 0;
    m_audioOutput = new QAudioOutput(m_device, m_format, this);
    connect(m_audioOutput, SIGNAL(notify()), SLOT(notified()));
    connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));
    m_generator->start();
    m_audioOutput->start(m_generator);
}

AudioTest::~AudioTest()
{

}

void AudioTest::deviceChanged(int index)
{
    m_pullTimer->stop();
    m_generator->stop();
    m_audioOutput->stop();
    m_audioOutput->disconnect(this);
    m_device = m_deviceBox->itemData(index).value<QAudioDeviceInfo>();
    createAudioOutput();
}

void AudioTest::notified()
{
    qWarning() << "bytesFree = " << m_audioOutput->bytesFree()
               << ", " << "elapsedUSecs = " << m_audioOutput->elapsedUSecs()
               << ", " << "processedUSecs = " << m_audioOutput->processedUSecs();
}

void AudioTest::pullTimerExpired()
{
    if (m_audioOutput && m_audioOutput->state() != QAudio::StoppedState) {
        int chunks = m_audioOutput->bytesFree()/m_audioOutput->periodSize();
        while (chunks) {
           const qint64 len = m_generator->read(m_buffer.data(), m_audioOutput->periodSize());
           if (len)
               m_output->write(m_buffer.data(), len);
           if (len != m_audioOutput->periodSize())
               break;
           --chunks;
        }
    }
}

void AudioTest::toggleMode()
{
    m_pullTimer->stop();
    m_audioOutput->stop();

    if (m_pullMode) {
        m_modeButton->setText(PullModeLabel);
        m_output = m_audioOutput->start();
        m_pullMode = false;
        m_pullTimer->start(20);
    } else {
        m_modeButton->setText(PushModeLabel);
        m_pullMode = true;
        m_audioOutput->start(m_generator);
    }

    m_suspendResumeButton->setText(SuspendLabel);
}

void AudioTest::toggleSuspendResume()
{
    if (m_audioOutput->state() == QAudio::SuspendedState) {
        qWarning() << "status: Suspended, resume()";
        m_audioOutput->resume();
        m_suspendResumeButton->setText(SuspendLabel);
    } else if (m_audioOutput->state() == QAudio::ActiveState) {
        qWarning() << "status: Active, suspend()";
        m_audioOutput->suspend();
        m_suspendResumeButton->setText(ResumeLabel);
    } else if (m_audioOutput->state() == QAudio::StoppedState) {
        qWarning() << "status: Stopped, resume()";
        m_audioOutput->resume();
        m_suspendResumeButton->setText(SuspendLabel);
    } else if (m_audioOutput->state() == QAudio::IdleState) {
        qWarning() << "status: IdleState";
    }
}

void AudioTest::stateChanged(QAudio::State state)
{
    qWarning() << "state = " << state;
}
