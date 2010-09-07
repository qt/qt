/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "tracer.h"

#include "mainwindow.h"

#include "bookmarkmanager.h"
#include "centralwidget.h"
#include "indexwindow.h"
#include "topicchooser.h"
#include "contentwindow.h"
#include "preferencesdialog.h"
#include "helpenginewrapper.h"
#include "remotecontrol.h"
#include "cmdlineparser.h"
#include "aboutdialog.h"
#include "searchwidget.h"
#include "qtdocinstaller.h"

// #define TRACING_REQUESTED

#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QPair>
#include <QtCore/QResource>
#include <QtCore/QByteArray>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>

#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QToolBar>
#include <QtGui/QStatusBar>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QLayout>
#include <QtGui/QDockWidget>
#include <QtGui/QTreeView>
#include <QtGui/QMessageBox>
#include <QtGui/QFontDatabase>
#include <QtGui/QComboBox>
#include <QtGui/QProgressBar>
#include <QtGui/QDesktopServices>
#include <QtGui/QToolButton>
#include <QtGui/QFileDialog>

#include <QtHelp/QHelpEngineCore>
#include <QtHelp/QHelpSearchEngine>
#include <QtHelp/QHelpContentModel>
#include <QtHelp/QHelpIndexModel>

QT_BEGIN_NAMESPACE

MainWindow::MainWindow(CmdLineParser *cmdLine, QWidget *parent)
    : QMainWindow(parent)
    , m_bookmarkWidget(0)
    , m_filterCombo(0)
    , m_toolBarMenu(0)
    , m_cmdLine(cmdLine)
    , m_progressWidget(0)
    , m_qtDocInstaller(0)
    , m_connectedInitSignals(false)
{
    TRACE_OBJ

    setToolButtonStyle(Qt::ToolButtonFollowStyle);

    QString collectionFile;
    if (usesDefaultCollection()) {
        MainWindow::collectionFileDirectory(true);
        collectionFile = MainWindow::defaultHelpCollectionFileName();
    } else {
        collectionFile = cmdLine->collectionFile();
    }
    HelpEngineWrapper &helpEngineWrapper =
        HelpEngineWrapper::instance(collectionFile);

    m_centralWidget = new CentralWidget(this);
    setCentralWidget(m_centralWidget);

    m_indexWindow = new IndexWindow(this);
    QDockWidget *indexDock = new QDockWidget(tr("Index"), this);
    indexDock->setObjectName(QLatin1String("IndexWindow"));
    indexDock->setWidget(m_indexWindow);
    addDockWidget(Qt::LeftDockWidgetArea, indexDock);

    m_contentWindow = new ContentWindow;
    QDockWidget *contentDock = new QDockWidget(tr("Contents"), this);
    contentDock->setObjectName(QLatin1String("ContentWindow"));
    contentDock->setWidget(m_contentWindow);
    addDockWidget(Qt::LeftDockWidgetArea, contentDock);

    QDockWidget *bookmarkDock = 0;
    if (BookmarkManager *manager = BookmarkManager::instance()) {
        bookmarkDock = new QDockWidget(tr("Bookmarks"), this);
        bookmarkDock->setObjectName(QLatin1String("BookmarkWindow"));
        bookmarkDock->setWidget(m_bookmarkWidget = manager->bookmarkDockWidget());
        addDockWidget(Qt::LeftDockWidgetArea, bookmarkDock);

        connect(manager, SIGNAL(escapePressed()), this,
            SLOT(activateCurrentCentralWidgetTab()));
        connect(manager, SIGNAL(setSource(QUrl)), m_centralWidget,
            SLOT(setSource(QUrl)));
        connect(manager, SIGNAL(setSourceInNewTab(QUrl)), m_centralWidget,
            SLOT(setSourceInNewTab(QUrl)));
        connect(m_centralWidget, SIGNAL(addBookmark(QString, QString)), manager,
            SLOT(addBookmark(QString, QString)));
    }

    QHelpSearchEngine *searchEngine = helpEngineWrapper.searchEngine();
    connect(searchEngine, SIGNAL(indexingStarted()), this, SLOT(indexingStarted()));
    connect(searchEngine, SIGNAL(indexingFinished()), this, SLOT(indexingFinished()));

    m_centralWidget->createSearchWidget(searchEngine);
    m_centralWidget->activateSearchWidget();

    QString defWindowTitle = tr("Qt Assistant");
    setWindowTitle(defWindowTitle);

    setupActions();
    statusBar()->show();

    if (initHelpDB()) {
        setupFilterToolbar();
        setupAddressToolbar();

        const QString windowTitle = helpEngineWrapper.windowTitle();
        setWindowTitle(windowTitle.isEmpty() ? defWindowTitle : windowTitle);
        QByteArray iconArray = helpEngineWrapper.applicationIcon();
        if (iconArray.size() > 0) {
            QPixmap pix;
            pix.loadFromData(iconArray);
            QIcon appIcon(pix);
            qApp->setWindowIcon(appIcon);
        } else {
            QIcon appIcon(QLatin1String(":/trolltech/assistant/images/assistant-128.png"));
            qApp->setWindowIcon(appIcon);
        }

        // Show the widget here, otherwise the restore geometry and state won't work
        // on x11.
        show();
        QByteArray ba(helpEngineWrapper.mainWindow());
        if (!ba.isEmpty())
            restoreState(ba);

        ba = helpEngineWrapper.mainWindowGeometry();
        if (!ba.isEmpty()) {
            restoreGeometry(ba);
        } else {
            tabifyDockWidget(contentDock, indexDock);
            if (bookmarkDock)
                tabifyDockWidget(indexDock, bookmarkDock);
            contentDock->raise();
            resize(QSize(800, 600));
        }

        if (!helpEngineWrapper.hasFontSettings()) {
            helpEngineWrapper.setUseAppFont(false);
            helpEngineWrapper.setUseBrowserFont(false);
            helpEngineWrapper.setAppFont(qApp->font());
            helpEngineWrapper.setAppWritingSystem(QFontDatabase::Latin);
            helpEngineWrapper.setBrowserFont(qApp->font());
            helpEngineWrapper.setBrowserWritingSystem(QFontDatabase::Latin);
        } else {
            updateApplicationFont();
        }

        updateAboutMenuText();

        QTimer::singleShot(0, this, SLOT(insertLastPages()));
        if (m_cmdLine->enableRemoteControl())
            (void)new RemoteControl(this);

        if (m_cmdLine->contents() == CmdLineParser::Show)
            showContents();
        else if (m_cmdLine->contents() == CmdLineParser::Hide)
            hideContents();

        if (m_cmdLine->index() == CmdLineParser::Show)
            showIndex();
        else if (m_cmdLine->index() == CmdLineParser::Hide)
            hideIndex();

        if (m_cmdLine->bookmarks() == CmdLineParser::Show)
            showBookmarksDockWidget();
        else if (m_cmdLine->bookmarks() == CmdLineParser::Hide)
            hideBookmarksDockWidget();

        if (m_cmdLine->search() == CmdLineParser::Show)
            showSearch();
        else if (m_cmdLine->search() == CmdLineParser::Hide)
            hideSearch();

        if (m_cmdLine->contents() == CmdLineParser::Activate)
            showContents();
        else if (m_cmdLine->index() == CmdLineParser::Activate)
            showIndex();
        else if (m_cmdLine->bookmarks() == CmdLineParser::Activate)
            showBookmarksDockWidget();

        if (!m_cmdLine->currentFilter().isEmpty()) {
            const QString &curFilter = m_cmdLine->currentFilter();
            if (helpEngineWrapper.customFilters().contains(curFilter))
                helpEngineWrapper.setCurrentFilter(curFilter);
        }

        if (usesDefaultCollection())
            QTimer::singleShot(0, this, SLOT(lookForNewQtDocumentation()));
        else
            checkInitState();

        connect(&helpEngineWrapper, SIGNAL(documentationRemoved(QString)),
                this, SLOT(documentationRemoved(QString)));
        connect(&helpEngineWrapper, SIGNAL(documentationUpdated(QString)),
                this, SLOT(documentationUpdated(QString)));
    }
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
}

