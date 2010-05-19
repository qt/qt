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

#include "levelmeter.h"

#include <math.h>

#include <QPainter>
#include <QTimer>
#include <QDebug>


// Constants
const int RedrawInterval = 100; // ms
const qreal PeakDecayRate = 0.001;
const int PeakHoldLevelDuration = 2000; // ms


LevelMeter::LevelMeter(QWidget *parent)
    :   QWidget(parent)
    ,   m_rmsLevel(0.0)
    ,   m_peakLevel(0.0)
    ,   m_decayedPeakLevel(0.0)
    ,   m_peakDecayRate(PeakDecayRate)
    ,   m_peakHoldLevel(0.0)
    ,   m_redrawTimer(new QTimer(this))
    ,   m_rmsColor(Qt::red)
    ,   m_peakColor(255, 200, 200, 255)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setMinimumWidth(30);

    connect(m_redrawTimer, SIGNAL(timeout()), this, SLOT(redrawTimerExpired()));
    m_redrawTimer->start(RedrawInterval);
}

LevelMeter::~LevelMeter()
{

}

void LevelMeter::reset()
{
    m_rmsLevel = 0.0;
    m_peakLevel = 0.0;
    update();
}

void LevelMeter::levelChanged(qreal rmsLevel, qreal peakLevel, int numSamples)
{
    // Smooth the RMS signal
    const qreal smooth = pow(qreal(0.9), static_cast<qreal>(numSamples) / 256); // TODO: remove this magic number
    m_rmsLevel = (m_rmsLevel * smooth) + (rmsLevel * (1.0 - smooth));

    if (peakLevel > m_decayedPeakLevel) {
        m_peakLevel = peakLevel;
        m_decayedPeakLevel = peakLevel;
        m_peakLevelChanged.start();
    }

    if (peakLevel > m_peakHoldLevel) {
        m_peakHoldLevel = peakLevel;
        m_peakHoldLevelChanged.start();
    }

    update();
}

void LevelMeter::redrawTimerExpired()
{
    // Decay the peak signal
    const int elapsedMs = m_peakLevelChanged.elapsed();
    const qreal decayAmount = m_peakDecayRate * elapsedMs;
    if (decayAmount < m_peakLevel)
        m_decayedPeakLevel = m_peakLevel - decayAmount;
    else
        m_decayedPeakLevel = 0.0;

    // Check whether to clear the peak hold level
    if (m_peakHoldLevelChanged.elapsed() > PeakHoldLevelDuration)
        m_peakHoldLevel = 0.0;

    update();
}

void LevelMeter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    QRect bar = rect();

    bar.setTop(rect().top() + (1.0 - m_peakHoldLevel) * rect().height());
    bar.setBottom(bar.top() + 5);
    painter.fillRect(bar, m_rmsColor);
    bar.setBottom(rect().bottom());

    bar.setTop(rect().top() + (1.0 - m_decayedPeakLevel) * rect().height());
    painter.fillRect(bar, m_peakColor);

    bar.setTop(rect().top() + (1.0 - m_rmsLevel) * rect().height());
    painter.fillRect(bar, m_rmsColor);
}
