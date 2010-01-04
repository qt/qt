/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include "qnetworkconfigmanager_p.h"
#include "qgenericengine_p.h"

#ifdef Q_OS_WIN
#include "qnlaengine_win_p.h"
#endif
#ifdef Q_OS_WIN32
#include "qnativewifiengine_win_p.h"
#endif
#if defined(BACKEND_NM)
#include "qnmwifiengine_unix_p.h"
#endif
#ifdef Q_OS_DARWIN
#include "qcorewlanengine_mac_p.h"
#endif

#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <QtCore/qstringlist.h>

QTM_BEGIN_NAMESPACE

void QNetworkConfigurationManagerPrivate::registerPlatformCapabilities()
{
    capFlags = QNetworkConfigurationManager::ForcedRoaming;
}

void QNetworkConfigurationManagerPrivate::configurationAdded(QNetworkConfigurationPrivate *cpPriv, QNetworkSessionEngine *engine)
{
    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(new QNetworkConfigurationPrivate);

    ptr.data()->isValid = cpPriv->isValid;
    ptr.data()->name = cpPriv->name;
    ptr.data()->id = cpPriv->id;
    ptr.data()->state = cpPriv->state;
    ptr.data()->type = cpPriv->type;
    ptr.data()->roamingSupported = cpPriv->roamingSupported;
    ptr.data()->purpose = cpPriv->purpose;
    ptr.data()->internet = cpPriv->internet;
    ptr.data()->bearer = cpPriv->bearer;

    accessPointConfigurations.insert(cpPriv->id, ptr);
    configurationEngine.insert(cpPriv->id, engine);

    if (!firstUpdate) {
        QNetworkConfiguration item;
        item.d = ptr;
        emit configurationAdded(item);
    }

    if (ptr.data()->state == QNetworkConfiguration::Active) {
        ++onlineConfigurations;
        if (!firstUpdate && onlineConfigurations == 1)
            emit onlineStateChanged(true);
    }
}

void QNetworkConfigurationManagerPrivate::configurationRemoved(const QString &id)
{
    if (!accessPointConfigurations.contains(id))
        return;

    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr =
        accessPointConfigurations.take(id);

    configurationEngine.remove(id);

    ptr.data()->isValid = false;

    if (!firstUpdate) {
        QNetworkConfiguration item;
        item.d = ptr;
        emit configurationRemoved(item);
    }

    if (ptr.data()->state == QNetworkConfiguration::Active) {
        --onlineConfigurations;
        if (!firstUpdate && onlineConfigurations == 0)
            emit onlineStateChanged(false);
    }
}

void QNetworkConfigurationManagerPrivate::configurationChanged(QNetworkConfigurationPrivate *cpPriv)
{
    if (!accessPointConfigurations.contains(cpPriv->id))
        return;

    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr =
        accessPointConfigurations.value(cpPriv->id);

    if (ptr.data()->isValid != cpPriv->isValid ||
        ptr.data()->name != cpPriv->name ||
        ptr.data()->id != cpPriv->id ||
        ptr.data()->state != cpPriv->state ||
        ptr.data()->type != cpPriv->type ||
        ptr.data()->roamingSupported != cpPriv->roamingSupported ||
        ptr.data()->purpose != cpPriv->purpose ||
        ptr.data()->bearer != cpPriv->bearer ||
        ptr.data()->internet != cpPriv->internet) {

        const QNetworkConfiguration::StateFlags oldState = ptr.data()->state;

        ptr.data()->isValid = cpPriv->isValid;
        ptr.data()->name = cpPriv->name;
        ptr.data()->id = cpPriv->id;
        ptr.data()->state = cpPriv->state;
        ptr.data()->type = cpPriv->type;
        ptr.data()->roamingSupported = cpPriv->roamingSupported;
        ptr.data()->purpose = cpPriv->purpose;
        ptr.data()->internet = cpPriv->internet;
        ptr.data()->bearer = cpPriv->bearer;

        if (!firstUpdate) {
            QNetworkConfiguration item;
            item.d = ptr;
            emit configurationChanged(item);
        }

        if (ptr.data()->state == QNetworkConfiguration::Active && oldState != ptr.data()->state) {
            // configuration went online
            ++onlineConfigurations;
            if (!firstUpdate && onlineConfigurations == 1)
                emit onlineStateChanged(true);
        } else if (ptr.data()->state != QNetworkConfiguration::Active && oldState == QNetworkConfiguration::Active) {
            // configuration went offline
            --onlineConfigurations;
            if (!firstUpdate && onlineConfigurations == 0)
                emit onlineStateChanged(false);
        }
    }
}

