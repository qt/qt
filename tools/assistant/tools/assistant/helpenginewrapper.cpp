/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "helpenginewrapper.h"
#include "../shared/collectionconfiguration.h"

#include <QtCore/QFileInfo>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QTimer>
#include <QtHelp/QHelpContentModel>
#include <QtHelp/QHelpEngine>
#include <QtHelp/QHelpIndexModel>
#include <QtHelp/QHelpSearchEngine>

QT_BEGIN_NAMESPACE

namespace {
    const QString AppFontKey(QLatin1String("appFont"));
    const QString AppWritingSystemKey(QLatin1String("appWritingSystem"));
    const QString BookmarksKey(QLatin1String("Bookmarks"));
    const QString BrowserFontKey(QLatin1String("browserFont"));
    const QString BrowserWritingSystemKey(QLatin1String("browserWritingSystem"));
    const QString HomePageKey(QLatin1String("homepage"));
    const QString MainWindowKey(QLatin1String("MainWindow"));
    const QString MainWindowGeometryKey(QLatin1String("MainWindowGeometry"));
    const QString SearchWasAttachedKey(QLatin1String("SearchWasAttached"));
    const QString StartOptionKey(QLatin1String("StartOption"));
    const QString UnfilteredInsertedKey(QLatin1String("UnfilteredFilterInserted"));
    const QString UseAppFontKey(QLatin1String("useAppFont"));
    const QString UseBrowserFontKey(QLatin1String("useBrowserFont"));
    const QString VersionKey(QString(QLatin1String("qtVersion%1$$$%2")).
                             arg(QLatin1String(QT_VERSION_STR)));
} // anonymous namespace

HelpEngineWrapper *HelpEngineWrapper::helpEngineWrapper = 0;

HelpEngineWrapper &HelpEngineWrapper::instance(const QString &collectionFile)
{
    TRACE_OBJ
    /*
     * Note that this Singleton cannot be static, because it has to be
     * deleted before the QApplication.
     */
    if (helpEngineWrapper == 0)
        helpEngineWrapper = new HelpEngineWrapper(collectionFile);
    return *helpEngineWrapper;
}

void HelpEngineWrapper::removeInstance()
{
    TRACE_OBJ
    delete helpEngineWrapper;
    helpEngineWrapper = 0;
}

HelpEngineWrapper::HelpEngineWrapper(const QString &collectionFile)
    : m_helpEngine(new QHelpEngine(collectionFile, this)),
      m_qchWatcher(new QFileSystemWatcher(this))
{
    TRACE_OBJ
    connect(m_helpEngine, SIGNAL(currentFilterChanged(QString)),
            this, SIGNAL(currentFilterChanged(QString)));
    connect(m_helpEngine, SIGNAL(setupFinished()),
            this, SIGNAL(setupFinished()));
    initFileSystemWatchers();
}

void HelpEngineWrapper::initFileSystemWatchers()
{
    TRACE_OBJ
    foreach(const QString &ns, m_helpEngine->registeredDocumentations()) {
        const QString &docFile = m_helpEngine->documentationFileName(ns);
        m_qchWatcher->addPath(docFile);
        connect(m_qchWatcher, SIGNAL(fileChanged(QString)),
                this, SLOT(qchFileChanged(QString)));
    }
    assertDocFilesWatched();
}

QHelpSearchEngine *HelpEngineWrapper::searchEngine() const
{
    TRACE_OBJ
    return m_helpEngine->searchEngine();
}

QHelpContentModel *HelpEngineWrapper::contentModel() const
{
    TRACE_OBJ
    return m_helpEngine->contentModel();
}

QHelpIndexModel *HelpEngineWrapper::indexModel() const
{
    TRACE_OBJ
    return m_helpEngine->indexModel();
}

QHelpContentWidget *HelpEngineWrapper::contentWidget()
{
    TRACE_OBJ
    return m_helpEngine->contentWidget();
}

QHelpIndexWidget *HelpEngineWrapper::indexWidget()
{
    TRACE_OBJ
    return m_helpEngine->indexWidget();
}

const QStringList HelpEngineWrapper::registeredDocumentations() const
{
    TRACE_OBJ
    return m_helpEngine->registeredDocumentations();
}

const QString HelpEngineWrapper::collectionFile() const
{
    TRACE_OBJ
    return m_helpEngine->collectionFile();
}

