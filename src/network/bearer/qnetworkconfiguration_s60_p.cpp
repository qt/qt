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

#include "qnetworkconfiguration_s60_p.h"

QTM_BEGIN_NAMESPACE

QNetworkConfigurationPrivate::QNetworkConfigurationPrivate()
    : isValid(false), type(QNetworkConfiguration::Invalid),
      roamingSupported(false), purpose(QNetworkConfiguration::UnknownPurpose), 
      bearer(QNetworkConfigurationPrivate::BearerUnknown), numericId(0),
      connectionId(0), manager(0)
{
}

QNetworkConfigurationPrivate::~QNetworkConfigurationPrivate()
{
    //release pointers to member configurations
    serviceNetworkMembers.clear(); 
}

QString QNetworkConfigurationPrivate::bearerName() const
{
    switch (bearer) {
        case QNetworkConfigurationPrivate::BearerEthernet:  return QLatin1String("Ethernet");
        case QNetworkConfigurationPrivate::BearerWLAN:      return QLatin1String("WLAN");
        case QNetworkConfigurationPrivate::Bearer2G:        return QLatin1String("2G");
        case QNetworkConfigurationPrivate::BearerCDMA2000:  return QLatin1String("CDMA2000");
        case QNetworkConfigurationPrivate::BearerWCDMA:     return QLatin1String("WCDMA");
        case QNetworkConfigurationPrivate::BearerHSPA:      return QLatin1String("HSPA");
        case QNetworkConfigurationPrivate::BearerBluetooth: return QLatin1String("Bluetooth");
        case QNetworkConfigurationPrivate::BearerWiMAX:     return QLatin1String("WiMAX");
        default: return QString();
    }
}


QTM_END_NAMESPACE