MainWindow::~MainWindow()
{
    TRACE_OBJ
    if (m_qtDocInstaller)
        delete m_qtDocInstaller;
}

bool MainWindow::usesDefaultCollection() const
{
    TRACE_OBJ
    return m_cmdLine->collectionFile().isEmpty();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    TRACE_OBJ
    BookmarkManager::destroy();
    HelpEngineWrapper::instance().setMainWindow(saveState());
    HelpEngineWrapper::instance().setMainWindowGeometry(saveGeometry());
    QMainWindow::closeEvent(e);
}

bool MainWindow::initHelpDB()
{
    TRACE_OBJ
    HelpEngineWrapper &helpEngineWrapper = HelpEngineWrapper::instance();
    if (!helpEngineWrapper.setupData())
        return false;

    bool assistantInternalDocRegistered = false;
    QString intern(QLatin1String("com.trolltech.com.assistantinternal-"));
    foreach (const QString &ns, helpEngineWrapper.registeredDocumentations()) {
        if (ns.startsWith(intern)) {
            intern = ns;
            assistantInternalDocRegistered = true;
            break;
        }
    }

    const QString &collectionFile = helpEngineWrapper.collectionFile();
    QFileInfo fi(collectionFile);
    QString helpFile;
    QTextStream(&helpFile) << fi.absolutePath() << QDir::separator()
        << QLatin1String("assistant.qch.") << (QT_VERSION >> 16)
        << QLatin1Char('.') << ((QT_VERSION >> 8) & 0xFF);

    bool needsSetup = false;
    if (!assistantInternalDocRegistered || !QFile::exists(helpFile)) {
        QFile file(helpFile);
        if (file.open(QIODevice::WriteOnly)) {
            QResource res(QLatin1String(":/trolltech/assistant/assistant.qch"));
            if (file.write((const char*)res.data(), res.size()) != res.size())
                qDebug() << QLatin1String("could not write assistant.qch...");

            file.close();
        }
        helpEngineWrapper.unregisterDocumentation(intern);
        helpEngineWrapper.registerDocumentation(helpFile);
        needsSetup = true;
    }

    if (needsSetup)
        helpEngineWrapper.setupData();
    return true;
}

