/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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
#include "tracer.h"

#include "centralwidget.h"
#include "findwidget.h"
#include "helpenginewrapper.h"
#include "searchwidget.h"
#include "mainwindow.h"
#include "../shared/collectionconfiguration.h"

#if defined(QT_NO_WEBKIT)
#include "helpviewer_qtb.h"
#else
#include "helpviewer_qwv.h"
#endif // QT_NO_WEBKIT

#include <QtCore/QTimer>

#include <QtGui/QApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QLayout>
#include <QtGui/QMenu>
#include <QtGui/QPrinter>
#include <QtGui/QTabBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTextBrowser>
#include <QtGui/QToolButton>
#include <QtGui/QPageSetupDialog>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrintPreviewDialog>

#include <QtHelp/QHelpSearchEngine>

QT_BEGIN_NAMESPACE

namespace {
    HelpViewer* helpViewerFromTabPosition(const QTabWidget *widget,
        const QPoint &point)
    {
        TRACE_OBJ
        QTabBar *tabBar = qFindChild<QTabBar*>(widget);
        for (int i = 0; i < tabBar->count(); ++i) {
            if (tabBar->tabRect(i).contains(point))
                return qobject_cast<HelpViewer*>(widget->widget(i));
        }
        return 0;
    }
    CentralWidget *staticCentralWidget = 0;
}

// -- CentralWidget

CentralWidget::CentralWidget(MainWindow *parent)
    : QWidget(parent)
    , lastTabPage(0)
    , tabWidget(0)
    , findWidget(0)
    , printer(0)
    , usesDefaultCollection(parent->usesDefaultCollection())
    , m_searchWidget(0)
{
    TRACE_OBJ
    globalActionList.clear();
    staticCentralWidget = this;
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    QString resourcePath = QLatin1String(":/trolltech/assistant/images/");

    vboxLayout->setMargin(0);
    tabWidget = new QTabWidget(this);
#ifndef Q_OS_MAC
    resourcePath.append(QLatin1String("win"));
#else
    resourcePath.append(QLatin1String("mac"));
    tabWidget->setDocumentMode(true);
#endif

    connect(tabWidget, SIGNAL(currentChanged(int)), this,
        SLOT(currentPageChanged(int)));

    QToolButton *newTabButton = new QToolButton(this);
    newTabButton->setAutoRaise(true);
    newTabButton->setToolTip(tr("Add new page"));
    newTabButton->setIcon(QIcon(resourcePath + QLatin1String("/addtab.png")));

    tabWidget->setCornerWidget(newTabButton, Qt::TopLeftCorner);
    connect(newTabButton, SIGNAL(clicked()), this, SLOT(newTab()));

    QToolButton *closeTabButton = new QToolButton(this);
    closeTabButton->setEnabled(false);
    closeTabButton->setAutoRaise(true);
    closeTabButton->setToolTip(tr("Close current page"));
    closeTabButton->setIcon(QIcon(resourcePath + QLatin1String("/closetab.png")));

    tabWidget->setCornerWidget(closeTabButton, Qt::TopRightCorner);
    connect(closeTabButton, SIGNAL(clicked()), this, SLOT(closeTab()));

    vboxLayout->addWidget(tabWidget);

    findWidget = new FindWidget(this);
    vboxLayout->addWidget(findWidget);
    findWidget->hide();

    connect(findWidget, SIGNAL(findNext()), this, SLOT(findNext()));
    connect(findWidget, SIGNAL(findPrevious()), this, SLOT(findPrevious()));
    connect(findWidget, SIGNAL(find(QString, bool)), this,
        SLOT(find(QString, bool)));
    connect(findWidget, SIGNAL(escapePressed()), this, SLOT(activateTab()));

    QTabBar *tabBar = qFindChild<QTabBar*>(tabWidget);
    if (tabBar) {
        tabBar->installEventFilter(this);
        tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tabBar, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(showTabBarContextMenu(QPoint)));
    }

