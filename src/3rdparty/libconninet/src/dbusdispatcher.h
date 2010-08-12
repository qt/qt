/* * This file is part of conn-dui-settings-inet *
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: Aapo Makela <aapo.makela@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef DBUSDISPATCHER_H
#define DBUSDISPATCHER_H

#include <QObject>
#include <QVariant>

namespace Maemo {

class DBusDispatcherPrivate;
class DBusDispatcher : public QObject
{
    Q_OBJECT

public:
    DBusDispatcher(const QString& service,
                   const QString& path,
                   const QString& interface,
                   QObject *parent = 0);
    DBusDispatcher(const QString& service,
                   const QString& path,
                   const QString& interface,
                   const QString& signalPath,
                   QObject *parent = 0);
    ~DBusDispatcher();

    QList<QVariant> call(const QString& method, 
                         const QVariant& arg1 = QVariant(),
                         const QVariant& arg2 = QVariant(),
                         const QVariant& arg3 = QVariant(),
                         const QVariant& arg4 = QVariant(),
                         const QVariant& arg5 = QVariant(),
                         const QVariant& arg6 = QVariant(),
                         const QVariant& arg7 = QVariant(),
                         const QVariant& arg8 = QVariant());
    bool callAsynchronous(const QString& method, 
                          const QVariant& arg1 = QVariant(),
                          const QVariant& arg2 = QVariant(),
                          const QVariant& arg3 = QVariant(),
                          const QVariant& arg4 = QVariant(),
                          const QVariant& arg5 = QVariant(),
                          const QVariant& arg6 = QVariant(),
                          const QVariant& arg7 = QVariant(),
                          const QVariant& arg8 = QVariant());
    void emitSignalReceived(const QString& interface, 
                            const QString& signal,
                            const QList<QVariant>& args);
    void emitCallReply(const QString& method,
                       const QList<QVariant>& args,
                       const QString& error = "");
    void synchronousDispatch(int timeout_ms);

Q_SIGNALS:
    void signalReceived(const QString& interface, 
                        const QString& signal,
                        const QList<QVariant>& args);
    void callReply(const QString& method,
                   const QList<QVariant>& args,
                   const QString& error);

protected:
    void setupDBus();

private:
    DBusDispatcherPrivate *d_ptr;
};

}  // Maemo namespace

#endif
