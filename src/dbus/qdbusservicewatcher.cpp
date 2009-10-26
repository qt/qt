/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

#include "qdbusservicewatcher.h"
#include "qdbusconnection.h"
#include "qdbus_symbols_p.h"

#include <QStringList>

#include <private/qobject_p.h>

Q_GLOBAL_STATIC_WITH_ARGS(QString, busService, (QLatin1String(DBUS_SERVICE_DBUS)))
Q_GLOBAL_STATIC_WITH_ARGS(QString, busPath, (QLatin1String(DBUS_PATH_DBUS)))
Q_GLOBAL_STATIC_WITH_ARGS(QString, busInterface, (QLatin1String(DBUS_INTERFACE_DBUS)))
Q_GLOBAL_STATIC_WITH_ARGS(QString, signalName, (QLatin1String("NameOwnerChanged")))

class QDBusServiceWatcherPrivate: public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDBusServiceWatcher)
public:
    QDBusServiceWatcherPrivate(const QDBusConnection &c, QDBusServiceWatcher::WatchMode wm)
        : connection(c), watchMode(wm)
    {
    }

    QStringList servicesWatched;
    QDBusConnection connection;
    QDBusServiceWatcher::WatchMode watchMode;

    void _q_serviceOwnerChanged(const QString &, const QString &, const QString &);
    void setConnection(const QStringList &services, const QDBusConnection &c, QDBusServiceWatcher::WatchMode watchMode);

    QStringList matchArgsForService(const QString &service);
    void addService(const QString &service);
    void removeService(const QString &service);
};

void QDBusServiceWatcherPrivate::_q_serviceOwnerChanged(const QString &service, const QString &oldOwner, const QString &newOwner)
{
    Q_Q(QDBusServiceWatcher);
    emit q->serviceOwnerChanged(service, oldOwner, newOwner);
    if (oldOwner.isEmpty())
        emit q->serviceRegistered(service);
    else if (newOwner.isEmpty())
        emit q->serviceUnregistered(service);
}

void QDBusServiceWatcherPrivate::setConnection(const QStringList &s, const QDBusConnection &c, QDBusServiceWatcher::WatchMode wm)
{
    if (connection.isConnected()) {
        // remove older rules
        foreach (const QString &s, servicesWatched)
            removeService(s);
    }

    connection = c;
    watchMode = wm;
    servicesWatched = s;

    if (connection.isConnected()) {
        // add new rules
        foreach (const QString &s, servicesWatched)
            addService(s);
    }
}

QStringList QDBusServiceWatcherPrivate::matchArgsForService(const QString &service)
{
    QStringList matchArgs;
    matchArgs << service;

    switch (watchMode) {
    case QDBusServiceWatcher::WatchForOwnerChange:
        break;

    case QDBusServiceWatcher::WatchForRegistration:
        matchArgs << QString::fromLatin1("", 0);
        break;

    case QDBusServiceWatcher::WatchForUnregistration:
        matchArgs << QString() << QString::fromLatin1("", 0);
        break;
    }
    return matchArgs;
}

void QDBusServiceWatcherPrivate::addService(const QString &service)
{
    QStringList matchArgs = matchArgsForService(service);
    connection.connect(*busService(), *busPath(), *busInterface(), *signalName(),
                       matchArgs, QString(), q_func(),
                       SLOT(_q_serviceOwnerChanged(QString,QString,QString)));
}

void QDBusServiceWatcherPrivate::removeService(const QString &service)
{
    QStringList matchArgs = matchArgsForService(service);
    connection.disconnect(*busService(), *busPath(), *busInterface(), *signalName(),
                          matchArgs, QString(), q_func(),
                          SLOT(_q_serviceOwnerChanged(QString,QString,QString)));
}

QDBusServiceWatcher::QDBusServiceWatcher(QObject *parent)
    : QObject(*new QDBusServiceWatcherPrivate(QDBusConnection(QString()), WatchForOwnerChange), parent)
{
}

QDBusServiceWatcher::QDBusServiceWatcher(const QString &service, const QDBusConnection &connection, WatchMode watchMode, QObject *parent)
    : QObject(*new QDBusServiceWatcherPrivate(connection, watchMode), parent)
{
    d_func()->setConnection(QStringList() << service, connection, watchMode);
}

QDBusServiceWatcher::~QDBusServiceWatcher()
{
}

QStringList QDBusServiceWatcher::watchedServices() const
{
    return d_func()->servicesWatched;
}

void QDBusServiceWatcher::setWatchedServices(const QStringList &services)
{
    Q_D(QDBusServiceWatcher);
    if (services == d->servicesWatched)
        return;
    d->setConnection(services, d->connection, d->watchMode);
}

void QDBusServiceWatcher::addWatchedService(const QString &newService)
{
    Q_D(QDBusServiceWatcher);
    if (d->servicesWatched.contains(newService))
        return;
    d->addService(newService);
    d->servicesWatched << newService;
}

bool QDBusServiceWatcher::removeWatchedService(const QString &service)
{
    Q_D(QDBusServiceWatcher);
    d->removeService(service);
    return d->servicesWatched.removeOne(service);
}

QDBusServiceWatcher::WatchMode QDBusServiceWatcher::watchMode() const
{
    return d_func()->watchMode;
}

void QDBusServiceWatcher::setWatchMode(WatchMode mode)
{
    Q_D(QDBusServiceWatcher);
    if (mode == d->watchMode)
        return;
    d->setConnection(d->servicesWatched, d->connection, mode);
}

QDBusConnection QDBusServiceWatcher::connection() const
{
    return d_func()->connection;
}

void QDBusServiceWatcher::setConnection(const QDBusConnection &connection)
{
    Q_D(QDBusServiceWatcher);
    if (connection.name() == d->connection.name())
        return;
    d->setConnection(d->servicesWatched, connection, d->watchMode);
}

#include "moc_qdbusservicewatcher.cpp"
