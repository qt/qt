/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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

#include "engine.h"
#include "levelmeter.h"
#include "mainwidget.h"
#include "waveform.h"
#include "progressbar.h"
#include "settingsdialog.h"
#include "spectrograph.h"
#include "tonegeneratordialog.h"
#include "utils.h"

#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyle>
#include <QMenu>
#include <QFileDialog>
#include <QTimerEvent>
#include <QMessageBox>

const int NullTimerId = -1;

MainWidget::MainWidget(QWidget *parent)
    :   QWidget(parent)
    ,   m_mode(NoMode)
    ,   m_engine(new Engine(this))
#ifndef DISABLE_WAVEFORM
    ,   m_waveform(new Waveform(m_engine->buffer(), this))
#endif
    ,   m_progressBar(new ProgressBar(this))
    ,   m_spectrograph(new Spectrograph(this))
    ,   m_levelMeter(new LevelMeter(this))
    ,   m_modeButton(new QPushButton(this))
    ,   m_recordButton(new QPushButton(this))
    ,   m_pauseButton(new QPushButton(this))
    ,   m_playButton(new QPushButton(this))
    ,   m_settingsButton(new QPushButton(this))
    ,   m_infoMessage(new QLabel(tr("Select a mode to begin"), this))
    ,   m_infoMessageTimerId(NullTimerId)
    ,   m_settingsDialog(new SettingsDialog(
            m_engine->availableAudioInputDevices(),
            m_engine->availableAudioOutputDevices(),
            this))
    ,   m_toneGeneratorDialog(new ToneGeneratorDialog(this))
    ,   m_modeMenu(new QMenu(this))
    ,   m_loadFileAction(0)
    ,   m_generateToneAction(0)
    ,   m_recordAction(0)
{
    m_spectrograph->setParams(SpectrumNumBands, SpectrumLowFreq, SpectrumHighFreq);

    createUi();
    connectUi();
}

MainWidget::~MainWidget()
{

}


//-----------------------------------------------------------------------------
// Public slots
//-----------------------------------------------------------------------------

void MainWidget::stateChanged(QAudio::Mode mode, QAudio::State state)
{
    Q_UNUSED(mode);

    updateButtonStates();

    if (QAudio::ActiveState != state && QAudio::SuspendedState != state) {
        m_levelMeter->reset();
        m_spectrograph->reset();
    }
}

void MainWidget::formatChanged(const QAudioFormat &format)
{
   infoMessage(formatToString(format), NullMessageTimeout);

#ifndef DISABLE_WAVEFORM
    if (QAudioFormat() != format) {
        m_waveform->initialize(format, WaveformTileLength,
                               WaveformWindowDuration);
    }
#endif
}

void MainWidget::spectrumChanged(qint64 position, qint64 length,
                                 const FrequencySpectrum &spectrum)
{
    m_progressBar->windowChanged(position, length);
    m_spectrograph->spectrumChanged(spectrum);
}

void MainWidget::infoMessage(const QString &message, int timeoutMs)
{
    m_infoMessage->setText(message);

    if (NullTimerId != m_infoMessageTimerId) {
        killTimer(m_infoMessageTimerId);
        m_infoMessageTimerId = NullTimerId;
    }

    if (NullMessageTimeout != timeoutMs)
        m_infoMessageTimerId = startTimer(timeoutMs);
}

void MainWidget::errorMessage(const QString &heading, const QString &detail)
{
#ifdef Q_OS_SYMBIAN
    const QString message = heading + "\n" + detail;
    QMessageBox::warning(this, "", message, QMessageBox::Close);
#else
    QMessageBox::warning(this, heading, detail, QMessageBox::Close);
#endif
}

void MainWidget::timerEvent(QTimerEvent *event)
{
    Q_ASSERT(event->timerId() == m_infoMessageTimerId);
    Q_UNUSED(event) // suppress warnings in release builds
    killTimer(m_infoMessageTimerId);
    m_infoMessageTimerId = NullTimerId;
    m_infoMessage->setText("");
}

void MainWidget::positionChanged(qint64 positionUs)
{
#ifndef DISABLE_WAVEFORM
    qint64 positionBytes = audioLength(m_engine->format(), positionUs);
    m_waveform->positionChanged(positionBytes);
#else
    Q_UNUSED(positionUs)
#endif
}

void MainWidget::bufferDurationChanged(qint64 durationUs)
{
    m_progressBar->bufferDurationChanged(durationUs);
}


//-----------------------------------------------------------------------------
// Private slots
//-----------------------------------------------------------------------------

void MainWidget::dataDurationChanged(qint64 duration)
{
#ifndef DISABLE_WAVEFORM
    const qint64 dataLength = audioLength(m_engine->format(), duration);
    m_waveform->dataLengthChanged(dataLength);
#else
    Q_UNUSED(duration)
#endif

    updateButtonStates();
}

