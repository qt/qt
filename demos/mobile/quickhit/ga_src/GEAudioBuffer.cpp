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

#include <QDebug>
#include <math.h>
#include "GEAudioBuffer.h"

using namespace GE;


struct SWavHeader {
    char chunkID[4];
    unsigned int chunkSize;
    char format[4];

    unsigned char subchunk1id[4];
    unsigned int subchunk1size;
    unsigned short audioFormat;
    unsigned short nofChannels;
    unsigned int sampleRate;
    unsigned int byteRate;

    unsigned short blockAlign;
    unsigned short bitsPerSample;

    unsigned char subchunk2id[4];
    unsigned int subchunk2size;

};

CAudioBuffer::CAudioBuffer() {
    m_data = 0;
    m_dataLength = 0;
    m_sampleFunction = 0;
};


CAudioBuffer::~CAudioBuffer() {
    reallocate(0);
}

void CAudioBuffer::reallocate( int length ) {
    if (m_data) delete [] ((char*)m_data);
    m_dataLength = length;
    if (m_dataLength>0) {
        m_data = new char[ m_dataLength ];
    } else m_data = 0;
};


CAudioBuffer* CAudioBuffer::loadWav( QString fileName ) {
    QFile *wavFile = new QFile( fileName );


    if (wavFile->open(QIODevice::ReadOnly)) {
        SWavHeader header;

        wavFile->read( header.chunkID, 4 );
        if (header.chunkID[0]!='R' || header.chunkID[1]!='I' || header.chunkID[2]!='F' || header.chunkID[3]!='F') return 0;    //  incorrect header

        wavFile->read( (char*)&header.chunkSize,4 );
        wavFile->read( (char*)&header.format,4 );

        if (header.format[0]!='W' || header.format[1]!='A' || header.format[2]!='V' || header.format[3]!='E') return 0;    //  incorrect header

        wavFile->read( (char*)&header.subchunk1id,4 );
        if (header.subchunk1id[0]!='f' || header.subchunk1id[1]!='m' || header.subchunk1id[2]!='t' || header.subchunk1id[3]!=' ') return 0;    //  incorrect header

        wavFile->read( (char*)&header.subchunk1size,4 );
        wavFile->read( (char*)&header.audioFormat,2 );
        wavFile->read( (char*)&header.nofChannels,2 );
        wavFile->read( (char*)&header.sampleRate,4 );
        wavFile->read( (char*)&header.byteRate,4 );
        wavFile->read( (char*)&header.blockAlign,2 );
        wavFile->read( (char*)&header.bitsPerSample,2 );

        qDebug() << fileName << " opened";

        while (1) {
            if (wavFile->read( (char*)&header.subchunk2id,4 ) != 4) return 0;
            if (wavFile->read( (char*)&header.subchunk2size,4 ) != 4) return 0;
            //int deb_size = header.subchunk2size;
            //char tes[4];
            //memcpy(tes, header.subchunk2id, 4 );
            //if (header.subchunk2id[0]!='d' || header.subchunk2id[1]!='a' || header.subchunk2id[2]!='t' || header.subchunk2id[3]!='a') return 0;    //  incorrect header
            if (header.subchunk2id[0]=='d' && header.subchunk2id[1]=='a' && header.subchunk2id[2]=='t' && header.subchunk2id[3]=='a') break;            // found the data, chunk
            // this was not the data-chunk. skip it
            if (header.subchunk2size<1) return 0;           // error in file
            char *unused = new char[header.subchunk2size];
            wavFile->read( unused, header.subchunk2size );
            delete [] unused;
        }



        // the data follows.
        if (header.subchunk2size<1) return 0;

        CAudioBuffer *rval = new CAudioBuffer;
        rval->m_nofChannels = header.nofChannels;
        rval->m_bitsPerSample = header.bitsPerSample;
        rval->m_samplesPerSec = header.sampleRate;
        rval->m_signedData = 0;        // where to know this?
        rval->reallocate( header.subchunk2size );

        wavFile->read( (char*)rval->m_data, header.subchunk2size );

        // choose a good sampling function.
        rval->m_sampleFunction = 0;
        if (rval->m_nofChannels==1) {
            if (rval->m_bitsPerSample == 8) rval->m_sampleFunction = sampleFunction8bitMono;
            if (rval->m_bitsPerSample == 16) rval->m_sampleFunction = sampleFunction16bitMono;
        } else {
            if (rval->m_bitsPerSample == 8) rval->m_sampleFunction = sampleFunction8bitStereo;
            if (rval->m_bitsPerSample == 16) rval->m_sampleFunction = sampleFunction16bitStereo;
        }

        return rval;


    } else {
        qDebug() << fileName << " NOT opened";
        return 0;
    }

    delete wavFile;
};