#if defined(QT_NO_WEBKIT)
    QPalette p = palette();
    p.setColor(QPalette::Inactive, QPalette::Highlight,
        p.color(QPalette::Active, QPalette::Highlight));
    p.setColor(QPalette::Inactive, QPalette::HighlightedText,
        p.color(QPalette::Active, QPalette::HighlightedText));
    setPalette(p);
#endif
}

CentralWidget::~CentralWidget()
{
    TRACE_OBJ
#ifndef QT_NO_PRINTER
    delete printer;
#endif

    QStringList zoomFactors;
    QStringList currentPages;
    bool searchAttached = m_searchWidget->isAttached();

    int i = searchAttached ? 1 : 0;
    for (; i < tabWidget->count(); ++i) {
        HelpViewer *viewer = qobject_cast<HelpViewer*>(tabWidget->widget(i));
        if (viewer && viewer->source().isValid()) {
            currentPages << viewer->source().toString();
            zoomFactors << QString::number(viewer->scale());
        }
    }

    HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();
    helpEngine.setLastTabPage(tabWidget->currentIndex());
    helpEngine.setLastShownPages(currentPages);
    helpEngine.setSearchWasAttached(searchAttached);
    helpEngine.setLastZoomFactors(zoomFactors);
}

CentralWidget *CentralWidget::instance()
{
    TRACE_OBJ
    return staticCentralWidget;
}

void CentralWidget::newTab()
{
    TRACE_OBJ
    HelpViewer *viewer = currentHelpViewer();
#if !defined(QT_NO_WEBKIT)
    if (viewer && viewer->hasLoadFinished())
#else
    if (viewer)
#endif
        setSourceInNewTab(viewer->source());
}

void CentralWidget::zoomIn()
{
    TRACE_OBJ
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->scaleUp();

    if (tabWidget->currentWidget() == m_searchWidget)
        m_searchWidget->zoomIn();
}

void CentralWidget::zoomOut()
{
    TRACE_OBJ
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->scaleDown();

    if (tabWidget->currentWidget() == m_searchWidget)
        m_searchWidget->zoomOut();
}

void CentralWidget::nextPage()
{
    TRACE_OBJ
    int index = tabWidget->currentIndex() + 1;
    if (index >= tabWidget->count())
        index = 0;
    tabWidget->setCurrentIndex(index);
}

void CentralWidget::resetZoom()
{
    TRACE_OBJ
    if (HelpViewer *viewer = currentHelpViewer())
        viewer->resetScale();

    if (tabWidget->currentWidget() == m_searchWidget)
        m_searchWidget->resetZoom();
}

void CentralWidget::previousPage()
{
    TRACE_OBJ
    int index = tabWidget->currentIndex() -1;
    if (index < 0)
        index = tabWidget->count() -1;
    tabWidget->setCurrentIndex(index);
}

void CentralWidget::closeTab()
{
    TRACE_OBJ
    HelpViewer *viewer = currentHelpViewer();
    if (!viewer|| tabWidget->count() == 1)
        return;

    tabWidget->removeTab(tabWidget->indexOf(viewer));
    QTimer::singleShot(0, viewer, SLOT(deleteLater()));
}

void CentralWidget::setSource(const QUrl &url)
{
    TRACE_OBJ
    HelpViewer *viewer = currentHelpViewer();
    HelpViewer *lastViewer =
        qobject_cast<HelpViewer*>(tabWidget->widget(lastTabPage));

    if (!viewer && !lastViewer) {
        viewer = new HelpViewer(this);
        viewer->installEventFilter(this);
        lastTabPage = tabWidget->addTab(viewer, QString());
        tabWidget->setCurrentIndex(lastTabPage);
        connectSignals();
    } else {
        viewer = lastViewer;
    }

    viewer->setSource(url);
    currentPageChanged(lastTabPage);
    viewer->setFocus(Qt::OtherFocusReason);
    tabWidget->setCurrentIndex(lastTabPage);
    tabWidget->setTabText(lastTabPage, quoteTabTitle(viewer->documentTitle()));
}

