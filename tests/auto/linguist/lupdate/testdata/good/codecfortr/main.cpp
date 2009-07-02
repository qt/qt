#include <QApplication>
#include <QtGui>
#include <QtCore>
#include <QTextCodec>

int main(int argc, char **argv)
{
        QApplication a(argc, argv);
        QTranslator trans(0);

        trans.load("t1_en", ".");

        a.installTranslator(&trans);
        QWidget w;
/*
        QLabel label1(QObject::tr("\33"), &w);
        QLabel label2(QObject::tr("\32"), &w);
        QLabel label3(QObject::tr("\176"), &w);
*/
        QLabel label4(QObject::tr("\301"), &w);

        w.show();
        return a.exec();
}
