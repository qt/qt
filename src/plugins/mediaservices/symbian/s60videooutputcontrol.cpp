/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "s60videooutputcontrol.h"

QT_BEGIN_NAMESPACE

S60VideoOutputControl::S60VideoOutputControl(QObject *parent)
    : QVideoOutputControl(parent)
    , m_output(NoOutput)
{
}

QList<QVideoOutputControl::Output> S60VideoOutputControl::availableOutputs() const
{
    return m_outputs;
}

void S60VideoOutputControl::setAvailableOutputs(const QList<Output> &outputs)
{
    emit availableOutputsChanged(m_outputs = outputs);
}

QVideoOutputControl::Output S60VideoOutputControl::output() const
{
    return m_output;
}

void S60VideoOutputControl::setOutput(Output output)
{
    if (!m_outputs.contains(output))
        output = NoOutput;

    if (m_output != output)
        emit outputChanged(m_output = output);
}

QT_END_NAMESPACE
