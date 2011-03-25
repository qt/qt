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

//#define QHOSTINFO_DEBUG

// Qt Headers
#include <QByteArray>
#include <QUrl>
#include <QList>

#include "qplatformdefs.h"

#include "qhostinfo_p.h"
#include <private/qcore_symbian_p.h>
#include <private/qsystemerror_p.h>
#include <private/qnetworksession_p.h>

QT_BEGIN_NAMESPACE



QHostInfo QHostInfoAgent::fromName(const QString &hostName, QSharedPointer<QNetworkSession> networkSession)
{
    QHostInfo results;

    // Connect to ESOCK
    RSocketServ socketServ(qt_symbianGetSocketServer());
    RHostResolver hostResolver;


    int err;
    if (networkSession)
        err = QNetworkSessionPrivate::nativeOpenHostResolver(*networkSession, hostResolver, KAfInet, KProtocolInetUdp);
    else
        err = hostResolver.Open(socketServ, KAfInet, KProtocolInetUdp);
    if (err) {
        results.setError(QHostInfo::UnknownError);
        results.setErrorString(QSystemError(err,QSystemError::NativeError).toString());

        return results;
    }

    TNameEntry nameResult;

#if defined(QHOSTINFO_DEBUG)
    qDebug("QHostInfoAgent::fromName(%s) looking up...",
           hostName.toLatin1().constData());
#endif

    QHostAddress address;
    if (address.setAddress(hostName)) {
        // Reverse lookup

        TInetAddr IpAdd;
        IpAdd.Input(qt_QString2TPtrC(hostName));

        // Synchronous request. nameResult returns Host Name.
        hostResolver.GetByAddress(IpAdd, nameResult);
        if (err) {
            // TODO - Could there be other errors? Symbian docs don't say.
            if (err == KErrNotFound) {
                results.setError(QHostInfo::HostNotFound);
                results.setErrorString(tr("Host not found"));
            } else {
                results.setError(QHostInfo::UnknownError);
                results.setErrorString(QSystemError(err,QSystemError::NativeError).toString());
            }

            return results;
        }

        results.setHostName(qt_TDesC2QString(nameResult().iName));
        results.setAddresses(QList<QHostAddress>() << address);
        return results;
    }

    // IDN support
    QByteArray aceHostname = QUrl::toAce(hostName);
    results.setHostName(hostName);
    if (aceHostname.isEmpty()) {
        results.setError(QHostInfo::HostNotFound);
        results.setErrorString(hostName.isEmpty() ?
                               QCoreApplication::translate("QHostInfoAgent", "No host name given") :
                               QCoreApplication::translate("QHostInfoAgent", "Invalid hostname"));
        return results;
    }


    // Call RHostResolver::GetByAddress, and place all IPv4 addresses at the start and
    // the IPv6 addresses at the end of the address list in results.

    // Synchronous request.
    err = hostResolver.GetByName(qt_QString2TPtrC(QString::fromLatin1(aceHostname)), nameResult);
    if (err) {
        // TODO - Could there be other errors? Symbian docs don't say.
        if (err == KErrNotFound) {
            results.setError(QHostInfo::HostNotFound);
            results.setErrorString(tr("Host not found"));
        } else {
            results.setError(QHostInfo::UnknownError);
            results.setErrorString(QSystemError(err,QSystemError::NativeError).toString());
        }

        return results;
    }

    QList<QHostAddress> hostAddresses;

    TInetAddr hostAdd = nameResult().iAddr;
    // 39 is the maximum length of an IPv6 address.
    TBuf<39> ipAddr;

    // Fill ipAddr with the IP address from hostAdd
    hostAdd.Output(ipAddr);
    if (ipAddr.Length() > 0)
        hostAddresses.append(QHostAddress(qt_TDesC2QString(ipAddr)));

    // Check if there's more than one IP address linkd to this name
    while (hostResolver.Next(nameResult) == KErrNone) {
        hostAdd = nameResult().iAddr;
        hostAdd.Output(ipAddr);

        // Ensure that record is valid (not an alias and with length greater than 0)
        if (!(nameResult().iFlags & TNameRecord::EAlias) && (ipAddr.Length() > 0)) {
            hostAddresses.append(QHostAddress(qt_TDesC2QString(ipAddr)));
        }
    }

    hostResolver.Close();

    results.setAddresses(hostAddresses);
    return results;
}

