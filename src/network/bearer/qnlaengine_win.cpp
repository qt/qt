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

#include "qnlaengine_win_p.h"
#include "qnetworkconfiguration_p.h"

#include <QtCore/qthread.h>
#include <QtCore/qmutex.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qstringlist.h>

#include <QtCore/qdebug.h>

#include "qnetworksessionengine_win_p.h"

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QNlaEngine, nlaEngine)

QWindowsSockInit::QWindowsSockInit()
:   version(0)
{
    //### should we try for 2.2 on all platforms ??
    WSAData wsadata;

    // IPv6 requires Winsock v2.0 or better.
    if (WSAStartup(MAKEWORD(2,0), &wsadata) != 0) {
        qWarning("QBearerManagementAPI: WinSock v2.0 initialization failed.");
    } else {
        version = 0x20;
    }
}

QWindowsSockInit::~QWindowsSockInit()
{
    WSACleanup();
}

#ifdef BEARER_MANAGEMENT_DEBUG
static void printBlob(NLA_BLOB *blob)
{
    qDebug() << "==== BEGIN NLA_BLOB ====";

    qDebug() << "type:" << blob->header.type;
    qDebug() << "size:" << blob->header.dwSize;
    qDebug() << "next offset:" << blob->header.nextOffset;

    switch (blob->header.type) {
    case NLA_RAW_DATA:
        qDebug() << "Raw Data";
        qDebug() << '\t' << blob->data.rawData;
        break;
    case NLA_INTERFACE:
        qDebug() << "Interface";
        qDebug() << "\ttype:" << blob->data.interfaceData.dwType;
        qDebug() << "\tspeed:" << blob->data.interfaceData.dwSpeed;
        qDebug() << "\tadapter:" << blob->data.interfaceData.adapterName;
        break;
    case NLA_802_1X_LOCATION:
        qDebug() << "802.1x Location";
        qDebug() << '\t' << blob->data.locationData.information;
        break;
    case NLA_CONNECTIVITY:
        qDebug() << "Connectivity";
        qDebug() << "\ttype:" << blob->data.connectivity.type;
        qDebug() << "\tinternet:" << blob->data.connectivity.internet;
        break;
    case NLA_ICS:
        qDebug() << "ICS";
        qDebug() << "\tspeed:" << blob->data.ICS.remote.speed;
        qDebug() << "\ttype:" << blob->data.ICS.remote.type;
        qDebug() << "\tstate:" << blob->data.ICS.remote.state;
        qDebug() << "\tmachine name:" << blob->data.ICS.remote.machineName;
        qDebug() << "\tshared adapter name:" << blob->data.ICS.remote.sharedAdapterName;
        break;
    default:
        qDebug() << "UNKNOWN BLOB TYPE";
    }

    qDebug() << "===== END NLA_BLOB =====";
}
#endif

static QString qGetInterfaceType(const QString &interface)
{
#ifdef Q_OS_WINCE
    Q_UNUSED(interface)
#else
    unsigned long oid;
    DWORD bytesWritten;

    NDIS_MEDIUM medium;
    NDIS_PHYSICAL_MEDIUM physicalMedium;

    HANDLE handle = CreateFile((TCHAR *)QString("\\\\.\\%1").arg(interface).utf16(), 0,
                               FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (handle == INVALID_HANDLE_VALUE)
        return QLatin1String("Unknown");

    oid = OID_GEN_MEDIA_SUPPORTED;
    bytesWritten = 0;
    bool result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                                  &medium, sizeof(medium), &bytesWritten, 0);
    if (!result) {
        CloseHandle(handle);
        return QLatin1String("Unknown");
    }

    oid = OID_GEN_PHYSICAL_MEDIUM;
    bytesWritten = 0;
    result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                             &physicalMedium, sizeof(physicalMedium), &bytesWritten, 0);
    if (!result) {
        CloseHandle(handle);

        if (medium == NdisMedium802_3)
            return QLatin1String("Ethernet");
        else
            return QLatin1String("Unknown");
    }

    CloseHandle(handle);

    if (medium == NdisMedium802_3) {
        switch (physicalMedium) {
        case NdisPhysicalMediumWirelessLan:
            return QLatin1String("WLAN");
        case NdisPhysicalMediumBluetooth:
            return QLatin1String("Bluetooth");
        case NdisPhysicalMediumWiMax:
            return QLatin1String("WiMAX");
        default:
#ifdef BEARER_MANAGEMENT_DEBUG
            qDebug() << "Physical Medium" << physicalMedium;
#endif
            return QLatin1String("Ethernet");
        }
    }