void MainWindow::lookForNewQtDocumentation()
{
    TRACE_OBJ
    HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();
    QStringList docs;
    docs << QLatin1String("assistant")
        << QLatin1String("designer")
        << QLatin1String("linguist")
        << QLatin1String("qmake")
        << QLatin1String("qt");
    QList<QtDocInstaller::DocInfo> qtDocInfos;
    foreach (const QString &doc, docs)
        qtDocInfos.append(QtDocInstaller::DocInfo(doc, helpEngine.qtDocInfo(doc)));

    m_qtDocInstaller = new QtDocInstaller(qtDocInfos);
    connect(m_qtDocInstaller, SIGNAL(docsInstalled(bool)), this,
        SLOT(qtDocumentationInstalled()));
    connect(m_qtDocInstaller, SIGNAL(qchFileNotFound(QString)), this,
            SLOT(resetQtDocInfo(QString)));
    connect(m_qtDocInstaller, SIGNAL(registerDocumentation(QString, QString)),
            this, SLOT(registerDocumentation(QString, QString)));
    if (helpEngine.qtDocInfo(QLatin1String("qt")).count() != 2)
        statusBar()->showMessage(tr("Looking for Qt Documentation..."));
    m_qtDocInstaller->installDocs();
}

void MainWindow::qtDocumentationInstalled()
{
    TRACE_OBJ
    statusBar()->clearMessage();
    checkInitState();
}

void MainWindow::checkInitState()
{
    TRACE_OBJ
    HelpEngineWrapper::instance().initialDocSetupDone();
    if (!m_cmdLine->enableRemoteControl())
        return;

    HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();
    if (helpEngine.contentModel()->isCreatingContents()
        || helpEngine.indexModel()->isCreatingIndex()) {
        if (!m_connectedInitSignals) {
            connect(helpEngine.contentModel(), SIGNAL(contentsCreated()),
                this, SLOT(checkInitState()));
            connect(helpEngine.indexModel(), SIGNAL(indexCreated()), this,
                SLOT(checkInitState()));
            m_connectedInitSignals = true;
        }
    } else {
        if (m_connectedInitSignals) {
            disconnect(helpEngine.contentModel(), 0, this, 0);
            disconnect(helpEngine.indexModel(), 0, this, 0);
        }
        emit initDone();
    }
}

void MainWindow::insertLastPages()
{
    TRACE_OBJ
    if (m_cmdLine->url().isValid())
        m_centralWidget->setSource(m_cmdLine->url());
    else
        m_centralWidget->setupWidget();

    if (m_cmdLine->search() == CmdLineParser::Activate)
        showSearch();
}

