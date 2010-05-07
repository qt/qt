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

#include "progressbar.h"
#include "spectrum.h"
#include <QPainter>

ProgressBar::ProgressBar(QWidget *parent)
    :   QWidget(parent)
    ,   m_bufferDuration(0)
    ,   m_recordPosition(0)
    ,   m_playPosition(0)
    ,   m_windowPosition(0)
    ,   m_windowLength(0)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setMinimumHeight(30);
#ifdef SUPERIMPOSE_PROGRESS_ON_WAVEFORM
    setAutoFillBackground(false);
#endif
}

ProgressBar::~ProgressBar()
{

}

void ProgressBar::reset()
{
    m_bufferDuration = 0;
    m_recordPosition = 0;
    m_playPosition = 0;
    m_windowPosition = 0;
    m_windowLength = 0;
    update();
}

void ProgressBar::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

    QColor bufferColor(0, 0, 255);
    QColor windowColor(0, 255, 0);

#ifdef SUPERIMPOSE_PROGRESS_ON_WAVEFORM
    bufferColor.setAlphaF(0.5);
    windowColor.setAlphaF(0.5);
#else
    painter.fillRect(rect(), Qt::black);
#endif

    if (m_bufferDuration) {
        QRect bar = rect();
        const qreal play = qreal(m_playPosition) / m_bufferDuration;
        bar.setLeft(rect().left() + play * rect().width());
        const qreal record = qreal(m_recordPosition) / m_bufferDuration;
        bar.setRight(rect().left() + record * rect().width());
        painter.fillRect(bar, bufferColor);

        QRect window = rect();
        const qreal windowLeft = qreal(m_windowPosition) / m_bufferDuration;
        window.setLeft(rect().left() + windowLeft * rect().width());
        const qreal windowWidth = qreal(m_windowLength) / m_bufferDuration;
        window.setWidth(windowWidth * rect().width());
        painter.fillRect(window, windowColor);
    }
}

void ProgressBar::bufferDurationChanged(qint64 bufferSize)
{
    m_bufferDuration = bufferSize;
    m_recordPosition = 0;
    m_playPosition = 0;
    m_windowPosition = 0;
    m_windowLength = 0;
    repaint();
}

void ProgressBar::recordPositionChanged(qint64 recordPosition)
{
    Q_ASSERT(recordPosition >= 0);
    Q_ASSERT(recordPosition <= m_bufferDuration);
    m_recordPosition = recordPosition;
    repaint();
}

void ProgressBar::playPositionChanged(qint64 playPosition)
{
    Q_ASSERT(playPosition >= 0);
    Q_ASSERT(playPosition <= m_bufferDuration);
    m_playPosition = playPosition;
    repaint();
}

void ProgressBar::windowChanged(qint64 position, qint64 length)
{
    Q_ASSERT(position >= 0);
    Q_ASSERT(position <= m_bufferDuration);
    Q_ASSERT(position + length <= m_bufferDuration);
    m_windowPosition = position;
    m_windowLength = length;
    repaint();
}