void CentralWidget::setupWidget()
{
    TRACE_OBJ
    HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();
    int option = helpEngine.startOption();
    if (option != ShowLastPages) {
        QString homePage;
        if (option == ShowHomePage)
            homePage = helpEngine.homePage();
        else if (option == ShowBlankPage)
            homePage = QLatin1String("about:blank");
        setSource(homePage);
    } else {
        setLastShownPages();
    }
}

void CentralWidget::setLastShownPages()
{
    TRACE_OBJ
    HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();
    const QStringList &lastShownPageList = helpEngine.lastShownPages();
    const int pageCount = lastShownPageList.count();
    if (pageCount == 0) {
        if (usesDefaultCollection)
            setSource(QUrl(QLatin1String("help")));
        else
            setSource(QUrl(QLatin1String("about:blank")));
        return;
    }
    QStringList zoomFactors = helpEngine.lastZoomFactors();
    while (zoomFactors.count() < pageCount)
        zoomFactors.append(CollectionConfiguration::DefaultZoomFactor);

    const bool searchIsAttached = m_searchWidget->isAttached();
    const bool searchWasAttached = helpEngine.searchWasAttached();
    int tabToShow = helpEngine.lastTabPage();
    if (searchWasAttached && !searchIsAttached && tabToShow != 0)
        --tabToShow;
    else if (!searchWasAttached && searchIsAttached)
        ++tabToShow;

    for (int curTab = 0; curTab < pageCount; ++curTab) {
        const QString &curFile = lastShownPageList.at(curTab);
        if (helpEngine.findFile(curFile).isValid()
            || curFile == QLatin1String("about:blank")) {
            setSourceInNewTab(curFile, zoomFactors.at(curTab).toFloat());
        } else if (curTab + searchIsAttached <= tabToShow)
            --tabToShow;
    }

    tabWidget->setCurrentIndex(tabToShow);
}

bool CentralWidget::hasSelection() const
{
    TRACE_OBJ
    const HelpViewer *viewer = currentHelpViewer();
    return viewer ? viewer->hasSelection() : false;
}

QUrl CentralWidget::currentSource() const
{
    TRACE_OBJ
    const HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        return viewer->source();

    return QUrl();
}

QString CentralWidget::currentTitle() const
{
    TRACE_OBJ
    const HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        return viewer->documentTitle();

    return QString();
}

void CentralWidget::copySelection()
{
    TRACE_OBJ
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->copy();
}

void CentralWidget::showTextSearch()
{
    TRACE_OBJ
    findWidget->show();
}

void CentralWidget::initPrinter()
{
    TRACE_OBJ
#ifndef QT_NO_PRINTER
    if (!printer)
        printer = new QPrinter(QPrinter::HighResolution);
#endif
}

void CentralWidget::print()
{
    TRACE_OBJ
#ifndef QT_NO_PRINTER
    HelpViewer *viewer = currentHelpViewer();
    if (!viewer)
        return;

    initPrinter();

    QPrintDialog dlg(printer, this);
#if defined(QT_NO_WEBKIT)
    if (viewer->textCursor().hasSelection())
        dlg.addEnabledOption(QAbstractPrintDialog::PrintSelection);
#endif
    dlg.addEnabledOption(QAbstractPrintDialog::PrintPageRange);
    dlg.addEnabledOption(QAbstractPrintDialog::PrintCollateCopies);
    dlg.setWindowTitle(tr("Print Document"));
    if (dlg.exec() == QDialog::Accepted) {
        viewer->print(printer);
    }
#endif
}

void CentralWidget::printPreview()
{
    TRACE_OBJ
#ifndef QT_NO_PRINTER
    initPrinter();
    QPrintPreviewDialog preview(printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)),
        SLOT(printPreview(QPrinter*)));
    preview.exec();
#endif
}

void CentralWidget::printPreview(QPrinter *p)
{
    TRACE_OBJ
#ifndef QT_NO_PRINTER
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->print(p);
#endif
}