void MainWindow::setupActions()
{
    TRACE_OBJ
    QString resourcePath = QLatin1String(":/trolltech/assistant/images/");
#ifdef Q_OS_MAC
    setUnifiedTitleAndToolBarOnMac(true);
    resourcePath.append(QLatin1String("mac"));
#else
    resourcePath.append(QLatin1String("win"));
#endif

    QMenu *menu = menuBar()->addMenu(tr("&File"));

    m_newTabAction = menu->addAction(tr("New &Tab"), m_centralWidget, SLOT(newTab()));
    m_newTabAction->setShortcut(QKeySequence::AddTab);

    menu->addSeparator();

    m_pageSetupAction = menu->addAction(tr("Page Set&up..."), m_centralWidget,
        SLOT(pageSetup()));
    m_printPreviewAction = menu->addAction(tr("Print Preview..."), m_centralWidget,
        SLOT(printPreview()));

    m_printAction = menu->addAction(tr("&Print..."), m_centralWidget, SLOT(print()));
    m_printAction->setPriority(QAction::LowPriority);
    m_printAction->setIcon(QIcon(resourcePath + QLatin1String("/print.png")));
    m_printAction->setShortcut(QKeySequence::Print);

    menu->addSeparator();

    m_closeTabAction = menu->addAction(tr("&Close Tab"), m_centralWidget,
        SLOT(closeTab()));
    m_closeTabAction->setShortcuts(QKeySequence::Close);

    QAction *tmp = menu->addAction(QIcon::fromTheme("application-exit"),
                                   tr("&Quit"), this, SLOT(close()));
    tmp->setMenuRole(QAction::QuitRole);
#ifdef Q_OS_WIN
    tmp->setShortcut(QKeySequence(tr("CTRL+Q")));
#else
    tmp->setShortcut(QKeySequence::Quit);
#endif

    menu = menuBar()->addMenu(tr("&Edit"));
    m_copyAction = menu->addAction(tr("&Copy selected Text"), m_centralWidget,
        SLOT(copySelection()));
    m_copyAction->setPriority(QAction::LowPriority);
    m_copyAction->setIconText("&Copy");
    m_copyAction->setIcon(QIcon(resourcePath + QLatin1String("/editcopy.png")));
    m_copyAction->setShortcuts(QKeySequence::Copy);
    m_copyAction->setEnabled(false);

    m_findAction = menu->addAction(tr("&Find in Text..."), m_centralWidget,
        SLOT(showTextSearch()));
    m_findAction->setIconText(tr("&Find"));
    m_findAction->setIcon(QIcon(resourcePath + QLatin1String("/find.png")));
    m_findAction->setShortcuts(QKeySequence::Find);

    QAction *findNextAction = menu->addAction(tr("Find &Next"), m_centralWidget,
        SLOT(findNext()));
    findNextAction->setShortcuts(QKeySequence::FindNext);

    QAction *findPreviousAction = menu->addAction(tr("Find &Previous"),
        m_centralWidget, SLOT(findPrevious()));
    findPreviousAction->setShortcuts(QKeySequence::FindPrevious);

    menu->addSeparator();
    tmp = menu->addAction(tr("Preferences..."), this, SLOT(showPreferences()));
    tmp->setMenuRole(QAction::PreferencesRole);

    m_viewMenu = menuBar()->addMenu(tr("&View"));
    m_zoomInAction = m_viewMenu->addAction(tr("Zoom &in"), m_centralWidget,
        SLOT(zoomIn()));
    m_zoomInAction->setPriority(QAction::LowPriority);
    m_zoomInAction->setIcon(QIcon(resourcePath + QLatin1String("/zoomin.png")));
    m_zoomInAction->setShortcut(QKeySequence::ZoomIn);

    m_zoomOutAction = m_viewMenu->addAction(tr("Zoom &out"), m_centralWidget,
        SLOT(zoomOut()));
    m_zoomOutAction->setPriority(QAction::LowPriority);
    m_zoomOutAction->setIcon(QIcon(resourcePath + QLatin1String("/zoomout.png")));
    m_zoomOutAction->setShortcut(QKeySequence::ZoomOut);

    m_resetZoomAction = m_viewMenu->addAction(tr("Normal &Size"), m_centralWidget,
        SLOT(resetZoom()));
    m_resetZoomAction->setPriority(QAction::LowPriority);
    m_resetZoomAction->setIcon(QIcon(resourcePath + QLatin1String("/resetzoom.png")));
    m_resetZoomAction->setShortcut(tr("Ctrl+0"));

    m_viewMenu->addSeparator();

    m_viewMenu->addAction(tr("Contents"), this, SLOT(showContents()),
        QKeySequence(tr("ALT+C")));
    m_viewMenu->addAction(tr("Index"), this, SLOT(showIndex()),
        QKeySequence(tr("ALT+I")));
    m_viewMenu->addAction(tr("Bookmarks"), this, SLOT(showBookmarksDockWidget()),
        QKeySequence(tr("ALT+O")));
    m_viewMenu->addAction(tr("Search"), this, SLOT(showSearchWidget()),
        QKeySequence(tr("ALT+S")));

    menu = menuBar()->addMenu(tr("&Go"));
    m_homeAction = menu->addAction(tr("&Home"), m_centralWidget, SLOT(home()));
    m_homeAction->setShortcut(tr("ALT+Home"));
    m_homeAction->setIcon(QIcon(resourcePath + QLatin1String("/home.png")));

    m_backAction = menu->addAction(tr("&Back"), m_centralWidget, SLOT(backward()));
    m_backAction->setEnabled(false);
    m_backAction->setShortcuts(QKeySequence::Back);
    m_backAction->setIcon(QIcon(resourcePath + QLatin1String("/previous.png")));

    m_nextAction = menu->addAction(tr("&Forward"), m_centralWidget, SLOT(forward()));
    m_nextAction->setPriority(QAction::LowPriority);
    m_nextAction->setEnabled(false);
    m_nextAction->setShortcuts(QKeySequence::Forward);
    m_nextAction->setIcon(QIcon(resourcePath + QLatin1String("/next.png")));

    m_syncAction = menu->addAction(tr("Sync with Table of Contents"), this,
        SLOT(syncContents()));
    m_syncAction->setIconText(tr("Sync"));
    m_syncAction->setIcon(QIcon(resourcePath + QLatin1String("/synctoc.png")));

    menu->addSeparator();

    tmp = menu->addAction(tr("Next Page"), m_centralWidget, SLOT(nextPage()));
    tmp->setShortcuts(QList<QKeySequence>() << QKeySequence(tr("Ctrl+Alt+Right"))
        << QKeySequence(Qt::CTRL + Qt::Key_PageDown));

    tmp = menu->addAction(tr("Previous Page"), m_centralWidget, SLOT(previousPage()));
    tmp->setShortcuts(QList<QKeySequence>() << QKeySequence(tr("Ctrl+Alt+Left"))
        << QKeySequence(Qt::CTRL + Qt::Key_PageUp));

    if (BookmarkManager *manager = BookmarkManager::instance())
        manager->takeBookmarksMenu(menuBar()->addMenu(tr("&Bookmarks")));

    menu = menuBar()->addMenu(tr("&Help"));
    m_aboutAction = menu->addAction(tr("About..."), this, SLOT(showAboutDialog()));
    m_aboutAction->setMenuRole(QAction::AboutRole);

#ifdef Q_WS_X11
    m_newTabAction->setIcon(QIcon::fromTheme("tab-new", m_newTabAction->icon()));
    m_closeTabAction->setIcon(QIcon::fromTheme("window-close", m_closeTabAction->icon()));
    m_backAction->setIcon(QIcon::fromTheme("go-previous" , m_backAction->icon()));
    m_nextAction->setIcon(QIcon::fromTheme("go-next" , m_nextAction->icon()));
    m_zoomInAction->setIcon(QIcon::fromTheme("zoom-in" , m_zoomInAction->icon()));
    m_zoomOutAction->setIcon(QIcon::fromTheme("zoom-out" , m_zoomOutAction->icon()));
    m_resetZoomAction->setIcon(QIcon::fromTheme("zoom-original" , m_resetZoomAction->icon()));
    m_syncAction->setIcon(QIcon::fromTheme("view-refresh" , m_syncAction->icon()));
    m_copyAction->setIcon(QIcon::fromTheme("edit-copy" , m_copyAction->icon()));
    m_findAction->setIcon(QIcon::fromTheme("edit-find" , m_findAction->icon()));
    m_homeAction->setIcon(QIcon::fromTheme("go-home" , m_homeAction->icon()));
    m_pageSetupAction->setIcon(QIcon::fromTheme("document-page-setup", m_pageSetupAction->icon()));
    m_printPreviewAction->setIcon(QIcon::fromTheme("document-print-preview", m_printPreviewAction->icon()));
    m_printAction->setIcon(QIcon::fromTheme("document-print" , m_printAction->icon()));
    m_aboutAction->setIcon(QIcon::fromTheme("help-about", m_aboutAction->icon()));
#endif

    QToolBar *navigationBar = addToolBar(tr("Navigation Toolbar"));
    navigationBar->setObjectName(QLatin1String("NavigationToolBar"));
    navigationBar->addAction(m_backAction);
    navigationBar->addAction(m_nextAction);
    navigationBar->addAction(m_homeAction);
    navigationBar->addAction(m_syncAction);
    QAction *sep = navigationBar->addSeparator();
    navigationBar->addAction(m_copyAction);
    navigationBar->addAction(m_printAction);
    navigationBar->addAction(m_findAction);
    QAction *sep2 = navigationBar->addSeparator();
    navigationBar->addAction(m_zoomInAction);
    navigationBar->addAction(m_zoomOutAction);
    navigationBar->addAction(m_resetZoomAction);

    QList<QAction*> actionList;
    actionList << m_backAction << m_nextAction << m_homeAction << sep
        << m_zoomInAction << m_zoomOutAction << sep2 << m_copyAction
        << m_printAction << m_findAction;
    m_centralWidget->setGlobalActions(actionList);

#if defined(Q_WS_MAC)
    QMenu *windowMenu = new QMenu(tr("&Window"), this);
    menuBar()->insertMenu(menu->menuAction(), windowMenu);
    windowMenu->addAction(tr("Zoom"), this, SLOT(showMaximized()));
    windowMenu->addAction(tr("Minimize"), this, SLOT(showMinimized()),
        QKeySequence(tr("Ctrl+M")));
#endif

    // content viewer connections
    connect(m_centralWidget, SIGNAL(copyAvailable(bool)), this,
        SLOT(copyAvailable(bool)));
    connect(m_centralWidget, SIGNAL(currentViewerChanged()), this,
        SLOT(updateNavigationItems()));
    connect(m_centralWidget, SIGNAL(currentViewerChanged()), this,
        SLOT(updateTabCloseAction()));
    connect(m_centralWidget, SIGNAL(forwardAvailable(bool)), this,
        SLOT(updateNavigationItems()));
    connect(m_centralWidget, SIGNAL(backwardAvailable(bool)), this,
        SLOT(updateNavigationItems()));
    connect(m_centralWidget, SIGNAL(highlighted(QString)), statusBar(),
        SLOT(showMessage(QString)));

    // index window
    connect(m_indexWindow, SIGNAL(linkActivated(QUrl)), m_centralWidget,
        SLOT(setSource(QUrl)));
    connect(m_indexWindow, SIGNAL(linksActivated(QMap<QString,QUrl>,QString)),
        this, SLOT(showTopicChooser(QMap<QString,QUrl>,QString)));
    connect(m_indexWindow, SIGNAL(escapePressed()), this,
        SLOT(activateCurrentCentralWidgetTab()));

    // content window
    connect(m_contentWindow, SIGNAL(linkActivated(QUrl)), m_centralWidget,
        SLOT(setSource(QUrl)));
    connect(m_contentWindow, SIGNAL(escapePressed()), this,
        SLOT(activateCurrentCentralWidgetTab()));

#if defined(QT_NO_PRINTER)
        m_pageSetupAction->setVisible(false);
        m_printPreviewAction->setVisible(false);
        m_printAction->setVisible(false);
#endif
}

