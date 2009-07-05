#include <QApplication>
#include <QDebug>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QTranslator translator;
    translator.load("whitespace");
    app.installTranslator(&translator);

    QObject::tr("\nnewline at the start");
    QObject::tr("newline at the end\n");
    QObject::tr("newline and space at the end\n ");
    QObject::tr("space and newline at the end \n");
    QObject::tr("\tTab at the start and newline at the end\n");
    QObject::tr("\n\tnewline and tab at the start");
    QObject::tr(" \tspace and tab at the start");
    QObject::tr(" space_first");
    QObject::tr("space_last ");
    return app.exec();
}
