/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <stdlib.h>
#include <math.h>

#include <QDebug>
#include <QPainter>
#include <QVBoxLayout>

#include <QAudioDeviceInfo>
#include <QAudioInput>

#include <QtCore/qendian.h>

#include "audioinput.h"

const QString InputTest::PushModeLabel(tr("Enable push mode"));
const QString InputTest::PullModeLabel(tr("Enable pull mode"));
const QString InputTest::SuspendLabel(tr("Suspend recording"));
const QString InputTest::ResumeLabel(tr("Resume recording"));

const int BufferSize = 4096;

AudioInfo::AudioInfo(const QAudioFormat &format, QObject *parent)
    :   QIODevice(parent)
    ,   m_format(format)
    ,   m_maxAmplitude(0)
    ,   m_level(0.0)

{
    switch (m_format.sampleSize()) {
    case 8:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 255;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 127;
            break;
        default:
            break;
        }
        break;
    case 16:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 65535;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 32767;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

AudioInfo::~AudioInfo()
{
}

void AudioInfo::start()
{
    open(QIODevice::WriteOnly);
}

void AudioInfo::stop()
{
    close();
}

qint64 AudioInfo::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return 0;
}

qint64 AudioInfo::writeData(const char *data, qint64 len)
{
    if (m_maxAmplitude) {
        Q_ASSERT(m_format.sampleSize() % 8 == 0);
        const int channelBytes = m_format.sampleSize() / 8;
        const int sampleBytes = m_format.channels() * channelBytes;
        Q_ASSERT(len % sampleBytes == 0);
        const int numSamples = len / sampleBytes;

        quint16 maxValue = 0;
        const unsigned char *ptr = reinterpret_cast<const unsigned char *>(data);

        for (int i = 0; i < numSamples; ++i) {
            for(int j = 0; j < m_format.channels(); ++j) {
                quint16 value = 0;

                if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                    value = *reinterpret_cast<const quint8*>(ptr);
                } else if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::SignedInt) {
                    value = qAbs(*reinterpret_cast<const qint8*>(ptr));
                } else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<quint16>(ptr);
                    else
                        value = qFromBigEndian<quint16>(ptr);
                } else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::SignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qAbs(qFromLittleEndian<qint16>(ptr));
                    else
                        value = qAbs(qFromBigEndian<qint16>(ptr));
                }

                maxValue = qMax(value, maxValue);
                ptr += channelBytes;
            }
        }

        maxValue = qMin(maxValue, m_maxAmplitude);
        m_level = qreal(maxValue) / m_maxAmplitude;
    }

    emit update();
    return len;
}

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    m_level = 0;
    setMinimumHeight(30);
    setMinimumWidth(200);
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    painter.setPen(Qt::black);
    painter.drawRect(QRect(painter.viewport().left()+10,
                           painter.viewport().top()+10,
                           painter.viewport().right()-20,
                           painter.viewport().bottom()-20));
    if (m_level == 0.0)
        return;

    painter.setPen(Qt::red);

    int pos = ((painter.viewport().right()-20)-(painter.viewport().left()+11))*m_level;
    for (int i = 0; i < 10; ++i) {
        int x1 = painter.viewport().left()+11;
        int y1 = painter.viewport().top()+10+i;
        int x2 = painter.viewport().left()+20+pos;
        int y2 = painter.viewport().top()+10+i;
        if (x2 < painter.viewport().left()+10)
            x2 = painter.viewport().left()+10;

        painter.drawLine(QPoint(x1, y1),QPoint(x2, y2));
    }
}

void RenderArea::setLevel(qreal value)
{
    m_level = value;
    repaint();
}


InputTest::InputTest()
    :   m_canvas(0)
    ,   m_modeButton(0)
    ,   m_suspendResumeButton(0)
    ,   m_deviceBox(0)
    ,   m_device(QAudioDeviceInfo::defaultInputDevice())
    ,   m_audioInfo(0)
    ,   m_audioInput(0)
    ,   m_input(0)
    ,   m_pullMode(false)
    ,   m_buffer(BufferSize, 0)
{
    initializeWindow();
    initializeAudio();
}

InputTest::~InputTest() {}