QMenu *MainWindow::toolBarMenu()
{
    TRACE_OBJ
    if (!m_toolBarMenu) {
        m_viewMenu->addSeparator();
        m_toolBarMenu = m_viewMenu->addMenu(tr("Toolbars"));
    }
    return m_toolBarMenu;
}

void MainWindow::setupFilterToolbar()
{
    TRACE_OBJ
    HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();
    if (!helpEngine.filterFunctionalityEnabled())
        return;

    m_filterCombo = new QComboBox(this);
    m_filterCombo->setMinimumWidth(QFontMetrics(QFont()).
        width(QLatin1String("MakeTheComboBoxWidthEnough")));

    QToolBar *filterToolBar = addToolBar(tr("Filter Toolbar"));
    filterToolBar->setObjectName(QLatin1String("FilterToolBar"));
    filterToolBar->addWidget(new QLabel(tr("Filtered by:").append(QLatin1Char(' ')),
        this));
    filterToolBar->addWidget(m_filterCombo);

    if (!helpEngine.filterToolbarVisible())
        filterToolBar->hide();
    toolBarMenu()->addAction(filterToolBar->toggleViewAction());

    connect(&helpEngine, SIGNAL(setupFinished()), this,
        SLOT(setupFilterCombo()), Qt::QueuedConnection);
    connect(m_filterCombo, SIGNAL(activated(QString)), this,
        SLOT(filterDocumentation(QString)));
    connect(&helpEngine, SIGNAL(currentFilterChanged(QString)), this,
        SLOT(currentFilterChanged(QString)));

    setupFilterCombo();
}

