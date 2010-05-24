/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include <qdeclarativeview.h>

#ifdef hz
#undef hz
#endif
#include "ui_recopts.h"

#include "qmlruntime.h"
#include <qdeclarativecontext.h>
#include <qdeclarativeengine.h>
#include <qdeclarativenetworkaccessmanagerfactory.h>
#include "qdeclarative.h"
#include <private/qabstractanimation_p.h>
#include <QAbstractAnimation>

#include <QSettings>
#include <QXmlStreamReader>
#include <QBuffer>
#include <QNetworkReply>
#include <QNetworkCookieJar>
#include <QNetworkDiskCache>
#include <QNetworkAccessManager>
#include <QSignalMapper>
#include <QDeclarativeComponent>
#include <QWidget>
#include <QApplication>
#include <QTranslator>
#include <QDir>
#include <QTextBrowser>
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
#include <QGraphicsObject>
#include <QNetworkProxyFactory>
#include <QKeyEvent>
#include <QMutex>
#include <QMutexLocker>
#include "proxysettings.h"
#include "deviceorientation.h"

#ifdef GL_SUPPORTED
#include <QGLWidget>
#endif

#include <qdeclarativetester.h>

#if defined (Q_OS_SYMBIAN)
#define SYMBIAN_NETWORK_INIT
#endif

#if defined (SYMBIAN_NETWORK_INIT)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <QTextCodec>
#include "sym_iap_util.h"
#endif

QT_BEGIN_NAMESPACE

class Runtime : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isActiveWindow READ isActiveWindow NOTIFY isActiveWindowChanged)
    Q_PROPERTY(DeviceOrientation::Orientation orientation READ orientation NOTIFY orientationChanged)

public:
    static Runtime* instance()
    {
        static Runtime *instance = 0;
        if (!instance)
            instance = new Runtime;
        return instance;
    }

    bool isActiveWindow() const { return activeWindow; }
    void setActiveWindow(bool active)
    {
        if (active == activeWindow)
            return;
        activeWindow = active;
        emit isActiveWindowChanged();
    }

    DeviceOrientation::Orientation orientation() const { return DeviceOrientation::instance()->orientation(); }

Q_SIGNALS:
    void isActiveWindowChanged();
    void orientationChanged();

private:
    Runtime(QObject *parent=0) : QObject(parent), activeWindow(false)
    {
        connect(DeviceOrientation::instance(), SIGNAL(orientationChanged()),
                this, SIGNAL(orientationChanged()));
    }

    bool activeWindow;
};


class SizedMenuBar : public QMenuBar
{
    Q_OBJECT
public:
    SizedMenuBar(QWidget *parent, QWidget *referenceWidget)
        : QMenuBar(parent), refWidget(referenceWidget)
    {
    }

    virtual QSize sizeHint() const
    {
        return QSize(refWidget->sizeHint().width(), QMenuBar::sizeHint().height());
    }

private:
    QWidget *refWidget;
};

static struct { const char *name, *args; } ffmpegprofiles[] = {
    {"Maximum Quality", "-sameq"},
    {"High Quality", "-qmax 2"},
    {"Medium Quality", "-qmax 6"},
    {"Low Quality", "-qmax 16"},
    {"Custom ffmpeg arguments", ""},
    {0,0}
};

class RecordingDialog : public QDialog, public Ui::RecordingOptions {
    Q_OBJECT

public:
    RecordingDialog(QWidget *parent) : QDialog(parent)
    {
        setupUi(this);
        hz->setValidator(new QDoubleValidator(hz));
        for (int i=0; ffmpegprofiles[i].name; ++i) {
            profile->addItem(ffmpegprofiles[i].name);
        }
    }

    void setArguments(QString a)
    {
        int i;
        for (i=0; ffmpegprofiles[i].args[0]; ++i) {
            if (ffmpegprofiles[i].args == a) {
                profile->setCurrentIndex(i);
                args->setText(QLatin1String(ffmpegprofiles[i].args));
                return;
            }
        }
        customargs = a;
        args->setText(a);
        profile->setCurrentIndex(i);
    }

    QString arguments() const
    {
        int i = profile->currentIndex();
        return ffmpegprofiles[i].args[0] ? QLatin1String(ffmpegprofiles[i].args) : customargs;
    }

private slots:
    void pickProfile(int i)
    {
        if (ffmpegprofiles[i].args[0]) {
            args->setText(QLatin1String(ffmpegprofiles[i].args));
        } else {
            args->setText(customargs);
        }
    }

    void storeCustomArgs(QString s)
    {
        setArguments(s);
    }

private:
    QString customargs;
};

class PersistentCookieJar : public QNetworkCookieJar {
public:
    PersistentCookieJar(QObject *parent) : QNetworkCookieJar(parent) { load(); }
    ~PersistentCookieJar() { save(); }

