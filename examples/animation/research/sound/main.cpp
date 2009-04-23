/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
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
