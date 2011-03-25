/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "private/qdeclarativeobserverservice_p.h"
#include "private/qdeclarativeobserverinterface_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>

#include <QtDeclarative/QDeclarativeView>

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QDeclarativeObserverService, serviceInstance)

QDeclarativeObserverService::QDeclarativeObserverService()
    : QDeclarativeDebugService(QLatin1String("QDeclarativeObserverMode"))
    , m_observer(0)
{
}

QDeclarativeObserverService *QDeclarativeObserverService::instance()
{
    return serviceInstance();
}

void QDeclarativeObserverService::addView(QDeclarativeView *view)
{
    m_views.append(view);
}

void QDeclarativeObserverService::removeView(QDeclarativeView *view)
{
    m_views.removeAll(view);
}

void QDeclarativeObserverService::sendMessage(const QByteArray &message)
{
    if (status() != Enabled)
        return;

    QDeclarativeDebugService::sendMessage(message);
}

void QDeclarativeObserverService::statusChanged(Status status)
{
    if (m_views.isEmpty())
        return;

    if (status == Enabled) {
        if (!m_observer)
            m_observer = loadObserverPlugin();

        if (!m_observer) {
            qWarning() << "Error while loading observer plugin";
            return;
        }

        m_observer->activate();
    } else {
        if (m_observer)
            m_observer->deactivate();
    }
}

void QDeclarativeObserverService::messageReceived(const QByteArray &message)
{
    emit gotMessage(message);
}

QDeclarativeObserverInterface *QDeclarativeObserverService::loadObserverPlugin()
{
    QStringList pluginCandidates;
    const QStringList paths = QCoreApplication::libraryPaths();
    foreach (const QString &libPath, paths) {
        const QDir dir(libPath + QLatin1String("/qmltooling"));
        if (dir.exists())
            foreach (const QString &pluginPath, dir.entryList(QDir::Files))
                pluginCandidates << dir.absoluteFilePath(pluginPath);
    }

    foreach (const QString &pluginPath, pluginCandidates) {
        QPluginLoader loader(pluginPath);
        if (!loader.load())
            continue;

        QDeclarativeObserverInterface *observer =
                qobject_cast<QDeclarativeObserverInterface*>(loader.instance());

        if (observer)
            return observer;
        loader.unload();
    }
    return 0;
}

QT_END_NAMESPACE
