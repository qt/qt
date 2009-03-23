
#include <QtGui>
#include <QtCore>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStringList args = app.arguments();

    if (argc <= 1) {
        qDebug() << "Usage: " << qPrintable(args[0]) << " <ts-file>";
        return 1;
    }

    QTranslator trans;
    trans.load(args[1], ".");
    app.installTranslator(&trans);

    QWidget w;
    QVBoxLayout *layout = new QVBoxLayout(&w);

    QLabel label1(QObject::tr("XXXXXXXXX \33 XXXXXXXXXXX  • and → "), 0);
    QLabel label2(QObject::tr("\32"), 0);
    QLabel label3(QObject::tr("\176"), 0);
    QLabel label4(QObject::tr("\301"), 0);

    layout->addWidget(&label1);
    layout->addWidget(&label2);
    layout->addWidget(&label3);
    layout->addWidget(&label4);

    w.show();

    return app.exec();
}
