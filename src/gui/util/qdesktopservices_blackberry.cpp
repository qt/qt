/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT_NO_DESKTOPSERVICES

#include <qurl.h>
#include <bps/navigator.h>
#include <qdir.h>

QT_BEGIN_NAMESPACE

static bool navigatorInvoke(const QUrl &url)
{
    if (!url.isValid() || url.isRelative())
        return false;

    int ret = navigator_invoke(url.toString().toUtf8(), 0);

    return (ret == BPS_SUCCESS);
}

static bool launchWebBrowser(const QUrl &url)
{
    return navigatorInvoke(url);
}

static bool openDocument(const QUrl &file)
{
    return navigatorInvoke(file);
}

QString QDesktopServices::storageLocation(StandardLocation type)
{
    if (type == QDesktopServices::TempLocation)
        return QDir::tempPath();
    if (type == QDesktopServices::CacheLocation)
        return QDir::homePath() + QLatin1String("/Cache");

    QString path;

    const int index = QDir::homePath().lastIndexOf("/data");

    const QString sharedRoot = QDir::homePath().replace(index,
            QDir::homePath().length() - index, "/shared");

    switch (type) {
    case DataLocation:
    case DesktopLocation:
    case HomeLocation:
        path = QDir::homePath();
        break;
    case DocumentsLocation:
        path = sharedRoot + QLatin1String("/documents");
       break;
    case PicturesLocation:
        path = sharedRoot + QLatin1String("/photos");
        break;
    case MusicLocation:
        path = sharedRoot + QLatin1String("/music");
        break;
    case MoviesLocation:
        path = sharedRoot + QLatin1String("/videos");
        break;
    default:
        break;
    }

    return path;
}

QString QDesktopServices::displayName(StandardLocation type)
{
    return storageLocation(type);
}

QT_END_NAMESPACE

#endif // QT_NO_DESKTOPSERVICES