QHostInfo QHostInfoAgent::fromName(const QString &hostName)
{
    // null shared pointer
    QSharedPointer<QNetworkSession> networkSession;
    return fromName(hostName, networkSession);
}

QString QHostInfo::localHostName()
{
    // Connect to ESOCK
    RSocketServ socketServ(qt_symbianGetSocketServer());
    RHostResolver hostResolver;

    // Will return both IPv4 and IPv6
    // TODO: Pass RHostResolver.Open() the global RConnection
    int err = hostResolver.Open(socketServ, KAfInet, KProtocolInetUdp);
    if (err)
        return QString();

    THostName hostName;
    err = hostResolver.GetHostName(hostName);
    if (err)
        return QString();

    hostResolver.Close();

    return qt_TDesC2QString(hostName);
}

QString QHostInfo::localDomainName()
{
    // TODO - fill with code.
    return QString();
}


QSymbianHostResolver::QSymbianHostResolver(const QString &hostName, int identifier, QSharedPointer<QNetworkSession> networkSession)
    : CActive(CActive::EPriorityStandard), iHostName(hostName),
      iSocketServ(qt_symbianGetSocketServer()), iNetworkSession(networkSession), iResults(identifier)
{
    CActiveScheduler::Add(this);
}

QSymbianHostResolver::~QSymbianHostResolver()
{
#if defined(QHOSTINFO_DEBUG)
    qDebug() << "QSymbianHostInfoLookupManger::~QSymbianHostResolver" << id();
#endif
    Cancel();
    iHostResolver.Close();
}

// Async equivalent to QHostInfoAgent::fromName()
void QSymbianHostResolver::requestHostLookup()
{

#if defined(QHOSTINFO_DEBUG)
    qDebug("QSymbianHostResolver::requestHostLookup(%s) looking up... (id = %d)",
        iHostName.toLatin1().constData(), id());
#endif

    int err;
    if (iNetworkSession) {
        err = QNetworkSessionPrivate::nativeOpenHostResolver(*iNetworkSession, iHostResolver, KAfInet, KProtocolInetUdp);
#if defined(QHOSTINFO_DEBUG)
        qDebug("using resolver from session (err = %d)", err);
#endif
    } else {
        err = iHostResolver.Open(iSocketServ, KAfInet, KProtocolInetUdp);
#if defined(QHOSTINFO_DEBUG)
        qDebug("using default resolver (err = %d)", err);
#endif
    }
    if (err) {
        // What are we doing with iResults??
        iResults.setError(QHostInfo::UnknownError);
        iResults.setErrorString(QSystemError(err, QSystemError::NativeError).toString());

    } else {

        if (iAddress.setAddress(iHostName)) {
            // Reverse lookup

            IpAdd.Input(qt_QString2TPtrC(iHostName));

            // Asynchronous request.
            iHostResolver.GetByAddress(IpAdd, iNameResult, iStatus); // <---- ASYNC
            iState = EGetByAddress;

        } else {

            // IDN support
            QByteArray aceHostname = QUrl::toAce(iHostName);
            iResults.setHostName(iHostName);
            if (aceHostname.isEmpty()) {
                iResults.setError(QHostInfo::HostNotFound);
                iResults.setErrorString(iHostName.isEmpty() ?
                                       QCoreApplication::translate("QHostInfoAgent", "No host name given") :
                                       QCoreApplication::translate("QHostInfoAgent", "Invalid hostname"));

                err = KErrArgument;
            } else {
                iEncodedHostName = QString::fromLatin1(aceHostname);
                iHostNamePtr.Set(qt_QString2TPtrC(iEncodedHostName));

                // Asynchronous request.
                iHostResolver.GetByName(iHostNamePtr, iNameResult, iStatus);
                iState = EGetByName;
            }
        }
    }
    SetActive();
    if (err) {
        iHostResolver.Close();

        //self complete so that RunL can inform manager without causing recursion
        iState = EError;
        TRequestStatus* stat = &iStatus;
        User::RequestComplete(stat, err);
    }
}

