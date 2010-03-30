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

#ifndef COLLECTIONCONFIGURATION_H
#define COLLECTIONCONFIGURATION_H

#include <QtCore/QByteArray>
#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

class QHelpEngineCore;

class CollectionConfiguration
{
public:
    static const QString windowTitle(const QHelpEngineCore &helpEngine);
    static void setWindowTitle(QHelpEngineCore &helpEngine,
                               const QString &windowTitle);

    static const QString cacheDir(const QHelpEngineCore &helpEngine);
    static bool cacheDirIsRelativeToCollection(const QHelpEngineCore &helpEngine);
    static void setCacheDir(QHelpEngineCore &helpEngine,
                            const QString &cacheDir, bool relativeToCollection);

    static uint creationTime(const QHelpEngineCore &helpEngine);
    static void setCreationTime(QHelpEngineCore &helpEngine, uint time);

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


    static bool documentationManagerEnabled(const QHelpEngineCore &helpEngine);
    static void setDocumentationManagerEnabled(QHelpEngineCore &helpEngine,
                                               bool enabled);

    static const QByteArray applicationIcon(const QHelpEngineCore &helpEngine);
    static void setApplicationIcon(QHelpEngineCore &helpEngine,
                                   const QByteArray &icon);

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

    // TODO: Don't allow last pages and zoom factors to be set in isolation
    //       Perhaps also fill up missing elements automatically or assert.
    static const QStringList lastShownPages(const QHelpEngineCore &helpEngine);
    static void setLastShownPages(QHelpEngineCore &helpEngine,
                                  const QStringList &lastShownPages);
    static const QStringList lastZoomFactors(const QHelpEngineCore &helpEngine);
    static void setLastZoomFactors(QHelpEngineCore &helPEngine,
                                   const QStringList &lastZoomFactors);

    static int lastTabPage(const QHelpEngineCore &helpEngine);
    static void setLastTabPage(QHelpEngineCore &helpEngine, int lastPage);

    static bool isNewer(const QHelpEngineCore &newer,
                        const QHelpEngineCore &older);
    static void copyConfiguration(const QHelpEngineCore &source,
                                  QHelpEngineCore &target);

    /*
     * Note that this only reflects register actions caused by the
     * "-register" command line switch, not GUI or remote control actions.
     */
    static const QDateTime lastRegisterTime(const QHelpEngineCore &helpEngine);
    static void updateLastRegisterTime(QHelpEngineCore &helpEngine);

    static const QString DefaultZoomFactor;
    static const QString ListSeparator;
};

QT_END_NAMESPACE

#endif // COLLECTIONCONFIGURATION_H