bool HelpEngineWrapper::registerDocumentation(const QString &docFile)
{
    TRACE_OBJ
    assertDocFilesWatched();
    if (!m_helpEngine->registerDocumentation(docFile))
        return false;
    m_qchWatcher->addPath(docFile);
    assertDocFilesWatched();
    return true;
}

bool HelpEngineWrapper::unregisterDocumentation(const QString &namespaceName)
{
    TRACE_OBJ
    assertDocFilesWatched();
    const QString &file = m_helpEngine->documentationFileName(namespaceName);
    if (!m_helpEngine->unregisterDocumentation(namespaceName))
        return false;
    m_qchWatcher->removePath(file);
    assertDocFilesWatched();
    return true;
}

bool HelpEngineWrapper::setupData()
{
    TRACE_OBJ
    return m_helpEngine->setupData();
}

bool HelpEngineWrapper::addCustomFilter(const QString &filterName,
                                        const QStringList &attributes)
{
    TRACE_OBJ
    return m_helpEngine->addCustomFilter(filterName, attributes);
}

bool HelpEngineWrapper::removeCustomFilter(const QString &filterName)
{
    TRACE_OBJ
    return m_helpEngine->removeCustomFilter(filterName);
}

void HelpEngineWrapper::setCurrentFilter(const QString &currentFilter)
{
    TRACE_OBJ
    m_helpEngine->setCurrentFilter(currentFilter);
}

const QString HelpEngineWrapper::currentFilter() const
{
    TRACE_OBJ
    return m_helpEngine->currentFilter();
}

const QStringList HelpEngineWrapper::customFilters() const
{
    TRACE_OBJ
    return m_helpEngine->customFilters();
}

QUrl HelpEngineWrapper::findFile(const QUrl &url) const
{
    TRACE_OBJ
    return m_helpEngine->findFile(url);
}

QByteArray HelpEngineWrapper::fileData(const QUrl &url) const
{
    TRACE_OBJ
    return m_helpEngine->fileData(url);
}

QMap<QString, QUrl> HelpEngineWrapper::linksForIdentifier(const QString &id) const
{
    TRACE_OBJ
    return m_helpEngine->linksForIdentifier(id);
}

const QStringList HelpEngineWrapper::filterAttributes() const
{
    TRACE_OBJ
    return m_helpEngine->filterAttributes();
}

const QStringList HelpEngineWrapper::filterAttributes(const QString &filterName) const
{
    TRACE_OBJ
    return m_helpEngine->filterAttributes(filterName);
}

QString HelpEngineWrapper::error() const
{
    TRACE_OBJ
    return m_helpEngine->error();
}

bool HelpEngineWrapper::unfilteredInserted() const
{
    TRACE_OBJ
    return m_helpEngine->customValue(UnfilteredInsertedKey).toInt() == 1;
}

void HelpEngineWrapper::setUnfilteredInserted()
{
    TRACE_OBJ
    m_helpEngine->setCustomValue(UnfilteredInsertedKey, 1);
}

const QStringList HelpEngineWrapper::qtDocInfo(const QString &component) const
{
    TRACE_OBJ
    return m_helpEngine->customValue(VersionKey.arg(component)).toString().
        split(CollectionConfiguration::ListSeparator);
}

void HelpEngineWrapper::setQtDocInfo(const QString &component,
                                     const QStringList &doc)
{
    TRACE_OBJ
    m_helpEngine->setCustomValue(VersionKey.arg(component),
                              doc.join(CollectionConfiguration::ListSeparator));
}

const QStringList HelpEngineWrapper::lastShownPages() const
{
    TRACE_OBJ
    return CollectionConfiguration::lastShownPages(*m_helpEngine);
}

void HelpEngineWrapper::setLastShownPages(const QStringList &lastShownPages)
{
    TRACE_OBJ
    CollectionConfiguration::setLastShownPages(*m_helpEngine, lastShownPages);
}

const QStringList HelpEngineWrapper::lastZoomFactors() const
{
    TRACE_OBJ
    return CollectionConfiguration::lastZoomFactors(*m_helpEngine);
}

void HelpEngineWrapper::setLastZoomFactors(const QStringList &lastZoomFactors)
{
    TRACE_OBJ
    CollectionConfiguration::setLastZoomFactors(*m_helpEngine, lastZoomFactors);
}

const QString HelpEngineWrapper::cacheDir() const
{
    TRACE_OBJ
    return CollectionConfiguration::cacheDir(*m_helpEngine);
}

