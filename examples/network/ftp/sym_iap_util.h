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

#include <es_sock.h>
#include <sys/socket.h>
#include <net/if.h>
#include <rconnmon.h>

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
    TUint count;
    TRequestStatus status;
    TUint ids[15];

    RSocketServ serv;
    CleanupClosePushL(serv);

    RConnection conn;
    CleanupClosePushL(conn);

    RConnectionMonitor monitor;
    CleanupClosePushL(monitor);

    monitor.ConnectL();
    monitor.GetConnectionCount(count, status);
    User::WaitForRequest(status);
    if(status.Int() != KErrNone) {
        User::Leave(status.Int());
    }

    TUint numSubConnections;

    if(count > 0) {
        for (TInt i = 1; i <= count; i++) {
            User::LeaveIfError(monitor.GetConnectionInfo(i, ids[i-1], numSubConnections));
        }
        /*
         * get IAP value for first active connection
         */
        TBuf< 50 > iapName;
        monitor.GetStringAttribute(ids[0], 0, KIAPName, iapName, status);
        User::WaitForRequest(status);
        if (status.Int() != KErrNone) {
            User::Leave(status.Int());
        } else {
            QString strIapName = qt_TDesC2QStringL(iapName);
            struct ifreq ifReq;
            strcpy(ifReq.ifr_name, strIapName.toLatin1().data());
            User::LeaveIfError(setdefaultif(&ifReq));
        }
    } else {
        /*
         * no active connections yet
         * use IAP dialog to select one
         */
        User::LeaveIfError(serv.Connect());
        User::LeaveIfError(conn.Open(serv));
        User::LeaveIfError(conn.Start());

        _LIT(KIapNameSetting, "IAP\\Name");
        TBuf8<50> iap8Name;
        User::LeaveIfError(conn.GetDesSetting(TPtrC(KIapNameSetting), iap8Name));
        iap8Name.ZeroTerminate();

        conn.Stop();

        struct ifreq ifReq;
        strcpy(ifReq.ifr_name, (char*)iap8Name.Ptr());
        User::LeaveIfError(setdefaultif(&ifReq));
    }
    CleanupStack::PopAndDestroy(&monitor);
    CleanupStack::PopAndDestroy(&conn);
    CleanupStack::PopAndDestroy(&serv);
}

static int qt_SetDefaultIap()
{
    TRAPD(err, qt_SetDefaultIapL());
    return err;
}

#endif // QSYM_IAP_UTIL_H