    virtual QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const
    {
        QMutexLocker lock(&mutex);
        return QNetworkCookieJar::cookiesForUrl(url);
    }

    virtual bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
    {
        QMutexLocker lock(&mutex);
        return QNetworkCookieJar::setCookiesFromUrl(cookieList, url);
    }

private:
    void save()
    {
        QMutexLocker lock(&mutex);
        QList<QNetworkCookie> list = allCookies();
        QByteArray data;
        foreach (QNetworkCookie cookie, list) {
            if (!cookie.isSessionCookie()) {
                data.append(cookie.toRawForm());
                data.append("\n");
            }
        }
        QSettings settings;
        settings.setValue("Cookies",data);
    }

    void load()
    {
        QMutexLocker lock(&mutex);
        QSettings settings;
        QByteArray data = settings.value("Cookies").toByteArray();
        setAllCookies(QNetworkCookie::parseCookies(data));
    }

    mutable QMutex mutex;
};

class NetworkAccessManagerFactory : public QDeclarativeNetworkAccessManagerFactory
{
public:
    NetworkAccessManagerFactory() : cacheSize(0) {}
    ~NetworkAccessManagerFactory() {}

    QNetworkAccessManager *create(QObject *parent);

    void setupProxy(QNetworkAccessManager *nam)
    {
        class SystemProxyFactory : public QNetworkProxyFactory
        {
        public:
            virtual QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery &query)
            {
                QString protocolTag = query.protocolTag();
                if (httpProxyInUse && (protocolTag == "http" || protocolTag == "https")) {
                    QList<QNetworkProxy> ret;
                    ret << httpProxy;
                    return ret;
                }
                return QNetworkProxyFactory::systemProxyForQuery(query);
            }
            void setHttpProxy (QNetworkProxy proxy)
            {
                httpProxy = proxy;
                httpProxyInUse = true;
            }
            void unsetHttpProxy ()
            {
                httpProxyInUse = false;
            }
        private:
            bool httpProxyInUse;
            QNetworkProxy httpProxy;
        };

        SystemProxyFactory *proxyFactory = new SystemProxyFactory;
        if (ProxySettings::httpProxyInUse())
            proxyFactory->setHttpProxy(ProxySettings::httpProxy());
        else
            proxyFactory->unsetHttpProxy();
        nam->setProxyFactory(proxyFactory);
    }

    void setCacheSize(int size) {
        if (size != cacheSize) {
            cacheSize = size;
        }
    }

    static PersistentCookieJar *cookieJar;
    QMutex mutex;
    int cacheSize;
};

PersistentCookieJar *NetworkAccessManagerFactory::cookieJar = 0;

static void cleanup_cookieJar()
{
    delete NetworkAccessManagerFactory::cookieJar;
    NetworkAccessManagerFactory::cookieJar = 0;
}

QNetworkAccessManager *NetworkAccessManagerFactory::create(QObject *parent)
{
    QMutexLocker lock(&mutex);
    QNetworkAccessManager *manager = new QNetworkAccessManager(parent);
    if (!cookieJar) {
        qAddPostRoutine(cleanup_cookieJar);
        cookieJar = new PersistentCookieJar(0);
    }
    manager->setCookieJar(cookieJar);
    cookieJar->setParent(0);
    setupProxy(manager);
    if (cacheSize > 0) {
        QNetworkDiskCache *cache = new QNetworkDiskCache;
        cache->setCacheDirectory(QDir::tempPath()+QLatin1String("/qml-viewer-network-cache"));
        cache->setMaximumCacheSize(cacheSize);
        manager->setCache(cache);
    } else {
        manager->setCache(0);
    }
    qDebug() << "created new network access manager for" << parent;
    return manager;
}

QString QDeclarativeViewer::getVideoFileName()
{
    QString title = convertAvailable || ffmpegAvailable ? tr("Save Video File") : tr("Save PNG Frames");
    QStringList types;
    if (ffmpegAvailable) types += tr("Common Video files")+QLatin1String(" (*.avi *.mpeg *.mov)");
    if (convertAvailable) types += tr("GIF Animation")+QLatin1String(" (*.gif)");
    types += tr("Individual PNG frames")+QLatin1String(" (*.png)");
    if (ffmpegAvailable) types += tr("All ffmpeg formats (*.*)");
    return QFileDialog::getSaveFileName(this, title, "", types.join(";; "));
}


QDeclarativeViewer::QDeclarativeViewer(QWidget *parent, Qt::WindowFlags flags)
#if defined(Q_OS_SYMBIAN)
    : QMainWindow(parent, flags)
#else
    : QWidget(parent, flags)
