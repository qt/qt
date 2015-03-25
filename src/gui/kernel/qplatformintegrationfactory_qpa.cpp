/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qplatformintegrationfactory_qpa_p.h"
#include <QPlatformIntegrationPlugin>
#include "private/qfactoryloader_p.h"
#include "qmutex.h"

#include "qapplication.h"
#include "qdebug.h"

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
    (QPlatformIntegrationFactoryInterface_iid, QLatin1String("/platforms"), Qt::CaseInsensitive))
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, directLoader,
                          (QPlatformIntegrationFactoryInterface_iid, QLatin1String(""), Qt::CaseInsensitive))
#endif

QPlatformIntegration *QPlatformIntegrationFactory::create(const QString& key, const QString &platformPluginPath)
{
    QPlatformIntegration *ret = 0;
    QStringList paramList = key.split(QLatin1Char(':'));
    QString platform = paramList.takeFirst().toLower();

#if !defined(QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
    // Try loading the plugin from platformPluginPath first:
    if (!platformPluginPath.isEmpty()) {
        QCoreApplication::addLibraryPath(platformPluginPath);
        if (QPlatformIntegrationFactoryInterface *factory =
            qobject_cast<QPlatformIntegrationFactoryInterface*>(directLoader()->instance(platform)))
            ret = factory->create(key, paramList);

        if (ret)
            return ret;
    }
    if (QPlatformIntegrationFactoryInterface *factory = qobject_cast<QPlatformIntegrationFactoryInterface*>(loader()->instance(platform)))
        ret = factory->create(platform, paramList);
#endif

    return ret;
}

/*!
    Returns the list of valid keys, i.e. the keys this factory can
    create styles for.

    \sa create()
*/
QStringList QPlatformIntegrationFactory::keys(const QString &platformPluginPath)
{
#if !defined(QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
    QStringList list;

    if (!platformPluginPath.isEmpty()) {
        QCoreApplication::addLibraryPath(platformPluginPath);
        foreach (const QString &key, directLoader()->keys()) {
            list += key + QString(QLatin1String(" (from %1)")).arg(platformPluginPath);
        }
    }

    list += loader()->keys();
#else
    QStringList list;
#endif
    return list;
}

QT_END_NAMESPACE

