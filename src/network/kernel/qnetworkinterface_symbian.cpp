/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
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

//#define QNETWORKINTERFACE_DEBUG

//#include "qset.h"
#include "qnetworkinterface.h"
#include "qnetworkinterface_p.h"
//#include <private/qnativesocketengine_p.h>
//#include "qalgorithms.h"


#ifndef QT_NO_NETWORKINTERFACE

#include <in_sock.h>
#include <in_iface.h>
#include <es_sock.h>

//#include <sys/types.h>
//#include <sys/socket.h>
//#include <errno.h>
//#include <net/if.h>
//#include <qplatformdefs.h>

QT_BEGIN_NAMESPACE


static QNetworkInterface::InterfaceFlags convertFlags( const TSoInetInterfaceInfo& aInfo )
{
    QNetworkInterface::InterfaceFlags flags = 0;
    flags |= (aInfo.iState == EIfUp) ? QNetworkInterface::IsUp : QNetworkInterface::InterfaceFlag(0);
    // We do not have separate flag for running in Symbian OS
    flags |= (aInfo.iState == EIfUp) ? QNetworkInterface::IsRunning : QNetworkInterface::InterfaceFlag(0);
    flags |= (aInfo.iFeatures&KIfCanBroadcast) ? QNetworkInterface::CanBroadcast : QNetworkInterface::InterfaceFlag(0);    
    flags |= (aInfo.iFeatures&KIfIsLoopback) ? QNetworkInterface::IsLoopBack : QNetworkInterface::InterfaceFlag(0);
    flags |= (aInfo.iFeatures&KIfIsPointToPoint) ? QNetworkInterface::IsPointToPoint : QNetworkInterface::InterfaceFlag(0);
    flags |= (aInfo.iFeatures&KIfCanMulticast) ? QNetworkInterface::CanMulticast : QNetworkInterface::InterfaceFlag(0);
    return flags;
}

QString qstringFromDesc( const TDesC& aData ) 
{
    return QString::fromUtf16(aData.Ptr(), aData.Length());
}