void MainWidget::showFileDialog()
{
    reset();
    const QString dir;
    const QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open WAV file"), dir, "*.wav");
    if (fileNames.count()) {
        setMode(LoadFileMode);
        m_engine->loadFile(fileNames.front());
        updateButtonStates();
    }
}

void MainWidget::showSettingsDialog()
{
    reset();
    m_settingsDialog->exec();
    if (m_settingsDialog->result() == QDialog::Accepted) {
        m_engine->setAudioInputDevice(m_settingsDialog->inputDevice());
        m_engine->setAudioOutputDevice(m_settingsDialog->outputDevice());
        m_engine->setWindowFunction(m_settingsDialog->windowFunction());
    }
}

void MainWidget::showToneGeneratorDialog()
{
    reset();
    m_toneGeneratorDialog->exec();
    if (m_toneGeneratorDialog->result() == QDialog::Accepted) {
        setMode(GenerateToneMode);
        const qreal amplitude = m_toneGeneratorDialog->amplitude();
        if (m_toneGeneratorDialog->isFrequencySweepEnabled()) {
            m_engine->generateSweptTone(amplitude);
        } else {
            const qreal frequency = m_toneGeneratorDialog->frequency();
            const Tone tone(frequency, amplitude);
            m_engine->generateTone(tone);
            updateButtonStates();
        }
    }
}

