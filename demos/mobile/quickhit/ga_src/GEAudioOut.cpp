/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qstring.h>
#include <QAudioOutput>

#include "GEAudioOut.h"

using namespace GE;
//using namespace QTM_NAMESPACE;

/*
#ifndef Q_OS_WIN32
QTM_USE_NAMESPACE
#endif
*/

const int CHANNELS = 2;
const QString CODEC = "audio/pcm";
const QAudioFormat::Endian BYTEORDER = QAudioFormat::LittleEndian;
const QAudioFormat::SampleType SAMTYPE = QAudioFormat::SignedInt;



AudioOut::AudioOut( QObject *parent, GE::IAudioSource *source ) : QThread(parent) {         // qobject
    m_source = source;
    QAudioFormat format;
    format.setFrequency(AUDIO_FREQUENCY);
    format.setChannels(CHANNELS);
    format.setSampleSize(AUDIO_SAMPLE_BITS);
    format.setCodec(CODEC);
    format.setByteOrder(BYTEORDER);
    format.setSampleType(SAMTYPE);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format))
        format = info.nearestFormat(format);


    m_audioOutput = new QAudioOutput(info,format);

#ifdef Q_WS_MAEMO_5
    m_audioOutput->setBufferSize(20000);
    m_sendBufferSize = 5000;
#else
    m_audioOutput->setBufferSize(16000);
    m_sendBufferSize = 4000;
#endif

    m_outTarget = m_audioOutput->start();


    m_sendBuffer = new AUDIO_SAMPLE_TYPE[ m_sendBufferSize ];
    m_samplesMixed = 0;

    m_runstate=0;

#ifndef Q_OS_SYMBIAN
    start();
#else
    m_audioOutput->setNotifyInterval(5);
    connect(m_audioOutput,SIGNAL(notify()),SLOT(audioNotify()));
#endif

};


AudioOut::~AudioOut() {
    if (m_runstate==0) m_runstate = 1;
    if (QThread::isRunning() == false) m_runstate = 2;
    while (m_runstate!=2) { msleep(50); }       // wait until the thread is finished
    m_audioOutput->stop();
    delete m_audioOutput;
    delete [] m_sendBuffer;
};


void AudioOut::audioNotify() {
    tick();
};

void AudioOut::tick() {
        // fill data to buffer as much as free space is available..
    int samplesToWrite = m_audioOutput->bytesFree() / (CHANNELS*AUDIO_SAMPLE_BITS/8);
    samplesToWrite*=2;

    if (samplesToWrite > m_sendBufferSize) samplesToWrite = m_sendBufferSize;
    if (samplesToWrite<=0) return;
    int mixedSamples = m_source->pullAudio( m_sendBuffer, samplesToWrite );
    m_outTarget->write( (char*)m_sendBuffer, mixedSamples*2 );

};


void AudioOut::run() {
    if (!m_source) { m_runstate=2; return; }
    int sleepTime = m_sendBufferSize * 340 / AUDIO_FREQUENCY;
    if (sleepTime<2) sleepTime = 2;

    while (m_runstate==0) {
        tick();
        msleep(sleepTime);
    };
    m_runstate = 2;
};