#ifdef BEARER_MANAGEMENT_DEBUG
    qDebug() << medium << physicalMedium;
#endif

#endif

    return QLatin1String("Unknown");
}

class QNlaThread : public QThread
{
    Q_OBJECT

public:
    QNlaThread(QNlaEngine *parent = 0);
    ~QNlaThread();

    QList<QNetworkConfigurationPrivate *> getConfigurations();

    void forceUpdate();

protected:
    virtual void run();

private:
    void updateConfigurations(QList<QNetworkConfigurationPrivate *> &configs);
    DWORD parseBlob(NLA_BLOB *blob, QNetworkConfigurationPrivate *cpPriv) const;
    QNetworkConfigurationPrivate *parseQuerySet(const WSAQUERYSET *querySet) const;
    void fetchConfigurations();

signals:
    void networksChanged();

private:
    QMutex mutex;
    HANDLE handle;
    bool done;
    QList<QNetworkConfigurationPrivate *> fetchedConfigurations;
};

QNlaThread::QNlaThread(QNlaEngine *parent)
:   QThread(parent), handle(0), done(false)
{
}

QNlaThread::~QNlaThread()
{
    mutex.lock();

    done = true;

    if (handle) {
        /* cancel completion event */
        if (WSALookupServiceEnd(handle) == SOCKET_ERROR)
            qWarning("WSALookupServiceEnd error %d", WSAGetLastError());
    }
    mutex.unlock();

    wait();
}

QList<QNetworkConfigurationPrivate *> QNlaThread::getConfigurations()
{
    QMutexLocker locker(&mutex);

    QList<QNetworkConfigurationPrivate *> foundConfigurations;

    for (int i = 0; i < fetchedConfigurations.count(); ++i) {
        QNetworkConfigurationPrivate *config = new QNetworkConfigurationPrivate;
        config->name = fetchedConfigurations.at(i)->name;
        config->isValid = fetchedConfigurations.at(i)->isValid;
        config->id = fetchedConfigurations.at(i)->id;
        config->state = fetchedConfigurations.at(i)->state;
        config->type = fetchedConfigurations.at(i)->type;
        config->roamingSupported = fetchedConfigurations.at(i)->roamingSupported;
        config->purpose = fetchedConfigurations.at(i)->purpose;
        config->internet = fetchedConfigurations.at(i)->internet;
        if (QNlaEngine *engine = qobject_cast<QNlaEngine *>(parent())) {
            config->bearer = engine->bearerName(config->id);
        }

        foundConfigurations.append(config);
    }

    return foundConfigurations;
}

void QNlaThread::forceUpdate()
{
    mutex.lock();

    if (handle) {
        /* cancel completion event */
        if (WSALookupServiceEnd(handle) == SOCKET_ERROR)
            qWarning("WSALookupServiceEnd error %d", WSAGetLastError());
        handle = 0;
    }
    mutex.unlock();
}

void QNlaThread::run()
{
    WSAEVENT changeEvent = WSACreateEvent();
    if (changeEvent == WSA_INVALID_EVENT) {
        qWarning("WSACreateEvent error %d", WSAGetLastError());
        return;
    }

    while (true) {
        fetchConfigurations();

        WSAQUERYSET qsRestrictions;

        memset(&qsRestrictions, 0, sizeof(qsRestrictions));
        qsRestrictions.dwSize = sizeof(qsRestrictions);
        qsRestrictions.dwNameSpace = NS_NLA;

        mutex.lock();
        if (done) {
            mutex.unlock();
            break;
        }
        int result = WSALookupServiceBegin(&qsRestrictions, LUP_RETURN_ALL, &handle);
        mutex.unlock();

        if (result == SOCKET_ERROR) {
            qWarning("%s: WSALookupServiceBegin error %d", __FUNCTION__, WSAGetLastError());
            break;
        }

        WSACOMPLETION completion;
        WSAOVERLAPPED overlapped;

        memset(&overlapped, 0, sizeof(overlapped));
        overlapped.hEvent = changeEvent;

        memset(&completion, 0, sizeof(completion));
        completion.Type = NSP_NOTIFY_EVENT;
        completion.Parameters.Event.lpOverlapped = &overlapped;

        DWORD bytesReturned = 0;
        result = WSANSPIoctl(handle, SIO_NSP_NOTIFY_CHANGE, 0, 0, 0, 0,
                             &bytesReturned, &completion);
        if (result == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error != WSA_IO_PENDING) {
                qWarning("WSANSPIoctl error %d", error);
                break;
            }
        }

#ifndef Q_OS_WINCE
        // Not interested in unrelated IO completion events
        // although we also don't want to block them
        while (WaitForSingleObjectEx(changeEvent, WSA_INFINITE, true) != WAIT_IO_COMPLETION) {}
#else
        WaitForSingleObject(changeEvent, WSA_INFINITE);
#endif

        mutex.lock();
        if (handle) {
            result = WSALookupServiceEnd(handle);
            if (result == SOCKET_ERROR) {
                qWarning("WSALookupServiceEnd error %d", WSAGetLastError());
                mutex.unlock();
                break;
            }
            handle = 0;
        }
        mutex.unlock();
    }

    WSACloseEvent(changeEvent);
}

