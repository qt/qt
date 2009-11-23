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

#ifndef COLLECTIONCONFIGURATION_H
#define COLLECTIONCONFIGURATION_H

#include <QtCore/QByteArray>
#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QFont>
#include <QtGui/QFontDatabase>

QT_BEGIN_NAMESPACE

class QHelpEngineCore;

class CollectionConfiguration
{
public:
    static int lastTabPage(const QHelpEngineCore &helpEngine);
    static void setLastTabPage(QHelpEngineCore &helpEngine, int lastPage);

    static const QStringList lastShownPages(const QHelpEngineCore &helpEngine);
    static void setLastShownPages(QHelpEngineCore &helpEngine,
                                  const QStringList &lastShownPages);


    // TODO: Don't allow last pages and zoom factors to be set in isolation
    //       Perhaps also fill up missing elements automatically or assert.
    static const QStringList lastZoomFactors(const QHelpEngineCore &helpEngine);
    static void setLastZoomFactors(QHelpEngineCore &helpEngine,
                                   const QStringList &lastZoomFactors);

    static const QString currentFilter(const QHelpEngineCore &helpEngine);
    static void setCurrentFilter(QHelpEngineCore &helpEngine,
                                 const QString &currentFilter);

    static const QString cacheDir(const QHelpEngineCore &helpEngine);
    static bool cacheDirIsRelativeToCollection(const QHelpEngineCore &helpEngine);
    static void setCacheDir(QHelpEngineCore &helpEngine,
                            const QString &cacheDir, bool relativeToCollection);

    static bool filterFunctionalityEnabled(const QHelpEngineCore &helpEngine);
    static void setFilterFunctionalityEnabled(QHelpEngineCore &helpEngine,
                                              bool enabled);

    static bool filterToolbarVisible(const QHelpEngineCore &helpEngine);
    static void setFilterToolbarVisible(QHelpEngineCore &helpEngine,
                                        bool visible);

    static bool addressBarEnabled(const QHelpEngineCore &helpEngine);
    static void setAddressBarEnabled(QHelpEngineCore &helpEngine, bool enabled);

    static bool addressBarVisible(const QHelpEngineCore &helpEngine);
    static void setAddressBarVisible(QHelpEngineCore &helpEngine, bool visible);

    static uint creationTime(const QHelpEngineCore &helpEngine);
    static void setCreationTime(QHelpEngineCore &helpEngine, uint time);

    static const QString windowTitle(const QHelpEngineCore &helpEngine);
    static void setWindowTitle(QHelpEngineCore &helpEngine,
                               const QString &windowTitle);

    static bool documentationManagerEnabled(const QHelpEngineCore &helpEngine);
    static void setDocumentationManagerEnabled(QHelpEngineCore &helpEngine,
                                               bool enabled);

    static const QByteArray applicationIcon(const QHelpEngineCore &helpEngine);
    static void setApplicationIcon(QHelpEngineCore &helpEngine,
                                   const QByteArray &icon);

    static const QString homePage(const QHelpEngineCore &helpEngine);
    static void setHomePage(QHelpEngineCore &helpEngine, const QString &page);

    static int startOption(const QHelpEngineCore &helpEngine);
    static void setStartOption(QHelpEngineCore &helpEngine, int option);

    // TODO: Encapsulate encoding from/to QByteArray here
    static const QByteArray aboutMenuTexts(const QHelpEngineCore &helpEngine);
    static void setAboutMenuTexts(QHelpEngineCore &helpEngine,
                                  const QByteArray &texts);

    static const QByteArray aboutIcon(const QHelpEngineCore &helpEngine);
    static void setAboutIcon(QHelpEngineCore &helpEngine,
                             const QByteArray &icon);

    // TODO: Encapsulate encoding from/to QByteArray here
    static const QByteArray aboutTexts(const QHelpEngineCore &helpEngine);
    static void setAboutTexts(QHelpEngineCore &helpEngine,
                              const QByteArray &texts);

    static const QByteArray aboutImages(const QHelpEngineCore &helpEngine);
    static void setAboutImages(QHelpEngineCore &helpEngine,
                               const QByteArray &images);

    static const QString defaultHomePage(const QHelpEngineCore &helpEngine);
    static void setDefaultHomePage(QHelpEngineCore &helpEngine,
                                   const QString &page);

    static bool docUpdatePending(const QHelpEngineCore &helpEngine);
    static void setDocUpdatePending(QHelpEngineCore &helpEngine, bool pending);

    // TODO: Put these GUI-specific functions in the help engine wrapper
    static const QByteArray bookmarks(const QHelpEngineCore &helpEngine);
    static void setBookmarks(QHelpEngineCore &helpEngine,
                             const QByteArray &bookmarks);

    static const QByteArray mainWindow(const QHelpEngineCore &helpEngine);
    static void setMainWindow(QHelpEngineCore &helpEngine,
                              const QByteArray &mainWindow);

    static const QByteArray mainWindowGeometry(const QHelpEngineCore &helpEngine);
    static void setMainWindowGeometry(QHelpEngineCore &helpEngine,
                                      const QByteArray &geometry);

    static bool usesAppFont(const QHelpEngineCore &helpEngine);
    static void setUseAppFont(QHelpEngineCore &helpEngine, bool useAppFont);

    static bool usesBrowserFont(const QHelpEngineCore &helpEngine);
    static void setUseBrowserFont(QHelpEngineCore &helpEngine,
                                  bool useBrowserFont);

    static const QFont appFont(const QHelpEngineCore &helpEngine);
    static void setAppFont(QHelpEngineCore &helpEngine, const QFont &font);

    static QFontDatabase::WritingSystem appWritingSystem(const QHelpEngineCore &helpEngine);
    static void setAppWritingSystem(QHelpEngineCore &helpEngine,
                                    QFontDatabase::WritingSystem system);

    static const QFont browserFont(const QHelpEngineCore &helpEngine);
    static void setBrowserFont(QHelpEngineCore &helpEngine, const QFont &font);

    static QFontDatabase::WritingSystem browserWritingSystem(const QHelpEngineCore &helpEngine);
    static void setBrowserWritingSystem(QHelpEngineCore &helpEngine,
                                        QFontDatabase::WritingSystem system);

    static bool unfilteredInserted(const QHelpEngineCore &helpEngine);
    static void setUnfilteredInserted(QHelpEngineCore &helpEngine);

    static const QStringList qtDocInfo(const QHelpEngineCore &helpEngine,
                                       const QString &component);
    static void setQtDocInfo(QHelpEngineCore &helpEngine,
                             const QString &component, const QStringList &doc);

    static bool searchWasAttached(const QHelpEngineCore &helpEngine);
    static void setSearchWasAttached(QHelpEngineCore &helpEngine, bool attached);

    static bool hasFontSettings(const QHelpEngineCore &helpEngine);
    static bool isNewer(const QHelpEngineCore &newer,
                        const QHelpEngineCore &older);
    static void copyConfiguration(const QHelpEngineCore &source,
                                  QHelpEngineCore &target);

    static const QString DefaultZoomFactor;
};

QT_END_NAMESPACE

#endif // COLLECTIONCONFIGURATION_H