void MainWindow::setupAddressToolbar()
{
    TRACE_OBJ
    HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();
    if (!helpEngine.addressBarEnabled())
        return;

    m_addressLineEdit = new QLineEdit(this);
    QToolBar *addressToolBar = addToolBar(tr("Address Toolbar"));
    addressToolBar->setObjectName(QLatin1String("AddressToolBar"));
    insertToolBarBreak(addressToolBar);

    addressToolBar->addWidget(new QLabel(tr("Address:").append(QLatin1String(" ")),
        this));
    addressToolBar->addWidget(m_addressLineEdit);

    if (!helpEngine.addressBarVisible())
        addressToolBar->hide();
    toolBarMenu()->addAction(addressToolBar->toggleViewAction());

    // address lineedit
    connect(m_addressLineEdit, SIGNAL(returnPressed()), this,
        SLOT(gotoAddress()));
    connect(m_centralWidget, SIGNAL(currentViewerChanged()), this,
        SLOT(showNewAddress()));
    connect(m_centralWidget, SIGNAL(sourceChanged(QUrl)), this,
        SLOT(showNewAddress(QUrl)));
}

void MainWindow::updateAboutMenuText()
{
    TRACE_OBJ
    QByteArray ba = HelpEngineWrapper::instance().aboutMenuTexts();
    if (ba.size() > 0) {
        QString lang;
        QString str;
        QString trStr;
        QString currentLang = QLocale::system().name();
        int i = currentLang.indexOf(QLatin1Char('_'));
        if (i > -1)
            currentLang = currentLang.left(i);
        QDataStream s(&ba, QIODevice::ReadOnly);
        while (!s.atEnd()) {
            s >> lang;
            s >> str;
            if (lang == QLatin1String("default") && trStr.isEmpty()) {
                trStr = str;
            } else if (lang == currentLang) {
                trStr = str;
                break;
            }
        }
        if (!trStr.isEmpty())
            m_aboutAction->setText(trStr);
    }
}

void MainWindow::showNewAddress()
{
    TRACE_OBJ
    showNewAddress(m_centralWidget->currentSource());
}

void MainWindow::showNewAddress(const QUrl &url)
{
    TRACE_OBJ
    m_addressLineEdit->setText(url.toString());
}

void MainWindow::gotoAddress()
{
    TRACE_OBJ
    m_centralWidget->setSource(m_addressLineEdit->text());
}

void MainWindow::updateNavigationItems()
{
    TRACE_OBJ
    bool hasCurrentViewer = m_centralWidget->isHomeAvailable();
    m_copyAction->setEnabled(m_centralWidget->hasSelection());
    m_homeAction->setEnabled(hasCurrentViewer);
    m_syncAction->setEnabled(hasCurrentViewer);
    m_printPreviewAction->setEnabled(hasCurrentViewer);
    m_printAction->setEnabled(hasCurrentViewer);
    m_nextAction->setEnabled(m_centralWidget->isForwardAvailable());
    m_backAction->setEnabled(m_centralWidget->isBackwardAvailable());
    m_newTabAction->setEnabled(hasCurrentViewer);
}

void MainWindow::updateTabCloseAction()
{
    TRACE_OBJ
    m_closeTabAction->setEnabled(m_centralWidget->enableTabCloseAction());
}

void MainWindow::showTopicChooser(const QMap<QString, QUrl> &links,
                                  const QString &keyword)
{
    TRACE_OBJ
    TopicChooser tc(this, keyword, links);
    if (tc.exec() == QDialog::Accepted) {
        m_centralWidget->setSource(tc.link());
    }
}