void QNlaThread::updateConfigurations(QList<QNetworkConfigurationPrivate *> &configs)
{
    mutex.lock();

    while (!fetchedConfigurations.isEmpty())
        delete fetchedConfigurations.takeFirst();

    fetchedConfigurations = configs;

    mutex.unlock();

    emit networksChanged();
}

DWORD QNlaThread::parseBlob(NLA_BLOB *blob, QNetworkConfigurationPrivate *cpPriv) const
{
#ifdef BEARER_MANAGEMENT_DEBUG
    printBlob(blob);
#endif

    switch (blob->header.type) {
    case NLA_RAW_DATA:
#ifdef BEARER_MANAGEMENT_DEBUG
        qWarning("%s: unhandled header type NLA_RAW_DATA", __FUNCTION__);
#endif
        break;
    case NLA_INTERFACE:
        cpPriv->state = QNetworkConfiguration::Active;
        if (QNlaEngine *engine = qobject_cast<QNlaEngine *>(parent())) {
            engine->configurationInterface[cpPriv->id.toUInt()] =
                QString(blob->data.interfaceData.adapterName);
        }
        break;
    case NLA_802_1X_LOCATION:
#ifdef BEARER_MANAGEMENT_DEBUG
        qWarning("%s: unhandled header type NLA_802_1X_LOCATION", __FUNCTION__);
#endif
        break;
    case NLA_CONNECTIVITY:
        if (blob->data.connectivity.internet == NLA_INTERNET_YES)
            cpPriv->internet = true;
        else
            cpPriv->internet = false;
#ifdef BEARER_MANAGEMENT_DEBUG
        qWarning("%s: unhandled header type NLA_CONNECTIVITY", __FUNCTION__);
#endif
        break;
    case NLA_ICS:
#ifdef BEARER_MANAGEMENT_DEBUG
        qWarning("%s: unhandled header type NLA_ICS", __FUNCTION__);
#endif
        break;
    default:
#ifdef BEARER_MANAGEMENT_DEBUG
        qWarning("%s: unhandled header type %d", __FUNCTION__, blob->header.type);
#endif
        ;
    }

    return blob->header.nextOffset;
}

QNetworkConfigurationPrivate *QNlaThread::parseQuerySet(const WSAQUERYSET *querySet) const
{
    QNetworkConfigurationPrivate *cpPriv = new QNetworkConfigurationPrivate;

    cpPriv->name = QString::fromWCharArray(querySet->lpszServiceInstanceName);
    cpPriv->isValid = true;
    cpPriv->id = QString::number(qHash(QLatin1String("NLA:") + cpPriv->name));
    cpPriv->state = QNetworkConfiguration::Defined;
    cpPriv->type = QNetworkConfiguration::InternetAccessPoint;

#ifdef BEARER_MANAGEMENT_DEBUG
    qDebug() << "size:" << querySet->dwSize;
    qDebug() << "service instance name:" << QString::fromUtf16(querySet->lpszServiceInstanceName);
    qDebug() << "service class id:" << querySet->lpServiceClassId;
    qDebug() << "version:" << querySet->lpVersion;
    qDebug() << "comment:" << QString::fromUtf16(querySet->lpszComment);
    qDebug() << "namespace:" << querySet->dwNameSpace;
    qDebug() << "namespace provider id:" << querySet->lpNSProviderId;
    qDebug() << "context:" << QString::fromUtf16(querySet->lpszContext);
    qDebug() << "number of protocols:" << querySet->dwNumberOfProtocols;
    qDebug() << "protocols:" << querySet->lpafpProtocols;
    qDebug() << "query string:" << QString::fromUtf16(querySet->lpszQueryString);
    qDebug() << "number of cs addresses:" << querySet->dwNumberOfCsAddrs;
    qDebug() << "cs addresses:" << querySet->lpcsaBuffer;
    qDebug() << "output flags:" << querySet->dwOutputFlags;
#endif

    if (querySet->lpBlob) {
#ifdef BEARER_MANAGEMENT_DEBUG
        qDebug() << "blob size:" << querySet->lpBlob->cbSize;
        qDebug() << "blob data:" << querySet->lpBlob->pBlobData;
#endif

        DWORD offset = 0;
        do {
            NLA_BLOB *blob = reinterpret_cast<NLA_BLOB *>(querySet->lpBlob->pBlobData + offset);
            DWORD nextOffset = parseBlob(blob, cpPriv);
            if (nextOffset == offset)
                break;
            else
                offset = nextOffset;
        } while (offset != 0 && offset < querySet->lpBlob->cbSize);
    }

    return cpPriv;
}