static QList<QNetworkInterfacePrivate *> interfaceListing()
{
    TInt err( KErrNone );
    QList<QNetworkInterfacePrivate *> interfaces;
    
    // Connect to Native socket server
    RSocketServ socketServ;
    err = socketServ.Connect();
    if( err )
        return interfaces;
    
    // Open dummy socket for interface queries
    RSocket socket;
    err = socket.Open( socketServ, _L("udp"));
    if( err )
    {
        socketServ.Close();
        return interfaces;    
    }

    // Ask socket to start enumerating interfaces
    err =  socket.SetOpt( KSoInetEnumInterfaces, KSolInetIfCtrl );
    if( err )
    {
        socket.Close();
        socketServ.Close();
        return interfaces;    
    }    

    int ifindex = 0;
    TPckgBuf<TSoInetInterfaceInfo> infoPckg;
    TSoInetInterfaceInfo &info = infoPckg();
    while( socket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, infoPckg) == KErrNone )
    {
        // Do not include IPv6 addresses because netmask and broadcast address cannot be determined correctly
        if( info.iName != KNullDesC && info.iAddress.IsV4Mapped() )
        {
            TName address;        
            QNetworkAddressEntry entry;            
            QNetworkInterfacePrivate *iface = 0;            
            
            iface = new QNetworkInterfacePrivate; 
            iface->index = ifindex++;
            interfaces << iface;
            iface->name = qstringFromDesc( info.iName );
            iface->flags = convertFlags( info );
            
            if( /*info.iFeatures&KIfHasHardwareAddr &&*/ info.iHwAddr.Family() != KAFUnspec )
            {
                for ( TInt i = sizeof(SSockAddr); i < sizeof(SSockAddr) + info.iHwAddr.GetUserLen(); i++ ) 
                {
                    address.AppendNumFixedWidth(info.iHwAddr[i], EHex, 2);
                    if( ( i + 1) < sizeof(SSockAddr) + info.iHwAddr.GetUserLen() )
                        address.Append( _L(":") );
                }  
                address.UpperCase();                
                iface->hardwareAddress = qstringFromDesc( address );
            }
             
            // Get the address of the interface
            info.iAddress.Output(address);
            entry.setIp( QHostAddress( qstringFromDesc( address ) ) );                

            // Get the interface netmask            
            // TODO: For some reason netmask is always 0.0.0.0                    
            //info.iNetMask.Output(address);
            //entry.setNetmask( QHostAddress( qstringFromDesc( address ) ) );     
            
            // Workaround: Let Symbian determine netmask based on IP address class
            // TODO: works only for IPv4
            TInetAddr netmask;
            netmask.NetMask( info.iAddress );
            netmask.Output(address);
            entry.setNetmask( QHostAddress( qstringFromDesc( address ) ) );                  
            
            // Get the interface broadcast address
            if (iface->flags & QNetworkInterface::CanBroadcast) 
            {
                // For some reason broadcast address is always 0.0.0.0
                // info.iBrdAddr.Output(address);
                // entry.setBroadcast( QHostAddress( qstringFromDesc( address ) ) );                    
                
                // Workaround: Let Symbian determine broadcast address based on IP address
                // TODO: works only for IPv4
                TInetAddr broadcast;
                broadcast.NetBroadcast( info.iAddress );   
                broadcast.Output(address);
                entry.setBroadcast( QHostAddress( qstringFromDesc( address ) ) );
                }
          
            // Add new entry to interface address entries
            iface->addressEntries << entry; 
            
#if defined(QNETWORKINTERFACE_DEBUG)                
            printf("\n       Found network interface %s, interface flags:\n\
                IsUp = %d, IsRunning = %d, CanBroadcast = %d,\n\
                IsLoopBack = %d, IsPointToPoint = %d, CanMulticast = %d, \n\
                ip = %s, netmask = %s, broadcast = %s,\n\
                hwaddress = %s", 
                iface->name.toLatin1().constData(),
                iface->flags & QNetworkInterface::IsUp, iface->flags & QNetworkInterface::IsRunning, iface->flags & QNetworkInterface::CanBroadcast,
                iface->flags & QNetworkInterface::IsLoopBack, iface->flags & QNetworkInterface::IsPointToPoint, iface->flags & QNetworkInterface::CanMulticast,
                entry.ip().toString().toLatin1().constData(), entry.netmask().toString().toLatin1().constData(), entry.broadcast().toString().toLatin1().constData(),
                iface->hardwareAddress.toLatin1().constData());                
#endif                    
        }
    }

    // we will try to use routing info to detect more precisely
    // netmask and then ::postProcess() should calculate
    // broadcast addresses

    // use dummy socket to start enumerating routes
    err =  socket.SetOpt(KSoInetEnumRoutes, KSolInetRtCtrl);
    if( err )
    {
        socket.Close();
        socketServ.Close();
        // return what we have
        // up to this moment
        return interfaces;
    }

    TSoInetRouteInfo routeInfo;
    TPckg<TSoInetRouteInfo> routeInfoPkg(routeInfo);
    while(socket.GetOpt(KSoInetNextRoute, KSolInetRtCtrl, routeInfoPkg) == KErrNone)
    {
        TName address;

        // get interface address
        routeInfo.iIfAddr.Output(address);
        QHostAddress ifAddr(qstringFromDesc(address));
        if(ifAddr.isNull())
            continue;

        routeInfo.iDstAddr.Output(address);
        QHostAddress destination(qstringFromDesc(address));
        if(destination.isNull() || destination != ifAddr)
            continue;

        // search interfaces
        for(int ifindex = 0; ifindex < interfaces.size(); ++ifindex) {
            QNetworkInterfacePrivate *iface = interfaces.at(ifindex);
            for(int eindex = 0; eindex < iface->addressEntries.size(); ++eindex) {
                QNetworkAddressEntry entry = iface->addressEntries.at(eindex);
                if(entry.ip() != ifAddr) {
                    continue;
                } else {
                    routeInfo.iNetMask.Output(address);
                    QHostAddress netmask(qstringFromDesc(address));
                    entry.setNetmask(netmask);
                    // NULL boradcast address for
                    // ::postProcess to have effect
                    entry.setBroadcast(QHostAddress());
                    iface->addressEntries.replace(eindex, entry);
//                    printf("for %s netmask = %s and destination = %s ; type = %d; state = %d; metric = %d \n",
//                            ifAddr.toString().toLatin1().constData(), netmask.toString().toLatin1().constData(), destination.toString().toLatin1().constData(),
//                            routeInfo.iType, routeInfo.iState, routeInfo.iMetric);
                }
            }
        }
    }

    socket.Close();
    socketServ.Close();

    return interfaces;
}

QList<QNetworkInterfacePrivate *> QNetworkInterfaceManager::scan()
{
    return interfaceListing();
}

QT_END_NAMESPACE

#endif // QT_NO_NETWORKINTERFACE
