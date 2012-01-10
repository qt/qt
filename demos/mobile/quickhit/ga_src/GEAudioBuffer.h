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

#ifndef __GE_IGA_AUDIOBUFFER__
#define __GE_IGA_AUDIOBUFFER__

#include <QFile>
#include "GEInterfaces.h"


namespace GE {

    class CAudioBufferPlayInstance;
    class CAudioBuffer;         // forward declaration
    typedef AUDIO_SAMPLE_TYPE(*SAMPLE_FUNCTION_TYPE)(CAudioBuffer *abuffer, int pos, int channel);

    class CAudioBuffer {            // container for a sound
    public:
        CAudioBuffer();
        virtual ~CAudioBuffer();

        static CAudioBuffer* loadWav( QString fileName );
        static CAudioBuffer* loadWav( FILE *wavFile );          // support for stdio
        void reallocate( int length );


        inline void* getRawData() { return m_data; }
        inline int getDataLength() { return m_dataLength; }

        inline int getBytesPerSample() { return (m_bitsPerSample>>3); }
        inline int getBitsPerSample() { return m_bitsPerSample; }
        inline int getSamplesPerSec() { return m_samplesPerSec; }
        inline short getNofChannels() { return m_nofChannels; }
        inline SAMPLE_FUNCTION_TYPE getSampleFunction() { return m_sampleFunction; }


        // static implementations of sample functions
        static AUDIO_SAMPLE_TYPE sampleFunction8bitMono( CAudioBuffer *abuffer, int pos, int channel );
        static AUDIO_SAMPLE_TYPE sampleFunction16bitMono( CAudioBuffer *abuffer, int pos, int channel );
        static AUDIO_SAMPLE_TYPE sampleFunction8bitStereo( CAudioBuffer *abuffer, int pos, int channel );
        static AUDIO_SAMPLE_TYPE sampleFunction16bitStereo( CAudioBuffer *abuffer, int pos, int channel );

        CAudioBufferPlayInstance *playWithMixer( GE::CAudioMixer &mixer );

    protected:
        SAMPLE_FUNCTION_TYPE m_sampleFunction;
        short m_nofChannels;
        void *m_data;
        int m_dataLength;                // in bytes
        short m_bitsPerSample;
        bool m_signedData;
        int m_samplesPerSec;
    };



    class CAudioBufferPlayInstance : public IAudioSource {
    public:
        CAudioBufferPlayInstance();
        CAudioBufferPlayInstance( CAudioBuffer *start_playing );
        virtual ~CAudioBufferPlayInstance();
        void playBuffer( CAudioBuffer *startPlaying, float volume, float fixedSpeed, int loopTimes = 0 );            // looptimes -1 = loop forever

        void setSpeed( float speed );
        void setLeftVolume( float lvol );
        void setRightVolume( float rvol );


        inline void setLoopTimes( int ltimes ) { m_loopTimes = ltimes; }
        void stop();



        int pullAudio( AUDIO_SAMPLE_TYPE *target, int bufferLength );
        bool canBeDestroyed();

        bool isPlaying() { if (m_buffer) return true; else return false; }
        inline bool isFinished() { return m_finished; }
        inline bool destroyWhenFinished() { return m_destroyWhenFinished; }
        inline void setDestroyWhenFinished( bool set ) { m_destroyWhenFinished = set; }

    protected:
        int mixBlock( AUDIO_SAMPLE_TYPE *target, int bufferLength );
        bool m_finished;
        bool m_destroyWhenFinished;
        int m_fixedPos;
        int m_fixedInc;

        int m_fixedLeftVolume;
        int m_fixedRightVolume;
        int m_fixedCenter;
        int m_loopTimes;
        CAudioBuffer *m_buffer;
    };

};



#endif
