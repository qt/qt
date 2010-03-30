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

#include "qt7playercontrol.h"
#include "qt7videooutputcontrol.h"
#include "qt7playersession.h"
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

QT7VideoOutputControl::QT7VideoOutputControl(QObject *parent)
   :QVideoOutputControl(parent), 
    m_session(0),
    m_output(QVideoOutputControl::NoOutput)
{    
}

QT7VideoOutputControl::~QT7VideoOutputControl()
{
}

void QT7VideoOutputControl::setSession(QT7PlayerSession *session)
{
    m_session = session;
}

QList<QVideoOutputControl::Output> QT7VideoOutputControl::availableOutputs() const
{
    return m_outputs;
}

void QT7VideoOutputControl::enableOutput(QVideoOutputControl::Output output)
{
    if (!m_outputs.contains(output))
        m_outputs.append(output);
}

QVideoOutputControl::Output QT7VideoOutputControl::output() const
{
    return m_output;
}

void QT7VideoOutputControl::setOutput(Output output)
{
    if (m_output != output) {
        m_output = output;
        emit videoOutputChanged(m_output);
    }

}

QT_END_NAMESPACE

#include "moc_qt7videooutputcontrol.cpp"

