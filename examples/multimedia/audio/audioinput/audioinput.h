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

#include <QPixmap>
#include <QWidget>
#include <QObject>
#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>

#include <qaudioinput.h>

#define BUFFER_SIZE_LOG 9
#define BUFFER_SIZE (1 << BUFFER_SIZE_LOG)

struct _struct_fft_state {
    float real[BUFFER_SIZE];
    float imag[BUFFER_SIZE];
};
typedef _struct_fft_state   fft_state;
typedef short int           sound_sample;

class Spectrum : public QIODevice
{
    Q_OBJECT
public:
    Spectrum(QObject* parent, QAudioInput* device, float* out);
    ~Spectrum();

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

    QAudioInput*   input;
    float*         output;
    fft_state*     fftState;

    unsigned int bitReverse[BUFFER_SIZE];
    float        sintable[BUFFER_SIZE / 2];
    float        costable[BUFFER_SIZE / 2];

    void prepFFT (const sound_sample *input, float *re, float *im);
    void calcFFT (float *re, float *im);
    void outputFFT (const float *re, const float *im);
    int  reverseBits (unsigned int initial);
    void performFFT (const sound_sample *input);

signals:
    void update();
};


class RenderArea : public QWidget
{
    Q_OBJECT

public:
    RenderArea(QWidget *parent = 0);

    void spectrum(float* output, int size);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QPixmap pixmap;

    float*  samples;
    int     sampleSize;
};

class InputTest : public QMainWindow
{
    Q_OBJECT
public:
    InputTest();
    ~InputTest();

    QAudioDeviceId device;
    QAudioFormat   format;
    QAudioInput*   audioInput;
    Spectrum*      spec;
    QIODevice*     input;
    RenderArea*    canvas;

    bool           pullMode;

    QPushButton*   button;
    QPushButton*   button2;
    QComboBox*     deviceBox;

    char*          buffer;
    float*         output;

private slots:
    void refreshDisplay();
    void status();
    void readMore();
    void toggleMode();
    void toggleSuspend();
    void state(QAudio::State s);
    void deviceChanged(int idx);
};

