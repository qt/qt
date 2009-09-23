/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "audioinput.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Spectrum::Spectrum(QObject* parent, QAudioInput* device, float* out)
    :QIODevice( parent )
{
    input = device;
    output = out;

    unsigned int i;

    // Allocate sample buffer and initialize sin and cos lookup tables
    fftState = (fft_state *) malloc (sizeof(fft_state));

    for(i = 0; i < BUFFER_SIZE; i++) {
        bitReverse[i] = reverseBits(i);
    }
    for(i = 0; i < BUFFER_SIZE / 2; i++) {
        float j = 2 * M_PI * i / BUFFER_SIZE;
        costable[i] = cos(j);
        sintable[i] = sin(j);
    }
}

Spectrum::~Spectrum()
{
}

void Spectrum::start()
{
    open(QIODevice::WriteOnly);
}

void Spectrum::stop()
{
    close();
}

qint64 Spectrum::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return 0;
}

qint64 Spectrum::writeData(const char *data, qint64 len)
{
    performFFT((sound_sample*)data);
    emit update();

    return len;
}

int Spectrum::reverseBits(unsigned int initial) {
    // BIT-REVERSE-COPY(a,A)

    unsigned int reversed = 0, loop;
    for(loop = 0; loop < BUFFER_SIZE_LOG; loop++) {
        reversed <<= 1;
        reversed += (initial & 1);
        initial >>= 1;
    }
    return reversed;
}

void Spectrum::performFFT(const sound_sample *input) {
    /* Convert to reverse bit order for FFT */
    prepFFT(input, fftState->real, fftState->imag);

    /* Calculate FFT */
    calcFFT(fftState->real, fftState->imag);

    /* Convert FFT to intensities */
    outputFFT(fftState->real, fftState->imag);
}

void Spectrum::prepFFT(const sound_sample *input, float * re, float * im) {
    unsigned int i;
    float *realptr = re;
    float *imagptr = im;

    /* Get input, in reverse bit order */
    for(i = 0; i < BUFFER_SIZE; i++) {
        *realptr++ = input[bitReverse[i]];
        *imagptr++ = 0;
    }
}

void Spectrum::calcFFT(float * re, float * im) {
    unsigned int i, j, k;
    unsigned int exchanges;
    float fact_real, fact_imag;
    float tmp_real, tmp_imag;
    unsigned int factfact;

    /* Set up some variables to reduce calculation in the loops */
    exchanges = 1;
    factfact = BUFFER_SIZE / 2;

    /* divide and conquer method */
    for(i = BUFFER_SIZE_LOG; i != 0; i--) {
        for(j = 0; j != exchanges; j++) {
            fact_real = costable[j * factfact];
            fact_imag = sintable[j * factfact];
            for(k = j; k < BUFFER_SIZE; k += exchanges << 1) {
                int k1 = k + exchanges;
                tmp_real = fact_real * re[k1] - fact_imag * im[k1];
                tmp_imag = fact_real * im[k1] + fact_imag * re[k1];
                re[k1] = re[k] - tmp_real;
                im[k1] = im[k] - tmp_imag;
                re[k]  += tmp_real;
                im[k]  += tmp_imag;
            }
        }
        exchanges <<= 1;
        factfact >>= 1;
    }
}

void Spectrum::outputFFT(const float * re, const float * im) {
    const float *realptr   = re;
    const float *imagptr   = im;
    float       *outputptr = output;

    float *endptr    = output + BUFFER_SIZE / 2;

    /* Convert FFT to intensities */

    while(outputptr <= endptr) {
        *outputptr = (*realptr * *realptr) + (*imagptr * *imagptr);
        outputptr++; realptr++; imagptr++;
    }
    *output /= 4;
    *endptr /= 4;
}


RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    samples = 0;
    sampleSize = 0;
    setMinimumHeight(30);
    setMinimumWidth(200);
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    if(sampleSize == 0)
        return;

    painter.setPen(Qt::red);
    int max = 0;
    for(int i=0;i<sampleSize;i++) {
        int m = (int)(sqrt(samples[i])/32768);
        if(m > max)
            max = m;
    }
    int x1,y1,x2,y2;

    for(int i=0;i<10;i++) {
        x1 = painter.viewport().left()+11;
        y1 = painter.viewport().top()+10+i;
        x2 = painter.viewport().right()-20-max;
        y2 = painter.viewport().top()+10+i;
        if(x2 < painter.viewport().left()+10)
            x2 = painter.viewport().left()+10;

        painter.drawLine(QPoint(x1,y1),QPoint(x2,y2));
    }

    painter.setPen(Qt::black);
    painter.drawRect(QRect(painter.viewport().left()+10, painter.viewport().top()+10,
                painter.viewport().right()-20, painter.viewport().bottom()-20));
}

