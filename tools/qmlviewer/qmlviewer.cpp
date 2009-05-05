/****************************************************************************
**
** Copyright (C) 1992-$THISYEAR$ $TROLLTECH$. All rights reserved.
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <qfxview.h>

#include "qmlbindablevalue.h"
#include "qmlviewer.h"
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlengine.h>
#include "qml.h"
#include "qperformancelog.h"
#include "qfxtestengine.h"
#include "deviceskin.h"

#include <QNetworkDiskCache>
#include <QNetworkAccessManager>
#include <QmlComponent>
#include <QWidget>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QProcess>
#include <QMenu>

QmlViewer::QmlViewer(QFxTestEngine::TestMode testMode, const QString &testDir, QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags), frame_stream(0)
{
    testEngine = 0;
    devicemode = false;
    skin = 0;
    canvas = 0;
    record_autotime = 0;
    record_period = 20;

    int width=240;
    int height=320;
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    canvas = new QFxView(this);
    if(testMode != QFxTestEngine::NoTest)
        testEngine = new QFxTestEngine(testMode, testDir, canvas, this);

    QObject::connect(canvas, SIGNAL(sceneResized(QSize)), this, SLOT(sceneResized(QSize)));
    canvas->setFixedSize(width, height);
    resize(width, height);
}

void QmlViewer::reload()
{
    openQml(currentFileName);
}

void QmlViewer::openQml(const QString& fileName)
{
    setWindowTitle(tr("%1 - Qt Declarative UI Viewer").arg(fileName));

    canvas->reset();

    currentFileName = fileName;
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
            if (dummyData) {
                qWarning() << "Loaded dummy data:" << dir.filePath(qml);
                qml.truncate(qml.length()-4);
                ctxt->setContextProperty(qml, dummyData);
                dummyData->setParent(this);
            }
        }
    }

    canvas->setUrl(url);

    QTime t;
    t.start();
    canvas->execute();
    qWarning() << "Wall startup time:" << t.elapsed();
#ifdef QTOPIA
    show();
#endif
}

class PreviewDeviceSkin : public  DeviceSkin
{
    Q_OBJECT
public:
    explicit PreviewDeviceSkin(const DeviceSkinParameters &parameters, QWidget *parent);

    void setPreview(QWidget *formWidget);
    void setScreenSize(const QSize& size)
    {
        QMatrix fit;
        fit = fit.scale(qreal(size.width())/m_screenSize.width(),
            qreal(size.height())/m_screenSize.height());
        setTransform(fit);
    }

private slots:
    void slotSkinKeyPressEvent(int code, const QString& text, bool autorep);
    void slotSkinKeyReleaseEvent(int code, const QString& text, bool autorep);
    void slotPopupMenu();

private:
    void populateContextMenu(QMenu *menu);
    const QSize m_screenSize;
};


PreviewDeviceSkin::PreviewDeviceSkin(const DeviceSkinParameters &parameters, QWidget *parent) :
    DeviceSkin(parameters, parent),
    m_screenSize(parameters.screenSize())
{
    connect(this, SIGNAL(skinKeyPressEvent(int,QString,bool)),
            this, SLOT(slotSkinKeyPressEvent(int,QString,bool)));
    connect(this, SIGNAL(skinKeyReleaseEvent(int,QString,bool)),
            this, SLOT(slotSkinKeyReleaseEvent(int,QString,bool)));
    connect(this, SIGNAL(popupMenu()), this, SLOT(slotPopupMenu()));
}

void PreviewDeviceSkin::setPreview(QWidget *formWidget)
{
    formWidget->setFixedSize(m_screenSize);
    formWidget->setParent(this, Qt::SubWindow);
    formWidget->setAutoFillBackground(true);
    setView(formWidget);
}

void PreviewDeviceSkin::slotSkinKeyPressEvent(int code, const QString& text, bool autorep)
{
    if (QWidget *focusWidget =  QApplication::focusWidget()) {
        QKeyEvent e(QEvent::KeyPress,code,0,text,autorep);
        QApplication::sendEvent(focusWidget, &e);
    }

}

void PreviewDeviceSkin::slotSkinKeyReleaseEvent(int code, const QString& text, bool autorep)
{
    if (QWidget *focusWidget =  QApplication::focusWidget()) {
        QKeyEvent e(QEvent::KeyRelease,code,0,text,autorep);
        QApplication::sendEvent(focusWidget, &e);
    }
}

void PreviewDeviceSkin::slotPopupMenu()
{
    QMenu menu(this);
    populateContextMenu(&menu);
    menu.exec(QCursor::pos());
}

void PreviewDeviceSkin::populateContextMenu(QMenu *menu)
{
     connect(menu->addAction(tr("&Close")), SIGNAL(triggered()), parentWidget(), SLOT(close()));
}


void QmlViewer::setSkin(const QString& skinDirectory)
{
    DeviceSkinParameters parameters;
    QString err;
    if (parameters.read(skinDirectory,DeviceSkinParameters::ReadAll,&err)) {
        delete skin;
        if (!err.isEmpty())
            qWarning() << err;
        skin = new PreviewDeviceSkin(parameters,this);
        skin->setScreenSize(canvas->size());
        canvas->setParent(skin, Qt::SubWindow);
        canvas->setAutoFillBackground(true);
        skin->setView(canvas);
        delete layout();

        canvas->show();
    }
}

void QmlViewer::setAutoRecord(int from, int to)
{
    record_autotime = to-from;
    if (from) {
        autoStartTimer.start(from,this);
    } else {
        autoTimer.start();
        setRecording(true);
    }
}

void QmlViewer::setRecordFile(const QString& f)
{
    record_file = f;
}

void QmlViewer::setRecordPeriod(int ms)
{
    record_period = ms;
}

void QmlViewer::sceneResized(QSize size)
{
    if (size.width() > 0 && size.height() > 0) {
        canvas->setFixedSize(size.width(), size.height());
        if (skin)
            skin->setScreenSize(size);
        else
            resize(size);
    }
}

void QmlViewer::resizeEvent(QResizeEvent *)
{
    if (!skin)
        canvas->setFixedSize(width(),height());
}

void QmlViewer::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_0 && devicemode)
        exit(0);
    else if (event->key() == Qt::Key_F1 || (event->key() == Qt::Key_1 && devicemode)) {
        qDebug() << "F1 - help\n"
                 << "F2 - toggle video recording\n"
                 << "F3 - take PNG snapshot\n"
                 << "F4 - show items and state\n"
                 << "F5 - reload QML\n"
                 << "F6 - show object tree\n"
                 << "F7 - show timing\n"
                 << "F8 - show performance (if available)\n"
                 << "device keys: 0=quit, 1..8=F1..F8"
                ;
    } else if (event->key() == Qt::Key_F2 || (event->key() == Qt::Key_2 && devicemode)) {
        setRecording(!recordTimer.isActive());
    } else if (event->key() == Qt::Key_F3 || (event->key() == Qt::Key_3 && devicemode)) {
        canvas->asImage().save("snapshot.png");
        qDebug() << "Wrote snapshot.png";
    } else if (event->key() == Qt::Key_F4 || (event->key() == Qt::Key_4 && devicemode)) {
        canvas->dumpItems();
        canvas->checkState();
    } else if (event->key() == Qt::Key_F5 || (event->key() == Qt::Key_5 && devicemode)) {
        reload();
    } else if (event->key() == Qt::Key_F6 || (event->key() == Qt::Key_6 && devicemode)) {
        canvas->dumpRoot();
    } else if (event->key() == Qt::Key_F7 || (event->key() == Qt::Key_7 && devicemode)) {
        canvas->dumpTiming();
    } else if (event->key() == Qt::Key_F8 || (event->key() == Qt::Key_8 && devicemode)) {
        QPerformanceLog::displayData();
        QPerformanceLog::clear();
    } else if (event->key() == Qt::Key_F9) {
        if(testEngine) testEngine->save();
    } else if (event->key() == Qt::Key_F10) {
        if(testEngine) testEngine->captureFullFrame();
    }

    QWidget::keyPressEvent(event);
}

void QmlViewer::setRecording(bool on)
{
    if (on == recordTimer.isActive())
        return;

    if (on) {
        recordTimer.start(record_period,this);
        QString fmt = record_file.right(4).toLower();
        if (fmt != ".png" && fmt != ".gif") {
            // Stream video to ffmpeg

            QProcess *proc = new QProcess(this);
            frame_stream = proc;

            QStringList args;
            args << "-sameq"; // ie. high
            args << "-y";
            args << "-r" << QString::number(1000/record_period);
            args << "-f" << "rawvideo";
            args << "-pix_fmt" << "rgb32";
            args << "-s" << QString("%1x%2").arg(canvas->width()).arg(canvas->height());
            args << "-i" << "-";
            args << record_file;
            proc->start("ffmpeg",args,QIODevice::WriteOnly);
        } else {
            // Store frames, save to GIF/PNG
            frame_stream = 0;
        }
    } else {
        recordTimer.stop();
        if (frame_stream) {
            qDebug() << "Saving video...";
            frame_stream->close();
            qDebug() << "Wrote" << record_file;
        } else {
            int frame=0;
            QStringList inputs;
            qDebug() << "Saving frames...";

            QString framename;
            bool png_output = false;
            if (record_file.right(4).toLower()==".png") {
                if (record_file.contains('%'))
                    framename = record_file;
                else
                    framename = record_file.left(record_file.length()-4)+"%04d"+record_file.right(4);
                png_output = true;
            } else {
                framename = "tmp-frame%04d.png";
                png_output = false;
            }
            foreach (QImage* img, frames) {
                QString name;
                name.sprintf(framename.toLocal8Bit(),frame++);
                if (record_dither=="ordered")
                    img->convertToFormat(QImage::Format_Indexed8,Qt::PreferDither|Qt::OrderedDither).save(name);
                else if (record_dither=="threshold")
                    img->convertToFormat(QImage::Format_Indexed8,Qt::PreferDither|Qt::ThresholdDither).save(name);
                else if (record_dither=="floyd")
                    img->convertToFormat(QImage::Format_Indexed8,Qt::PreferDither).save(name);
                else
                    img->save(name);
                inputs << name;
                delete img;
            }

            if (png_output) {
                framename.replace(QRegExp("%\\d*."),"*");
                qDebug() << "Wrote frames" << framename;
                inputs.clear(); // don't remove them
            } else {
                // ImageMagick and gifsicle for GIF encoding
                QStringList args;
                args << "-delay" << QString::number(record_period/10);
                args << inputs;
                args << record_file;
                qDebug() << "Converting..." << record_file;
                if (0!=QProcess::execute("convert", args)) {
                    qWarning() << "Cannot run ImageMagick 'convert' - recorded frames not converted";
                    inputs.clear(); // don't remove them
                    qDebug() << "Wrote frames tmp-frame*.png";
                } else {
                    if (record_file.right(4).toLower() == ".gif") {
                        qDebug() << "Compressing..." << record_file;
                        if (0!=QProcess::execute("gifsicle", QStringList() << "-O2" << "-o" << record_file << record_file))
                            qWarning() << "Cannot run 'gifsicle' - not compressed";
                    }
                    qDebug() << "Wrote" << record_file;
                }
            }

            foreach (QString name, inputs)
                QFile::remove(name);

            frames.clear();
        }
    }
    qDebug() << "Recording: " << (recordTimer.isActive()?"ON":"OFF");
}

void QmlViewer::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == recordTimer.timerId()) {
        if (frame_stream) {
            QImage frame(canvas->asImage());
            frame_stream->write((char*)frame.bits(),frame.numBytes());
        } else {
            frames.append(new QImage(canvas->asImage()));
        }
        if (record_autotime && autoTimer.elapsed() >= record_autotime)
            setRecording(false);
    } else if (event->timerId() == autoStartTimer.timerId()) {
        autoTimer.start();
        autoStartTimer.stop();
        setRecording(true);
    } else {
        QWidget::timerEvent(event);
    }
}

void QmlViewer::setDeviceKeys(bool on)
{
    devicemode = on;
}

void QmlViewer::setCacheEnabled(bool on)
{
    QNetworkAccessManager * nam = canvas->engine()->networkAccessManager();
    if (on == !!nam->cache())
        return;
    if (on) {
        // Setup a caching network manager
        QNetworkDiskCache *cache = new QNetworkDiskCache;
        cache->setCacheDirectory(QDir::tempPath()+QLatin1String("/qml-duiviewer-network-cache"));
        cache->setMaximumCacheSize(8000000);
        nam->setCache(cache);
    } else {
        nam->setCache(0);
    }
}

#include "qmlviewer.moc"
