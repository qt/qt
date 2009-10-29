/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
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
#include <QmlEngine>
#include <qmlview.h>
#include <QDebug>
#include <QNetworkDiskCache>
#include <QNetworkAccessManager>
#include <QtCore>

#if defined (Q_OS_SYMBIAN)
#define SYMBIAN_NETWORK_INIT
#endif

#if defined (SYMBIAN_NETWORK_INIT)
#include "sym_iap_util.h"
#endif

QmlView *canvas = 0;

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
        closeAction->setSoftKeyRole(QAction::NegativeSoftKey);
        connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
        addAction(closeAction);
#endif
        connect(logText, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
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

signals:
    void textChanged();

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
        QFileInfo fi(url.toLocalFile());
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

        canvas->setUrl(url);
        canvas->execute();
    }

signals:
    void logUpdated();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow() : QMainWindow()
    {
    }

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

    void enableNetwork()
    {
#if defined(SYMBIAN_NETWORK_INIT)
        qt_SetDefaultIap();
#endif
    }
};

class ConfiguredNetworkAccessManager : public QNetworkAccessManager {
public:
    ConfiguredNetworkAccessManager()
    {
        QNetworkDiskCache *cache = new QNetworkDiskCache;
        cache->setCacheDirectory(QDir::tempPath()+QLatin1String("/qml-loader-network-cache"));
        setCache(cache);
    }

    QNetworkReply *createRequest (Operation op, const QNetworkRequest &req, QIODevice * outgoingData)
    {
        QNetworkRequest request = req;
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
        return QNetworkAccessManager::createRequest(op,request,outgoingData);
    }
};


int main(int argc, char *argv[])
{
    qInstallMsgHandler(myMessageOutput);
    QApplication app(argc, argv);

    MainWindow *mw = new MainWindow;

    QmlLauncher *launcher = new QmlLauncher;

    QObject::connect(Logger::instance(), SIGNAL(textChanged()), launcher, SIGNAL(logUpdated()));

    canvas = new QmlView;
    canvas->setFocusPolicy(Qt::StrongFocus);
    canvas->engine()->setNetworkAccessManager(new ConfiguredNetworkAccessManager);

    QMenuBar *mb = mw->menuBar();
    QAction *showLogAction = new QAction("View Log...", mw);
    mb->addAction(showLogAction);
    QObject::connect(showLogAction, SIGNAL(triggered()), mw, SLOT(showLog()));
    QAction *toggleFSAction = new QAction("Fullscreen", mw);
    mb->addAction(toggleFSAction);
    QObject::connect(toggleFSAction, SIGNAL(triggered()), mw, SLOT(toggleFullScreen()));
#if defined(SYMBIAN_NETWORK_INIT)
    QAction *enableNetworkAction = new QAction("Enable Network", mw);
    mb->addAction(enableNetworkAction);
    QObject::connect(enableNetworkAction, SIGNAL(triggered()), mw, SLOT(enableNetwork()));
#endif

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
    mw->setCentralWidget(canvas);

    return app.exec();
}

#include "main.moc"
