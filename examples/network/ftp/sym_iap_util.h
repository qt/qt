/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QSYM_IAP_UTIL_H
#define QSYM_IAP_UTIL_H

// Symbian
#include <es_sock.h>
#include <es_enum.h>
#include <commdbconnpref.h>

// OpenC
#include <sys/socket.h>
#include <net/if.h>

//Qt
#include <QSettings>
#include <QStringList>

_LIT(KIapNameSetting, "IAP\\Name");             // text - mandatory
_LIT(KIapDialogPref, "IAP\\DialogPref");        // TUnit32 - optional
_LIT(KIapService, "IAP\\IAPService");           // TUnit32 - mandatory
_LIT(KIapServiceType, "IAP\\IAPServiceType");   // text - mandatory
_LIT(KIapBearer, "IAP\\IAPBearer");             // TUint32 - optional
_LIT(KIapBearerType, "IAP\\IAPBearerType");     // text - optional
_LIT(KIapNetwork, "IAP\\IAPNetwork");           // TUint32 - optional

const QLatin1String qtOrganization("Trolltech");
const QLatin1String qtModule("QtNetwork");
const QLatin1String qtIapGroup("IAP");
const QLatin1String namesArray("Names");
const QLatin1String nameRecord("Name");


void clearIapNamesSettings(QSettings &settings) {
    settings.beginGroup(qtModule);
        settings.beginGroup(qtIapGroup);
           settings.remove(namesArray);
        settings.endGroup();
    settings.endGroup();
}

void writeIapNamesSettings(QSettings &settings, const QStringList& iapNames) {
    clearIapNamesSettings(settings);
    settings.beginGroup(qtModule);
        settings.beginGroup(qtIapGroup);
            settings.beginWriteArray(namesArray);
            for (int index = 0; index < iapNames.size(); ++index) {
                settings.setArrayIndex(index);
                settings.setValue(nameRecord, iapNames.at(index));
            }
            settings.endArray();
        settings.endGroup();
    settings.endGroup();
}

void readIapNamesSettings(QSettings &settings, QStringList& iapNames) {
    settings.beginGroup(qtModule);
        settings.beginGroup(qtIapGroup);
            int last = settings.beginReadArray(namesArray);
            for (int index = 0; index < last; ++index) {
                settings.setArrayIndex(index);
                iapNames.append(settings.value(nameRecord).toString());
            }
            settings.endArray();
        settings.endGroup();
    settings.endGroup();
}

QString qt_TDesC2QStringL(const TDesC& aDescriptor) 
{
#ifdef QT_NO_UNICODE
    return QString::fromLocal8Bit(aDescriptor.Ptr(), aDescriptor.Length());
#else
    return QString::fromUtf16(aDescriptor.Ptr(), aDescriptor.Length());
#endif
}

static void qt_SetDefaultIapL() 
{
    // settings @ /c/data/.config/Trolltech.com
    QSettings settings(QSettings::UserScope, qtOrganization);
    // populate iap name list
    QStringList iapNames;
    readIapNamesSettings(settings, iapNames);

    TUint count;
    TBool activeLanConnectionFound = EFalse;

    RSocketServ serv;
    CleanupClosePushL(serv);

    RConnection conn;
    CleanupClosePushL(conn);

    User::LeaveIfError(serv.Connect());
    User::LeaveIfError(conn.Open(serv));

    TConnectionInfoBuf connInfo;

    TBuf8<256> iapName;
    TBuf8<256> iapServiceType;

    if (conn.EnumerateConnections(count) == KErrNone) {
        if(count > 0) {
            for (TUint i = 1; i <= count; i++) {
                // we will loop all active connections
                // if we find one of the LAN type - will use it silently
                // if not prompt thet user as other types
                /// are more likely to be charged :(
                if (conn.GetConnectionInfo(i, connInfo) == KErrNone) {
                    if (conn.Attach(connInfo, RConnection::EAttachTypeNormal) == KErrNone) {
                        conn.GetDesSetting(TPtrC(KIapNameSetting), iapName);
                        conn.GetDesSetting(TPtrC(KIapServiceType), iapServiceType);
                        if(iapServiceType.Find(_L8("LANService")) != KErrNotFound) {
                            activeLanConnectionFound = ETrue;
                            break;
                        }
                        // close & reopen from previous attach
                        conn.Close();
                        User::LeaveIfError(conn.Open(serv));
                    }
                }
            }
        }
    }

    if (!activeLanConnectionFound) {
        /*
         * no active LAN connections yet
         * offer IAP dialog to user
         */
        TCommDbConnPref prefs;
        conn.Close(); // might be opened after attach
        User::LeaveIfError(conn.Open(serv));
        prefs.SetDialogPreference( ECommDbDialogPrefPrompt );
        User::LeaveIfError(conn.Start(prefs));
        conn.GetDesSetting(TPtrC(KIapNameSetting), iapName);
        conn.GetDesSetting(TPtrC(KIapServiceType), iapServiceType);
    }

    // for some reason makes
    // setting default more stable
    conn.Stop();

    // just in case ...
    iapName.ZeroTerminate();
    iapServiceType.ZeroTerminate();

    QString iapNameValue((char*)iapName.Ptr());
    QString iapServiceTypeValue((char*)iapServiceType.Ptr());

    // save IAP name for latere user
    // but only if (W)LAN service type
    // and is not already in QSettings
    if(iapServiceTypeValue.contains(QString("LANService"))) {
        if(iapNames.contains(iapNameValue) && iapNames.first() == iapNameValue) {
            // no need to update
        } else {
            // new selection alway on top
            iapNames.removeAll(iapNameValue);
            iapNames.prepend(iapNameValue);
            writeIapNamesSettings(settings, iapNames);
        }
    }

    struct ifreq ifReq;
    strcpy(ifReq.ifr_name, (char*)iapName.Ptr());

    User::LeaveIfError(setdefaultif(&ifReq));

    CleanupStack::PopAndDestroy(&conn);
    CleanupStack::PopAndDestroy(&serv);
}

static int qt_SetDefaultIap()
{
    TRAPD(err, qt_SetDefaultIapL());
    return err;
}

#endif // QSYM_IAP_UTIL_H
