/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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

#include "InvSounds.h"
#include <QDir>
#include <QDebug>

using namespace GE;

CInvSounds::CInvSounds(QObject *parent) : QObject( parent )
{
    m_musicInstance = 0;
    m_sounds = 0;
    m_internalSounds = 0;
    m_soundCount = 0;
    m_internalSoundCount = 0;

    enableSounds(true);

    m_audioOut = new AudioOut( this, &m_mixer );

    enableInternalSounds();
}

void CInvSounds::enableSounds(bool enable)
{
    if (enable)
        m_mixer.setGeneralVolume(0.2f);
    else
        m_mixer.setGeneralVolume(0);
}

void CInvSounds::enableInternalSounds()
{
    disableInternalSounds();

    // Application internal sounda
    m_internalSoundPaths.append(":/sound/menu1.wav");
    m_internalSoundPaths.append(":/sound/menu2.wav");
    m_internalSoundPaths.append(":/sound/gamestart.wav");
    m_internalSoundPaths.append(":/sound/gameover.wav");
    m_internalSoundPaths.append(":/sound/youwin.wav");

    m_internalSoundCount = m_internalSoundPaths.count();
    m_internalSounds = new CAudioBuffer*[m_internalSoundCount];

    // Load general application sounds
    for (int ff=0; ff<m_internalSoundPaths.count(); ff++) {
        m_internalSounds[ff] =  CAudioBuffer::loadWav(m_internalSoundPaths[ff]);
    }
}

void CInvSounds::enableSounds(QStringList sounds)
{
    disableSounds();

    // Sounds from the level
    m_soundPaths = sounds;
    m_soundCount = sounds.count();
    m_sounds = new CAudioBuffer*[m_soundCount];
    // Load level sounds
    for (int f=0; f<m_soundPaths.count(); f++) {
        m_sounds[f] =  CAudioBuffer::loadWav(m_soundPaths[f]);
    }

}

void CInvSounds::disableInternalSounds()
{
    for (int f=0; f<m_internalSoundCount; f++) {
        if (m_internalSounds[f]) {
            delete m_internalSounds[f];
        }
    }
    delete [] m_internalSounds;
    m_internalSoundCount = 0;
    m_internalSoundPaths.clear();

    m_musicInstance = 0;
}

void CInvSounds::disableSounds()
{
    for (int f=0; f<m_soundCount; f++) {
        if (m_sounds[f]) {
            delete m_sounds[f];
        }
    }
    delete [] m_sounds;
    m_sounds = 0;
    m_soundCount = 0;
    m_soundPaths.clear();

    m_musicInstance = 0;
}

CInvSounds::~CInvSounds() {
    if (m_audioOut) {
        delete m_audioOut;
        m_audioOut = 0;
    }

    disableSounds();
    disableInternalSounds();
}


void CInvSounds::beginMusicOn() {
    m_musicInstance = (GE::CAudioBufferPlayInstance*)m_mixer.addAudioSource( new GE::CAudioBufferPlayInstance( m_sounds[0] ) );
    m_musicInstance->setLoopTimes(-1);
}


void CInvSounds::beginMusicOff() {
    if (!m_musicInstance) return;
    m_musicInstance->setLoopTimes(0);       // stop when finished
    m_musicInstance = 0;
};


void CInvSounds::playSound(int index) {
    if (index < m_soundCount && index > -1) {
        m_sounds[index]->playWithMixer( m_mixer );
    }
}

void CInvSounds::playSounds(int index, int count)
{
    if (index < m_soundCount && index > -1) {
        CAudioBufferPlayInstance* i = m_sounds[index]->playWithMixer( m_mixer );
        i->setLoopTimes(count);
    }
}

void CInvSounds::playInternalSound(int index) {
    if (index < m_internalSoundCount && index > -1) {
        m_internalSounds[index]->playWithMixer( m_mixer );
    }
}

void CInvSounds::playInternalSounds(int index, int count)
{
    if (index < m_internalSoundCount && index > -1) {
        CAudioBufferPlayInstance* i = m_internalSounds[index]->playWithMixer( m_mixer );
        i->setLoopTimes(count);
    }
}

void CInvSounds::gameStartSound() {
    CAudioBufferPlayInstance* i = m_internalSounds[2]->playWithMixer( m_mixer );
    i->setLoopTimes(2);
};
