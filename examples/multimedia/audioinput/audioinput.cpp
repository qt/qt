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

#include <stdlib.h>
#include <math.h>

#include <QDebug>
#include <QPainter>
#include <QVBoxLayout>

#include <QAudioDeviceInfo>
#include <QAudioInput>

#include <QtCore/qendian.h>

#include "audioinput.h"

#define BUFFER_SIZE 4096

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
        }
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

    level = 0;
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
    if (level == 0.0)
        return;

    painter.setPen(Qt::red);

    int pos = ((painter.viewport().right()-20)-(painter.viewport().left()+11))*level;
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
    level = value;
    repaint();
}


InputTest::InputTest()
{
    QWidget *window = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    canvas = new RenderArea;
    layout->addWidget(canvas);

    deviceBox = new QComboBox(this);
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for(int i = 0; i < devices.size(); ++i)
        deviceBox->addItem(devices.at(i).deviceName(), qVariantFromValue(devices.at(i)));

    connect(deviceBox, SIGNAL(activated(int)), SLOT(deviceChanged(int)));
    layout->addWidget(deviceBox);

    button = new QPushButton(this);
    button->setText(tr("Click for Push Mode"));
    connect(button, SIGNAL(clicked()), SLOT(toggleMode()));
    layout->addWidget(button);

    button2 = new QPushButton(this);
    button2->setText(tr("Click To Suspend"));
    connect(button2, SIGNAL(clicked()), SLOT(toggleSuspend()));
    layout->addWidget(button2);

    window->setLayout(layout);
    setCentralWidget(window);
    window->show();

    buffer = new char[BUFFER_SIZE];

    pullMode = true;

    format.setFrequency(8000);
    format.setChannels(1);
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning()<<"default format not supported try to use nearest";
        format = info.nearestFormat(format);
    }

    if(format.sampleSize() != 16) {
        qWarning()<<"audio device doesn't support 16 bit samples, example cannot run";
        audioInput = 0;
        button->setDisabled(true);
        button2->setDisabled(true);
        return;
    }

    audioInput = new QAudioInput(format,this);
    connect(audioInput, SIGNAL(notify()), SLOT(status()));
    connect(audioInput, SIGNAL(stateChanged(QAudio::State)), SLOT(state(QAudio::State)));
    audioinfo  = new AudioInfo(format, this);
    connect(audioinfo, SIGNAL(update()), SLOT(refreshDisplay()));
    audioinfo->start();
    audioInput->start(audioinfo);
}

InputTest::~InputTest() {}

void InputTest::status()
{
    qWarning()<<"bytesReady = "<<audioInput->bytesReady()<<" bytes, elapsedUSecs = "<<audioInput->elapsedUSecs()<<", processedUSecs = "<<audioInput->processedUSecs();
}

void InputTest::readMore()
{
    if(!audioInput)
        return;
    qint64 len = audioInput->bytesReady();
    if(len > 4096)
        len = 4096;
    qint64 l = input->read(buffer,len);
    if(l > 0) {
        audioinfo->write(buffer,l);
    }
}

void InputTest::toggleMode()
{
    // Change bewteen pull and push modes
    audioInput->stop();

    if (pullMode) {
        button->setText(tr("Click for Pull Mode"));
        input = audioInput->start();
        connect(input, SIGNAL(readyRead()), SLOT(readMore()));
        pullMode = false;
    } else {
        button->setText(tr("Click for Push Mode"));
        pullMode = true;
        audioInput->start(audioinfo);
    }

    button2->setText("Click To Suspend");
}

void InputTest::toggleSuspend()
{
    // toggle suspend/resume
    if(audioInput->state() == QAudio::SuspendedState) {
        qWarning() << "status: Suspended, resume()";
        audioInput->resume();
        button2->setText("Click To Suspend");
    } else if (audioInput->state() == QAudio::ActiveState) {
        qWarning() << "status: Active, suspend()";
        audioInput->suspend();
        button2->setText("Click To Resume");
    } else if (audioInput->state() == QAudio::StoppedState) {
        qWarning() << "status: Stopped, resume()";
        audioInput->resume();
        button2->setText("Click To Suspend");
    } else if (audioInput->state() == QAudio::IdleState) {
        qWarning() << "status: IdleState";
    }
}

void InputTest::state(QAudio::State state)
{
    qWarning() << " state=" << state;
}

void InputTest::refreshDisplay()
{
    canvas->setLevel(audioinfo->level());
    canvas->repaint();
}

void InputTest::deviceChanged(int idx)
{
    audioinfo->stop();
    audioInput->stop();
    audioInput->disconnect(this);
    delete audioInput;

    device = deviceBox->itemData(idx).value<QAudioDeviceInfo>();
    audioInput = new QAudioInput(device, format, this);
    connect(audioInput, SIGNAL(notify()), SLOT(status()));
    connect(audioInput, SIGNAL(stateChanged(QAudio::State)), SLOT(state(QAudio::State)));
    audioinfo->start();
    audioInput->start(audioinfo);
}