#endif
      , loggerWindow(new LoggerWidget())
      , frame_stream(0), mb(0)
      , portraitOrientation(0), landscapeOrientation(0)
      , showWarningsWindow(0)
      , m_scriptOptions(0)
      , tester(0)
      , useQmlFileBrowser(true)
      , translator(0)
{
    QDeclarativeViewer::registerTypes();
    setWindowTitle(tr("Qt QML Viewer"));

    devicemode = false;
    canvas = 0;
    record_autotime = 0;
    record_rate = 50;
    record_args += QLatin1String("-sameq");

    recdlg = new RecordingDialog(this);
    connect(recdlg->pickfile, SIGNAL(clicked()), this, SLOT(pickRecordingFile()));
    senseFfmpeg();
    senseImageMagick();
    if (!ffmpegAvailable)
        recdlg->ffmpegOptions->hide();
    if (!ffmpegAvailable && !convertAvailable)
        recdlg->rateOptions->hide();
    QString warn;
    if (!ffmpegAvailable) {
        if (!convertAvailable)
            warn = tr("ffmpeg and ImageMagick not available - no video output");
        else
            warn = tr("ffmpeg not available - GIF and PNG outputs only");
        recdlg->warning->setText(warn);
    } else {
        recdlg->warning->hide();
    }

    canvas = new QDeclarativeView(this);

    canvas->setAttribute(Qt::WA_OpaquePaintEvent);
    canvas->setAttribute(Qt::WA_NoSystemBackground);

    canvas->setFocus();

    QObject::connect(canvas, SIGNAL(sceneResized(QSize)), this, SLOT(sceneResized(QSize)));
    QObject::connect(canvas, SIGNAL(statusChanged(QDeclarativeView::Status)), this, SLOT(statusChanged()));
    QObject::connect(canvas->engine(), SIGNAL(quit()), QCoreApplication::instance (), SLOT(quit()));

    QObject::connect(warningsWidget(), SIGNAL(opened()), this, SLOT(warningsWidgetOpened()));
    QObject::connect(warningsWidget(), SIGNAL(closed()), this, SLOT(warningsWidgetClosed()));

    if (!(flags & Qt::FramelessWindowHint)) {
        createMenu(menuBar(),0);
        setPortrait();
    }

#if !defined(Q_OS_SYMBIAN)
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);
    if (mb)
        layout->addWidget(mb);
    layout->addWidget(canvas);
#else
    setCentralWidget(canvas);
#endif
    namFactory = new NetworkAccessManagerFactory;
    canvas->engine()->setNetworkAccessManagerFactory(namFactory);

    connect(&autoStartTimer, SIGNAL(triggered()), this, SLOT(autoStartRecording()));
    connect(&autoStopTimer, SIGNAL(triggered()), this, SLOT(autoStopRecording()));
    connect(&recordTimer, SIGNAL(triggered()), this, SLOT(recordFrame()));
    connect(DeviceOrientation::instance(), SIGNAL(orientationChanged()),
            this, SLOT(orientationChanged()), Qt::QueuedConnection);
    autoStartTimer.setRunning(false);
    autoStopTimer.setRunning(false);
    recordTimer.setRunning(false);
    recordTimer.setRepeating(true);
}

QDeclarativeViewer::~QDeclarativeViewer()
{
    delete loggerWindow;
    canvas->engine()->setNetworkAccessManagerFactory(0);
    delete namFactory;
}

void QDeclarativeViewer::enableExperimentalGestures()
{
    canvas->viewport()->grabGesture(Qt::TapGesture,Qt::DontStartGestureOnChildren|Qt::ReceivePartialGestures|Qt::IgnoredGesturesPropagateToParent);
    canvas->viewport()->grabGesture(Qt::TapAndHoldGesture,Qt::DontStartGestureOnChildren|Qt::ReceivePartialGestures|Qt::IgnoredGesturesPropagateToParent);
    canvas->viewport()->grabGesture(Qt::PanGesture,Qt::DontStartGestureOnChildren|Qt::ReceivePartialGestures|Qt::IgnoredGesturesPropagateToParent);
    canvas->viewport()->grabGesture(Qt::PinchGesture,Qt::DontStartGestureOnChildren|Qt::ReceivePartialGestures|Qt::IgnoredGesturesPropagateToParent);
    canvas->viewport()->grabGesture(Qt::SwipeGesture,Qt::DontStartGestureOnChildren|Qt::ReceivePartialGestures|Qt::IgnoredGesturesPropagateToParent);
    canvas->viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
}

int QDeclarativeViewer::menuBarHeight() const
{
    if (!(windowFlags() & Qt::FramelessWindowHint))
        return menuBar()->height();
    else
        return 0; // don't create menu
}

QMenuBar *QDeclarativeViewer::menuBar() const
{
#if !defined(Q_OS_SYMBIAN)
    if (!mb)
        mb = new SizedMenuBar((QWidget*)this, canvas);
#else
    mb = QMainWindow::menuBar();
#endif

    return mb;
}

QDeclarativeView *QDeclarativeViewer::view() const
{
    return canvas;
}

