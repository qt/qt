/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the qmake application of the Qt Toolkit.
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

#include <iostream>

#include <QtCore/qdir.h>
#include <QtCore/qxmlstream.h>

#include "epocroot.h"
#include "../windows/registry.h"

// Registry key under which the location of the Symbian devices.xml file is
// stored.
// Note that, on 64-bit machines, this key is located under the 32-bit
// compatibility key:
//     HKEY_LOCAL_MACHINE\Software\Wow6432Node
#define SYMBIAN_SDKS_REG_SUBKEY "Software\\Symbian\\EPOC SDKs\\CommonPath"

#ifdef Q_OS_WIN32
#   define SYMBIAN_SDKS_REG_HANDLE HKEY_LOCAL_MACHINE
#else
#   define SYMBIAN_SDKS_REG_HANDLE 0
#endif

// Value which is populated and returned by the epocRoot() function.
// Stored as a static value in order to avoid unnecessary re-evaluation.
static QString epocRootValue;

#ifdef QT_BUILD_QMAKE
std::ostream &operator<<(std::ostream &s, const QString &val) {
    s << val.toLocal8Bit().data();
    return s;
}
#else
// Operator implemented in configureapp.cpp
std::ostream &operator<<(std::ostream &s, const QString &val);
#endif

QString getDevicesXmlPath()
    {
    // Note that the following call will return a null string on platforms other
    // than Windows.  If support is required on other platforms for devices.xml,
    // an alternative mechanism for retrieving the location of this file will
    // be required.
    return readRegistryKey(SYMBIAN_SDKS_REG_HANDLE, SYMBIAN_SDKS_REG_SUBKEY);
    }

/**
 * Checks whether epocRootValue points to an existent directory.
 * If not, epocRootValue is set to an empty string and an error message is printed.
 */
void checkEpocRootExists(const QString &source)
{
    if (!epocRootValue.isEmpty()) {
        QDir dir(epocRootValue);
        if (!dir.exists()) {
            std::cerr << "Warning: " << source << " is set to an invalid path: " << epocRootValue << std::endl;
            epocRootValue = QString();
        }
    }
}

/**
 * Translate path from Windows to Qt format.
 */
static void fixEpocRoot(QString &path)
{
    path.replace("\\", "/");

    if (!path.size() || path[path.size()-1] != QChar('/')) {
        path += QChar('/');
    }
}

/**
 * Determine the epoc root for the currently active SDK.
 */
QString epocRoot()
{
    if (epocRootValue.isEmpty()) {
        // 1. If environment variable EPOCROOT is set and points to an existent
        //    directory, this is returned.
        epocRootValue = qgetenv("EPOCROOT");
        checkEpocRootExists("EPOCROOT");

        if (epocRootValue.isEmpty()) {
            // 2. The location of devices.xml is specified by a registry key.  If this
            //    file exists, it is parsed.
            QString devicesXmlPath = getDevicesXmlPath();
            if (devicesXmlPath.isEmpty()) {
                std::cerr << "Error: Symbian SDK registry key not found" << std::endl;
            } else {
                devicesXmlPath += "/devices.xml";
                QFile devicesFile(devicesXmlPath);
                if (devicesFile.open(QIODevice::ReadOnly)) {

                    // 3. If the EPOCDEVICE environment variable is set and a corresponding
                    //    entry is found in devices.xml, and its epocroot value points to an
                    //    existent directory, it is returned.
                    // 4. If a device element marked as default is found in devices.xml and its
                    //    epocroot value points to an existent directory, this is returned.

                    const QString epocDeviceValue = qgetenv("EPOCDEVICE");
                    bool epocDeviceFound = false;

                    QXmlStreamReader xml(&devicesFile);
                    while (!xml.atEnd()) {
                        xml.readNext();
                        if (xml.isStartElement() && xml.name() == "devices") {
                            if (xml.attributes().value("version") == "1.0") {
                                while (!(xml.isEndElement() && xml.name() == "devices") && !xml.atEnd()) {
                                    xml.readNext();
                                    if (xml.isStartElement() && xml.name() == "device") {
                                        const bool isDefault = xml.attributes().value("default") == "yes";
                                        const QString id = xml.attributes().value("id").toString();
                                        const QString name = xml.attributes().value("name").toString();
                                        const QString alias = xml.attributes().value("alias").toString();
                                        bool epocDeviceMatch = (id + ":" + name) == epocDeviceValue;
                                        if (!alias.isEmpty())
                                            epocDeviceMatch |= alias == epocDeviceValue;
                                        epocDeviceFound |= epocDeviceMatch;

                                        if((epocDeviceValue.isEmpty() && isDefault) || epocDeviceMatch) {
                                            // Found a matching device
                                            while (!(xml.isEndElement() && xml.name() == "device") && !xml.atEnd()) {
                                                xml.readNext();
                                                if (xml.isStartElement() && xml.name() == "epocroot") {
                                                    epocRootValue = xml.readElementText();
                                                    const QString deviceSource = epocDeviceValue.isEmpty()
                                                        ? "default device"
                                                        : "EPOCDEVICE (" + epocDeviceValue + ")";
                                                    checkEpocRootExists(deviceSource);
                                                }
                                            }

                                            if (epocRootValue.isEmpty())
                                                xml.raiseError("No epocroot element found");
                                        }
                                    }
                                }
                            } else {
                                xml.raiseError("Invalid 'devices' element version");
                            }
                        }
                    }
                    if (xml.hasError()) {
                        std::cerr << "Error: \"" << xml.errorString() << "\" when parsing devices.xml" << std::endl;
                    } else {
                        if (epocRootValue.isEmpty()) {
                            if (!epocDeviceValue.isEmpty()) {
                                if (epocDeviceFound) {
                                    std::cerr << "Error: missing or invalid epocroot attribute "
                                              << "in device '" << epocDeviceValue << "'";
                                } else {
                                    std::cerr << "Error: no device matching EPOCDEVICE ("
                                              << epocDeviceValue << ")";
                                }
                            } else {
                                if (epocDeviceFound) {
                                    std::cerr << "Error: missing or invalid epocroot attribute "
                                              << "in default device";
                                } else {
                                    std::cerr << "Error: no default device";
                                }
                            }
                            std::cerr << " found in devices.xml file." << std::endl;
                        }
                    }
                } else {
                    std::cerr << "Error: could not open file " << devicesXmlPath << std::endl;
                }
            }
        }

        if (epocRootValue.isEmpty()) {
            // 5. An empty string is returned.
            std::cerr << "Error: failed to find epoc root" << std::endl
                 << "Either" << std::endl
                 << "    1. Set EPOCROOT environment variable to a valid value" << std::endl
                 << " or 2. Ensure that the HKEY_LOCAL_MACHINE\\" SYMBIAN_SDKS_REG_SUBKEY
                    " registry key is set, and then" << std::endl
                 << "       a. Set EPOCDEVICE environment variable to a valid device" << std::endl
                 << "    or b. Specify a default device in the devices.xml file." << std::endl;
        } else {
            fixEpocRoot(epocRootValue);
        }
    }

    return epocRootValue;
}