void MainWindow::showPreferences()
{
    TRACE_OBJ
    PreferencesDialog dia(this);
    connect(&dia, SIGNAL(updateApplicationFont()), this,
        SLOT(updateApplicationFont()));
    connect(&dia, SIGNAL(updateBrowserFont()), m_centralWidget,
        SLOT(updateBrowserFont()));
    dia.showDialog();
}

void MainWindow::syncContents()
{
    TRACE_OBJ
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    const QUrl url = m_centralWidget->currentSource();
    showContents();
    if (!m_contentWindow->syncToContent(url))
        statusBar()->showMessage(
            tr("Could not find the associated content item."), 3000);
    qApp->restoreOverrideCursor();
}

void MainWindow::copyAvailable(bool yes)
{
    TRACE_OBJ
    m_copyAction->setEnabled(yes);
}

void MainWindow::showAboutDialog()
{
    TRACE_OBJ
    HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();
    QByteArray contents;
    QByteArray ba = helpEngine.aboutTexts();
    if (!ba.isEmpty()) {
        QString lang;
        QByteArray cba;
        QString currentLang = QLocale::system().name();
        int i = currentLang.indexOf(QLatin1Char('_'));
        if (i > -1)
            currentLang = currentLang.left(i);
        QDataStream s(&ba, QIODevice::ReadOnly);
        while (!s.atEnd()) {
            s >> lang;
            s >> cba;
            if (lang == QLatin1String("default") && contents.isEmpty()) {
                contents = cba;
            } else if (lang == currentLang) {
                contents = cba;
                break;
            }
        }
    }

    AboutDialog aboutDia(this);

    QByteArray iconArray;
    if (!contents.isEmpty()) {
        iconArray = helpEngine.aboutIcon();
        QByteArray resources = helpEngine.aboutImages();
        QPixmap pix;
        pix.loadFromData(iconArray);
        aboutDia.setText(QString::fromUtf8(contents), resources);
        if (!pix.isNull())
            aboutDia.setPixmap(pix);
        aboutDia.setWindowTitle(aboutDia.documentTitle());
    } else {
        QByteArray resources;
        aboutDia.setText(QString::fromLatin1("<center>"
            "<h3>%1</h3>"
            "<p>Version %2</p></center>"
            "<p>Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).</p>")
            .arg(tr("Qt Assistant")).arg(QLatin1String(QT_VERSION_STR)),
            resources);
        QLatin1String path(":/trolltech/assistant/images/assistant-128.png");
        aboutDia.setPixmap(QString(path));
    }
    if (aboutDia.windowTitle().isEmpty())
        aboutDia.setWindowTitle(tr("About %1").arg(windowTitle()));
    aboutDia.exec();
}

void MainWindow::setContentsVisible(bool visible)
{
    TRACE_OBJ
    if (visible)
        showContents();
    else
        hideContents();
}

void MainWindow::showContents()
{
    TRACE_OBJ
    activateDockWidget(m_contentWindow);
}

void MainWindow::hideContents()
{
    TRACE_OBJ
    m_contentWindow->parentWidget()->hide();
}

void MainWindow::setIndexVisible(bool visible)
{
    TRACE_OBJ
    if (visible)
        showIndex();
    else
        hideIndex();
}

void MainWindow::showIndex()
{
    TRACE_OBJ
    activateDockWidget(m_indexWindow);
}

void MainWindow::hideIndex()
{
    TRACE_OBJ
    m_indexWindow->parentWidget()->hide();
}

void MainWindow::setBookmarksVisible(bool visible)
{
    TRACE_OBJ
    if (visible)
        showBookmarksDockWidget();
    else
        hideBookmarksDockWidget();
}

void MainWindow::showBookmarksDockWidget()
{
    TRACE_OBJ
    if (m_bookmarkWidget)
        activateDockWidget(m_bookmarkWidget);
}

void MainWindow::hideBookmarksDockWidget()
{
    TRACE_OBJ
    if (m_bookmarkWidget)
        m_bookmarkWidget->parentWidget()->hide();
}

void MainWindow::setSearchVisible(bool visible)
{
    TRACE_OBJ
    if (visible)
        showSearch();
    else
        hideSearch();
}

void MainWindow::showSearch()
{
    TRACE_OBJ
    m_centralWidget->activateSearchWidget();
}

void MainWindow::hideSearch()
{
    TRACE_OBJ
    m_centralWidget->removeSearchWidget();
}

void MainWindow::activateDockWidget(QWidget *w)
{
    TRACE_OBJ
    w->parentWidget()->show();
    w->parentWidget()->raise();
    w->setFocus();
}

void MainWindow::setIndexString(const QString &str)
{
    TRACE_OBJ
    m_indexWindow->setSearchLineEditText(str);
}

void MainWindow::activateCurrentBrowser()
{
    TRACE_OBJ
    CentralWidget *cw = CentralWidget::instance();
    if (cw) {
        cw->activateTab(true);
    }
}

void MainWindow::activateCurrentCentralWidgetTab()
{
    TRACE_OBJ
    m_centralWidget->activateTab();
}

void MainWindow::showSearchWidget()
{
    TRACE_OBJ
    m_centralWidget->activateSearchWidget(true);
}