LoggerWidget *QDeclarativeViewer::warningsWidget() const
{
    return loggerWindow;
}

void QDeclarativeViewer::createMenu(QMenuBar *menu, QMenu *flatmenu)
{
    QObject *parent = flatmenu ? (QObject*)flatmenu : (QObject*)menu;

    QMenu *fileMenu = flatmenu ? flatmenu : menu->addMenu(tr("&File"));

    QAction *openAction = new QAction(tr("&Open..."), parent);
    openAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));
    fileMenu->addAction(openAction);

    QAction *reloadAction = new QAction(tr("&Reload"), parent);
    reloadAction->setShortcut(QKeySequence("Ctrl+R"));
    connect(reloadAction, SIGNAL(triggered()), this, SLOT(reload()));
    fileMenu->addAction(reloadAction);

#if defined(Q_OS_SYMBIAN)
    QAction *networkAction = new QAction(tr("Start &Network"), parent);
    connect(networkAction, SIGNAL(triggered()), this, SLOT(startNetwork()));
    fileMenu->addAction(networkAction);
#endif

#if !defined(Q_OS_SYMBIAN)
    if (flatmenu) flatmenu->addSeparator();

    QMenu *recordMenu = flatmenu ? flatmenu : menu->addMenu(tr("&Recording"));

    QAction *snapshotAction = new QAction(tr("&Take Snapshot\tF3"), parent);
    connect(snapshotAction, SIGNAL(triggered()), this, SLOT(takeSnapShot()));
    recordMenu->addAction(snapshotAction);

    recordAction = new QAction(tr("Start Recording &Video\tF9"), parent);
    connect(recordAction, SIGNAL(triggered()), this, SLOT(toggleRecordingWithSelection()));
    recordMenu->addAction(recordAction);

    QAction *recordOptions = new QAction(tr("Video &Options..."), parent);
    connect(recordOptions, SIGNAL(triggered()), this, SLOT(chooseRecordingOptions()));

    if (flatmenu)
        flatmenu->addAction(recordOptions);

    if (flatmenu) flatmenu->addSeparator();

    QMenu *debugMenu = flatmenu ? flatmenu->addMenu(tr("&Debugging")) : menu->addMenu(tr("&Debugging"));

    QAction *slowAction = new QAction(tr("&Slow Down Animations"), parent);
    slowAction->setShortcut(QKeySequence("Ctrl+."));
    slowAction->setCheckable(true);
    connect(slowAction, SIGNAL(triggered(bool)), this, SLOT(setSlowMode(bool)));
    debugMenu->addAction(slowAction);

    showWarningsWindow = new QAction(tr("Show Warnings"), parent);
    showWarningsWindow->setCheckable((true));
    showWarningsWindow->setChecked(loggerWindow->isVisible());
    connect(showWarningsWindow, SIGNAL(triggered(bool)), this, SLOT(showWarnings(bool)));

#if !defined(Q_OS_SYMBIAN)
    debugMenu->addAction(showWarningsWindow);
#endif

    if (flatmenu) flatmenu->addSeparator();

#endif // Q_OS_SYMBIAN

    QMenu *settingsMenu = flatmenu ? flatmenu : menu->addMenu(tr("S&ettings"));
    QAction *proxyAction = new QAction(tr("Http &proxy..."), parent);
    connect(proxyAction, SIGNAL(triggered()), this, SLOT(showProxySettings()));
    settingsMenu->addAction(proxyAction);
#if !defined(Q_OS_SYMBIAN)
    if (!flatmenu)
        settingsMenu->addAction(recordOptions);

    settingsMenu->addMenu(loggerWindow->preferencesMenu());
#else
    QAction *fullscreenAction = new QAction(tr("Full Screen"), parent);
    fullscreenAction->setCheckable(true);
    connect(fullscreenAction, SIGNAL(triggered()), this, SLOT(toggleFullScreen()));
    settingsMenu->addAction(fullscreenAction);
#endif

    if (flatmenu) flatmenu->addSeparator();

    QMenu *propertiesMenu = settingsMenu->addMenu(tr("Properties"));

    QActionGroup *orientation = new QActionGroup(parent);

    QAction *toggleOrientation = new QAction(tr("&Toggle Orientation"), parent);
    toggleOrientation->setCheckable(true);
    toggleOrientation->setShortcut(QKeySequence("Ctrl+T"));
    settingsMenu->addAction(toggleOrientation);
    connect(toggleOrientation, SIGNAL(triggered()), this, SLOT(toggleOrientation()));

    orientation->setExclusive(true);
    portraitOrientation = new QAction(tr("orientation: Portrait"), parent);
    portraitOrientation->setCheckable(true);
    connect(portraitOrientation, SIGNAL(triggered()), this, SLOT(setPortrait()));
    orientation->addAction(portraitOrientation);
    propertiesMenu->addAction(portraitOrientation);

    landscapeOrientation = new QAction(tr("orientation: Landscape"), parent);
    landscapeOrientation->setCheckable(true);
    connect(landscapeOrientation, SIGNAL(triggered()), this, SLOT(setLandscape()));
    orientation->addAction(landscapeOrientation);
    propertiesMenu->addAction(landscapeOrientation);

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

