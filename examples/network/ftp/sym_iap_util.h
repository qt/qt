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

const QLatin1String qtOrganizationTag("Trolltech");
const QLatin1String qtNetworkModuleTag("QtNetwork");
const QLatin1String iapGroupTag("IAP");
const QLatin1String iapNamesArrayTag("Names");
const QLatin1String iapNameItemTag("Name");

void clearIapNamesSettings(QSettings &settings) {
    settings.beginGroup(qtNetworkModuleTag);
        settings.beginGroup(iapGroupTag);
           settings.remove(iapNamesArrayTag);
        settings.endGroup();
    settings.endGroup();
}

void writeIapNamesSettings(QSettings &settings, const QStringList& iapNames) {
    clearIapNamesSettings(settings);
    settings.beginGroup(qtNetworkModuleTag);
        settings.beginGroup(iapGroupTag);
            settings.beginWriteArray(iapNamesArrayTag);
            for (int index = 0; index < iapNames.size(); ++index) {
                settings.setArrayIndex(index);
                settings.setValue(iapNameItemTag, iapNames.at(index));
            }
            settings.endArray();
        settings.endGroup();
    settings.endGroup();
}

void readIapNamesSettings(QSettings &settings, QStringList& iapNames) {
    settings.beginGroup(qtNetworkModuleTag);
        settings.beginGroup(iapGroupTag);
            int last = settings.beginReadArray(iapNamesArrayTag);
            for (int index = 0; index < last; ++index) {
                settings.setArrayIndex(index);
                iapNames.append(settings.value(iapNameItemTag).toString());
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

static bool qt_SetDefaultIapName(const QString &iapName, int &error) {
    struct ifreq ifReq;
    // clear structure
    memset(&ifReq, 0, sizeof(struct ifreq));
    // set IAP name value
    // make sure it is in UTF8
    strcpy(ifReq.ifr_name, iapName.toUtf8().data());

    if(setdefaultif(&ifReq) == 0) {
        // OK
        error = 0;
        return true;
    } else {
        error = errno;
        return false;
    }

}
static bool qt_SetDefaultSnapId(const int snapId, int &error) {
    struct ifreq ifReq;
    // clear structure
    memset(&ifReq, 0, sizeof(struct ifreq));
    // set SNAP ID value
    ifReq.ifr_ifru.snap_id = snapId;

    if(setdefaultif(&ifReq) == 0) {
        // OK
        error = 0;
        return true;
    } else {
        error = errno;
        return false;
    }

}

static void qt_SaveIapName(QSettings& settings, QStringList& iapNames, QString& iapNameValue) {
    if(iapNames.contains(iapNameValue) && iapNames.first() == iapNameValue) {
        // no need to update
    } else {
        if(iapNameValue != QString("Easy WLAN")) {
            // new selection alway on top
            iapNames.removeAll(iapNameValue);
            iapNames.prepend(iapNameValue);
            writeIapNamesSettings(settings, iapNames);
        } else {
            // Unbeliveable ... if IAP dodn't exist before
            // no matter what you choose from IAP selection list
            // you will get "Easy WLAN" as IAP name value

            // somehow commsdb is not in sync
        }
    }
}

static QString qt_OfferIapDialog() {
    TBuf8<256> iapName;

    RSocketServ socketServ;
    CleanupClosePushL(socketServ);

    RConnection connection;
    CleanupClosePushL(connection);

    socketServ.Connect();
    connection.Open(socketServ);
    connection.Start();

    connection.GetDesSetting(TPtrC(KIapNameSetting), iapName);

    //connection.Stop();

    iapName.ZeroTerminate();
    QString strIapName((char*)iapName.Ptr());

    int error = 0;
    if(!qt_SetDefaultIapName(strIapName, error)) {
        //printf("failed setdefaultif @ %i with %s and errno = %d \n", __LINE__, strIapName.toUtf8().data(), error);
        strIapName = QString("");
    }

    CleanupStack::PopAndDestroy(&connection);
    CleanupStack::PopAndDestroy(&socketServ);

    return strIapName;
}

static QString qt_CheckForActiveConnection() {
    TUint count;

    RSocketServ serv;
    CleanupClosePushL(serv);

    RConnection conn;
    CleanupClosePushL(conn);

    serv.Connect();
    conn.Open(serv);

    TConnectionInfoBuf connInfo;

    TBuf8<256> iapName;
    TBuf8<256> iapServiceType;

    QString strIapName;

    if (conn.EnumerateConnections(count) == KErrNone) {
        if(count > 0) {
            for (TUint i = 1; i <= count; i++) {
                if (conn.GetConnectionInfo(i, connInfo) == KErrNone) {
                    RConnection tempConn;
                    CleanupClosePushL(tempConn);
                    tempConn.Open(serv);
                    if (tempConn.Attach(connInfo, RConnection::EAttachTypeNormal) == KErrNone) {
                       tempConn.GetDesSetting(TPtrC(KIapNameSetting), iapName);
                       tempConn.GetDesSetting(TPtrC(KIapServiceType), iapServiceType);
                       //tempConn.Stop();
                       iapName.ZeroTerminate();
		       iapServiceType.ZeroTerminate();

//                        if(iapServiceType.Find(_L8("LANService")) != KErrNotFound) {
//                            activeLanConnectionFound = ETrue;
//                            break;
//                        }
			strIapName = QString((char*)iapName.Ptr());
                        int error = 0;
                        if(!qt_SetDefaultIapName(strIapName, error)) {
                            //printf("failed setdefaultif @ %i with %s and errno = %d \n", __LINE__, strIapName.toUtf8().data(), error);
                            strIapName = QString("");
                        }

                        CleanupStack::PopAndDestroy(&tempConn);
                        break;
                    }
                }
            }
        }
    }

    //conn.Stop();

    CleanupStack::PopAndDestroy(&conn);
    CleanupStack::PopAndDestroy(&serv);

    return strIapName;
}

static QString qt_CheckSettingsForConnection(QStringList& iapNames) {
    for(int index = 0; index < iapNames.size(); ++index) {
        QString strIapName = iapNames.at(index);
        int error = 0;
        if(!qt_SetDefaultIapName(strIapName, error)) {
            //printf("failed setdefaultif @ %i with %s and errno = %d \n", __LINE__, strIapName.toUtf8().data(), error);
            strIapName = QString("");
        } else {
            return strIapName;
        }
    }
    return strIapName;
}

static void qt_SetDefaultIapL()
{
    // settings @ /c/data/.config/Trolltech.com
    QSettings settings(QSettings::UserScope, qtOrganizationTag);
    // populate iap name list
    QStringList iapNames;
    readIapNamesSettings(settings, iapNames);

    QString iapNameValue;

    iapNameValue = qt_CheckForActiveConnection();

    if(!iapNameValue.isEmpty()) {
        qt_SaveIapName(settings, iapNames, iapNameValue);
        return;
    }

    iapNameValue = qt_CheckSettingsForConnection(iapNames);

    if(!iapNameValue.isEmpty()) {
        qt_SaveIapName(settings, iapNames, iapNameValue);
        return;
    }

    /*
     * no active LAN connections yet
     * no IAP in settings
     * offer IAP dialog to user
     */
    iapNameValue = qt_OfferIapDialog();
    qt_SaveIapName(settings, iapNames, iapNameValue);
    return;

}

static int qt_SetDefaultIap()
{
    TRAPD(err, qt_SetDefaultIapL());
    return err;
}

#endif // QSYM_IAP_UTIL_H
