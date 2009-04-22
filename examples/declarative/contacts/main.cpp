#include "qml.h"
#include <qfxview.h>

#include <QWidget>
#include <QApplication>
#include <QFile>
#include <QTime>
#include <QVBoxLayout>

const char *defaultFileName("contacts.xml");

class Contacts : public QWidget
{
Q_OBJECT
public:
    Contacts(const QString &fileName, int = 240, int = 320, QWidget *parent=0, Qt::WindowFlags flags=0);

public slots:
    void sceneResized(QSize size)
    {
        if(size.width() > 0 && size.height() > 0)
            canvas->setFixedSize(size.width(), size.height());
    }

private:
    QFxView *canvas;
};

Contacts::Contacts(const QString &fileName, int width, int height, QWidget *parent, Qt::WindowFlags flags)
: QWidget(parent, flags), canvas(0)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setMargin(0);
    setLayout(vbox);

    canvas = new QFxView(this);
    QObject::connect(canvas, SIGNAL(sceneResized(QSize)), this, SLOT(sceneResized(QSize)));
    canvas->setFixedSize(width, height);
    vbox->addWidget(canvas);

    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QString xml = file.readAll();
    canvas->setXml(xml, fileName);

    canvas->execute();
}

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    bool frameless = false;

    int width = 240;
    int height = 320;

    QString fileName;
    for (int i = 1; i < argc; ++i) {
        QString arg = argv[i];
        if (arg == "-frameless") {
            frameless = true;
        } else {
            fileName = arg;
            break;
        }
    }
    if (fileName.isEmpty())
        fileName = QLatin1String(defaultFileName);

    Contacts contacts(fileName, width, height, 0, frameless ? Qt::FramelessWindowHint : Qt::Widget);
    contacts.show();

    return app.exec();
}

#include "main.moc"