void InputTest::initializeWindow()
{
    QScopedPointer<QWidget> window(new QWidget);
    QScopedPointer<QVBoxLayout> layout(new QVBoxLayout);

    m_canvas = new RenderArea(this);
    layout->addWidget(m_canvas);

    m_deviceBox = new QComboBox(this);
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for(int i = 0; i < devices.size(); ++i)
        m_deviceBox->addItem(devices.at(i).deviceName(), QVariant::fromValue(devices.at(i)));

    connect(m_deviceBox, SIGNAL(activated(int)), SLOT(deviceChanged(int)));
    layout->addWidget(m_deviceBox);

    m_modeButton = new QPushButton(this);
    m_modeButton->setText(PushModeLabel);
    connect(m_modeButton, SIGNAL(clicked()), SLOT(toggleMode()));
    layout->addWidget(m_modeButton);

    m_suspendResumeButton = new QPushButton(this);
    m_suspendResumeButton->setText(SuspendLabel);
    connect(m_suspendResumeButton, SIGNAL(clicked()), SLOT(toggleSuspend()));
    layout->addWidget(m_suspendResumeButton);

    window->setLayout(layout.data());
    layout.take(); // ownership transferred

    setCentralWidget(window.data());
    QWidget *const windowPtr = window.take(); // ownership transferred
    windowPtr->show();
}

void InputTest::initializeAudio()
{
    m_pullMode = true;

    m_format.setFrequency(8000);
    m_format.setChannels(1);
    m_format.setSampleSize(16);
    m_format.setSampleType(QAudioFormat::SignedInt);
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setCodec("audio/pcm");

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
    if (!info.isFormatSupported(m_format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        m_format = info.nearestFormat(m_format);
    }

    m_audioInfo  = new AudioInfo(m_format, this);
    connect(m_audioInfo, SIGNAL(update()), SLOT(refreshDisplay()));

    createAudioInput();
}

void InputTest::createAudioInput()
{
    m_audioInput = new QAudioInput(m_device, m_format, this);
    connect(m_audioInput, SIGNAL(notify()), SLOT(notified()));
    connect(m_audioInput, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));
    m_audioInfo->start();
    m_audioInput->start(m_audioInfo);
}

void InputTest::notified()
{
    qWarning() << "bytesReady = " << m_audioInput->bytesReady()
               << ", " << "elapsedUSecs = " <<m_audioInput->elapsedUSecs()
               << ", " << "processedUSecs = "<<m_audioInput->processedUSecs();
}

void InputTest::readMore()
{
    if(!m_audioInput)
        return;
    qint64 len = m_audioInput->bytesReady();
    if(len > 4096)
        len = 4096;
    qint64 l = m_input->read(m_buffer.data(), len);
    if(l > 0) {
        m_audioInfo->write(m_buffer.constData(), l);
    }
}

void InputTest::toggleMode()
{
    // Change bewteen pull and push modes
    if (m_input != 0) {
        disconnect(m_input, 0, this, 0);
        m_input = 0;
    }

    m_audioInput->stop();

    if (m_pullMode) {
        m_modeButton->setText(PullModeLabel);
        m_input = m_audioInput->start();
        connect(m_input, SIGNAL(readyRead()), SLOT(readMore()));
        m_pullMode = false;
    } else {
        m_modeButton->setText(PushModeLabel);
        m_pullMode = true;
        m_audioInput->start(m_audioInfo);
    }

    m_suspendResumeButton->setText(SuspendLabel);
}

void InputTest::toggleSuspend()
{
    // toggle suspend/resume
    if(m_audioInput->state() == QAudio::SuspendedState) {
        qWarning() << "status: Suspended, resume()";
        m_audioInput->resume();
        m_suspendResumeButton->setText(SuspendLabel);
    } else if (m_audioInput->state() == QAudio::ActiveState) {
        qWarning() << "status: Active, suspend()";
        m_audioInput->suspend();
        m_suspendResumeButton->setText(ResumeLabel);
    } else if (m_audioInput->state() == QAudio::StoppedState) {
        qWarning() << "status: Stopped, resume()";
        m_audioInput->resume();
        m_suspendResumeButton->setText(SuspendLabel);
    } else if (m_audioInput->state() == QAudio::IdleState) {
        qWarning() << "status: IdleState";
    }
}

void InputTest::stateChanged(QAudio::State state)
{
    qWarning() << "state = " << state;
}

void InputTest::refreshDisplay()
{
    m_canvas->setLevel(m_audioInfo->level());
    m_canvas->repaint();
}

void InputTest::deviceChanged(int index)
{
    m_audioInfo->stop();
    m_audioInput->stop();
    m_audioInput->disconnect(this);
    delete m_audioInput;

    m_device = m_deviceBox->itemData(index).value<QAudioDeviceInfo>();
    createAudioInput();
}