void QDeclarativeViewer::showProxySettings()
{
    ProxySettings settingsDlg (this);

    connect (&settingsDlg, SIGNAL (accepted()), this, SLOT (proxySettingsChanged ()));

    settingsDlg.exec();
}

void QDeclarativeViewer::proxySettingsChanged()
{
    reload ();
}

void QDeclarativeViewer::setPortrait()
{
    DeviceOrientation::instance()->setOrientation(DeviceOrientation::Portrait);
    portraitOrientation->setChecked(true);
}

void QDeclarativeViewer::setLandscape()
{
    DeviceOrientation::instance()->setOrientation(DeviceOrientation::Landscape);
    landscapeOrientation->setChecked(true);
}

void QDeclarativeViewer::toggleOrientation()
{
    DeviceOrientation::instance()->setOrientation(DeviceOrientation::instance()->orientation()==DeviceOrientation::Portrait?DeviceOrientation::Landscape:DeviceOrientation::Portrait);
}

void QDeclarativeViewer::toggleFullScreen()
{
    if (isFullScreen())
        showMaximized();
    else
        showFullScreen();
}

void QDeclarativeViewer::showWarnings(bool show)
{
    loggerWindow->setVisible(show);
}

void QDeclarativeViewer::warningsWidgetOpened()
{
    showWarningsWindow->setChecked(true);
}

void QDeclarativeViewer::warningsWidgetClosed()
{
    showWarningsWindow->setChecked(false);
}

void QDeclarativeViewer::takeSnapShot()
{
    static int snapshotcount = 1;
    QString snapFileName = QString(QLatin1String("snapshot%1.png")).arg(snapshotcount);
    QPixmap::grabWidget(canvas).save(snapFileName);
    qDebug() << "Wrote" << snapFileName;
    ++snapshotcount;
}

void QDeclarativeViewer::pickRecordingFile()
{
    QString fileName = getVideoFileName();
    if (!fileName.isEmpty())
        recdlg->file->setText(fileName);
}

void QDeclarativeViewer::chooseRecordingOptions()
{
    // File
    recdlg->file->setText(record_file);

    // Size
    recdlg->sizeOriginal->setText(tr("Original (%1x%2)").arg(canvas->width()).arg(canvas->height()));
    if (recdlg->sizeWidth->value()<=1) {
        recdlg->sizeWidth->setValue(canvas->width());
        recdlg->sizeHeight->setValue(canvas->height());
    }

    // Rate
    if (record_rate == 24)
        recdlg->hz24->setChecked(true);
    else if (record_rate == 25)
        recdlg->hz25->setChecked(true);
    else if (record_rate == 50)
        recdlg->hz50->setChecked(true);
    else if (record_rate == 60)
        recdlg->hz60->setChecked(true);
    else {
        recdlg->hzCustom->setChecked(true);
        recdlg->hz->setText(QString::number(record_rate));
    }

    // Profile
    recdlg->setArguments(record_args.join(" "));
    if (recdlg->exec()) {
        // File
        record_file = recdlg->file->text();
        // Size
        if (recdlg->sizeOriginal->isChecked())
            record_outsize = QSize();
        else if (recdlg->size720p->isChecked())
            record_outsize = QSize(1280,720);
        else if (recdlg->sizeVGA->isChecked())
            record_outsize = QSize(640,480);
        else if (recdlg->sizeQVGA->isChecked())
            record_outsize = QSize(320,240);
        else
            record_outsize = QSize(recdlg->sizeWidth->value(),recdlg->sizeHeight->value());
        // Rate
        if (recdlg->hz24->isChecked())
            record_rate = 24;
        else if (recdlg->hz25->isChecked())
            record_rate = 25;
        else if (recdlg->hz50->isChecked())
            record_rate = 50;
        else if (recdlg->hz60->isChecked())
            record_rate = 60;
        else {
            record_rate = recdlg->hz->text().toDouble();
        }
        // Profile
        record_args = recdlg->arguments().split(" ",QString::SkipEmptyParts);
    }
}

void QDeclarativeViewer::toggleRecordingWithSelection()
{
    if (!recordTimer.isRunning()) {
        if (record_file.isEmpty()) {
            QString fileName = getVideoFileName();
            if (fileName.isEmpty())
                return;
            if (!fileName.contains(QRegExp(".[^\\/]*$")))
                fileName += ".avi";
            setRecordFile(fileName);
        }
    }
    toggleRecording();
}