bool HelpEngineWrapper::cacheDirIsRelativeToCollection() const
{
    TRACE_OBJ
    return CollectionConfiguration::cacheDirIsRelativeToCollection(*m_helpEngine);
}

void HelpEngineWrapper::setCacheDir(const QString &cacheDir,
                                    bool relativeToCollection)
{
    TRACE_OBJ
    CollectionConfiguration::setCacheDir(*m_helpEngine, cacheDir,
                                         relativeToCollection);
}

bool HelpEngineWrapper::filterFunctionalityEnabled() const
{
    TRACE_OBJ
    return CollectionConfiguration::filterFunctionalityEnabled(*m_helpEngine);
}

void HelpEngineWrapper::setFilterFunctionalityEnabled(bool enabled)
{
    TRACE_OBJ
    CollectionConfiguration::setFilterFunctionalityEnabled(*m_helpEngine,
                                                           enabled);
}

bool HelpEngineWrapper::filterToolbarVisible() const
{
    TRACE_OBJ
    return CollectionConfiguration::filterToolbarVisible(*m_helpEngine);
}

void HelpEngineWrapper::setFilterToolbarVisible(bool visible)
{
    TRACE_OBJ
    CollectionConfiguration::setFilterToolbarVisible(*m_helpEngine, visible);
}

bool HelpEngineWrapper::addressBarEnabled() const
{
    TRACE_OBJ
    return CollectionConfiguration::addressBarEnabled(*m_helpEngine);
}

void HelpEngineWrapper::setAddressBarEnabled(bool enabled)
{
    TRACE_OBJ
    CollectionConfiguration::setAddressBarEnabled(*m_helpEngine, enabled);
}

bool HelpEngineWrapper::addressBarVisible() const
{
    TRACE_OBJ
    return CollectionConfiguration::addressBarVisible(*m_helpEngine);
}

void HelpEngineWrapper::setAddressBarVisible(bool visible)
{
    TRACE_OBJ
    CollectionConfiguration::setAddressBarVisible(*m_helpEngine, visible);
}

bool HelpEngineWrapper::documentationManagerEnabled() const
{
    TRACE_OBJ
    return CollectionConfiguration::documentationManagerEnabled(*m_helpEngine);
}

void HelpEngineWrapper::setDocumentationManagerEnabled(bool enabled)
{
    TRACE_OBJ
    CollectionConfiguration::setDocumentationManagerEnabled(*m_helpEngine,
                                                            enabled);
}

const QByteArray HelpEngineWrapper::aboutMenuTexts() const
{
    TRACE_OBJ
    return CollectionConfiguration::aboutMenuTexts(*m_helpEngine);
}

void HelpEngineWrapper::setAboutMenuTexts(const QByteArray &texts)
{
    TRACE_OBJ
    CollectionConfiguration::setAboutMenuTexts(*m_helpEngine, texts);
}

const QByteArray HelpEngineWrapper::aboutIcon() const
{
    TRACE_OBJ
    return CollectionConfiguration::aboutIcon(*m_helpEngine);
}

void HelpEngineWrapper::setAboutIcon(const QByteArray &icon)
{
    TRACE_OBJ
    CollectionConfiguration::setAboutIcon(*m_helpEngine, icon);
}

const QByteArray HelpEngineWrapper::aboutImages() const
{
    TRACE_OBJ
    return CollectionConfiguration::aboutImages(*m_helpEngine);
}

void HelpEngineWrapper::setAboutImages(const QByteArray &images)
{
    TRACE_OBJ
    CollectionConfiguration::setAboutImages(*m_helpEngine, images);
}

const QByteArray HelpEngineWrapper::aboutTexts() const
{
    TRACE_OBJ
    return CollectionConfiguration::aboutTexts(*m_helpEngine);
}

void HelpEngineWrapper::setAboutTexts(const QByteArray &texts)
{
    TRACE_OBJ
    CollectionConfiguration::setAboutTexts(*m_helpEngine, texts);
}

const QString HelpEngineWrapper::windowTitle() const
{
    TRACE_OBJ
    return CollectionConfiguration::windowTitle(*m_helpEngine);
}

void HelpEngineWrapper::setWindowTitle(const QString &windowTitle)
{
    TRACE_OBJ
    CollectionConfiguration::setWindowTitle(*m_helpEngine, windowTitle);
}