void QSymbianHostResolver::DoCancel()
{
#if defined(QHOSTINFO_DEBUG)
    qDebug() << "QSymbianHostResolver::DoCancel" << QThread::currentThreadId() << id() << (int)iState << this;
#endif
    if (iState == EGetByAddress || iState == EGetByName) {
        //these states have made an async request to host resolver
        iHostResolver.Cancel();
    } else {
        //for the self completing states there is nothing to cancel
        Q_ASSERT(iState == EError);
    }
}

void QSymbianHostResolver::RunL()
{
    QT_TRYCATCH_LEAVING(run());
}

void QSymbianHostResolver::run()
{
    if (iState == EGetByName)
        processNameResults();
    else if (iState == EGetByAddress)
        processAddressResults();

    iState = EIdle;

    QSymbianHostInfoLookupManger *manager = QSymbianHostInfoLookupManger::globalInstance();
    manager->lookupFinished(this);

    resultEmitter.emitResultsReady(iResults);

    delete this;
}

TInt QSymbianHostResolver::RunError(TInt aError)
{
    QT_TRY {
        iState = EIdle;

        QSymbianHostInfoLookupManger *manager = QSymbianHostInfoLookupManger::globalInstance();
        manager->lookupFinished(this);

        iResults.setError(QHostInfo::UnknownError);
        iResults.setErrorString(QSystemError(aError,QSystemError::NativeError).toString());

        resultEmitter.emitResultsReady(iResults);
    }
    QT_CATCH(...) {}

    delete this;

    return KErrNone;
}

void QSymbianHostResolver::processNameResults()
{
    TInt err = iStatus.Int();
    if (err < 0) {
        // TODO - Could there be other errors? Symbian docs don't say.
        if (err == KErrNotFound) {
            iResults.setError(QHostInfo::HostNotFound);
            iResults.setErrorString(QObject::tr("Host not found"));
        } else {
            iResults.setError(QHostInfo::UnknownError);
            iResults.setErrorString(QSystemError(err,QSystemError::NativeError).toString());
        }

        iHostResolver.Close();
        return;
    }

    QList<QHostAddress> hostAddresses;

    TInetAddr hostAdd = iNameResult().iAddr;
    // 39 is the maximum length of an IPv6 address.
    TBuf<39> ipAddr;

    // Fill ipAddr with the IP address from hostAdd
    hostAdd.Output(ipAddr);
    if (ipAddr.Length() > 0)
        hostAddresses.append(QHostAddress(qt_TDesC2QString(ipAddr)));

    // Check if there's more than one IP address linkd to this name
    while (iHostResolver.Next(iNameResult) == KErrNone) {
        hostAdd = iNameResult().iAddr;
        hostAdd.Output(ipAddr);

        // Ensure that record is valid (not an alias and with length greater than 0)
        if (!(iNameResult().iFlags & TNameRecord::EAlias) && (ipAddr.Length() > 0)) {
            hostAddresses.append(QHostAddress(qt_TDesC2QString(ipAddr)));
        }
    }

    iResults.setAddresses(hostAddresses);
}

void QSymbianHostResolver::processAddressResults()
{
    TInt err = iStatus.Int();

    if (err < 0) {
        // TODO - Could there be other errors? Symbian docs don't say.
        if (err == KErrNotFound) {
            iResults.setError(QHostInfo::HostNotFound);
            iResults.setErrorString(QObject::tr("Host not found"));
        } else {
            iResults.setError(QHostInfo::UnknownError);
            iResults.setErrorString(QSystemError(err,QSystemError::NativeError).toString());
        }

        return;
    }

    iResults.setHostName(qt_TDesC2QString(iNameResult().iName));
    iResults.setAddresses(QList<QHostAddress>() << iAddress);
}


int QSymbianHostResolver::id()
{
    return iResults.lookupId();
}

QSymbianHostInfoLookupManger::QSymbianHostInfoLookupManger()
{
}

QSymbianHostInfoLookupManger::~QSymbianHostInfoLookupManger()
{
}

