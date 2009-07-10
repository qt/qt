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
#include <private/qperformancelog_p.h>
#include "deviceskin.h"

#include <QNetworkDiskCache>
#include <QNetworkAccessManager>
#include <QSignalMapper>
#include <QmlComponent>
#include <QWidget>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QProgressDialog>
#include <QProcess>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QTimer>
#include <QNetworkProxyFactory>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE

class PreviewDeviceSkin : public DeviceSkin
{
    Q_OBJECT
public:
    explicit PreviewDeviceSkin(const DeviceSkinParameters &parameters, QWidget *parent);

    void setPreview(QWidget *formWidget);
    void setPreviewAndScale(QWidget *formWidget);

    void setScreenSize(const QSize& size)
    {
        QMatrix fit;
        fit = fit.scale(qreal(size.width())/m_screenSize.width(),
            qreal(size.height())/m_screenSize.height());
        setTransform(fit);
        QApplication::syncX();
    }

    QSize standardScreenSize() const { return m_screenSize; }

    QMenu* menu;

private slots:
    void slotSkinKeyPressEvent(int code, const QString& text, bool autorep);
    void slotSkinKeyReleaseEvent(int code, const QString& text, bool autorep);
    void slotPopupMenu();

private:
    const QSize m_screenSize;
};


PreviewDeviceSkin::PreviewDeviceSkin(const DeviceSkinParameters &parameters, QWidget *parent) :
    DeviceSkin(parameters, parent),
    m_screenSize(parameters.screenSize())
{
    menu = new QMenu(this);
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

void PreviewDeviceSkin::setPreviewAndScale(QWidget *formWidget)
{
    setScreenSize(formWidget->sizeHint());
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
    menu->exec(QCursor::pos());
}


QmlViewer::QmlViewer(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags), frame_stream(0), scaleSkin(true), mb(0)
{
    devicemode = false;
    skin = 0;
    canvas = 0;
    record_autotime = 0;
    record_period = 20;

    if (!(flags & Qt::FramelessWindowHint))
        createMenu(menuBar(),0);

    canvas = new QFxView(this);
    canvas->setAttribute(Qt::WA_OpaquePaintEvent);
    canvas->setAttribute(Qt::WA_NoSystemBackground);
    canvas->setContentResizable(!skin || !scaleSkin);

    QObject::connect(canvas, SIGNAL(sceneResized(QSize)), this, SLOT(sceneResized(QSize)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);
    if (mb)
        layout->addWidget(mb);
    layout->addWidget(canvas);

    setupProxy();
}

QMenuBar *QmlViewer::menuBar() const
{
    if (!mb)
        mb = new QMenuBar((QWidget*)this);

    return mb;
}

void QmlViewer::createMenu(QMenuBar *menu, QMenu *flatmenu)
{
    QObject *parent = flatmenu ? (QObject*)flatmenu : (QObject*)menu;

    QMenu *fileMenu = flatmenu ? flatmenu : menu->addMenu(tr("&File"));

    QAction *openAction = new QAction(tr("&Open..."), parent);
    openAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    fileMenu->addAction(openAction);

    QAction *reloadAction = new QAction(tr("&Reload"), parent);
    reloadAction->setShortcut(QKeySequence("Ctrl+R"));
    connect(reloadAction, SIGNAL(triggered()), this, SLOT(reload()));
    fileMenu->addAction(reloadAction);

    if (flatmenu) flatmenu->addSeparator();

    QMenu *recordMenu = flatmenu ? flatmenu : menu->addMenu(tr("&Recording"));

    QAction *snapshotAction = new QAction(tr("&Take Snapsot\tF3"), parent);
    connect(snapshotAction, SIGNAL(triggered()), this, SLOT(takeSnapShot()));
    recordMenu->addAction(snapshotAction);
    
    recordAction = new QAction(tr("Start Recording &Video\tF2"), parent);
    connect(recordAction, SIGNAL(triggered()), this, SLOT(toggleRecordingWithSelection()));
    recordMenu->addAction(recordAction);

    if (flatmenu) flatmenu->addSeparator();

    QMenu *skinMenu = flatmenu ? flatmenu->addMenu(tr("&Skin")) : menu->addMenu(tr("&Skin"));

    QActionGroup *skinActions;
    QAction *skinAction;

    skinActions = new QActionGroup(parent);
    skinAction = new QAction(tr("Scale skin"), parent);
    skinAction->setCheckable(true);
    skinAction->setChecked(scaleSkin);
    skinActions->addAction(skinAction);
    skinMenu->addAction(skinAction);
    connect(skinAction, SIGNAL(triggered()), this, SLOT(setScaleSkin()));
    skinAction = new QAction(tr("Resize view"), parent);
    skinAction->setCheckable(true);
    skinAction->setChecked(!scaleSkin);
    skinActions->addAction(skinAction);
    skinMenu->addAction(skinAction);
    connect(skinAction, SIGNAL(triggered()), this, SLOT(setScaleView()));
    skinMenu->addSeparator();

    skinActions = new QActionGroup(parent);
    QSignalMapper *mapper = new QSignalMapper(parent);
    skinAction = new QAction(tr("None"), parent);
    skinAction->setCheckable(true);
    if (currentSkin.isEmpty())
        skinAction->setChecked(true);
    skinActions->addAction(skinAction);
    skinMenu->addAction(skinAction);
    mapper->setMapping(skinAction, "");
    connect(skinAction, SIGNAL(triggered()), mapper, SLOT(map()));
    skinMenu->addSeparator();

    QDir dir(":/skins/","*.skin");
    const QFileInfoList l = dir.entryInfoList();
    for (QFileInfoList::const_iterator it = l.begin(); it != l.end(); ++it) {
        QString name = (*it).baseName(); // should perhaps be in file
        QString file = (*it).filePath();
        skinAction = new QAction(name, parent);
        skinActions->addAction(skinAction);
        skinMenu->addAction(skinAction);
        skinAction->setCheckable(true);
        if (file == currentSkin)
            skinAction->setChecked(true);
        mapper->setMapping(skinAction, file);
        connect(skinAction, SIGNAL(triggered()), mapper, SLOT(map()));
    }
    //connect(skinActions, SIGNAL(triggered(QAction*)), mapper, SLOT(map(QObject*))); // "incompatible"
    connect(mapper, SIGNAL(mapped(QString)), this, SLOT(setSkin(QString)));

    if (flatmenu) flatmenu->addSeparator();

    QMenu *helpMenu = flatmenu ? flatmenu : menu->addMenu(tr("&Help"));
    QAction *aboutAction = new QAction(tr("&About Qt..."), parent);
    connect(aboutAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    helpMenu->addAction(aboutAction);

    QAction *quitAction = new QAction(tr("&Quit"), parent);
    quitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);
    if (menu) {
        menu->setFixedHeight(menu->sizeHint().height());
        menu->setMinimumWidth(10);
    }
}

void QmlViewer::setScaleSkin()
{
    if (scaleSkin)
        return;
    scaleSkin = true;
    canvas->setContentResizable(!skin || !scaleSkin);
    if (skin) {
        canvas->setFixedSize(canvas->sizeHint());
        skin->setScreenSize(canvas->sizeHint());
    }
}

void QmlViewer::setScaleView()
{
    if (!scaleSkin)
        return;
    scaleSkin = false;
    if (skin) {
        canvas->setContentResizable(!skin || !scaleSkin);
        canvas->setMinimumSize(QSize(0,0));
        canvas->setMaximumSize(QSize(16777215,16777215));
        canvas->resize(skin->standardScreenSize());
        skin->setScreenSize(skin->standardScreenSize());
    }
}


void QmlViewer::takeSnapShot()
{
    static int snapshotcount = 1;
    QString snapFileName = QString(QLatin1String("snapshot%1.png")).arg(snapshotcount);
    // ### GV
    // canvas->asImage().save(snapFileName);
    qDebug() << "Wrote" << snapFileName;
    ++snapshotcount;
}

void QmlViewer::toggleRecordingWithSelection()
{
    if (!recordTimer.isActive()) {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Video File"), "", tr("Common Video files (*.avi *.mpeg *.mov);; GIF Animation (*.gif);; Individual PNG frames (*.png);; All ffmpeg formats (*.*)"));
        if (fileName.isEmpty())
            return;
        if (!fileName.contains(QRegExp(".[^\\/]*$")))
            fileName += ".avi";
        setRecordFile(fileName);
    }
    toggleRecording();
}

void QmlViewer::toggleRecording()
{
    if (record_file.isEmpty()) {
        toggleRecordingWithSelection();
        return;
    }
    bool recording = !recordTimer.isActive();
    recordAction->setText(recording ? tr("&Stop Recording Video\tF2") : tr("&Start Recording Video\tF2"));
    setRecording(recording);
}

void QmlViewer::addLibraryPath(const QString& lib)
{
    canvas->engine()->addNameSpacePath("",lib);
}

void QmlViewer::reload()
{
    openQml(currentFileName);
}

void QmlViewer::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open QML file"), "", tr("QML Files (*.qml)"));
    if (!fileName.isEmpty()) {
        openQml(fileName);
        QTimer::singleShot(0, this, SLOT(reload()));
    }
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

            if(comp.isError()) {
                QList<QmlError> errors = comp.errors();
                foreach (const QmlError &error, errors) {
                    qWarning() << error;
                }
            }

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

    if (!skin) {
        canvas->resize(canvas->sizeHint());
        resize(sizeHint());
    } else {
        if (scaleSkin)
            canvas->resize(canvas->sizeHint());
        else {
            canvas->setFixedSize(skin->standardScreenSize());
            canvas->resize(skin->standardScreenSize());
        }
    }

#ifdef QTOPIA
    show();
#endif
}


void QmlViewer::setSkin(const QString& skinDirectory)
{
    // XXX QWidget::setMask does not handle changes well, and we may
    // XXX have been signalled from an item in a menu we're replacing,
    // XXX hence some rather convoluted resetting here...

    if (currentSkin == skinDirectory)
        return;

    currentSkin = skinDirectory;

    QString err;
    if (skin) {
        skin->hide();
        skin->deleteLater();
    }

    canvas->setContentResizable(!skin || !scaleSkin);

    DeviceSkinParameters parameters;
    if (!skinDirectory.isEmpty() && parameters.read(skinDirectory,DeviceSkinParameters::ReadAll,&err)) {
        layout()->setEnabled(false);
        //setMenuBar(0);
        if (mb)
            mb->hide();
        if (!err.isEmpty())
            qWarning() << err;
        skin = new PreviewDeviceSkin(parameters,this);
        canvas->resize(canvas->sizeHint());
        if (scaleSkin)
            skin->setPreviewAndScale(canvas);
        else
            skin->setPreview(canvas);
        createMenu(0,skin->menu);
        skin->show();
    } else {
        skin = 0;
        clearMask();
        menuBar()->clear();
        canvas->setParent(this, Qt::SubWindow);
        createMenu(menuBar(),0);
        mb->show();
        setMinimumSize(QSize(0,0));
        setMaximumSize(QSize(16777215,16777215));
        canvas->setMinimumSize(QSize(0,0));
        canvas->setMaximumSize(QSize(16777215,16777215));
        QRect g = geometry();
        g.setSize(sizeHint());
        setParent(0,windowFlags()); // recreate
        canvas->move(0,menuBar()->sizeHint().height());
        setGeometry(g);
        layout()->setEnabled(true);
        show();
    }
    canvas->show();
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

void QmlViewer::setRecordArgs(const QStringList& a)
{
    record_args = a;
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
        if (skin && scaleSkin)
            skin->setScreenSize(size);
    }
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
        toggleRecording();
    } else if (event->key() == Qt::Key_F3 || (event->key() == Qt::Key_3 && devicemode)) {
        takeSnapShot();
    } else if (event->key() == Qt::Key_F5 || (event->key() == Qt::Key_5 && devicemode)) {
        reload();
    } else if (event->key() == Qt::Key_F8 || (event->key() == Qt::Key_8 && devicemode)) {
        QPerformanceLog::displayData();
        QPerformanceLog::clear();
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
            connect(proc, SIGNAL(finished(int)), this, SLOT(ffmpegFinished(int)));
            frame_stream = proc;

            QStringList args;
            args << "-sameq"; // ie. high
            args << "-y";
            args << "-r" << QString::number(1000/record_period);
            args << "-f" << "rawvideo";
            args << "-pix_fmt" << "rgb32";
            args << "-s" << QString("%1x%2").arg(canvas->width()).arg(canvas->height());
            args << "-i" << "-";
            args += record_args;
            args << record_file;
            proc->start("ffmpeg",args);

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
            QProgressDialog progress(tr("Saving frames..."), tr("Cancel"), 0, frames.count()+10, this);
            progress.setWindowModality(Qt::WindowModal);

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
                progress.setValue(progress.value()+1);
                if (progress.wasCanceled())
                    break;
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

            if (!progress.wasCanceled()) {
                if (png_output) {
                    framename.replace(QRegExp("%\\d*."),"*");
                    qDebug() << "Wrote frames" << framename;
                    inputs.clear(); // don't remove them
                } else {
                    // ImageMagick and gifsicle for GIF encoding
                    progress.setLabelText(tr("Converting frames to GIF file..."));
                    QStringList args;
                    args << "-delay" << QString::number(record_period/10);
                    args << inputs;
                    args << record_file;
                    qDebug() << "Converting..." << record_file << "(this may take a while)";
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
            }

            progress.setValue(progress.maximum()-1);
            foreach (QString name, inputs)
                QFile::remove(name);

            frames.clear();
        }
    }
    qDebug() << "Recording: " << (recordTimer.isActive()?"ON":"OFF");
}

void QmlViewer::ffmpegFinished(int code)
{
    qDebug() << "ffmpeg returned" << code << frame_stream->readAllStandardError();
}

void QmlViewer::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == recordTimer.timerId()) {
        if (frame_stream) {
            // ### GV
            // QImage frame(canvas->asImage());
            // frame_stream->write((char*)frame.bits(),frame.numBytes());
        } else {
            // ### GV
            // frames.append(new QImage(canvas->asImage()));
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

void QmlViewer::setupProxy()
{
    class SystemProxyFactory : public QNetworkProxyFactory
    {
    public:
        virtual QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery &query)
        {
            return QNetworkProxyFactory::systemProxyForQuery(query);
        }
    };

    QNetworkAccessManager * nam = canvas->engine()->networkAccessManager();
    nam->setProxyFactory(new SystemProxyFactory);
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

QT_END_NAMESPACE

#include "qmlviewer.moc"