const QByteArray HelpEngineWrapper::applicationIcon() const
{
    TRACE_OBJ
    return CollectionConfiguration::applicationIcon(*m_helpEngine);
}

void HelpEngineWrapper::setApplicationIcon(const QByteArray &icon)
{
    TRACE_OBJ
    CollectionConfiguration::setApplicationIcon(*m_helpEngine, icon);
}

const QByteArray HelpEngineWrapper::mainWindow() const
{
    TRACE_OBJ
    return m_helpEngine->customValue(MainWindowKey).toByteArray();
}

void HelpEngineWrapper::setMainWindow(const QByteArray &mainWindow)
{
    TRACE_OBJ
    m_helpEngine->setCustomValue(MainWindowKey, mainWindow);
}

const QByteArray HelpEngineWrapper::mainWindowGeometry() const
{
    TRACE_OBJ
    return m_helpEngine->customValue(MainWindowGeometryKey).toByteArray();
}

void HelpEngineWrapper::setMainWindowGeometry(const QByteArray &geometry)
{
    TRACE_OBJ
    m_helpEngine->setCustomValue(MainWindowGeometryKey, geometry);
}

const QByteArray HelpEngineWrapper::bookmarks() const
{
    TRACE_OBJ
    return m_helpEngine->customValue(BookmarksKey).toByteArray();
}

void HelpEngineWrapper::setBookmarks(const QByteArray &bookmarks)
{
    TRACE_OBJ
    m_helpEngine->setCustomValue(BookmarksKey, bookmarks);
}

int HelpEngineWrapper::lastTabPage() const
{
    TRACE_OBJ
    return CollectionConfiguration::lastTabPage(*m_helpEngine);
}

void HelpEngineWrapper::setLastTabPage(int lastPage)
{
    TRACE_OBJ
    CollectionConfiguration::setLastTabPage(*m_helpEngine, lastPage);
}

bool HelpEngineWrapper::searchWasAttached() const
{
    TRACE_OBJ
    return m_helpEngine->customValue(SearchWasAttachedKey).toBool();
}

void HelpEngineWrapper::setSearchWasAttached(bool attached)
{
    TRACE_OBJ
    m_helpEngine->setCustomValue(SearchWasAttachedKey, attached);
}

int HelpEngineWrapper::startOption() const
{
    TRACE_OBJ
    return m_helpEngine->customValue(StartOptionKey, ShowLastPages).toInt();
}

void HelpEngineWrapper::setStartOption(int option)
{
    TRACE_OBJ
    m_helpEngine->setCustomValue(StartOptionKey, option);
}

const QString HelpEngineWrapper::homePage() const
{
    TRACE_OBJ
    const QString &homePage = m_helpEngine->customValue(HomePageKey).toString();
    if (!homePage.isEmpty())
        return homePage;
    return defaultHomePage();
}

void HelpEngineWrapper::setHomePage(const QString &page)
{
    TRACE_OBJ
    m_helpEngine->setCustomValue(HomePageKey, page);

}

const QString HelpEngineWrapper::defaultHomePage() const
{
    TRACE_OBJ
    return CollectionConfiguration::defaultHomePage(*m_helpEngine);
}

void HelpEngineWrapper::setDefaultHomePage(const QString &page)
{
    TRACE_OBJ
    CollectionConfiguration::setDefaultHomePage(*m_helpEngine, page);
}

bool HelpEngineWrapper::hasFontSettings() const
{
    TRACE_OBJ
    return m_helpEngine->customValue(UseAppFontKey).isValid();
}

bool HelpEngineWrapper::usesAppFont() const
{
    TRACE_OBJ
    return m_helpEngine->customValue(UseAppFontKey).toBool();
}

void HelpEngineWrapper::setUseAppFont(bool useAppFont)
{
    TRACE_OBJ
    m_helpEngine->setCustomValue(UseAppFontKey, useAppFont);
}

bool HelpEngineWrapper::usesBrowserFont() const
{
    TRACE_OBJ
    return m_helpEngine->customValue(UseBrowserFontKey, false).toBool();
}

void HelpEngineWrapper::setUseBrowserFont(bool useBrowserFont)
{
    TRACE_OBJ
    m_helpEngine->setCustomValue(UseBrowserFontKey, useBrowserFont);
}

const QFont HelpEngineWrapper::appFont() const
{
    TRACE_OBJ
    return qVariantValue<QFont>(m_helpEngine->customValue(AppFontKey));
}