void MainWidget::initializeRecord()
{
    reset();
    setMode(RecordMode);
    if (m_engine->initializeRecord())
        updateButtonStates();
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void MainWidget::createUi()
{
    createMenus();

    setWindowTitle(tr("Spectrum Analyser"));

    QVBoxLayout *windowLayout = new QVBoxLayout(this);

    m_infoMessage->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_infoMessage->setAlignment(Qt::AlignHCenter);
    windowLayout->addWidget(m_infoMessage);

#ifdef SUPERIMPOSE_PROGRESS_ON_WAVEFORM
    QScopedPointer<QHBoxLayout> waveformLayout(new QHBoxLayout);
    waveformLayout->addWidget(m_progressBar);
    m_progressBar->setMinimumHeight(m_waveform->minimumHeight());
    waveformLayout->setMargin(0);
    m_waveform->setLayout(waveformLayout.data());
    waveformLayout.take();
    windowLayout->addWidget(m_waveform);
#else
#ifndef DISABLE_WAVEFORM
    windowLayout->addWidget(m_waveform);
#endif // DISABLE_WAVEFORM
    windowLayout->addWidget(m_progressBar);
#endif // SUPERIMPOSE_PROGRESS_ON_WAVEFORM

    // Spectrograph and level meter

    QScopedPointer<QHBoxLayout> analysisLayout(new QHBoxLayout);
    analysisLayout->addWidget(m_spectrograph);
    analysisLayout->addWidget(m_levelMeter);
    windowLayout->addLayout(analysisLayout.data());
    analysisLayout.take();

    // Button panel

    const QSize buttonSize(30, 30);

    m_modeButton->setText(tr("Mode"));

    m_recordIcon = QIcon(":/images/record.png");
    m_recordButton->setIcon(m_recordIcon);
    m_recordButton->setEnabled(false);
    m_recordButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_recordButton->setMinimumSize(buttonSize);

    m_pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
    m_pauseButton->setIcon(m_pauseIcon);
    m_pauseButton->setEnabled(false);
    m_pauseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pauseButton->setMinimumSize(buttonSize);

    m_playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
    m_playButton->setIcon(m_playIcon);
    m_playButton->setEnabled(false);
    m_playButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_playButton->setMinimumSize(buttonSize);

    m_settingsIcon = QIcon(":/images/settings.png");
    m_settingsButton->setIcon(m_settingsIcon);
    m_settingsButton->setEnabled(true);
    m_settingsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_settingsButton->setMinimumSize(buttonSize);

    QScopedPointer<QHBoxLayout> buttonPanelLayout(new QHBoxLayout);
    buttonPanelLayout->addStretch();
    buttonPanelLayout->addWidget(m_modeButton);
    buttonPanelLayout->addWidget(m_recordButton);
    buttonPanelLayout->addWidget(m_pauseButton);
    buttonPanelLayout->addWidget(m_playButton);
    buttonPanelLayout->addWidget(m_settingsButton);

    QWidget *buttonPanel = new QWidget(this);
    buttonPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    buttonPanel->setLayout(buttonPanelLayout.data());
    buttonPanelLayout.take(); // ownership transferred to buttonPanel

    QScopedPointer<QHBoxLayout> bottomPaneLayout(new QHBoxLayout);
    bottomPaneLayout->addWidget(buttonPanel);
    windowLayout->addLayout(bottomPaneLayout.data());
    bottomPaneLayout.take(); // ownership transferred to windowLayout

    // Apply layout

    setLayout(windowLayout);
}

void MainWidget::connectUi()
{
    CHECKED_CONNECT(m_recordButton, SIGNAL(clicked()),
            m_engine, SLOT(startRecording()));

    CHECKED_CONNECT(m_pauseButton, SIGNAL(clicked()),
            m_engine, SLOT(suspend()));

    CHECKED_CONNECT(m_playButton, SIGNAL(clicked()),
            m_engine, SLOT(startPlayback()));

    CHECKED_CONNECT(m_settingsButton, SIGNAL(clicked()),
            this, SLOT(showSettingsDialog()));

    CHECKED_CONNECT(m_engine, SIGNAL(stateChanged(QAudio::Mode,QAudio::State)),
            this, SLOT(stateChanged(QAudio::Mode,QAudio::State)));

    CHECKED_CONNECT(m_engine, SIGNAL(formatChanged(const QAudioFormat &)),
            this, SLOT(formatChanged(const QAudioFormat &)));

    m_progressBar->bufferDurationChanged(m_engine->bufferDuration());

    CHECKED_CONNECT(m_engine, SIGNAL(bufferDurationChanged(qint64)),
            this, SLOT(bufferDurationChanged(qint64)));

    CHECKED_CONNECT(m_engine, SIGNAL(dataDurationChanged(qint64)),
            this, SLOT(dataDurationChanged(qint64)));

    CHECKED_CONNECT(m_engine, SIGNAL(recordPositionChanged(qint64)),
            m_progressBar, SLOT(recordPositionChanged(qint64)));

    CHECKED_CONNECT(m_engine, SIGNAL(playPositionChanged(qint64)),
            m_progressBar, SLOT(playPositionChanged(qint64)));

    CHECKED_CONNECT(m_engine, SIGNAL(recordPositionChanged(qint64)),
            this, SLOT(positionChanged(qint64)));

    CHECKED_CONNECT(m_engine, SIGNAL(playPositionChanged(qint64)),
            this, SLOT(positionChanged(qint64)));

    CHECKED_CONNECT(m_engine, SIGNAL(levelChanged(qreal, qreal, int)),
            m_levelMeter, SLOT(levelChanged(qreal, qreal, int)));

    CHECKED_CONNECT(m_engine, SIGNAL(spectrumChanged(qint64, qint64, const FrequencySpectrum &)),
            this, SLOT(spectrumChanged(qint64, qint64, const FrequencySpectrum &)));

    CHECKED_CONNECT(m_engine, SIGNAL(infoMessage(QString, int)),
            this, SLOT(infoMessage(QString, int)));

    CHECKED_CONNECT(m_engine, SIGNAL(errorMessage(QString, QString)),
            this, SLOT(errorMessage(QString, QString)));

    CHECKED_CONNECT(m_spectrograph, SIGNAL(infoMessage(QString, int)),
            this, SLOT(infoMessage(QString, int)));
}

void MainWidget::createMenus()
{
    m_modeButton->setMenu(m_modeMenu);

    m_generateToneAction = m_modeMenu->addAction(tr("Play generated tone"));
    m_recordAction = m_modeMenu->addAction(tr("Record and play back"));
    m_loadFileAction = m_modeMenu->addAction(tr("Play file"));

    m_loadFileAction->setCheckable(true);
    m_generateToneAction->setCheckable(true);
    m_recordAction->setCheckable(true);

    connect(m_loadFileAction, SIGNAL(triggered(bool)), this, SLOT(showFileDialog()));
    connect(m_generateToneAction, SIGNAL(triggered(bool)), this, SLOT(showToneGeneratorDialog()));
    connect(m_recordAction, SIGNAL(triggered(bool)), this, SLOT(initializeRecord()));
}

void MainWidget::updateButtonStates()
{
    const bool recordEnabled = ((QAudio::AudioOutput == m_engine->mode() ||
                                (QAudio::ActiveState != m_engine->state() &&
                                 QAudio::IdleState != m_engine->state())) &&
                                RecordMode == m_mode);
    m_recordButton->setEnabled(recordEnabled);

    const bool pauseEnabled = (QAudio::ActiveState == m_engine->state() ||
                               QAudio::IdleState == m_engine->state());
    m_pauseButton->setEnabled(pauseEnabled);

    const bool playEnabled = (m_engine->dataDuration() &&
                              (QAudio::AudioOutput != m_engine->mode() ||
                               (QAudio::ActiveState != m_engine->state() &&
                                QAudio::IdleState != m_engine->state())));
    m_playButton->setEnabled(playEnabled);
}

void MainWidget::reset()
{
#ifndef DISABLE_WAVEFORM
    m_waveform->reset();
#endif
    m_engine->reset();
    m_levelMeter->reset();
    m_spectrograph->reset();
    m_progressBar->reset();
}

void MainWidget::setMode(Mode mode)
{

    m_mode = mode;
    m_loadFileAction->setChecked(LoadFileMode == mode);
    m_generateToneAction->setChecked(GenerateToneMode == mode);
    m_recordAction->setChecked(RecordMode == mode);
}