void QNlaThread::fetchConfigurations()
{
    QList<QNetworkConfigurationPrivate *> foundConfigurations;

    WSAQUERYSET qsRestrictions;
    HANDLE hLookup = 0;

    memset(&qsRestrictions, 0, sizeof(qsRestrictions));
    qsRestrictions.dwSize = sizeof(qsRestrictions);
    qsRestrictions.dwNameSpace = NS_NLA;

    int result = WSALookupServiceBegin(&qsRestrictions, LUP_RETURN_ALL | LUP_DEEP, &hLookup);
    if (result == SOCKET_ERROR) {
        qWarning("%s: WSALookupServiceBegin error %d", __FUNCTION__, WSAGetLastError());
        mutex.lock();
        fetchedConfigurations.clear();
        mutex.unlock();
    }

    char buffer[0x10000];
    while (result == 0) {
        DWORD bufferLength = sizeof(buffer);
        result = WSALookupServiceNext(hLookup, LUP_RETURN_ALL,
                                      &bufferLength, reinterpret_cast<WSAQUERYSET *>(buffer));

        if (result == SOCKET_ERROR) {
            int error = WSAGetLastError();

            if (error == WSA_E_NO_MORE)
                break;

            if (error == WSAEFAULT) {
                qDebug() << "buffer not big enough" << bufferLength;
                break;
            }

            qWarning("WSALookupServiceNext error %d", WSAGetLastError());
            break;
        }

        QNetworkConfigurationPrivate *cpPriv =
            parseQuerySet(reinterpret_cast<WSAQUERYSET *>(buffer));

        foundConfigurations.append(cpPriv);
    }

    if (hLookup) {
        result = WSALookupServiceEnd(hLookup);
        if (result == SOCKET_ERROR) {
            qWarning("WSALookupServiceEnd error %d", WSAGetLastError());
        }
    }

    updateConfigurations(foundConfigurations);
}

QNlaEngine::QNlaEngine(QObject *parent)
:   QNetworkSessionEngine(parent), nlaThread(0)
{
    nlaThread = new QNlaThread(this);
    connect(nlaThread, SIGNAL(networksChanged()),
            this, SIGNAL(configurationsChanged()));
    nlaThread->start();

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

QNlaEngine::~QNlaEngine()
{
    delete nlaThread;
}

QList<QNetworkConfigurationPrivate *> QNlaEngine::getConfigurations(bool *ok)
{
    if (ok)
        *ok = true;

    return nlaThread->getConfigurations();
}

QString QNlaEngine::getInterfaceFromId(const QString &id)
{
    return configurationInterface.value(id.toUInt());
}

bool QNlaEngine::hasIdentifier(const QString &id)
{
    if (configurationInterface.contains(id.toUInt()))
        return true;

    bool result = false;
    QList<QNetworkConfigurationPrivate *> l = nlaThread->getConfigurations();
    while (!l.isEmpty()) {
        QNetworkConfigurationPrivate* cpPriv = l.takeFirst();
        if (!result && cpPriv->id == id) {
            result = true;
        }
        delete cpPriv;
    }
   
    return result; 
}

QString QNlaEngine::bearerName(const QString &id)
{
    QString interface = getInterfaceFromId(id);

    if (interface.isEmpty())
        return QString();

    return qGetInterfaceType(interface);
}

void QNlaEngine::connectToId(const QString &id)
{
    emit connectionError(id, OperationNotSupported);
}

void QNlaEngine::disconnectFromId(const QString &id)
{
    emit connectionError(id, OperationNotSupported);
}

void QNlaEngine::requestUpdate()
{
    nlaThread->forceUpdate();
}

QNlaEngine *QNlaEngine::instance()
{
    return nlaEngine();
}

#include "qnlaengine_win.moc"
#include "moc_qnlaengine_win_p.cpp"
QTM_END_NAMESPACE



