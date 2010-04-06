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

#include "centralwidget.h"

#include "findwidget.h"
#include "helpenginewrapper.h"
#include "helpviewer_qtb.h"
#include "helpviewer_qwv.h"
#include "tracer.h"
#include "../shared/collectionconfiguration.h"

#include <QtCore/QTimer>

#include <QtGui/QKeyEvent>
#include <QtGui/QPageSetupDialog>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrintPreviewDialog>
#include <QtGui/QPrinter>
#include <QtGui/QStackedWidget>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>

#include <QtHelp/QHelpSearchEngine>

QT_BEGIN_NAMESPACE

namespace {
    CentralWidget *staticCentralWidget = 0;
}

// -- CentralWidget

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget(parent)
    , findWidget(0)
    , printer(0)
{
    TRACE_OBJ
    staticCentralWidget = this;
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);

    vboxLayout->setMargin(0);
    m_stackedWidget = new QStackedWidget(this);
    vboxLayout->addWidget(m_stackedWidget);

    findWidget = new FindWidget(this);
    vboxLayout->addWidget(findWidget);
    findWidget->hide();

    connect(findWidget, SIGNAL(findNext()), this, SLOT(findNext()));
    connect(findWidget, SIGNAL(findPrevious()), this, SLOT(findPrevious()));
    connect(findWidget, SIGNAL(find(QString, bool)), this,
        SLOT(find(QString, bool)));
    connect(findWidget, SIGNAL(escapePressed()), this, SLOT(activateTab()));

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
    for (int i = 0; i < m_stackedWidget->count(); ++i) {
        const HelpViewer * const viewer = viewerAt(i);
        const QUrl &source = viewer->source();
        if (source.isValid()) {
            currentPages << source.toString();
            zoomFactors << QString::number(viewer->scale());
        }
    }

    HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();
    helpEngine.setLastShownPages(currentPages);
    helpEngine.setLastZoomFactors(zoomFactors);
    helpEngine.setLastTabPage(m_stackedWidget->currentIndex());
}

CentralWidget *CentralWidget::instance()
{
    TRACE_OBJ
    return staticCentralWidget;
}

void CentralWidget::zoomIn()
{
    TRACE_OBJ
    currentHelpViewer()->scaleUp();
}

void CentralWidget::zoomOut()
{
    TRACE_OBJ
    currentHelpViewer()->scaleDown();
}

void CentralWidget::resetZoom()
{
    TRACE_OBJ
    currentHelpViewer()->resetScale();
}

void CentralWidget::setSource(const QUrl &url)
{
    TRACE_OBJ
    HelpViewer *viewer = currentHelpViewer();
    viewer->setSource(url);
    viewer->setFocus(Qt::OtherFocusReason);
}

bool CentralWidget::hasSelection() const
{
    TRACE_OBJ
    return currentHelpViewer()->hasSelection();
}

QUrl CentralWidget::currentSource() const
{
    TRACE_OBJ
    return currentHelpViewer()->source();
}

QString CentralWidget::currentTitle() const
{
    TRACE_OBJ
    return currentHelpViewer()->documentTitle();
}

void CentralWidget::copySelection()
{
    TRACE_OBJ
    currentHelpViewer()->copy();
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
    currentHelpViewer()->print(p);
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

void CentralWidget::home()
{
    TRACE_OBJ
    currentHelpViewer()->home();
}

bool CentralWidget::isForwardAvailable() const
{
    TRACE_OBJ
    return currentHelpViewer()->isForwardAvailable();
}

void CentralWidget::forward()
{
    TRACE_OBJ
    currentHelpViewer()->forward();
}

bool CentralWidget::isBackwardAvailable() const
{
    TRACE_OBJ
    return currentHelpViewer()->isBackwardAvailable();
}

void CentralWidget::backward()
{
    TRACE_OBJ
    currentHelpViewer()->backward();
}

void CentralWidget::connectSignals(HelpViewer *page)
{
    TRACE_OBJ
    connect(page, SIGNAL(copyAvailable(bool)), this, SIGNAL(copyAvailable(bool)));
    connect(page, SIGNAL(forwardAvailable(bool)), this,
        SIGNAL(forwardAvailable(bool)));
    connect(page, SIGNAL(backwardAvailable(bool)), this,
        SIGNAL(backwardAvailable(bool)));
    connect(page, SIGNAL(sourceChanged(QUrl)), this,
        SLOT(handleSourceChanged(QUrl)));
    connect(page, SIGNAL(highlighted(QString)), this,
            SIGNAL(highlighted(QString)));
}

HelpViewer* CentralWidget::viewerAt(int index) const
{
    TRACE_OBJ
    return static_cast<HelpViewer*>(m_stackedWidget->widget(index));
}

HelpViewer* CentralWidget::currentHelpViewer() const
{
    TRACE_OBJ
    return static_cast<HelpViewer *>(m_stackedWidget->currentWidget());
}

void CentralWidget::activateTab()
{
    TRACE_OBJ
    currentHelpViewer()->setFocus();
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
    HelpViewer *viewer = currentHelpViewer();
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
    const int count = m_stackedWidget->count();
    const QFont &font = viewerAt(count - 1)->viewerFont();
    for (int i = 0; i < count; ++i)
        viewerAt(i)->setViewerFont(font);
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
CentralWidget::highlightSearchTerms()
{
    TRACE_OBJ
    HelpViewer *viewer = currentHelpViewer();

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

void CentralWidget::addPage(HelpViewer *page, bool fromSearch)
{
    TRACE_OBJ
    page->installEventFilter(this);
    page->setFocus(Qt::OtherFocusReason);
    connectSignals(page);
    m_stackedWidget->addWidget(page);
    if (fromSearch) {
#if defined(QT_NO_WEBKIT)
        highlightSearchTerms();
#else
        connect(currentHelpViewer(), SIGNAL(loadFinished(bool)), this,
            SLOT(highlightSearchTerms()));
#endif
    }
}

void CentralWidget::removePage(int index)
{
    TRACE_OBJ
    const bool  currentChanged = index == currentIndex();
    m_stackedWidget->removeWidget(m_stackedWidget->widget(index));
    if (currentChanged)
        emit currentViewerChanged();
}

void CentralWidget::setCurrentPage(HelpViewer *page)
{
    TRACE_OBJ
    m_stackedWidget->setCurrentWidget(page);
    emit currentViewerChanged();
}

int CentralWidget::currentIndex() const
{
    return  m_stackedWidget->currentIndex();
}

void CentralWidget::handleSourceChanged(const QUrl &url)
{
    if (sender() == currentHelpViewer())
        emit sourceChanged(url);
}

QT_END_NAMESPACE