void CentralWidget::pageSetup()
{
    TRACE_OBJ
#ifndef QT_NO_PRINTER
    initPrinter();
    QPageSetupDialog dlg(printer);
    dlg.exec();
#endif
}

bool CentralWidget::isHomeAvailable() const
{
    TRACE_OBJ
    return currentHelpViewer() ? true : false;
}

void CentralWidget::home()
{
    TRACE_OBJ
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->home();
}

bool CentralWidget::isForwardAvailable() const
{
    TRACE_OBJ
    const HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        return viewer->isForwardAvailable();

    return false;
}

void CentralWidget::forward()
{
    TRACE_OBJ
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->forward();
}

bool CentralWidget::isBackwardAvailable() const
{
    TRACE_OBJ
    const HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        return viewer->isBackwardAvailable();

    return false;
}

void CentralWidget::backward()
{
    TRACE_OBJ
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->backward();
}


QList<QAction*> CentralWidget::globalActions() const
{
    TRACE_OBJ
    return globalActionList;
}

void CentralWidget::setGlobalActions(const QList<QAction*> &actions)
{
    TRACE_OBJ
    globalActionList = actions;
}

void CentralWidget::setSourceInNewTab(const QUrl &url, qreal zoom)
{
    TRACE_OBJ
    if (HelpViewer *viewer = currentHelpViewer()) {
        if (viewer->launchWithExternalApp(url))
            return;
    }

    HelpViewer *viewer = new HelpViewer(this, zoom);
    viewer->installEventFilter(this);
    viewer->setSource(url);
    viewer->setFocus(Qt::OtherFocusReason);
    tabWidget->setCurrentIndex(tabWidget->addTab(viewer,
        quoteTabTitle(viewer->documentTitle())));
    connectSignals();
}

HelpViewer *CentralWidget::newEmptyTab()
{
    TRACE_OBJ
    HelpViewer *viewer = new HelpViewer(this);
    viewer->installEventFilter(this);
    viewer->setFocus(Qt::OtherFocusReason);
#if defined(QT_NO_WEBKIT)
    viewer->setDocumentTitle(tr("unknown"));
#endif
    tabWidget->setCurrentIndex(tabWidget->addTab(viewer, tr("unknown")));

    connectSignals();
    return viewer;
}

void CentralWidget::connectSignals()
{
    TRACE_OBJ
    const HelpViewer *viewer = currentHelpViewer();
    if (viewer) {
        connect(viewer, SIGNAL(copyAvailable(bool)), this,
            SIGNAL(copyAvailable(bool)));
        connect(viewer, SIGNAL(forwardAvailable(bool)), this,
            SIGNAL(forwardAvailable(bool)));
        connect(viewer, SIGNAL(backwardAvailable(bool)), this,
            SIGNAL(backwardAvailable(bool)));
        connect(viewer, SIGNAL(sourceChanged(QUrl)), this,
            SIGNAL(sourceChanged(QUrl)));
        connect(viewer, SIGNAL(highlighted(QString)), this,
            SIGNAL(highlighted(QString)));
        connect(viewer, SIGNAL(sourceChanged(QUrl)), this,
            SLOT(setTabTitle(QUrl)));
        connect(viewer, SIGNAL(printRequested()), this, SLOT(print()));
    }
}

HelpViewer* CentralWidget::viewerAt(int index) const
{
    TRACE_OBJ
    return qobject_cast<HelpViewer*>(tabWidget->widget(index));
}

HelpViewer* CentralWidget::currentHelpViewer() const
{
    TRACE_OBJ
    return qobject_cast<HelpViewer*>(tabWidget->currentWidget());
}

void CentralWidget::activateTab(bool onlyHelpViewer)
{
    TRACE_OBJ
    if (currentHelpViewer()) {
        currentHelpViewer()->setFocus();
    } else {
        int idx = 0;
        if (onlyHelpViewer)
            idx = lastTabPage;
        tabWidget->setCurrentIndex(idx);
        tabWidget->currentWidget()->setFocus();
    }
}

