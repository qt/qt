

#include <QtGui>
#include <phonon>


int main(int argv, char **args)
{
    QApplication app(argv, args);

    Phonon::VideoPlayer *player = new Phonon::VideoPlayer(Phonon::VideoCategory, 0);

//![0]
    player->mediaObject()->isSeekable();
//![0]

    return app.exec();
}

