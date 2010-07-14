/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "serenum.h"
#include <QByteArray>
#include <QString>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

#include <usb.h>

class InterfaceInfo
{
public:
    InterfaceInfo(const QString &mf, const QString &pr, int mfid, int prid);
    QString manufacturer;
    QString product;
    int manufacturerid;
    int productid;
};

InterfaceInfo::InterfaceInfo(const QString &mf, const QString &pr, int mfid, int prid) :
    manufacturer(mf),
    product(pr),
    manufacturerid(mfid),
    productid(prid)
{
    if(mf.isEmpty())
        manufacturer = QString("[%1]").arg(mfid, 4, 16, QChar('0'));
    if(pr.isEmpty())
        product = QString("[%1]").arg(prid, 4, 16, QChar('0'));
}

QList<SerialPortId> enumerateSerialPorts(int loglevel)
{
    QList<QString> eligibleInterfaces;
    QList<InterfaceInfo> eligibleInterfacesInfo;
    QList<SerialPortId> list;

    usb_init();
    usb_find_busses();
    usb_find_devices();

    for (struct usb_bus *bus = usb_get_busses(); bus; bus = bus->next) {
        for (struct usb_device *device = bus->devices; device; device = device->next) {
            for (int n = 0; n < device->descriptor.bNumConfigurations; ++n) {
                struct usb_config_descriptor &usbConfig =device->config[n];
                QList<int> usableInterfaces;
                for (int m = 0; m < usbConfig.bNumInterfaces; ++m) {
                    for (int o = 0; o < usbConfig.interface[m].num_altsetting; ++o) {
                        struct usb_interface_descriptor &descriptor = usbConfig.interface[m].altsetting[o];
                        if (descriptor.bInterfaceClass != 2 // "Communication"
                                || descriptor.bInterfaceSubClass != 2 // Abstract (modem)
                                || descriptor.bInterfaceProtocol != 255) // Vendor Specific
                            continue;

                        unsigned char *buf = descriptor.extra;
                        unsigned int size = descriptor.extralen;
                        while (size >= 2 * sizeof(u_int8_t)) {
                            // for Communication devices there is a slave interface for the actual
                            // data transmission.
                            // the extra info stores that as a index for the interface
                            if (buf[0] >= 5 && buf[1] == 36 && buf[2] == 6) { // CDC Union
                                for (int i = 4; i < buf[0]; i++)
                                    usableInterfaces.append((int) buf[i]);
                            }
                            size -= buf[0];
                            buf += buf[0];
                        }
                    }
                }
                
                if (usableInterfaces.isEmpty())
                    continue;
                
                QString manufacturerString;
                QString productString;
                
                usb_dev_handle *devh = usb_open(device);
                if (devh) {
                    QByteArray buf;
                    buf.resize(256);
                    int err = usb_get_string_simple(devh, device->descriptor.iManufacturer, buf.data(), buf.size());
                    if (err < 0) {
                        if (loglevel > 1)
                            qDebug() << "      can't read manufacturer name, error:" << err;
                    } else {
                        manufacturerString = QString::fromAscii(buf);
                        if (loglevel > 1)
                            qDebug() << "      manufacturer:" << manufacturerString;
                    }

                    buf.resize(256);
                    err = usb_get_string_simple(devh, device->descriptor.iProduct, buf.data(), buf.size());
                    if (err < 0) {
                        if (loglevel > 1)
                            qDebug() << "      can't read product name, error:" << err;
                    } else {
                        productString = QString::fromAscii(buf);
                        if (loglevel > 1)
                            qDebug() << "      product:" << productString;
                    }
                    usb_close(devh);
                } else if (loglevel > 0) {
                    qDebug() << "      can't open usb device";
                }

                // second loop to find the actual data interface.
                foreach (int i, usableInterfaces) {
                    for (int m = 0; m < usbConfig.bNumInterfaces; ++m) {
                        for (int o = 0; o < usbConfig.interface[m].num_altsetting; ++o) {
                            struct usb_interface_descriptor &descriptor = usbConfig.interface[m].altsetting[o];
                            if (descriptor.bInterfaceNumber != i)
                                continue;
                            if (descriptor.bInterfaceClass == 10) { // "CDC Data"
                                if (loglevel > 1) {
                                    qDebug() << "      found the data port"
                                             << "bus:" << bus->dirname
                                             << "device" << device->filename
                                             << "interface" << descriptor.bInterfaceNumber;
                                }
                                // ### manufacturer and product strings are only readable as root :(
                                if (!manufacturerString.isEmpty() && !productString.isEmpty()) {
                                    eligibleInterfaces << QString("usb-%1_%2-if%3")
                                                          .arg(manufacturerString.replace(QChar(' '), QChar('_')))
                                                          .arg(productString.replace(QChar(' '), QChar('_')))
                                                          .arg(i, 2, 16, QChar('0'));
                                } else {
                                    eligibleInterfaces << QString("if%1").arg(i, 2, 16, QChar('0')); // fix!
                                }
                                eligibleInterfacesInfo << InterfaceInfo(manufacturerString, productString, device->descriptor.idVendor, device->descriptor.idProduct);
                            }
                        }
                    }
                }
            }
        }
    }
    
    if (loglevel > 1)
        qDebug() << "      searching for interfaces:" << eligibleInterfaces;

    QDir dir("/dev/serial/by-id/");
    foreach (const QFileInfo &info, dir.entryInfoList()) {
        if (!info.isDir()) {
            bool usable = eligibleInterfaces.isEmpty();
            foreach (const QString &iface, eligibleInterfaces) {
                if (info.fileName().contains(iface)) {
                    if (loglevel > 1)
                        qDebug() << "      found device file:" << info.fileName() << endl;
                    usable = true;
                    break;
                }
            }
            if (!usable)
                continue;

            SerialPortId id;
            id.friendlyName = info.fileName();
            id.portName = info.canonicalFilePath();
            list << id;
        }
    }

    if (list.isEmpty() && !eligibleInterfacesInfo.isEmpty() && loglevel > 0) {
        qDebug() << "Possible USB devices found, but without serial drivers:";
        foreach(const InterfaceInfo &iface, eligibleInterfacesInfo) {
            qDebug() << "    Manufacturer:"
                     << iface.manufacturer
                     << "Product:"
                     << iface.product
                     << endl
                     << "    Load generic driver using:"
                     << QString("sudo modprobe usbserial vendor=0x%1 product=0x%2")
                        .arg(iface.manufacturerid, 4, 16, QChar('0'))
                        .arg(iface.productid, 4, 16, QChar('0'));
        }
    }
    return list;
}

