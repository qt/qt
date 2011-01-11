#include <QtGui>
#include <QApplication>
#include <QEvent>

struct MyApplication : public QApplication
{
    MyApplication(int& argc, char** argv)
    : QApplication(argc, argv)
    {}

    bool event(QEvent * event)
    {
        if (event->type() == QEvent::FileOpen) {
            QFileOpenEvent* ev = static_cast<QFileOpenEvent *>(event);
            QFile file;
            bool ok = ev->openFile(file, QFile::Append | QFile::Unbuffered);
            if (ok)
                file.write(QByteArray("+external"));
            return true;
        } else {
            return QApplication::event(event);
        }
    }
};

int main(int argc, char *argv[])
{
    MyApplication a(argc, argv);
    a.sendPostedEvents(&a, QEvent::FileOpen);
    return 0;
}