void MainWindow::updateApplicationFont()
{
    TRACE_OBJ
    HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();
    QFont font = qApp->font();
    if (helpEngine.usesAppFont())
        font = helpEngine.appFont();

    const QWidgetList &widgets = qApp->allWidgets();
    foreach (QWidget* widget, widgets)
        widget->setFont(font);
}

void MainWindow::setupFilterCombo()
{
    TRACE_OBJ
    HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();
    QString curFilter = m_filterCombo->currentText();
    if (curFilter.isEmpty())
        curFilter = helpEngine.currentFilter();
    m_filterCombo->clear();
    m_filterCombo->addItems(helpEngine.customFilters());
    int idx = m_filterCombo->findText(curFilter);
    if (idx < 0)
        idx = 0;
    m_filterCombo->setCurrentIndex(idx);
}

void MainWindow::filterDocumentation(const QString &customFilter)
{
    TRACE_OBJ
    HelpEngineWrapper::instance().setCurrentFilter(customFilter);
}

void MainWindow::expandTOC(int depth)
{
    TRACE_OBJ
    Q_ASSERT(depth >= -1);
    m_contentWindow->expandToDepth(depth);
}

void MainWindow::indexingStarted()
{
    TRACE_OBJ
    if (!m_progressWidget) {
        m_progressWidget = new QWidget();
        QLayout* hlayout = new QHBoxLayout(m_progressWidget);

        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

        QLabel *label = new QLabel(tr("Updating search index"));
        label->setSizePolicy(sizePolicy);
        hlayout->addWidget(label);

        QProgressBar *progressBar = new QProgressBar();
        progressBar->setRange(0, 0);
        progressBar->setTextVisible(false);
        progressBar->setSizePolicy(sizePolicy);

        hlayout->setSpacing(6);
        hlayout->setMargin(0);
        hlayout->addWidget(progressBar);

        statusBar()->addPermanentWidget(m_progressWidget);
    }
}

void MainWindow::indexingFinished()
{
    TRACE_OBJ
    statusBar()->removeWidget(m_progressWidget);
    delete m_progressWidget;
    m_progressWidget = 0;
}

QString MainWindow::collectionFileDirectory(bool createDir, const QString &cacheDir)
{
    TRACE_OBJ
    QString collectionPath =
        QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    if (collectionPath.isEmpty()) {
        if (cacheDir.isEmpty())
            collectionPath = QDir::homePath() + QDir::separator()
                + QLatin1String(".assistant");
        else
            collectionPath = QDir::homePath() + QLatin1String("/.") + cacheDir;
    } else {
        if (cacheDir.isEmpty())
            collectionPath = collectionPath + QLatin1String("/Trolltech/Assistant");
        else
            collectionPath = collectionPath + QDir::separator() + cacheDir;
    }
    collectionPath = QDir::cleanPath(collectionPath);
    if (createDir) {
        QDir dir;
        if (!dir.exists(collectionPath))
            dir.mkpath(collectionPath);
    }
    return collectionPath;
}

QString MainWindow::defaultHelpCollectionFileName()
{
    TRACE_OBJ
    // forces creation of the default collection file path
    return collectionFileDirectory(true) + QDir::separator() +
        QString(QLatin1String("qthelpcollection_%1.qhc")).
        arg(QLatin1String(QT_VERSION_STR));
}

void MainWindow::currentFilterChanged(const QString &filter)
{
    TRACE_OBJ
    const int index = m_filterCombo->findText(filter);
    Q_ASSERT(index != -1);
    m_filterCombo->setCurrentIndex(index);
}

void MainWindow::documentationRemoved(const QString &namespaceName)
{
    TRACE_OBJ
    CentralWidget* widget = CentralWidget::instance();
    widget->closeOrReloadTabs(widget->currentSourceFileList().
        keys(namespaceName), false);
}

void MainWindow::documentationUpdated(const QString &namespaceName)
{
    TRACE_OBJ
    CentralWidget* widget = CentralWidget::instance();
    widget->closeOrReloadTabs(widget->currentSourceFileList().
        keys(namespaceName), true);
}

void MainWindow::resetQtDocInfo(const QString &component)
{
    TRACE_OBJ
    HelpEngineWrapper::instance().setQtDocInfo(component,
        QStringList(QDateTime().toString(Qt::ISODate)));
}

void MainWindow::registerDocumentation(const QString &component,
                                       const QString &absFileName)
{
    TRACE_OBJ
    QString ns = QHelpEngineCore::namespaceName(absFileName);
    if (ns.isEmpty())
        return;

    HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();
    if (helpEngine.registeredDocumentations().contains(ns))
        helpEngine.unregisterDocumentation(ns);
    if (!helpEngine.registerDocumentation(absFileName)) {
        QMessageBox::warning(this, tr("Qt Assistant"),
            tr("Could not register file '%1': %2").
            arg(absFileName).arg(helpEngine.error()));
    } else {
        QStringList docInfo;
        docInfo << QFileInfo(absFileName).lastModified().toString(Qt::ISODate)
                << absFileName;
        helpEngine.setQtDocInfo(component, docInfo);
    }
}

QT_END_NAMESPACE
