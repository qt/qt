#include <QtGui>

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/videowidget.h>
#include <phonon/medianode.h>
#include <phonon/path.h>


int main(int argv, char **args)
{
    QApplication app(argv, args);

//![0]
    Phonon::MediaObject *mediaObject = new Phonon::MediaObject;
    Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput;
    Phonon::VideoWidget *videoWidget = new Phonon::VideoWidget;

    Phonon::createPath(mediaObject, audioOutput);
    Phonon::createPath(mediaObject, videoWidget);

    QList<Phonon::Path> inputPaths =
        audioOutput->inputPaths();   // inputPaths = [ mediaObject ]
    QList<Phonon::Path> outputPaths =
        mediaObject->outputPaths(); // outputPaths = [ audioOutput, videoWidget ]
//![0]

    return app.exec();
}
