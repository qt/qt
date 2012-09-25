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

#include <memory.h>
#include "GEInterfaces.h"


using namespace GE;

/**
 * CAudioSource
 * common functionality
 *
 */
IAudioSource::IAudioSource() {
    m_next = 0;
};

IAudioSource::~IAudioSource() {


};

/**
 * CAudioMixer
 *
 */
CAudioMixer::CAudioMixer() {
    m_sourceList = 0;
    m_mixingBuffer = 0;
    m_mixingBufferLength = 0;
    m_fixedGeneralVolume = 4096;
};


CAudioMixer::~CAudioMixer() {
    destroyList();
    if (m_mixingBuffer) {
        delete [] m_mixingBuffer;
        m_mixingBuffer = 0;
    };
};

void CAudioMixer::destroyList() {
    m_mutex.lock();
    IAudioSource *l = m_sourceList;
    while (l) {
        IAudioSource *n = l->m_next;
        delete l;
        l = n;
    };
    m_sourceList = 0;
    m_mutex.unlock();
};


IAudioSource* CAudioMixer::addAudioSource( IAudioSource *source ) {
    m_mutex.lock();
    source->m_next = 0;
    if (m_sourceList) {
        IAudioSource *l = m_sourceList;
        while (l->m_next) l = l->m_next;
        l->m_next = source;
    } else m_sourceList = source;
    m_mutex.unlock();
    return source;

};


bool CAudioMixer::removeAudioSource( IAudioSource *source ) {
    return true;
};

void CAudioMixer::setGeneralVolume( float vol ) {
    m_fixedGeneralVolume = (4096.0f*vol);
};

int CAudioMixer::pullAudio( AUDIO_SAMPLE_TYPE *target, int bufferLength ) {

    if (!m_sourceList) return 0;

    m_mutex.lock();


    if (m_mixingBufferLength<bufferLength) {
        if (m_mixingBuffer) delete [] m_mixingBuffer;
        m_mixingBufferLength = bufferLength;
        m_mixingBuffer = new AUDIO_SAMPLE_TYPE[ m_mixingBufferLength ];
    };

    memset( target, 0, sizeof( AUDIO_SAMPLE_TYPE ) * bufferLength );

    AUDIO_SAMPLE_TYPE *t;
    AUDIO_SAMPLE_TYPE *t_target;
    AUDIO_SAMPLE_TYPE *s;

    IAudioSource *prev = 0;
    IAudioSource *l = m_sourceList;
    while (l) {
        IAudioSource *next = l->m_next;

            // process l
        int mixed = l->pullAudio( m_mixingBuffer, bufferLength );
        if (mixed>0) {
            // mix to main..
            t = target;
            t_target = t+mixed;
            s = m_mixingBuffer;
            while (t!=t_target) {
                *t +=(((*s)*m_fixedGeneralVolume)>>12);
                t++;
                s++;
            };
        };



        // autodestroy
        if (l->canBeDestroyed() == true) {          // NOTE, IS UNDER TESTING,... MIGHT CAUSE UNPREDICTABLE CRASHING WITH SOME USE CASES!!!
            if (!prev)
                m_sourceList = next;
            else prev->m_next = next;
            delete l;
            l = 0;
        };




        prev = l;
        l = next;
    };
    m_mutex.unlock();
    return bufferLength;
};




