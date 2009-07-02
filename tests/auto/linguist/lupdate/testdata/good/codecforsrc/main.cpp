#include <QApplication>
#include <QtGui>
#include <QtCore>
#include <QTextCodec>

int main(int argc, char **argv)
{
        QApplication a(argc, argv);
        QWidget w;
        QLabel label1(QObject::tr("abc", "ascii"), &w);
        QLabel label2(QObject::tr("æøå", "utf-8"), &w);

//      I would expect the following to work !?
//        QLabel label3(QObject::trUtf8("F\374r \310lise", "trUtf8"), &w);

        w.show();
        return a.exec();
}
