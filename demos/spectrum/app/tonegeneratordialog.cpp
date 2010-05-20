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

#include "tonegeneratordialog.h"
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>

const int ToneGeneratorFreqMin = 1;
const int ToneGeneratorFreqMax = 1000;
const int ToneGeneratorFreqDefault = 440;
const int ToneGeneratorAmplitudeDefault = 75;

ToneGeneratorDialog::ToneGeneratorDialog(QWidget *parent)
    :   QDialog(parent)
    ,   m_toneGeneratorSweepCheckBox(new QCheckBox(tr("Frequency sweep"), this))
    ,   m_frequencySweepEnabled(true)
    ,   m_toneGeneratorControl(new QWidget(this))
    ,   m_toneGeneratorFrequencyControl(new QWidget(this))
    ,   m_frequencySlider(new QSlider(Qt::Horizontal, this))
    ,   m_frequencySpinBox(new QSpinBox(this))
    ,   m_frequency(ToneGeneratorFreqDefault)
    ,   m_amplitudeSlider(new QSlider(Qt::Horizontal, this))
{
    QVBoxLayout *dialogLayout = new QVBoxLayout(this);

    m_toneGeneratorSweepCheckBox->setChecked(true);

    // Configure tone generator controls
    m_frequencySlider->setRange(ToneGeneratorFreqMin, ToneGeneratorFreqMax);
    m_frequencySlider->setValue(ToneGeneratorFreqDefault);
    m_frequencySpinBox->setRange(ToneGeneratorFreqMin, ToneGeneratorFreqMax);
    m_frequencySpinBox->setValue(ToneGeneratorFreqDefault);
    m_amplitudeSlider->setRange(0, 100);
    m_amplitudeSlider->setValue(ToneGeneratorAmplitudeDefault);

    // Add widgets to layout

    QScopedPointer<QGridLayout> frequencyControlLayout(new QGridLayout);
    QLabel *frequencyLabel = new QLabel(tr("Frequency (Hz)"), this);
    frequencyControlLayout->addWidget(frequencyLabel, 0, 0, 2, 1);
    frequencyControlLayout->addWidget(m_frequencySlider, 0, 1);
    frequencyControlLayout->addWidget(m_frequencySpinBox, 1, 1);
    m_toneGeneratorFrequencyControl->setLayout(frequencyControlLayout.data());
    frequencyControlLayout.take(); // ownership transferred to m_toneGeneratorFrequencyControl
    m_toneGeneratorFrequencyControl->setEnabled(false);

    QScopedPointer<QGridLayout> toneGeneratorLayout(new QGridLayout);
    QLabel *amplitudeLabel = new QLabel(tr("Amplitude"), this);
    toneGeneratorLayout->addWidget(m_toneGeneratorSweepCheckBox, 0, 1);
    toneGeneratorLayout->addWidget(m_toneGeneratorFrequencyControl, 1, 0, 1, 2);
    toneGeneratorLayout->addWidget(amplitudeLabel, 2, 0);
    toneGeneratorLayout->addWidget(m_amplitudeSlider, 2, 1);
    m_toneGeneratorControl->setLayout(toneGeneratorLayout.data());
    m_toneGeneratorControl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    dialogLayout->addWidget(m_toneGeneratorControl);
    toneGeneratorLayout.take(); // ownership transferred

    // Connect
    CHECKED_CONNECT(m_toneGeneratorSweepCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(frequencySweepEnabled(bool)));
    CHECKED_CONNECT(m_frequencySlider, SIGNAL(valueChanged(int)),
                    m_frequencySpinBox, SLOT(setValue(int)));
    CHECKED_CONNECT(m_frequencySpinBox, SIGNAL(valueChanged(int)),
                    m_frequencySlider, SLOT(setValue(int)));

    // Add standard buttons to layout
    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    dialogLayout->addWidget(buttonBox);

    // Connect standard buttons
    CHECKED_CONNECT(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
                    this, SLOT(accept()));
    CHECKED_CONNECT(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
                    this, SLOT(reject()));

    setLayout(dialogLayout);
}

ToneGeneratorDialog::~ToneGeneratorDialog()
{

}

bool ToneGeneratorDialog::isFrequencySweepEnabled() const
{
    return m_toneGeneratorSweepCheckBox->isChecked();
}

qreal ToneGeneratorDialog::frequency() const
{
    return qreal(m_frequencySlider->value());
}

qreal ToneGeneratorDialog::amplitude() const
{
    return qreal(m_amplitudeSlider->value()) / 100.0;
}

void ToneGeneratorDialog::frequencySweepEnabled(bool enabled)
{
    m_frequencySweepEnabled = enabled;
    m_toneGeneratorFrequencyControl->setEnabled(!enabled);
}