CAudioBuffer* CAudioBuffer::loadWav( FILE *wavFile ) {
    // read the header.
    SWavHeader header;
    fread( header.chunkID, 4, 1, wavFile );
    if (header.chunkID[0]!='R' || header.chunkID[1]!='I' || header.chunkID[2]!='F' || header.chunkID[3]!='F') return 0;    //  incorrect header

    fread( &header.chunkSize, 4, 1, wavFile );
    fread( header.format, 4, 1, wavFile );
    if (header.format[0]!='W' || header.format[1]!='A' || header.format[2]!='V' || header.format[3]!='E') return 0;    //  incorrect header

    fread( header.subchunk1id, 4, 1, wavFile );
    if (header.subchunk1id[0]!='f' || header.subchunk1id[1]!='m' || header.subchunk1id[2]!='t' || header.subchunk1id[3]!=' ') return 0;    //  incorrect header

    fread( &header.subchunk1size, 4, 1, wavFile );
    fread( &header.audioFormat, 2, 1, wavFile );
    fread( &header.nofChannels, 2, 1, wavFile );
    fread( &header.sampleRate, 4, 1, wavFile );
    fread( &header.byteRate, 4, 1, wavFile );

    fread( &header.blockAlign, 2, 1, wavFile );
    fread( &header.bitsPerSample, 2, 1, wavFile );

    fread( header.subchunk2id, 4, 1, wavFile );
    if (header.subchunk2id[0]!='d' || header.subchunk2id[1]!='a' || header.subchunk2id[2]!='t' || header.subchunk2id[3]!='a') return 0;    //  incorrect header
    fread( &header.subchunk2size, 4, 1, wavFile );


    // the data follows.
    if (header.subchunk2size<1) return 0;

    CAudioBuffer *rval = new CAudioBuffer;
    rval->m_nofChannels = header.nofChannels;
    rval->m_bitsPerSample = header.bitsPerSample;
    rval->m_samplesPerSec = header.sampleRate;
    rval->m_signedData = 0;        // where to know this?
    rval->reallocate( header.subchunk2size );

    fread( rval->m_data, 1, header.subchunk2size, wavFile );



    // choose a good sampling function.
    rval->m_sampleFunction = 0;
    if (rval->m_nofChannels==1) {
        if (rval->m_bitsPerSample == 8) rval->m_sampleFunction = sampleFunction8bitMono;
        if (rval->m_bitsPerSample == 16) rval->m_sampleFunction = sampleFunction16bitMono;
    } else {
        if (rval->m_bitsPerSample == 8) rval->m_sampleFunction = sampleFunction8bitStereo;
        if (rval->m_bitsPerSample == 16) rval->m_sampleFunction = sampleFunction16bitStereo;
    }

    return rval;
};



AUDIO_SAMPLE_TYPE CAudioBuffer::sampleFunction8bitMono( CAudioBuffer *abuffer, int pos, int channel ) {
    return (AUDIO_SAMPLE_TYPE)(((unsigned char*)(abuffer->m_data))[pos]-128)<<8;
};

AUDIO_SAMPLE_TYPE CAudioBuffer::sampleFunction16bitMono( CAudioBuffer *abuffer, int pos, int channel ) {
    return (AUDIO_SAMPLE_TYPE)(((short*)(abuffer->m_data))[pos]);
};

AUDIO_SAMPLE_TYPE CAudioBuffer::sampleFunction8bitStereo( CAudioBuffer *abuffer, int pos, int channel ) {
    return ((AUDIO_SAMPLE_TYPE)(((char*)(abuffer->m_data))[pos*abuffer->m_nofChannels + channel])<<8);
};


AUDIO_SAMPLE_TYPE CAudioBuffer::sampleFunction16bitStereo( CAudioBuffer *abuffer, int pos, int channel ) {
    return (AUDIO_SAMPLE_TYPE)(((short*)(abuffer->m_data))[pos*abuffer->m_nofChannels + channel]);
};

CAudioBufferPlayInstance *CAudioBuffer::playWithMixer( CAudioMixer &mixer ) {
    CAudioBufferPlayInstance *i = (CAudioBufferPlayInstance*)mixer.addAudioSource( new CAudioBufferPlayInstance( this ));
    return i;
};


CAudioBufferPlayInstance::CAudioBufferPlayInstance() {
    m_fixedPos = 0;
    m_fixedInc = 0;
    m_buffer = 0;
    m_fixedLeftVolume = 4096;
    m_fixedRightVolume = 4096;
    m_destroyWhenFinished = true;
    m_finished = false;
};

CAudioBufferPlayInstance::CAudioBufferPlayInstance( CAudioBuffer *startPlaying ) {
    m_fixedPos = 0;
    m_fixedInc = 0;
    m_fixedLeftVolume = 4096;
    m_fixedRightVolume = 4096;
    m_destroyWhenFinished = true;
    m_finished = false;
    playBuffer( startPlaying, 1.0f, 1.0f );
};