void QDeclarativeViewer::toggleRecording()
{
    if (record_file.isEmpty()) {
        toggleRecordingWithSelection();
        return;
    }
    bool recording = !recordTimer.isRunning();
    recordAction->setText(recording ? tr("&Stop Recording Video\tF9") : tr("&Start Recording Video\tF9"));
    setRecording(recording);
}

void QDeclarativeViewer::setSlowMode(bool enable)
{
    QUnifiedTimer::instance()->setSlowModeEnabled(enable);
}

void QDeclarativeViewer::addLibraryPath(const QString& lib)
{
    canvas->engine()->addImportPath(lib);
}

void QDeclarativeViewer::addPluginPath(const QString& plugin)
{
    canvas->engine()->addPluginPath(plugin);
}

void QDeclarativeViewer::reload()
{
    open(currentFileOrUrl);
}

void QDeclarativeViewer::openFile()
{
    QString cur = canvas->source().toLocalFile();
    if (useQmlFileBrowser) {
        open("qrc:/content/Browser.qml");
    } else {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open QML file"), cur, tr("QML Files (*.qml)"));
        if (!fileName.isEmpty()) {
            QFileInfo fi(fileName);
            open(fi.absoluteFilePath());
        }
    }
}

void QDeclarativeViewer::statusChanged()
{
    if (canvas->status() == QDeclarativeView::Error && tester)
        tester->executefailure();

    if (canvas->status() == QDeclarativeView::Ready) {
        initialSize = canvas->sizeHint();
        if (canvas->resizeMode() == QDeclarativeView::SizeRootObjectToView) {
            updateSizeHints();
            resize(QSize(initialSize.width(), initialSize.height()+menuBarHeight()));
        }
    }
}

void QDeclarativeViewer::launch(const QString& file_or_url)
{
    QMetaObject::invokeMethod(this, "open", Qt::QueuedConnection, Q_ARG(QString, file_or_url));
}

void QDeclarativeViewer::loadTranslationFile(const QString& directory)
{
    if (!translator) {
        translator = new QTranslator(this);
        QApplication::installTranslator(translator);
    }

    translator->load(QLatin1String("qml_" )+QLocale::system().name(), directory + QLatin1String("/i18n"));
}

void QDeclarativeViewer::loadDummyDataFiles(const QString& directory)
{
    QDir dir(directory+"/dummydata", "*.qml");
    QStringList list = dir.entryList();
    for (int i = 0; i < list.size(); ++i) {
        QString qml = list.at(i);
        QFile f(dir.filePath(qml));
        f.open(QIODevice::ReadOnly);
        QByteArray data = f.readAll();
        QDeclarativeComponent comp(canvas->engine());
        comp.setData(data, QUrl());
        QObject *dummyData = comp.create();

        if(comp.isError()) {
            QList<QDeclarativeError> errors = comp.errors();
            foreach (const QDeclarativeError &error, errors) {
                qWarning() << error;
            }
            if (tester) tester->executefailure();
        }

        if (dummyData) {
            qWarning() << "Loaded dummy data:" << dir.filePath(qml);
            qml.truncate(qml.length()-4);
            canvas->rootContext()->setContextProperty(qml, dummyData);
            dummyData->setParent(this);
        }
    }
}

bool QDeclarativeViewer::open(const QString& file_or_url)
{
    currentFileOrUrl = file_or_url;

    QUrl url;
    QFileInfo fi(file_or_url);
    if (fi.exists())
        url = QUrl::fromLocalFile(fi.absoluteFilePath());
    else
        url = QUrl(file_or_url);
    setWindowTitle(tr("%1 - Qt QML Viewer").arg(file_or_url));

    if (!m_script.isEmpty())
        tester = new QDeclarativeTester(m_script, m_scriptOptions, canvas);

    delete canvas->rootObject();
    canvas->engine()->clearComponentCache();
    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("qmlViewer", this);
#ifdef Q_OS_SYMBIAN
    ctxt->setContextProperty("qmlViewerFolder", "E:\\"); // Documents on your S60 phone
#else
    ctxt->setContextProperty("qmlViewerFolder", QDir::currentPath());
#endif

    ctxt->setContextProperty("runtime", Runtime::instance());

    QString fileName = url.toLocalFile();
    if (!fileName.isEmpty()) {
        fi.setFile(fileName);
        if (fi.exists()) {
            if (fi.suffix().toLower() != QLatin1String("qml")) {
                qWarning() << "qml cannot open non-QML file" << fileName;
                return false;
            }

            QFileInfo fi(fileName);
            loadTranslationFile(fi.path());
            loadDummyDataFiles(fi.path());
        } else {
            qWarning() << "qml cannot find file:" << fileName;
            return false;
        }
    }

    QTime t;
    t.start();

    canvas->setSource(url);

    return true;
}