void CentralWidget::setTabTitle(const QUrl &url)
{
    TRACE_OBJ
    Q_UNUSED(url)
#if !defined(QT_NO_WEBKIT)
    QTabBar *tabBar = qFindChild<QTabBar*>(tabWidget);
    for (int tab = 0; tab < tabBar->count(); ++tab) {
        HelpViewer *viewer = qobject_cast<HelpViewer*>(tabWidget->widget(tab));
        if (viewer) {
            tabWidget->setTabText(tab,
                quoteTabTitle(viewer->documentTitle().trimmed()));
        }
    }
#else
    HelpViewer *viewer = currentHelpViewer();
    if (viewer) {
        tabWidget->setTabText(lastTabPage,
            quoteTabTitle(viewer->documentTitle().trimmed()));
    }
#endif
}

void CentralWidget::currentPageChanged(int index)
{
    TRACE_OBJ
    const HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        lastTabPage = index;

    QWidget *widget = tabWidget->cornerWidget(Qt::TopRightCorner);
    widget->setEnabled(viewer && enableTabCloseAction());

    widget = tabWidget->cornerWidget(Qt::TopLeftCorner);
    widget->setEnabled(viewer ? true : false);

    emit currentViewerChanged();
}

void CentralWidget::showTabBarContextMenu(const QPoint &point)
{
    TRACE_OBJ
    HelpViewer *viewer = helpViewerFromTabPosition(tabWidget, point);
    if (!viewer)
        return;

    QTabBar *tabBar = qFindChild<QTabBar*>(tabWidget);

    QMenu menu(QLatin1String(""), tabBar);
    QAction *newPage = menu.addAction(tr("Add New Page"));

    bool enableAction = enableTabCloseAction();
    QAction *closePage = menu.addAction(tr("Close This Page"));
    closePage->setEnabled(enableAction);

    QAction *closePages = menu.addAction(tr("Close Other Pages"));
    closePages->setEnabled(enableAction);

    menu.addSeparator();

    QAction *newBookmark = menu.addAction(tr("Add Bookmark for this Page..."));
    const QString &url = viewer->source().toString();
    if (url.isEmpty() || url == QLatin1String("about:blank"))
        newBookmark->setEnabled(false);

    QAction *pickedAction = menu.exec(tabBar->mapToGlobal(point));
    if (pickedAction == newPage)
        setSourceInNewTab(viewer->source());

    if (pickedAction == closePage) {
        tabWidget->removeTab(tabWidget->indexOf(viewer));
        QTimer::singleShot(0, viewer, SLOT(deleteLater()));
    }

    if (pickedAction == closePages) {
        int currentPage = tabWidget->indexOf(viewer);
        for (int i = tabBar->count() -1; i >= 0; --i) {
            viewer = qobject_cast<HelpViewer*>(tabWidget->widget(i));
            if (i != currentPage && viewer) {
                tabWidget->removeTab(i);
                QTimer::singleShot(0, viewer, SLOT(deleteLater()));

                if (i < currentPage)
                    --currentPage;
            }
        }
    }

    if (pickedAction == newBookmark)
        emit addBookmark(viewer->documentTitle(), viewer->source().toString());
}

