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

#include "monitor.h"
#include "qicdengine.h"

#include <wlancond.h>
#include <libicd-network-wlan-dev.h>
#include <maemo_icd.h>
#include <iapconf.h>

#define IAP "/system/osso/connectivity/IAP"

static int iap_prefix_len;

/* Notify func that is called when IAP is added or deleted */
void notify_iap(GConfClient *, guint, GConfEntry *entry, gpointer user_data)
{
    const char *key = gconf_entry_get_key(entry);
    if (key && g_str_has_prefix(key, IAP)) {
    IapMonitor *ptr = (IapMonitor *)user_data;
    if (gconf_entry_get_value(entry)) {
        ptr->iapAdded(key, entry);
    } else {
        ptr->iapDeleted(key, entry);
    }
    }
}


void IapMonitor::setup(QIcdEngine *d_ptr)
{
    if (first_call) {
    d = d_ptr;
    iap_prefix_len = strlen(IAP);
    iap = new Maemo::IAPMonitor(notify_iap, (gpointer)this);
    first_call = false;
    }
}


void IapMonitor::cleanup()
{
    if (!first_call) {
    delete iap;
    timers.removeAll();
    first_call = true;
    }
}


void IapMonitor::iapAdded(const char *key, GConfEntry * /*entry*/)
{
    //qDebug("Notify called for added element: %s=%s",
    //       gconf_entry_get_key(entry), gconf_value_to_string(gconf_entry_get_value(entry)));

    /* We cannot know when the IAP is fully added to gconf, so a timer is
     * installed instead. When the timer expires we hope that IAP is added ok.
     */
    QString iap_id = QString(key + iap_prefix_len + 1).section('/',0,0);
    timers.add(iap_id, d);
}


void IapMonitor::iapDeleted(const char *key, GConfEntry * /*entry*/)
{
    //qDebug("Notify called for deleted element: %s", gconf_entry_get_key(entry));

    /* We are only interested in IAP deletions so we skip the config entries
     */
    if (strstr(key + iap_prefix_len + 1, "/")) {
    //qDebug("Deleting IAP config %s", key+iap_prefix_len);
    return;
    }

    QString iap_id = key + iap_prefix_len + 1;
    d->deleteConfiguration(iap_id);
}
