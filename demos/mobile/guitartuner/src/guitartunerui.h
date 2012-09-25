/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
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

#ifndef GUITARTUNERUI_H
#define GUITARTUNERUI_H

#include <QWidget>
#include <QDebug>

namespace Ui {
    class GuitarTunerUI;
}

const qreal FrequencyE = 82.407;
const qreal FrequencyA = 110.00;
const qreal FrequencyD = 146.83;
const qreal FrequencyG = 196.00;
const qreal FrequencyB = 246.94;
const qreal FrequencyE2 = 329.63;

class GuitarTunerUI : public QWidget
{
    Q_OBJECT

public:
    explicit GuitarTunerUI(QWidget *parent = 0);
    ~GuitarTunerUI();

    qreal getVolume() const;
    bool getMuteState() const;
    qreal getMicrophoneSensitivity() const;
    bool isInputModeActive() const;
    qreal getFrequency() const;
    int getScaleMaximumValue() const;
    void setMaximumVoiceDifference(int max);
    void setMaximumPrecisionPerNote(int max);

public slots:
    void toggleSound(bool off);
    void next();
    void prev();
    void changeVolume();
    void toggleInputOrOutput();

    void lowVoice();
    void voiceDifference(qreal difference);
    void correctFrequencyObtained();

signals:
    void modeChanged(bool isInput);
    void volumeChanged(qreal volume);
    void microphoneSensitivityChanged(qreal sensitivity);
    void muteChanged(bool isMuted);
    void targetFrequencyChanged(qreal targetFrequency);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GuitarTunerUI *ui;
    void changeTone(int newIndex);
    qreal getVolumeFromSoundSlider() const;
    int m_currentToneIndex;
    QString m_currentToneString;
    qreal m_currentToneFrequency;
    void updateFrequencyByToneIndex(int index);
    bool m_outputActive;
    bool m_muted;
    qreal m_outputVolumeLevel;
    qreal m_inputVolumeLevel;
    int m_maximumPrecision;
};

#endif // GUITARTUNERUI_H