bool CentralWidget::eventFilter(QObject *object, QEvent *e)
{
    TRACE_OBJ
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        switch (ke->key()) {
            default: {
                return QWidget::eventFilter(object, e);
            }   break;

            case Qt::Key_Backspace: {
                HelpViewer *viewer = currentHelpViewer();
                if (viewer == object) {
#if defined(QT_NO_WEBKIT)
                    if (viewer->isBackwardAvailable()) {
#else
                    if (viewer->isBackwardAvailable() && !viewer->hasFocus()) {
#endif
                        viewer->backward();
                        return true;
                    }
                }
            }   break;
        }
    }

    if (qobject_cast<QTabBar*>(object)) {
        const bool dblClick = e->type() == QEvent::MouseButtonDblClick;
        if ((e->type() == QEvent::MouseButtonRelease) || dblClick) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
            HelpViewer *viewer = helpViewerFromTabPosition(tabWidget,
                mouseEvent->pos());
            if (viewer) {
                if ((mouseEvent->button() == Qt::MidButton) || dblClick) {
                    if (availableHelpViewer() > 1) {
                        tabWidget->removeTab(tabWidget->indexOf(viewer));
                        QTimer::singleShot(0, viewer, SLOT(deleteLater()));
                        currentPageChanged(tabWidget->currentIndex());
                        return true;
                    }
                }
            }
        }
    }

    return QWidget::eventFilter(object, e);
}

void CentralWidget::keyPressEvent(QKeyEvent *e)
{
    TRACE_OBJ
    const QString &text = e->text();
    if (text.startsWith(QLatin1Char('/'))) {
        if (!findWidget->isVisible()) {
            findWidget->showAndClear();
        } else {
            findWidget->show();
        }
    } else {
        QWidget::keyPressEvent(e);
    }
}

void CentralWidget::findNext()
{
    TRACE_OBJ
    find(findWidget->text(), true);
}

void CentralWidget::findPrevious()
{
    TRACE_OBJ
    find(findWidget->text(), false);
}

void CentralWidget::find(const QString &ttf, bool forward)
{
    TRACE_OBJ
    bool found = false;
#if defined(QT_NO_WEBKIT)
    found = findInTextBrowser(ttf, forward);
#else
    found = findInWebPage(ttf, forward);
#endif

    if (!found && ttf.isEmpty())
        found = true;   // the line edit is empty, no need to mark it red...

    if (!findWidget->isVisible())
        findWidget->show();
    findWidget->setPalette(found);
}

bool CentralWidget::findInWebPage(const QString &ttf, bool forward)
{
    TRACE_OBJ
#if !defined(QT_NO_WEBKIT)
    if (HelpViewer *viewer = currentHelpViewer()) {
        bool found = false;
        QWebPage::FindFlags options;
        if (!ttf.isEmpty()) {
            if (!forward)
                options |= QWebPage::FindBackward;

            if (findWidget->caseSensitive())
                options |= QWebPage::FindCaseSensitively;

            found = viewer->findText(ttf, options);
            findWidget->setTextWrappedVisible(false);

            if (!found) {
                options |= QWebPage::FindWrapsAroundDocument;
                found = viewer->findText(ttf, options);
                if (found)
                    findWidget->setTextWrappedVisible(true);
            }
        }
        // force highlighting of all other matches, also when empty (clear)
        options = QWebPage::HighlightAllOccurrences;
        if (findWidget->caseSensitive())
            options |= QWebPage::FindCaseSensitively;
        viewer->findText(QLatin1String(""), options);
        viewer->findText(ttf, options);
        return found;
    }

    // this needs to stay, case for active search results page
    return findInTextBrowser(ttf, forward);
#else
    Q_UNUSED(ttf);
    Q_UNUSED(forward);
#endif
    return false;
}

bool CentralWidget::findInTextBrowser(const QString &ttf, bool forward)
{
    TRACE_OBJ
    QTextBrowser *browser = qobject_cast<QTextBrowser*>(currentHelpViewer());
    if (tabWidget->currentWidget() == m_searchWidget)
        browser = qFindChild<QTextBrowser*>(m_searchWidget);

    if (!browser || ttf.isEmpty())
        return false;

    QTextDocument *doc = browser->document();
    QTextCursor cursor = browser->textCursor();

    if (!doc || cursor.isNull())
        return false;

    QTextDocument::FindFlags options;

    if (cursor.hasSelection()) {
        cursor.setPosition(forward ? cursor.position() : cursor.anchor(),
            QTextCursor::MoveAnchor);
    }

    if (!forward)
        options |= QTextDocument::FindBackward;

    if (findWidget->caseSensitive())
        options |= QTextDocument::FindCaseSensitively;

    findWidget->setTextWrappedVisible(false);

    bool found = true;
    QTextCursor newCursor = doc->find(ttf, cursor, options);
    if (newCursor.isNull()) {
        QTextCursor ac(doc);
        ac.movePosition(options & QTextDocument::FindBackward
            ? QTextCursor::End : QTextCursor::Start);
        newCursor = doc->find(ttf, ac, options);
        if (newCursor.isNull()) {
            found = false;
            newCursor = cursor;
        } else {
            findWidget->setTextWrappedVisible(true);
        }
    }
    browser->setTextCursor(newCursor);
    return found;
}

