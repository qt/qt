/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtMultimedia/qaudio.h>


QT_BEGIN_NAMESPACE

namespace QAudio
{

class RegisterMetaTypes
{
public:
    RegisterMetaTypes()
    {
        qRegisterMetaType<QAudio::Error>();
        qRegisterMetaType<QAudio::State>();
        qRegisterMetaType<QAudio::Mode>();
    }

} _register;

}

/*!
    \namespace QAudio
    \brief The QAudio namespace contains enums used by the audio classes.
    \inmodule QtMultimedia
    \ingroup multimedia
    \since 4.6
*/

/*!
    \enum QAudio::Error

    \value NoError         No errors have occurred
    \value OpenError       An error opening the audio device
    \value IOError         An error occurred during read/write of audio device
    \value UnderrunError   Audio data is not being fed to the audio device at a fast enough rate
    \value FatalError      A non-recoverable error has occurred, the audio device is not usable at this time.
*/

/*!
    \enum QAudio::State

    \value ActiveState     Audio data is being processed, this state is set after start() is called
                           and while audio data is available to be processed.
    \value SuspendedState  The audio device is in a suspended state, this state will only be entered
                           after suspend() is called.
    \value StoppedState    The audio device is closed, not processing any audio data
    \value IdleState       The QIODevice passed in has no data and audio system's buffer is empty, this state
                           is set after start() is called and while no audio data is available to be processed.
*/

/*!
    \enum QAudio::Mode

    \value AudioOutput   audio output device
    \value AudioInput    audio input device
*/


QT_END_NAMESPACE

