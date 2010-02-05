/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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
#include "qbearerplugin.h"

#include <QtCore/private/qfactoryloader_p.h>

#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
                          (QBearerEngineFactoryInterface_iid, QLatin1String("/bearer")))

QNetworkConfigurationManagerPrivate::~QNetworkConfigurationManagerPrivate()
{
    while (!sessionEngines.isEmpty())
        delete sessionEngines.takeFirst();
}

void QNetworkConfigurationManagerPrivate::configurationAdded(QNetworkConfigurationPrivatePointer ptr)
{
    if (!firstUpdate) {
        QNetworkConfiguration item;
        item.d = ptr;
        emit configurationAdded(item);
    }

    if (ptr->state == QNetworkConfiguration::Active) {
        onlineConfigurations.insert(ptr);
        if (!firstUpdate && onlineConfigurations.count() == 1)
            emit onlineStateChanged(true);
    }
}

void QNetworkConfigurationManagerPrivate::configurationRemoved(QNetworkConfigurationPrivatePointer ptr)
{
    ptr->isValid = false;

    if (!firstUpdate) {
        QNetworkConfiguration item;
        item.d = ptr;
        emit configurationRemoved(item);
    }

    onlineConfigurations.remove(ptr);
    if (!firstUpdate && onlineConfigurations.isEmpty())
        emit onlineStateChanged(false);
}

void QNetworkConfigurationManagerPrivate::configurationChanged(QNetworkConfigurationPrivatePointer ptr)
{
    if (!firstUpdate) {
        QNetworkConfiguration item;
        item.d = ptr;
        emit configurationChanged(item);
    }

    bool previous = !onlineConfigurations.isEmpty();

    if (ptr->state == QNetworkConfiguration::Active)
        onlineConfigurations.insert(ptr);
    else
        onlineConfigurations.remove(ptr);

    bool online = !onlineConfigurations.isEmpty();

    if (!firstUpdate && online != previous)
        emit onlineStateChanged(online);
}

void QNetworkConfigurationManagerPrivate::updateInternetServiceConfiguration()
{
#if 0
    if (!generic->snapConfigurations.contains(QLatin1String("Internet Service Network"))) {
        QNetworkConfigurationPrivate *serviceNetwork = new QNetworkConfigurationPrivate;
        serviceNetwork->name = tr("Internet");
        serviceNetwork->isValid = true;
        serviceNetwork->id = QLatin1String("Internet Service Network");
        serviceNetwork->state = QNetworkConfiguration::Defined;
        serviceNetwork->type = QNetworkConfiguration::ServiceNetwork;

        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(serviceNetwork);

        generic->snapConfigurations.insert(serviceNetwork->id, ptr);

        if (!firstUpdate) {
            QNetworkConfiguration item;
            item.d = ptr;
            emit configurationAdded(item);
        }
    }

    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr =
        generic->snapConfigurations.value(QLatin1String("Internet Service Network"));

    QList<QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > serviceNetworkMembers;

    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> >::const_iterator i =
        generic->accessPointConfigurations.constBegin();

    QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Defined;
    while (i != generic->accessPointConfigurations.constEnd()) {
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
#endif
}

void QNetworkConfigurationManagerPrivate::updateConfigurations()
{
    if (firstUpdate) {
        updating = false;

        QFactoryLoader *l = loader();

        foreach (const QString &key, l->keys()) {
            QBearerEnginePlugin *plugin = qobject_cast<QBearerEnginePlugin *>(l->instance(key));
            if (plugin) {
                QBearerEngine *engine = plugin->create(key);
                if (!engine)
                    continue;

                sessionEngines.append(engine);
                connect(engine, SIGNAL(updateCompleted()),
                        this, SLOT(updateConfigurations()));
                connect(engine, SIGNAL(configurationAdded(QNetworkConfigurationPrivatePointer)),
                        this, SLOT(configurationAdded(QNetworkConfigurationPrivatePointer)));
                connect(engine, SIGNAL(configurationRemoved(QNetworkConfigurationPrivatePointer)),
                        this, SLOT(configurationRemoved(QNetworkConfigurationPrivatePointer)));
                connect(engine, SIGNAL(configurationChanged(QNetworkConfigurationPrivatePointer)),
                        this, SLOT(configurationChanged(QNetworkConfigurationPrivatePointer)));

                capFlags |= engine->capabilities();
            }
        }
    }

    QBearerEngine *engine = qobject_cast<QBearerEngine *>(sender());
    if (!updatingEngines.isEmpty() && engine) {
        int index = sessionEngines.indexOf(engine);
        if (index >= 0)
            updatingEngines.remove(index);
    }

    if (updating && updatingEngines.isEmpty()) {
        updating = false;
        emit configurationUpdateComplete();
    }

    if (firstUpdate)
        firstUpdate = false;
}

/*!
    Returns the default configuration of the first plugin, if one exists; otherwise returns an
    invalid configuration.

    \internal
*/
QNetworkConfiguration QNetworkConfigurationManagerPrivate::defaultConfiguration()
{
    foreach (QBearerEngine *engine, sessionEngines) {
        QNetworkConfigurationPrivatePointer ptr = engine->defaultConfiguration();

        if (ptr) {
            QNetworkConfiguration config;
            config.d = ptr;
            return config;
        }
    }

    return QNetworkConfiguration();
}

void QNetworkConfigurationManagerPrivate::performAsyncConfigurationUpdate()
{
    updating = true;

    for (int i = 0; i < sessionEngines.count(); ++i) {
        updatingEngines.insert(i);
        sessionEngines.at(i)->requestUpdate();
    }
}

QT_END_NAMESPACE
