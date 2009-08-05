/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QDebug>
#include <QVBoxLayout>

#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include "audiooutput.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Generator::Generator(QObject *parent)
    :QIODevice( parent )
{
    finished = false;
    buffer = new char[SECONDS*44100*4+1000];
    t=buffer;
    len=fillData(t+4,450,SECONDS); /* left channel, 450Hz sine */
    len+=fillData(t+6,452,SECONDS); /* right channel, 452Hz sine */
    putLong(t,len);
    putLong(buffer+4,len+8+16+8);
    pos   = 0;
    total = len+8+16+8;
}

Generator::~Generator()
{
    delete [] buffer;
}

void Generator::start()
{
    open(QIODevice::ReadOnly);
}

void Generator::stop()
{
    close();
}

int Generator::putShort(char *t, unsigned int value)
{
    *(unsigned char *)(t++)=value&255;
    *(unsigned char *)(t)=(value/256)&255;
    return 2;
}

int Generator::putLong(char *t, unsigned int value)
{
    *(unsigned char *)(t++)=value&255;
    *(unsigned char *)(t++)=(value/256)&255;
    *(unsigned char *)(t++)=(value/(256*256))&255;
    *(unsigned char *)(t)=(value/(256*256*256))&255;
    return 4;
}

int Generator::fillData(char *start, int frequency, int seconds)
{
    int i, len=0;
    int value;
    for(i=0; i<seconds*44100; i++) {
        value=(int)(32767.0*sin(2.0*M_PI*((double)(i))*(double)(frequency)/44100.0));
        putShort(start, value);
        start += 4;
        len+=2;
    }
    return len;
}

qint64 Generator::readData(char *data, qint64 maxlen)
{
    int len = maxlen;
    if(len > 16384)
        len = 16384;

    if(len < (SECONDS*44100*4+1000)-pos) {
        // Normal
        memcpy(data,t+pos,len);
        pos+=len;
        return len;
    } else {
        // Whats left and reset to start
        qint64 left = (SECONDS*44100*4+1000)-pos;
        memcpy(data,t+pos,left);
        pos=0;
        return left;
    }
}

qint64 Generator::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

AudioTest::AudioTest()
{
    QWidget *window = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    deviceBox = new QComboBox(this);
    QList<QAudioDeviceId> devices = QAudioDeviceInfo::deviceList(QAudio::AudioOutput);
    for(int i = 0; i < devices.size(); ++i) {
        deviceBox->addItem(QAudioDeviceInfo(devices.at(i)).deviceName(), qVariantFromValue(devices.at(i)));
    }
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

    gen = new Generator(this);

    pullMode = true;

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),SLOT(writeMore()));

    gen->start();

    settings.setFrequency(44100);
    settings.setChannels(2);
    settings.setSampleSize(16);
    settings.setCodec("audio/pcm");
    settings.setByteOrder(QAudioFormat::LittleEndian);
    settings.setSampleType(QAudioFormat::SignedInt);
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

    device = deviceBox->itemData(idx).value<QAudioDeviceId>();
    audioOutput = new QAudioOutput(device,settings,this);
    connect(audioOutput,SIGNAL(notify()),SLOT(status()));
    connect(audioOutput,SIGNAL(stateChanged(QAudio::State)),SLOT(state(QAudio::State)));
    gen->start();
    audioOutput->start(gen);
}

void AudioTest::status()
{
    qWarning()<<"byteFree = "<<audioOutput->bytesFree()<<" bytes, clock = "<<audioOutput->clock()<<"ms, totalTime = "<<audioOutput->totalTime()/1000<<"ms";
}

void AudioTest::writeMore()
{
    if(!audioOutput)
        return;

    if(audioOutput->state() == QAudio::StopState)
        return;

    int    l;
    int    out;

    int chunks = audioOutput->bytesFree()/audioOutput->periodSize();
    while(chunks) {
       l = gen->read(buffer,audioOutput->periodSize());
       if(l > 0)
           out = output->write(buffer,l);
       if(l != audioOutput->periodSize())
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
        output = audioOutput->start(0);
        pullMode = false;
        timer->start(20);
    } else {
        button->setText("Click for Push Mode");
        pullMode = true;
        audioOutput->start(gen);
    }
}

void AudioTest::togglePlay()
{
    // toggle suspend/resume
    if(audioOutput->state() == QAudio::SuspendState) {
        qWarning()<<"status: Suspended, resume()";
        audioOutput->resume();
        button2->setText("Click To Suspend");
    } else if (audioOutput->state() == QAudio::ActiveState) {
        qWarning()<<"status: Active, suspend()";
        audioOutput->suspend();
        button2->setText("Click To Resume");
    } else if (audioOutput->state() == QAudio::StopState) {
        qWarning()<<"status: Stopped, resume()";
        audioOutput->resume();
        button2->setText("Click To Suspend");
    } else if (audioOutput->state() == QAudio::IdleState) {
        qWarning()<<"status: IdleState";
    }
}

void AudioTest::state(QAudio::State state)
{
    qWarning()<<" state="<<state;
}