void QDeclarativeViewer::startNetwork()
{
#if defined(SYMBIAN_NETWORK_INIT)
    qt_SetDefaultIap();
#endif
}

void QDeclarativeViewer::setAutoRecord(int from, int to)
{
    if (from==0) from=1; // ensure resized
    record_autotime = to-from;
    autoStartTimer.setInterval(from);
    autoStartTimer.setRunning(true);
}

void QDeclarativeViewer::setRecordArgs(const QStringList& a)
{
    record_args = a;
}

void QDeclarativeViewer::setRecordFile(const QString& f)
{
    record_file = f;
}

void QDeclarativeViewer::setRecordRate(int fps)
{
    record_rate = fps;
}

void QDeclarativeViewer::sceneResized(QSize size)
{
    if (size.width() > 0 && size.height() > 0) {
        if (canvas->resizeMode() == QDeclarativeView::SizeViewToRootObject) {
            updateSizeHints();
        }
     }
}

void QDeclarativeViewer::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_0 && devicemode)
        exit(0);
    else if (event->key() == Qt::Key_F1 || (event->key() == Qt::Key_1 && devicemode)) {
        qDebug() << "F1 - help\n"
                 << "F2 - save test script\n"
                 << "F3 - take PNG snapshot\n"
                 << "F4 - show items and state\n"
                 << "F5 - reload QML\n"
                 << "F6 - show object tree\n"
                 << "F7 - show timing\n"
                 << "F9 - toggle video recording\n"
                 << "F10 - toggle orientation\n"
                 << "device keys: 0=quit, 1..8=F1..F8"
                ;
    } else if (event->key() == Qt::Key_F2 || (event->key() == Qt::Key_2 && devicemode)) {
        if (tester && m_scriptOptions & Record)
            tester->save();
    } else if (event->key() == Qt::Key_F3 || (event->key() == Qt::Key_3 && devicemode)) {
        takeSnapShot();
    } else if (event->key() == Qt::Key_F5 || (event->key() == Qt::Key_5 && devicemode)) {
        reload();
    } else if (event->key() == Qt::Key_F9 || (event->key() == Qt::Key_9 && devicemode)) {
        toggleRecording();
    } else if (event->key() == Qt::Key_F10) {
        if (portraitOrientation) {
            if (portraitOrientation->isChecked())
                setLandscape();
            else
                setPortrait();
        }
    }

    QWidget::keyPressEvent(event);
}

bool QDeclarativeViewer::event(QEvent *event)
{
    if (event->type() == QEvent::WindowActivate) {
        Runtime::instance()->setActiveWindow(true);
    } else if (event->type() == QEvent::WindowDeactivate) {
        Runtime::instance()->setActiveWindow(false);
    }
    return QWidget::event(event);
}

void QDeclarativeViewer::senseImageMagick()
{
    QProcess proc;
    proc.start("convert", QStringList() << "-h");
    proc.waitForFinished(2000);
    QString help = proc.readAllStandardOutput();
    convertAvailable = help.contains("ImageMagick");
}

void QDeclarativeViewer::senseFfmpeg()
{
    QProcess proc;
    proc.start("ffmpeg", QStringList() << "-h");
    proc.waitForFinished(2000);
    QString ffmpegHelp = proc.readAllStandardOutput();
    ffmpegAvailable = ffmpegHelp.contains("-s ");
    ffmpegHelp = tr("Video recording uses ffmpeg:")+"\n\n"+ffmpegHelp;

    QDialog *d = new QDialog(recdlg);
    QVBoxLayout *l = new QVBoxLayout(d);
    QTextBrowser *b = new QTextBrowser(d);
    QFont f = b->font();
    f.setFamily("courier");
    b->setFont(f);
    b->setText(ffmpegHelp);
    l->addWidget(b);
    d->setLayout(l);
    ffmpegHelpWindow = d;
    connect(recdlg->ffmpegHelp,SIGNAL(clicked()), ffmpegHelpWindow, SLOT(show()));
}