void HelpEngineWrapper::setAppFont(const QFont &font)
{
    TRACE_OBJ
    m_helpEngine->setCustomValue(AppFontKey, font);
}

QFontDatabase::WritingSystem HelpEngineWrapper::appWritingSystem() const
{
    TRACE_OBJ
    return static_cast<QFontDatabase::WritingSystem>(
        m_helpEngine->customValue(AppWritingSystemKey).toInt());
}

void HelpEngineWrapper::setAppWritingSystem(QFontDatabase::WritingSystem system)
{
    TRACE_OBJ
    m_helpEngine->setCustomValue(AppWritingSystemKey, system);
}

const QFont HelpEngineWrapper::browserFont() const
{
    TRACE_OBJ
    return qVariantValue<QFont>(m_helpEngine->customValue(BrowserFontKey));
}

void HelpEngineWrapper::setBrowserFont(const QFont &font)
{
    TRACE_OBJ
    m_helpEngine->setCustomValue(BrowserFontKey, font);
}

QFontDatabase::WritingSystem HelpEngineWrapper::browserWritingSystem() const
{
    TRACE_OBJ
    return static_cast<QFontDatabase::WritingSystem>(
        m_helpEngine->customValue(BrowserWritingSystemKey).toInt());
}

void HelpEngineWrapper::setBrowserWritingSystem(QFontDatabase::WritingSystem system)
{
    TRACE_OBJ
    m_helpEngine->setCustomValue(BrowserWritingSystemKey, system);
}


void HelpEngineWrapper::assertDocFilesWatched()
{
    TRACE_OBJ
    Q_ASSERT(m_qchWatcher->files().count()
             == m_helpEngine->registeredDocumentations().count());
}

TimeoutForwarder::TimeoutForwarder(const QString &fileName)
    : m_fileName(fileName)
{
    TRACE_OBJ
}

void TimeoutForwarder::forward()
{
    TRACE_OBJ
    HelpEngineWrapper::instance().qchFileChanged(m_fileName, true);
}

void HelpEngineWrapper::qchFileChanged(const QString &fileName)
{
    TRACE_OBJ
    qchFileChanged(fileName, false);
}

void HelpEngineWrapper::qchFileChanged(const QString &fileName, bool fromTimeout)
{
    TRACE_OBJ

    /*
     * We don't use QHelpEngineCore::namespaceName(fileName), because the file
     * may not exist anymore or contain a different namespace.
     */
    QString ns;
    foreach (const QString &curNs, m_helpEngine->registeredDocumentations()) {
        if (m_helpEngine->documentationFileName(curNs) == fileName) {
            ns = curNs;
            break;
        }
    }

    /*
     * We can't do an assertion here, because QFileSystemWatcher may send the
     * signal more than  once.
     */
    if (ns.isEmpty()) {
        m_recentQchUpdates.remove(fileName);
        return;
    }

    /*
     * Since the QFileSystemWatcher typically sends the signal more than once,
     * we repeatedly delay our reaction a bit until we think the last signal
     * was sent.
     */

    QMap<QString, RecentSignal>::Iterator it = m_recentQchUpdates.find(fileName);
    const QDateTime &now = QDateTime::currentDateTime();

     // Case 1: This is the first recent signal for the file.
    if (it == m_recentQchUpdates.end()) {
        QSharedPointer<TimeoutForwarder> forwarder(new TimeoutForwarder(fileName));
        m_recentQchUpdates.insert(fileName, RecentSignal(now, forwarder));
        QTimer::singleShot(UpdateGracePeriod, forwarder.data(), SLOT(forward()));
        return;
    }

    // Case 2: The last signal for this file has not expired yet.
    if (it.value().first > now.addMSecs(-UpdateGracePeriod)) {
        if (!fromTimeout)
            it.value().first = now;
        else
            QTimer::singleShot(UpdateGracePeriod, it.value().second.data(),
                               SLOT(forward()));
        return;
    }

    // Case 3: The last signal for this file has expired.
    if (m_helpEngine->unregisterDocumentation(ns)) {
        if (!QFileInfo(fileName).exists()
            || !m_helpEngine->registerDocumentation(fileName)) {
            m_qchWatcher->removePath(fileName);
            emit documentationRemoved(ns);
        } else {
            emit documentationUpdated(ns);
        }
        m_helpEngine->setupData();
    }
    m_recentQchUpdates.erase(it);
}

QT_END_NAMESPACE
