/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Assistant module of the Qt Toolkit.
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
#include "openpagesmanager.h"
 
#include "centralwidget.h"
#include "helpenginewrapper.h"
#include "helpviewer.h"
#include "openpagesmodel.h"
#include "openpageswidget.h"
#include "tracer.h"
#include "../shared/collectionconfiguration.h"

#include <QtGui/QTreeView>
 
QT_BEGIN_NAMESPACE

OpenPagesManager *OpenPagesManager::createInstance(QObject *parent,
                      bool defaultCollection, const QUrl &cmdLineUrl)
{
    TRACE_OBJ
    Q_ASSERT(!m_instance);
    m_instance = new OpenPagesManager(parent, defaultCollection, cmdLineUrl);
    return m_instance;
}

OpenPagesManager *OpenPagesManager::instance()
{
    TRACE_OBJ
    Q_ASSERT(m_instance);
    return m_instance;
}

OpenPagesManager::OpenPagesManager(QObject *parent, bool defaultCollection,
                                   const QUrl &cmdLineUrl)
    : QObject(parent), m_model(new OpenPagesModel(this)),
      m_openPagesWidget(new OpenPagesWidget(m_model))
{
    TRACE_OBJ
    connect(m_openPagesWidget, SIGNAL(setCurrentPage(QModelIndex)), this,
            SLOT(setCurrentPage(QModelIndex)));
    connect(m_openPagesWidget, SIGNAL(closePage(QModelIndex)), this,
            SLOT(closePage(QModelIndex)));
    connect(m_openPagesWidget, SIGNAL(closePagesExcept(QModelIndex)), this,
            SLOT(closePagesExcept(QModelIndex)));
    setupInitialPages(defaultCollection, cmdLineUrl);
}
 
int OpenPagesManager::pageCount() const
{
    TRACE_OBJ
    return m_model->rowCount();
}
 
void OpenPagesManager::setupInitialPages(bool defaultCollection,
            const QUrl &cmdLineUrl)
{
    TRACE_OBJ
    if (cmdLineUrl.isValid()) {
        createPage(cmdLineUrl);
        return;
    }

    HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();
    int initialPage = 0;
    switch (helpEngine.startOption()) {
    case ShowHomePage:
        m_model->addPage(helpEngine.homePage());
        break;
    case ShowBlankPage:
        m_model->addPage(QUrl(QLatin1String("about:blank")));
        break;
    case ShowLastPages: {
        const QStringList &lastShownPageList = helpEngine.lastShownPages();
        const int pageCount = lastShownPageList.count();
        if (pageCount == 0) {
            if (defaultCollection)
                m_model->addPage(QUrl(QLatin1String("help")));
            else
                m_model->addPage(QUrl(QLatin1String("about:blank")));
        } else {
            QStringList zoomFactors = helpEngine.lastZoomFactors();
            while (zoomFactors.count() < pageCount)
                zoomFactors.append(CollectionConfiguration::DefaultZoomFactor);
            initialPage = helpEngine.lastTabPage();
            if (initialPage >= pageCount) {
                qWarning("Initial page set to %d, maximum possible value is %d",
                         initialPage, pageCount - 1);
                initialPage = 0;
            }
            for (int curPage = 0; curPage < pageCount; ++curPage) {
                const QString &curFile = lastShownPageList.at(curPage);
                if (helpEngine.findFile(curFile).isValid()
                    || curFile == QLatin1String("about:blank")) {
                    m_model->addPage(curFile, zoomFactors.at(curPage).toFloat());
                } else if (curPage <= initialPage && initialPage > 0)
                    --initialPage;
            }
        }
        break;
    }
    default:
        Q_ASSERT(!"Unhandled option");
    }

    if (m_model->rowCount() == 0)
        m_model->addPage(helpEngine.homePage());
    for (int i = 0; i < m_model->rowCount(); ++i)
        CentralWidget::instance()->addPage(m_model->pageAt(i));
    setCurrentPage(initialPage);
}

HelpViewer *OpenPagesManager::createPage()
{
    TRACE_OBJ
    return createPage(QUrl(QLatin1String("about:blank")));
}