void RenderArea::spectrum(float* output, int size)
{
    samples = output;
    sampleSize = size;
    repaint();
}


InputTest::InputTest()
{
    QWidget *window = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    canvas = new RenderArea;
    layout->addWidget(canvas);

    deviceBox = new QComboBox(this);
    QList<QAudioDeviceId> devices = QAudioDeviceInfo::deviceList(QAudio::AudioInput);
    for(int i = 0; i < devices.size(); ++i) {
        deviceBox->addItem(QAudioDeviceInfo(devices.at(i)).deviceName(), qVariantFromValue(devices.at(i)));
    }
    connect(deviceBox,SIGNAL(activated(int)),SLOT(deviceChanged(int)));
    layout->addWidget(deviceBox);

    button = new QPushButton(this);
    button->setText(tr("Click for Push Mode"));
    connect(button,SIGNAL(clicked()),SLOT(toggleMode()));
    layout->addWidget(button);

    button2 = new QPushButton(this);
    button2->setText(tr("Click To Suspend"));
    connect(button2,SIGNAL(clicked()),SLOT(toggleSuspend()));
    layout->addWidget(button2);

    window->setLayout(layout);
    setCentralWidget(window);
    window->show();

    buffer = new char[BUFFER_SIZE*10];
    output = new float[1024];

    pullMode = true;

    format.setFrequency(8000);
    format.setChannels(1);
    format.setSampleSize(8);
    format.setSampleType(QAudioFormat::UnSignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");

    audioInput = new QAudioInput(format,this);
    connect(audioInput,SIGNAL(notify()),SLOT(status()));
    connect(audioInput,SIGNAL(stateChanged(QAudio::State)),SLOT(state(QAudio::State)));
    spec  = new Spectrum(this,audioInput,output);
    connect(spec,SIGNAL(update()),SLOT(refreshDisplay()));
    spec->start();
    audioInput->start(spec);
}

InputTest::~InputTest() {}

void InputTest::status()
{
    qWarning()<<"bytesReady = "<<audioInput->bytesReady()<<" bytes, clock = "<<audioInput->clock()<<"ms, totalTime = "<<audioInput->totalTime()/1000<<"ms";
}

void InputTest::readMore()
{
    if(!audioInput)
        return;
    qint64 len = audioInput->bytesReady();
    if(len > BUFFER_SIZE*10)
        len = BUFFER_SIZE*10;
    qint64 l = input->read(buffer,len);
    if(l > 0) {
        spec->write(buffer,l);
    }
}

void InputTest::toggleMode()
{
    // Change bewteen pull and push modes
    audioInput->stop();

    if(pullMode) {
        button->setText(tr("Click for Push Mode"));
        input = audioInput->start(0);
        connect(input,SIGNAL(readyRead()),SLOT(readMore()));
        pullMode = false;
    } else {
        button->setText(tr("Click for Pull Mode"));
        pullMode = true;
        audioInput->start(spec);
    }
}

void InputTest::toggleSuspend()
{
    // toggle suspend/resume
    if(audioInput->state() == QAudio::SuspendState) {
        qWarning()<<"status: Suspended, resume()";
        audioInput->resume();
        button2->setText("Click To Suspend");
    } else if (audioInput->state() == QAudio::ActiveState) {
        qWarning()<<"status: Active, suspend()";
        audioInput->suspend();
        button2->setText("Click To Resume");
    } else if (audioInput->state() == QAudio::StopState) {
        qWarning()<<"status: Stopped, resume()";
        audioInput->resume();
        button2->setText("Click To Suspend");
    } else if (audioInput->state() == QAudio::IdleState) {
        qWarning()<<"status: IdleState";
    }
}

void InputTest::state(QAudio::State state)
{
    qWarning()<<" state="<<state;
}

void InputTest::refreshDisplay()
{
    canvas->spectrum(output,256);
    canvas->repaint();
}

void InputTest::deviceChanged(int idx)
{
    spec->stop();
    audioInput->stop();
    audioInput->disconnect(this);
    delete audioInput;

    device = deviceBox->itemData(idx).value<QAudioDeviceId>();
    audioInput = new QAudioInput(device, format, this);
    connect(audioInput,SIGNAL(notify()),SLOT(status()));
    connect(audioInput,SIGNAL(stateChanged(QAudio::State)),SLOT(state(QAudio::State)));
    spec->start();
    audioInput->start(spec);
}
