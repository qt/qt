#include <QApplication>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QPlainTextEdit>
#include <QAction>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QDebug>
#include <QmlContext>
#include <QmlComponent>
#include <qfxview.h>
#include <QDebug>

QFxView *canvas = 0;

class Logger : public QWidget
{
    Q_OBJECT
public:
    Logger() : QWidget() {
        logText = new QPlainTextEdit;
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(logText);
        layout->setMargin(0);
        setLayout(layout);
#ifdef Q_OS_SYMBIAN
        QAction *closeAction = new QAction("Close", this);
        closeAction->setSoftKeyRole(QAction::BackSoftKey);
        connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));

        QList<QAction*> softKeys;
        softKeys.append(closeAction);
        setSoftKeys(softKeys);
#endif
    }

    void append(const QString &text) {
        logText->appendPlainText(text);
    }

    static Logger *instance() {
        static Logger *logger = 0;
        if (!logger)
            logger = new Logger();
        return logger;
    }

private:
    QPlainTextEdit *logText;
};

void myMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        Logger::instance()->append(QString(msg));
        break;
    case QtWarningMsg:
        Logger::instance()->append(QString(msg));
        break;
    case QtCriticalMsg:
        Logger::instance()->append(QString(msg));
        break;
    case QtFatalMsg:
        Logger::instance()->append(QString(msg));
        abort();
    }
}


class QmlLauncher : public QObject
{
    Q_OBJECT
public:
    QmlLauncher() {}

    Q_INVOKABLE void launch(const QString &fileName)
    {
        QUrl url(fileName);
        QFileInfo fi(fileName);
        if (fi.exists()) {
            url = QUrl::fromLocalFile(fi.absoluteFilePath());
            QmlContext *ctxt = canvas->rootContext();
            QDir dir(fi.path()+"/dummydata", "*.qml");
            QStringList list = dir.entryList();
            for (int i = 0; i < list.size(); ++i) {
                QString qml = list.at(i);
                QFile f(dir.filePath(qml));
                f.open(QIODevice::ReadOnly);
                QByteArray data = f.readAll();
                QmlComponent comp(canvas->engine());
                comp.setData(data, QUrl());
                QObject *dummyData = comp.create();

                if(comp.isError()) {
                    QList<QmlError> errors = comp.errors();
                    foreach (const QmlError &error, errors) {
                        qWarning() << error;
                    }
                }

                if (dummyData) {
                    qml.truncate(qml.length()-4);
                    ctxt->setContextProperty(qml, dummyData);
                    dummyData->setParent(this);
                }
            }
        }

        canvas->setUrl(fileName);
        canvas->execute();
    }

signals:
    void logUpdated();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow() : QMainWindow() {}


public slots:
    void toggleFullScreen()
    {
        if (isFullScreen()) {
#ifdef Q_OS_SYMBIAN
            showMaximized();
#else
            showNormal();
#endif
        } else {
            showFullScreen();
        }
    }

    void showLog()
    {
#ifdef Q_OS_SYMBIAN
        Logger::instance()->showMaximized();
#else
        Logger::instance()->show();
#endif
    }
};


int main(int argc, char *argv[])
{
    qInstallMsgHandler(myMessageOutput);
    QApplication app(argc, argv);

    MainWindow *mw = new MainWindow;

    QmlLauncher *launcher = new QmlLauncher;

    QObject::connect(Logger::instance(), SIGNAL(textChanged()), launcher, SIGNAL(logUpdated()));

    canvas = new QFxView;
    canvas->setFocusPolicy(Qt::StrongFocus);
    mw->setCentralWidget(canvas);

    QMenuBar *mb = mw->menuBar();
    QAction *showLogAction = new QAction("View Log...", mw);
    mb->addAction(showLogAction);
    QObject::connect(showLogAction, SIGNAL(triggered()), mw, SLOT(showLog()));
    QAction *toggleFSAction = new QAction("Fullscreen", mw);
    mb->addAction(toggleFSAction);
    QObject::connect(toggleFSAction, SIGNAL(triggered()), mw, SLOT(toggleFullScreen()));

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("qmlLauncher", launcher);
    canvas->setUrl(QUrl("qrc:/loader.qml"));
#ifdef Q_OS_SYMBIAN
    canvas->setContentResizable(true);
    mw->showMaximized();
#else
    mw->show();
#endif
    canvas->execute();

    return app.exec();
}

#include "main.moc"
