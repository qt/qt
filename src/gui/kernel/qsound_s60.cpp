/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_EMBEDDED_LICENSE$
**
****************************************************************************/



#ifndef QT_NO_SOUND

#include "qdir.h"
#include "qapplication.h"
#include "qsound.h"
#include "qsound_p.h"
#include "qfileinfo.h"
#include <private/qcore_symbian_p.h>

#include <e32std.h>
#include <MdaAudioSamplePlayer.h>

QT_BEGIN_NAMESPACE

class QAuServerS60;

class QAuBucketS60 : public QAuBucket, public MMdaAudioPlayerCallback
{
public:
    QAuBucketS60( QAuServerS60 *server, QSound *sound);
    ~QAuBucketS60();

    void play();
    void stop();
    
    inline QSound* sound() const { return m_sound; }
    
public: // from MMdaAudioPlayerCallback
    void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    void MapcPlayComplete(TInt aError);    

private:
    QSound *m_sound;
    QAuServerS60 *m_server;    
    bool m_prepared;
    bool m_playCalled;    
    CMdaAudioPlayerUtility* m_playUtility;    
};

    
class QAuServerS60 : public QAuServer
{
public:
    QAuServerS60( QObject* parent );

    void init( QSound* s )
    {
        QAuBucketS60 *bucket = new QAuBucketS60( this, s );
        setBucket( s, bucket );
    }

    void play( QSound* s )
    {
        bucket( s )->play();
    }

    void stop( QSound* s )
    {
        bucket( s )->stop();
    }

    bool okay() { return true; }
    
protected:
    void playCompleted(QAuBucketS60* bucket, int error)  
    {
        QSound *sound = bucket->sound();        
        if(!error) {
            // We need to handle repeats by ourselves, since with Symbian API we don't
            // know how many loops have been played when user asks it
            if( decLoop( sound ) ) {
                play( sound );
            }
        } else {
            // We don't have a way to inform about errors -> just decrement loops
            // in order that QSound::isFinished will return true;
            while(decLoop(sound));
        }
    }    

protected:
    QAuBucketS60* bucket( QSound *s )
    {
        return (QAuBucketS60*)QAuServer::bucket( s );
    }
    
    friend class QAuBucketS60;

};

QAuServerS60::QAuServerS60(QObject* parent) :
    QAuServer(parent)
{
    setObjectName(QLatin1String("QAuServerS60"));
}


QAuServer* qt_new_audio_server()
{
    return new QAuServerS60(qApp);
}

QAuBucketS60::QAuBucketS60( QAuServerS60 *server, QSound *sound )
    : m_sound( sound ), m_server( server ), m_prepared(false), m_playCalled(false)
{
    TRAPD(err, m_playUtility = CMdaAudioPlayerUtility::NewL(*this);
               QString filepath = QFileInfo( m_sound->fileName() ).absoluteFilePath();
               filepath = QDir::toNativeSeparators(filepath);
               m_playUtility->OpenFileL(qt_QString2TPtrC(filepath)));
    if(err){
        m_server->playCompleted(this, err);
    }
}

void QAuBucketS60::play()
{
    if(m_prepared) {
        // OpenFileL call is completed we can start playing immediately
        m_playUtility->Play();      
    } else {
        m_playCalled = true;
    }
        
}

void QAuBucketS60::stop()
{
    m_playCalled = false;
    m_playUtility->Stop();    
}

void QAuBucketS60::MapcPlayComplete(TInt aError)
{
    m_server->playCompleted(this, aError);
}
 
void QAuBucketS60::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)    
{
    if(aError) {
        m_server->playCompleted(this, aError);
    } else {
        m_prepared = true;    
        if(m_playCalled){
            play(); 
        }
    }
}

QAuBucketS60::~QAuBucketS60()
{
    if(m_playUtility){
        m_playUtility->Stop();
        m_playUtility->Close();
    }
    
    delete m_playUtility;
}


#endif // QT_NO_SOUND

QT_END_NAMESPACE