void QNetworkConfigurationManagerPrivate::updateInternetServiceConfiguration()
{
    if (!snapConfigurations.contains(QLatin1String("Internet Service Network"))) {
        QNetworkConfigurationPrivate *serviceNetwork = new QNetworkConfigurationPrivate;
        serviceNetwork->name = tr("Internet");
        serviceNetwork->isValid = true;
        serviceNetwork->id = QLatin1String("Internet Service Network");
        serviceNetwork->state = QNetworkConfiguration::Defined;
        serviceNetwork->type = QNetworkConfiguration::ServiceNetwork;

        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(serviceNetwork);

        snapConfigurations.insert(serviceNetwork->id, ptr);

        if (!firstUpdate) {
            QNetworkConfiguration item;
            item.d = ptr;
            emit configurationAdded(item);
        }
    }

    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr =
        snapConfigurations.value(QLatin1String("Internet Service Network"));

    QList<QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > serviceNetworkMembers;

    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> >::const_iterator i =
        accessPointConfigurations.constBegin();

    QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Defined;
    while (i != accessPointConfigurations.constEnd()) {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> child = i.value();

        if (child.data()->internet && ((child.data()->state & QNetworkConfiguration::Defined)
                    == QNetworkConfiguration::Defined)) {
            serviceNetworkMembers.append(child);

            state |= child.data()->state;
        }

        ++i;
    }


    if (ptr.data()->state != state || ptr.data()->serviceNetworkMembers != serviceNetworkMembers) {
        ptr.data()->state = state;
        ptr.data()->serviceNetworkMembers = serviceNetworkMembers;

        QNetworkConfiguration item;
        item.d = ptr;
        emit configurationChanged(item);
    }
}

void QNetworkConfigurationManagerPrivate::updateConfigurations()
{
    if (firstUpdate) {
        updateState = NotUpdating;
        onlineConfigurations = 0;

#if defined (Q_OS_DARWIN)
        coreWifi = QCoreWlanEngine::instance();
        if (coreWifi) {
            connect(coreWifi, SIGNAL(configurationsChanged()),
                    this, SLOT(updateConfigurations()));
        }
#else
#if defined(BACKEND_NM)
        nmWifi = QNmWifiEngine::instance();
        if (nmWifi) {
            connect(nmWifi, SIGNAL(configurationsChanged()),
                    this, SLOT(updateConfigurations()));
        } else {
#endif
            generic = QGenericEngine::instance();
            if (generic) {
                connect(generic, SIGNAL(configurationsChanged()),
                        this, SLOT(updateConfigurations()));
            }
#if defined(BACKEND_NM)
        }
#endif
#endif

#ifdef Q_OS_WIN
            nla = QNlaEngine::instance();
            if (nla) {
                connect(nla, SIGNAL(configurationsChanged()),
                        this, SLOT(updateConfigurations()));
            }
#endif

#ifdef Q_OS_WIN32
            nativeWifi = QNativeWifiEngine::instance();
            if (nativeWifi) {
                connect(nativeWifi, SIGNAL(configurationsChanged()),
                        this, SLOT(updateConfigurations()));

                capFlags |= QNetworkConfigurationManager::CanStartAndStopInterfaces;
            }
#endif
    }

    QNetworkSessionEngine *engine = qobject_cast<QNetworkSessionEngine *>(sender());
    if (updateState & Updating && engine) {
#if defined (Q_OS_DARWIN)
        if (engine == coreWifi)
            updateState &= ~CoreWifiUpdating;
#else
#if defined(BACKEND_NM)
        if (engine == nmWifi)
            updateState &= ~NmUpdating;
        else if (engine == generic)
            updateState &= ~GenericUpdating;
#else
        if (engine == generic)
            updateState &= ~GenericUpdating;
#endif
#endif

#ifdef Q_OS_WIN
        else if (engine == nla)
            updateState &= ~NlaUpdating;
#ifdef Q_OS_WIN32
        else if (engine == nativeWifi)
            updateState &= ~NativeWifiUpdating;
#endif
#endif
    }
    QList<QNetworkSessionEngine *> engines;
    if (firstUpdate) {
#if defined (Q_OS_DARWIN)
        if (coreWifi)
            engines << coreWifi;
#else
#if defined(BACKEND_NM)
        if (nmWifi)
            engines << nmWifi;
        else if (generic)
            engines << generic;
#else
        if (generic)
            engines << generic;
#endif
#endif

#ifdef Q_OS_WIN
        if (nla)
            engines << nla;
#ifdef Q_OS_WIN32
        if (nativeWifi)
            engines << nativeWifi;
#endif
#endif
    } else if (engine) {
        engines << engine;
    }

    while (!engines.isEmpty()) {
        engine = engines.takeFirst();

        bool ok;
        QList<QNetworkConfigurationPrivate *> foundConfigurations = engine->getConfigurations(&ok);

        // Find removed configurations.
        QList<QString> removedIdentifiers = configurationEngine.keys();
        for (int i = 0; i < foundConfigurations.count(); ++i)
            removedIdentifiers.removeOne(foundConfigurations.at(i)->id);

        // Update or add configurations.
        while (!foundConfigurations.isEmpty()) {
            QNetworkConfigurationPrivate *cpPriv = foundConfigurations.takeFirst();

            if (accessPointConfigurations.contains(cpPriv->id))
                configurationChanged(cpPriv);
            else
                configurationAdded(cpPriv, engine);

            delete cpPriv;
        }

        // Remove configurations.
        while (!removedIdentifiers.isEmpty()) {
            const QString id = removedIdentifiers.takeFirst();

            if (configurationEngine.value(id) == engine)
                configurationRemoved(id);
        }
    }

    updateInternetServiceConfiguration();

    if (updateState == Updating) {
        updateState = NotUpdating;
        emit configurationUpdateComplete();
    }

    if (firstUpdate)
        firstUpdate = false;
}