void CentralWidget::updateBrowserFont()
{
    TRACE_OBJ
    const bool searchAttached = searchWidgetAttached();
    if (searchAttached) {
        HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();
        m_searchWidget->setFont(helpEngine.usesBrowserFont()
            ? helpEngine.browserFont() : qApp->font());
    }

    const int count = tabWidget->count();
    if (HelpViewer* viewer = viewerAt(count - 1)) {
        const QFont &font = viewer->viewerFont();
        for (int i = searchAttached ? 1 : 0; i < count; ++i)
            viewerAt(i)->setViewerFont(font);
    }
}

bool CentralWidget::searchWidgetAttached() const
{
    TRACE_OBJ
    return m_searchWidget && m_searchWidget->isAttached();
}

void CentralWidget::createSearchWidget(QHelpSearchEngine *searchEngine)
{
    TRACE_OBJ
    if (m_searchWidget)
        return;

    m_searchWidget = new SearchWidget(searchEngine, this);
    connect(m_searchWidget, SIGNAL(requestShowLink(QUrl)), this,
        SLOT(setSourceFromSearch(QUrl)));
    connect(m_searchWidget, SIGNAL(requestShowLinkInNewTab(QUrl)), this,
        SLOT(setSourceFromSearchInNewTab(QUrl)));

    HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();
    m_searchWidget->setFont(!helpEngine.usesBrowserFont() ? qApp->font()
        : helpEngine.browserFont());
}

void CentralWidget::activateSearchWidget(bool updateLastTabPage)
{
    TRACE_OBJ
    if (!m_searchWidget)
        createSearchWidget(HelpEngineWrapper::instance().searchEngine());

    if (!m_searchWidget->isAttached()) {
        tabWidget->insertTab(0, m_searchWidget, tr("Search"));
        m_searchWidget->setAttached(true);

        if (updateLastTabPage)
            lastTabPage++;
    }

    tabWidget->setCurrentWidget(m_searchWidget);
    m_searchWidget->setFocus();
}

void CentralWidget::removeSearchWidget()
{
    TRACE_OBJ
    if (searchWidgetAttached()) {
        tabWidget->removeTab(0);
        m_searchWidget->setAttached(false);
    }
}

int CentralWidget::availableHelpViewer() const
{
    TRACE_OBJ
    int count = tabWidget->count();
    if (searchWidgetAttached())
        count--;
    return count;
}

bool CentralWidget::enableTabCloseAction() const
{
    TRACE_OBJ
    int minTabCount = 1;
    if (searchWidgetAttached())
        minTabCount = 2;

    return (tabWidget->count() > minTabCount);
}

QString CentralWidget::quoteTabTitle(const QString &title) const
{
    TRACE_OBJ
    QString s = title;
    return s.replace(QLatin1Char('&'), QLatin1String("&&"));
}

void
CentralWidget::setSourceFromSearch(const QUrl &url)
{
    TRACE_OBJ
    setSource(url);
#if defined(QT_NO_WEBKIT)
    highlightSearchTerms();
#else
    connect(currentHelpViewer(), SIGNAL(loadFinished(bool)), this,
        SLOT(highlightSearchTerms()));
#endif
}

