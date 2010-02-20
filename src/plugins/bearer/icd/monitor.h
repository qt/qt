/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef MONITOR_H
#define MONITOR_H

#include <QtCore/qhash.h>
#include <QtCore/qtimer.h>

#include <gconf/gconf.h>
#include <gconf/gconf-client.h>

#include <iapmonitor.h>

class QIcdEngine;

/* The IapAddTimer is a helper class that makes sure we update
 * the configuration only after all gconf additions to certain
 * iap are finished (after a certain timeout)
 */
class _IapAddTimer : public QObject
{
    Q_OBJECT

public:
    _IapAddTimer() {}
    ~_IapAddTimer()
    {
    if (timer.isActive()) {
        QObject::disconnect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
        timer.stop();
    }
    }

    void add(QString& iap_id, QIcdEngine *d);

    QString iap_id;
    QTimer timer;
    QIcdEngine *d;

public Q_SLOTS:
    void timeout();
};

class IapAddTimer {
    QHash<QString, _IapAddTimer* > timers;

public:
    IapAddTimer() {}
    ~IapAddTimer() {}

    void add(QString& iap_id, QIcdEngine *d);
    void del(QString& iap_id);
    void removeAll();
};

class IapMonitor
{
public:
    IapMonitor() : first_call(true) { }
    friend void notify_iap(GConfClient *, guint,
            GConfEntry *entry, gpointer user_data);

    void setup(QIcdEngine *d);
    void cleanup();

private:
    bool first_call;

    void iapAdded(const char *key, GConfEntry *entry);
    void iapDeleted(const char *key, GConfEntry *entry);

    Maemo::IAPMonitor *iap;
    QIcdEngine *d;
    IapAddTimer timers;
};

#endif // MONITOR_H