/*!
    Returns the first active configuration found, if one exists; otherwise returns the first
    discovered configuration found, if one exists; otherwise returns an empty configuration.

    \internal
*/
QNetworkConfiguration QNetworkConfigurationManagerPrivate::defaultConfiguration()
{
    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> firstActive;
    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> firstDiscovered;

    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> >::const_iterator i =
        accessPointConfigurations.constBegin();
    while (i != accessPointConfigurations.constEnd()) {
        QNetworkConfigurationPrivate *priv = i.value().data();

        if (!firstActive && priv->isValid &&
            (priv->state & QNetworkConfiguration::Active) == QNetworkConfiguration::Active)
            firstActive = i.value();
        if (!firstDiscovered && priv->isValid &&
            (priv->state & QNetworkConfiguration::Discovered) == QNetworkConfiguration::Discovered)
            firstDiscovered = i.value();

        ++i;
    }

    QNetworkConfiguration item;

    if (firstActive)
        item.d = firstActive;
    else if (firstDiscovered)
        item.d = firstDiscovered;

    return item;
}

void QNetworkConfigurationManagerPrivate::performAsyncConfigurationUpdate()
{
    updateState = Updating;
#if defined (Q_OS_DARWIN)
    if (coreWifi) {
        updateState |= CoreWifiUpdating;
        coreWifi->requestUpdate();
    }
#else
#if defined(BACKEND_NM)
    if (nmWifi) {
        updateState |= NmUpdating;
        nmWifi->requestUpdate();
    } else if (generic) {
        updateState |= GenericUpdating;
        generic->requestUpdate();
    }
#else
    if (generic) {
        updateState |= GenericUpdating;
        generic->requestUpdate();
    }
#endif
#endif
#ifdef Q_OS_WIN
    if (nla) {
        updateState |= NlaUpdating;
        nla->requestUpdate();
    }
#endif

#ifdef Q_OS_WIN32
    if (nativeWifi) {
        updateState |= NativeWifiUpdating;
        nativeWifi->requestUpdate();
    }
#endif
}

#include "moc_qnetworkconfigmanager_p.cpp"

QTM_END_NAMESPACE