void
CentralWidget::setSourceFromSearchInNewTab(const QUrl &url)
{
    TRACE_OBJ
    setSourceInNewTab(url);
#if defined(QT_NO_WEBKIT)
    highlightSearchTerms();
#else
    connect(currentHelpViewer(), SIGNAL(loadFinished(bool)), this,
        SLOT(highlightSearchTerms()));
#endif
}

void
CentralWidget::highlightSearchTerms()
{
    TRACE_OBJ
    HelpViewer *viewer = currentHelpViewer();
    if (!viewer)
        return;

    QHelpSearchEngine *searchEngine =
        HelpEngineWrapper::instance().searchEngine();
    QList<QHelpSearchQuery> queryList = searchEngine->query();

    QStringList terms;
    foreach (const QHelpSearchQuery &query, queryList) {
        switch (query.fieldName) {
            default: break;
            case QHelpSearchQuery::ALL: {
            case QHelpSearchQuery::PHRASE:
            case QHelpSearchQuery::DEFAULT:
            case QHelpSearchQuery::ATLEAST:
                foreach (QString term, query.wordList)
                    terms.append(term.remove(QLatin1String("\"")));
            }
        }
    }

#if defined(QT_NO_WEBKIT)
    viewer->viewport()->setUpdatesEnabled(false);

    QTextCharFormat marker;
    marker.setForeground(Qt::red);

    QTextCursor firstHit;

    QTextCursor c = viewer->textCursor();
    c.beginEditBlock();
    foreach (const QString& term, terms) {
        c.movePosition(QTextCursor::Start);
        viewer->setTextCursor(c);

        while (viewer->find(term, QTextDocument::FindWholeWords)) {
            QTextCursor hit = viewer->textCursor();
            if (firstHit.isNull() || hit.position() < firstHit.position())
                firstHit = hit;

            hit.mergeCharFormat(marker);
        }
    }

    if (firstHit.isNull()) {
        firstHit = viewer->textCursor();
        firstHit.movePosition(QTextCursor::Start);
    }
    firstHit.clearSelection();
    c.endEditBlock();
    viewer->setTextCursor(firstHit);

    viewer->viewport()->setUpdatesEnabled(true);
#else
    viewer->findText("", QWebPage::HighlightAllOccurrences);
         // clears existing selections
    foreach (const QString& term, terms)
        viewer->findText(term, QWebPage::HighlightAllOccurrences);

    disconnect(viewer, SIGNAL(loadFinished(bool)), this,
        SLOT(highlightSearchTerms()));
#endif
}


void CentralWidget::closeOrReloadTabs(const QList<int> &indices, bool tryReload)
{
    TRACE_OBJ
    QList<int> sortedIndices = indices;
    qSort(sortedIndices);
    for (int i = sortedIndices.count(); --i >= 0;) {
        const int tab = sortedIndices.at(i);
        bool close = true;
        if (tryReload) {
            HelpViewer *viewer =
                    qobject_cast<HelpViewer*>(tabWidget->widget(tab));
            if (HelpEngineWrapper::instance().findFile(viewer->source()).isValid()) {
                viewer->reload();
                close = false;
            }
        }
        if (close)
            closeTabAt(tab);
    }
    if (availableHelpViewer() == 0)
        setSource(QUrl(QLatin1String("about:blank")));
}

void CentralWidget::closeTabAt(int index)
{
    TRACE_OBJ
    HelpViewer *viewer = qobject_cast<HelpViewer*>(tabWidget->widget(index));
    tabWidget->removeTab(index);
    QTimer::singleShot(0, viewer, SLOT(deleteLater()));
}

QMap<int, QString> CentralWidget::currentSourceFileList() const
{
    TRACE_OBJ
    QMap<int, QString> sourceList;
    for (int i = 0; i < tabWidget->count(); ++i) {
        HelpViewer *viewer = qobject_cast<HelpViewer*>(tabWidget->widget(i));
        if (viewer && viewer->source().isValid())
            sourceList.insert(i, viewer->source().host());
    }
    return sourceList;
}

QT_END_NAMESPACE