void QDeclarativeViewer::setRecording(bool on)
{
    if (on == recordTimer.isRunning())
        return;

    int period = int(1000/record_rate+0.5);
    QUnifiedTimer::instance()->setTimingInterval(on ? period:16);
    QUnifiedTimer::instance()->setConsistentTiming(on);
    if (on) {
        canvas->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        recordTimer.setInterval(period);
        recordTimer.setRunning(true);
        frame_fmt = record_file.right(4).toLower();
        frame = QImage(canvas->width(),canvas->height(),QImage::Format_RGB32);
        if (frame_fmt != ".png" && (!convertAvailable || frame_fmt != ".gif")) {
            // Stream video to ffmpeg

            QProcess *proc = new QProcess(this);
            connect(proc, SIGNAL(finished(int)), this, SLOT(ffmpegFinished(int)));
            frame_stream = proc;

            QStringList args;
            args << "-y";
            args << "-r" << QString::number(record_rate);
            args << "-f" << "rawvideo";
            args << "-pix_fmt" << (frame_fmt == ".gif" ? "rgb24" : "rgb32");
            args << "-s" << QString("%1x%2").arg(canvas->width()).arg(canvas->height());
            args << "-i" << "-";
            if (record_outsize.isValid()) {
                args << "-s" << QString("%1x%2").arg(record_outsize.width()).arg(record_outsize.height());
                args << "-aspect" << QString::number(double(canvas->width())/canvas->height());
            }
            args += record_args;
            args << record_file;
            proc->start("ffmpeg",args);

        } else {
            // Store frames, save to GIF/PNG
            frame_stream = 0;
        }
    } else {
        canvas->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
        recordTimer.setRunning(false);
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
                if (record_outsize.isValid())
                    *img = img->scaled(record_outsize,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
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
                    args << "-delay" << QString::number(period/10);
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
    qDebug() << "Recording: " << (recordTimer.isRunning()?"ON":"OFF");
}

void QDeclarativeViewer::ffmpegFinished(int code)
{
    qDebug() << "ffmpeg returned" << code << frame_stream->readAllStandardError();
}

void QDeclarativeViewer::autoStartRecording()
{
    setRecording(true);
    autoStopTimer.setInterval(record_autotime);
    autoStopTimer.setRunning(true);
}

void QDeclarativeViewer::autoStopRecording()
{
    setRecording(false);
}

void QDeclarativeViewer::recordFrame()
{
    canvas->QWidget::render(&frame);
    if (frame_stream) {
        if (frame_fmt == ".gif") {
            // ffmpeg can't do 32bpp with gif
            QImage rgb24 = frame.convertToFormat(QImage::Format_RGB888);
            frame_stream->write((char*)rgb24.bits(),rgb24.numBytes());
        } else {
            frame_stream->write((char*)frame.bits(),frame.numBytes());
        }
    } else {
        frames.append(new QImage(frame));
    }
}

void QDeclarativeViewer::orientationChanged()
{
    if (canvas->resizeMode() == QDeclarativeView::SizeRootObjectToView) {
        if (canvas->rootObject()) {
            QSizeF rootObjectSize = canvas->rootObject()->boundingRect().size();
            QSize newSize(rootObjectSize.width(), rootObjectSize.height()+menuBarHeight());
            if (size() != newSize) {
                resize(newSize);
            }
        }
    }
}

void QDeclarativeViewer::setDeviceKeys(bool on)
{
    devicemode = on;
}

void QDeclarativeViewer::setNetworkCacheSize(int size)
{
    namFactory->setCacheSize(size);
}

void QDeclarativeViewer::setUseGL(bool useGL)
{
#ifdef GL_SUPPORTED
    if (useGL) {
        QGLFormat format = QGLFormat::defaultFormat();
#ifdef Q_WS_MAC
        format.setSampleBuffers(true);
#else
        format.setSampleBuffers(false);
#endif

        QGLWidget *glWidget = new QGLWidget(format);
        //### potentially faster, but causes junk to appear if top-level is Item, not Rectangle
        //glWidget->setAutoFillBackground(false);

        canvas->setViewport(glWidget);
    }
#endif
}

void QDeclarativeViewer::setUseNativeFileBrowser(bool use)
{
    useQmlFileBrowser = !use;
}

void QDeclarativeViewer::setSizeToView(bool sizeToView)
{
    QDeclarativeView::ResizeMode resizeMode = sizeToView ? QDeclarativeView::SizeRootObjectToView : QDeclarativeView::SizeViewToRootObject;
    if (resizeMode != canvas->resizeMode()) {
        canvas->setResizeMode(resizeMode);
        updateSizeHints();
    }
}

void QDeclarativeViewer::updateSizeHints()
{
    if (canvas->resizeMode() == QDeclarativeView::SizeViewToRootObject) {
        QSize newWindowSize = canvas->sizeHint();
        newWindowSize.setHeight(newWindowSize.height()+menuBarHeight());
        if (!isFullScreen() && !isMaximized()) {
            resize(newWindowSize);
            setFixedSize(newWindowSize);
        }
    } else { // QDeclarativeView::SizeRootObjectToView
        canvas->setMinimumSize(QSize(0,0));
        canvas->setMaximumSize(QSize(16777215,16777215));
        setMinimumSize(QSize(0,0));
        setMaximumSize(QSize(16777215,16777215));
    }
    updateGeometry();
}

void QDeclarativeViewer::registerTypes()
{
    static bool registered = false;

    if (!registered) {
        // registering only for exposing the DeviceOrientation::Orientation enum
        qmlRegisterUncreatableType<DeviceOrientation>("Qt",4,7,"Orientation","");
        registered = true;
    }
}

QT_END_NAMESPACE

#include "qmlruntime.moc"