void QSymbianHostInfoLookupManger::clear()
{
    QMutexLocker locker(&mutex);
#if defined(QHOSTINFO_DEBUG)
    qDebug() << "QSymbianHostInfoLookupManger::clear" << QThread::currentThreadId();
#endif
    //TODO: these aren't deleted because of thread unsafety, but that is a behaviour difference
    //qDeleteAll(iCurrentLookups);
    //qDeleteAll(iScheduledLookups);
    cache.clear();
}

void QSymbianHostInfoLookupManger::lookupFinished(QSymbianHostResolver *r)
{
    QMutexLocker locker(&mutex);

#if defined(QHOSTINFO_DEBUG)
    qDebug() << "QSymbianHostInfoLookupManger::lookupFinished" << QThread::currentThreadId() << r->id() << "current" << iCurrentLookups.count() << "queued" << iScheduledLookups.count();
#endif
    // remove finished lookup from array and destroy
    TInt count = iCurrentLookups.count();
    for (TInt i = 0; i < count; i++) {
        if (iCurrentLookups[i]->id() == r->id()) {
            iCurrentLookups.removeAt(i);
            break;
        }
    }

    runNextLookup();
}

void QSymbianHostInfoLookupManger::runNextLookup()
{
#if defined(QHOSTINFO_DEBUG)
    qDebug() << "QSymbianHostInfoLookupManger::runNextLookup" << QThread::currentThreadId() << "current" << iCurrentLookups.count() << "queued" << iScheduledLookups.count();
#endif
    // check to see if there are any scheduled lookups
    if (iScheduledLookups.count() > 0) {
        // if so, move one to the current lookups and run it
        // FIFO
        QSymbianHostResolver* hostResolver = iScheduledLookups.takeFirst();
        iCurrentLookups.append(hostResolver);
        hostResolver->requestHostLookup();
    }
}

// called from QHostInfo
void QSymbianHostInfoLookupManger::scheduleLookup(QSymbianHostResolver* r)
{
    QMutexLocker locker(&mutex);

#if defined(QHOSTINFO_DEBUG)
    qDebug() << "QSymbianHostInfoLookupManger::scheduleLookup" << QThread::currentThreadId() << r->id() << "current" << iCurrentLookups.count() << "queued" << iScheduledLookups.count();
#endif
    // Check to see if we have space on the current lookups pool.
    if (iCurrentLookups.count() >= KMaxConcurrentLookups) {
        // If no, schedule for later.
        iScheduledLookups.append(r);
#if defined(QHOSTINFO_DEBUG)
    qDebug(" - scheduled");
#endif
        return;
    } else {
        // If yes, add it to the current lookups.
        iCurrentLookups.append(r);

        // ... and trigger the async call.
        r->requestHostLookup();
    }
}

void QSymbianHostInfoLookupManger::abortLookup(int id)
{
    QMutexLocker locker(&mutex);

#if defined(QHOSTINFO_DEBUG)
    qDebug() << "QSymbianHostInfoLookupManger::abortLookup" << QThread::currentThreadId() << id << "current" << iCurrentLookups.count() << "queued" << iScheduledLookups.count();
#endif
    int i = 0;
    // Find the aborted lookup by ID.
    // First in the current lookups.
    for (i = 0; i < iCurrentLookups.count(); i++) {
        if (id == iCurrentLookups[i]->id()) {
            QSymbianHostResolver* r = iCurrentLookups.at(i);
            iCurrentLookups.removeAt(i);
            delete r; //cancels via destructor
            runNextLookup();
            return;
        }
    }
    // Then in the scheduled lookups.
    for (i = 0; i < iScheduledLookups.count(); i++) {
        if (id == iScheduledLookups[i]->id()) {
            QSymbianHostResolver* r = iScheduledLookups.at(i);
            iScheduledLookups.removeAt(i);
            delete r;
            return;
        }
    }
}

QSymbianHostInfoLookupManger* QSymbianHostInfoLookupManger::globalInstance()
{
    return static_cast<QSymbianHostInfoLookupManger*>
            (QAbstractHostInfoLookupManger::globalInstance());
}

QT_END_NAMESPACE
