/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

//The purpose of this example is to show that it is possible to have your own 
// animation with undefined duration

#include <QtGui/QtGui>
#include <phonon>

class SoundAnimation : public QAbstractAnimation
{
public:
    SoundAnimation(const QString &file)
    {
        //in an idea case we should also control the errors
        Phonon::createPath(&m_media, &m_audio);
        m_media.setCurrentSource(file);
        connect(&m_media, SIGNAL(finished()), SLOT(stop()));
    }

    int duration() const
    {
        return -1;
    }

    void updateCurrentTime(int msecs)
    {
        //nothing to do here...
        qDebug() << "updateCurrentTime" << msecs;
    }

    void updateState(State state)
    {
        switch(state)
        {
        case Running:
            m_media.play();
            break;
        case Stopped:
            m_media.stop();
            break;
        case Paused:
            m_media.pause();
            break;
        }
    }


private:
    Phonon::MediaObject m_media;
    Phonon::AudioOutput m_audio;

};


int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(sound);

    SoundAnimation anim(QLatin1String(":/media/sax.mp3"));
    app.connect(&anim, SIGNAL(finished()), SLOT(quit()));
    anim.start();

    return app.exec();
}