void CAudioBufferPlayInstance::playBuffer( CAudioBuffer *startPlaying, float volume, float speed, int loopTimes ) {
    m_buffer = startPlaying;
    m_fixedLeftVolume = (int)(4096.0f*volume);
    m_fixedRightVolume = m_fixedLeftVolume;
    m_fixedPos = 0;
    //m_fixedInc = ( startPlaying->getSamplesPerSec() * (int)(4096.0f*speed)) / AUDIO_FREQUENCY;
    setSpeed( speed );
    m_loopTimes = loopTimes;

};

CAudioBufferPlayInstance::~CAudioBufferPlayInstance() {

};


void CAudioBufferPlayInstance::stop() {
    m_buffer = 0;
    m_finished = true;
};

void CAudioBufferPlayInstance::setSpeed( float speed ) {
    if (!m_buffer) return;
    m_fixedInc = (int)( ((float)m_buffer->getSamplesPerSec() * 4096.0f*speed) / (float)AUDIO_FREQUENCY );
};

void CAudioBufferPlayInstance::setLeftVolume( float vol ) {
    m_fixedLeftVolume = (int)(4096.0f*vol);
};

void CAudioBufferPlayInstance::setRightVolume( float vol ) {
    m_fixedRightVolume = (int)(4096.0f*vol);
};

bool CAudioBufferPlayInstance::canBeDestroyed() {
    if (m_finished==true &&
        m_destroyWhenFinished==true) return true; else return false;
};



// Does not do any bound-checking. Must be checked before called.
int CAudioBufferPlayInstance::mixBlock( AUDIO_SAMPLE_TYPE *target, int samplesToMix ) {
    SAMPLE_FUNCTION_TYPE sampleFunction = m_buffer->getSampleFunction();
    if (!sampleFunction) return 0; // unsupported sampletype
    AUDIO_SAMPLE_TYPE *t_target = target+samplesToMix*2;
    int sourcepos;
    //int tempCounter = 0;

    if (m_buffer->getNofChannels() == 2) {         // stereo
        while (target!=t_target) {
            sourcepos = m_fixedPos>>12;
            target[0] = (((((sampleFunction)( m_buffer, sourcepos, 0) * (4096-(m_fixedPos&4095)) + (sampleFunction)( m_buffer, sourcepos+1, 0) * (m_fixedPos&4095) ) >> 12) * m_fixedLeftVolume) >> 12);
            target[1] = (((((sampleFunction)( m_buffer, sourcepos, 1) * (4096-(m_fixedPos&4095)) + (sampleFunction)( m_buffer, sourcepos+1, 1) * (m_fixedPos&4095) ) >> 12) * m_fixedRightVolume) >> 12);
            m_fixedPos+=m_fixedInc;
            target+=2;
            //tempCounter++;
        };
    } else {                                      // mono
        int temp;
        while (target!=t_target) {
            sourcepos = m_fixedPos>>12;
            temp = (((sampleFunction)( m_buffer, sourcepos, 0 ) * (4096-(m_fixedPos&4095)) + (sampleFunction)( m_buffer, sourcepos+1, 0 ) * (m_fixedPos&4095) ) >> 12);
            target[0] = ((temp*m_fixedLeftVolume)>>12);
            target[1] = ((temp*m_fixedRightVolume)>>12);
            m_fixedPos+=m_fixedInc;
            target+=2;
            //tempCounter++;
        };

    };

    return samplesToMix;
};



int CAudioBufferPlayInstance::pullAudio( AUDIO_SAMPLE_TYPE *target, int bufferLength ) {
    if (!m_buffer) return 0;            // no sample associated to mix..

    int channelLength = ((m_buffer->getDataLength()) / (m_buffer->getNofChannels()*m_buffer->getBytesPerSample()))-2;

    int samplesToWrite = bufferLength/2;
    int amount;
    int totalMixed = 0;


    while (samplesToWrite>0) {
        int samplesLeft = channelLength - (m_fixedPos>>12);
        int maxMixAmount = (int)(((long long int)(samplesLeft)<<12) / m_fixedInc );         // This is how much we can mix at least
        //int maxMixAmount = (int)((float)samplesLeft / ((float)m_fixedInc/4096.0f));
        //if (maxMixAmount<1) maxMixAmount = 1;           // NOTE, THIS MIGHT CAUSE PROBLEMS. NEEDS CHECKING
        if (maxMixAmount>samplesToWrite) {
            maxMixAmount=samplesToWrite;
        }

        if (maxMixAmount > 0) {
            amount=mixBlock(target+totalMixed * 2, maxMixAmount);

            if (amount == 0)
            {
                // Error!
                break;
            }

            totalMixed+=amount;
        } else {
            amount = 0;
            m_fixedPos = channelLength<<12;
        }

        // sample is ended,.. check the looping variables and see what to do.
        if ((m_fixedPos>>12)>=channelLength) {
            m_fixedPos -= (channelLength<<12);
            if (m_loopTimes>0) m_loopTimes--;
            if (m_loopTimes==0) {
                stop();
                return totalMixed;
            }
        }

        samplesToWrite-=amount;
        if (samplesToWrite<1) break;
    };
    return  totalMixed*2;
};