void OpenPagesManager::closeCurrentPage()
{
    TRACE_OBJ
    Q_ASSERT(m_model->rowCount() > 1);
    const QModelIndexList selectedIndexes
        = m_openPagesWidget->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty())
        return;
    Q_ASSERT(selectedIndexes.count() == 1);
    removePage(selectedIndexes.first().row());
}

HelpViewer *OpenPagesManager::createPage(const QUrl &url, bool fromSearch)
{
    TRACE_OBJ
    if (HelpViewer::launchWithExternalApp(url))
        return 0;

    m_model->addPage(url);
    const int index = m_model->rowCount() - 1;
    HelpViewer * const page = m_model->pageAt(index);
    CentralWidget::instance()->addPage(page, fromSearch);
    setCurrentPage(index);
    return page;
}

HelpViewer *OpenPagesManager::createNewPageFromSearch(const QUrl &url)
{
    TRACE_OBJ
    return createPage(url, true);
}

void OpenPagesManager::closePage(const QModelIndex &index)
{
    TRACE_OBJ
    if (index.isValid())
        removePage(index.row());
}

void OpenPagesManager::closePages(const QString &nameSpace)
{
    TRACE_OBJ
    closeOrReloadPages(nameSpace, false);
}

void OpenPagesManager::reloadPages(const QString &nameSpace)
{
    TRACE_OBJ
    closeOrReloadPages(nameSpace, true);
    selectCurrentPage();
}

void OpenPagesManager::closeOrReloadPages(const QString &nameSpace, bool tryReload)
{
    TRACE_OBJ

    for (int i = m_model->rowCount() - 1; i >= 0; --i) {
        HelpViewer *page = m_model->pageAt(i);
        if (page->source().host() != nameSpace)
            continue;
        if (tryReload  && HelpEngineWrapper::instance().findFile(page->source()).isValid())
            page->reload();
        else if (m_model->rowCount() == 1)
            page->setSource(QUrl(QLatin1String("about:blank")));
        else
            removePage(i);
    }
}

bool OpenPagesManager::pagesOpenForNamespace(const QString &nameSpace) const
{
    TRACE_OBJ
    for (int i = 0; i < m_model->rowCount(); ++i)
        if (m_model->pageAt(i)->source().host() == nameSpace)
            return true;
    return false;
}

void OpenPagesManager::setCurrentPage(const QModelIndex &index)
{
    TRACE_OBJ
    if (!index.isValid())
        return;
    HelpViewer * const page = m_model->pageAt(index.row());
    CentralWidget::instance()->setCurrentPage(page);
}

void OpenPagesManager::setCurrentPage(int index)
{
    TRACE_OBJ
    CentralWidget::instance()->setCurrentPage(m_model->pageAt(index));
    selectCurrentPage();
}

void OpenPagesManager::selectCurrentPage()
{
    TRACE_OBJ
    QItemSelectionModel * const selModel = m_openPagesWidget->selectionModel();
    selModel->clearSelection();
    selModel->select(m_model->index(CentralWidget::instance()->currentIndex(), 0),
        QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    m_openPagesWidget->scrollTo(m_openPagesWidget->currentIndex());
}

void OpenPagesManager::removePage(int index)
{
    TRACE_OBJ
    CentralWidget::instance()->removePage(index);
    m_model->removePage(index);
    selectCurrentPage();
}


void OpenPagesManager::closePagesExcept(const QModelIndex &index)
{
    TRACE_OBJ
    if (!index.isValid())
        return;

    int i = 0;
    HelpViewer *viewer = m_model->pageAt(index.row());
    while (m_model->rowCount() > 1) {
        if (m_model->pageAt(i) != viewer)
            removePage(i);
        else
            ++i;
    }
}

QAbstractItemView *OpenPagesManager::openPagesWidget() const
{
    TRACE_OBJ
    return m_openPagesWidget;
}

void OpenPagesManager::nextPage()
{
    TRACE_OBJ
    nextOrPreviousPage(1);
}

void OpenPagesManager::previousPage()
{
    TRACE_OBJ
    nextOrPreviousPage(-1);
}

void OpenPagesManager::nextOrPreviousPage(int offset)
{
    TRACE_OBJ
    setCurrentPage((CentralWidget::instance()->currentIndex() + offset
        + m_model->rowCount()) % m_model->rowCount());
}

OpenPagesManager *OpenPagesManager::m_instance = 0;

QT_END_NAMESPACE
