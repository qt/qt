/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "tools.h"

#include <QDir>
#include <QFile>
#include <QByteArray>


// std stuff ------------------------------------
#include <iostream>
#include <windows.h>
#include <conio.h>
#define NUMBER_OF_PARTS 7

std::ostream &operator<<(std::ostream &s, const QString &val); // defined in configureapp.cpp
using namespace std;

void Tools::checkLicense(QMap<QString,QString> &dictionary, QMap<QString,QString> &licenseInfo,
                         const QString &path)
{
    if (dictionary[ "BUILDNOKIA" ] == "yes") {
        dictionary["EDITION"] = "NokiaInternalBuild";
        dictionary["LICENSE_FILE"] = ""; // No License for nokia developers
        dictionary["QT_EDITION"] = "QT_EDITION_OPENSOURCE";
        return; // No license key checking in internal builds
    }

    QString tpLicense = dictionary["QT_SOURCE_TREE"] + "/LICENSE.PREVIEW.OPENSOURCE";
    if (QFile::exists(tpLicense)) {
        dictionary["EDITION"] = "Preview";
        dictionary["LICENSE FILE"] = tpLicense;
        dictionary["QT_EDITION"] = "QT_EDITION_OPENSOURCE";
        return; // No license key checking in Tech Preview
    }
    tpLicense = dictionary["QT_SOURCE_TREE"] + "/LICENSE.PREVIEW.COMMERCIAL";
    if (QFile::exists(tpLicense)) {
        dictionary["EDITION"] = "Preview";
        dictionary["LICENSE FILE"] = tpLicense;
        dictionary["QT_EDITION"] = "QT_EDITION_DESKTOP";
        return; // No license key checking in Tech Preview
    }

    // Read in the license file
    QFile licenseFile(path);
    if( !path.isEmpty() && licenseFile.open( QFile::ReadOnly ) ) {
        cout << "Reading license file in....." << qPrintable(path) << endl;

        QString buffer = licenseFile.readLine(1024);
        while (!buffer.isEmpty()) {
            if( buffer[ 0 ] != '#' ) {
                QStringList components = buffer.split( '=' );
                if ( components.size() >= 2 ) {
                    QStringList::Iterator it = components.begin();
                    QString key = (*it++).trimmed().replace( "\"", QString() ).toUpper();
                    QString value = (*it++).trimmed().replace( "\"", QString() );
                    licenseInfo[ key ] = value;
                }
            }
            // read next line
            buffer = licenseFile.readLine(1024);
        }
        licenseFile.close();
    } else {
        cout << "License file not found in " << QDir::homePath() << endl;
        cout << "Please put the Qt license file, '.qt-license' in your home "
             << "directory and run configure again.";
        dictionary["DONE"] = "error";
        return;
    }

    // Verify license info...
    QString licenseKey = licenseInfo["LICENSEKEYEXT"];
    const char * clicenseKey = licenseKey.toLatin1();
    //We check the licence
#ifndef _WIN32_WCE
        char *buffer = strdup(clicenseKey);
#else
        char *buffer = (char*) malloc(strlen(clicenseKey) + 1);
        strcpy(buffer, clicenseKey);
#endif
    static const char * const SEP = "-";
    char *licenseParts[NUMBER_OF_PARTS];
    int partNumber = 0;
    for (char *part = strtok(buffer, SEP); part != 0; part = strtok(0, SEP))
        licenseParts[partNumber++] = part;
    if (partNumber < (NUMBER_OF_PARTS-1)) {
        dictionary["DONE"] = "error";
        cout << "License file does not contain proper license key." <<partNumber<< endl;
        free(buffer);
        return;
    }

    char products = licenseParts[0][0];
    char platforms = licenseParts[1][0];
    char* licenseSchema = licenseParts[2];
    char licenseFeatures = licenseParts[3][0];

    // Determine edition ---------------------------------------------------------------------------
    QString licenseType;
    if (strcmp(licenseSchema,"F4M") == 0) {
        licenseType = "Commercial";
        if (products == 'F') {
            dictionary["EDITION"] = "Universal";
            dictionary["QT_EDITION"] = "QT_EDITION_UNIVERSAL";
        } else if (products == 'B') {
            dictionary["EDITION"] = "FullFramework";
            dictionary["QT_EDITION"] = "QT_EDITION_DESKTOP";
        } else {
            dictionary["EDITION"] = "GUIFramework";
            dictionary["QT_EDITION"] = "QT_EDITION_DESKTOPLIGHT";
        }

        if (platforms == 'X') {
            dictionary["LICENSE_EXTENSION"] = "-ALLOS";
        } else if (strchr("3679ACDEHJKMSUWX", platforms)) {
            dictionary["LICENSE_EXTENSION"] = "-EMBEDDED";
        } else if (strchr("4BFPQRTY", platforms)) {
            dictionary["LICENSE_EXTENSION"] = "-DESKTOP";
        }
    } else if (strcmp(licenseSchema,"Z4M") == 0 || strcmp(licenseSchema,"R4M") == 0 || strcmp(licenseSchema,"Q4M") == 0) {
        if (products == 'B') {
            dictionary["EDITION"] = "Evaluation";
            dictionary["QT_EDITION"] = "QT_EDITION_EVALUATION";
            dictionary["LICENSE_EXTENSION"] = "-EVALUATION";
        }
    }

    if (QFile::exists(dictionary["QT_SOURCE_TREE"] + "/.LICENSE")) {
        // Generic, no-suffix license
        dictionary["LICENSE_EXTENSION"] = QString();
    } else if (dictionary["LICENSE_EXTENSION"].isEmpty()) {
        cout << "License file does not contain proper license key." << endl;
        dictionary["DONE"] = "error";
    }
    if (licenseType.isEmpty()
        || dictionary["EDITION"].isEmpty()
        || dictionary["QT_EDITION"].isEmpty()) {
        cout << "License file does not contain proper license key." << endl;
        dictionary["DONE"] = "error";
        return;
    }

    if (dictionary["PLATFORM NAME"].contains("Windows CE")) {
        // verify that we are licensed to use Qt for Windows CE
        if (dictionary["LICENSE_EXTENSION"] != "-EMBEDDED" && dictionary["LICENSE_EXTENSION"] != "-ALLOS") {
            cout << "You are not licensed for the " << dictionary["PLATFORM NAME"] << " platform." << endl << endl;
            cout << "Please contact sales@trolltech.com to upgrade your license" << endl;
            cout << "to include the " << dictionary["PLATFORM NAME"] << " platform, or install the" << endl;
            cout << "Qt Open Source Edition if you intend to develop free software." << endl;
            dictionary["DONE"] = "error";
            return;
        }
    }

    // copy one of .LICENSE-*(-US) to LICENSE
    QString toLicenseFile   = dictionary["QT_SOURCE_TREE"] + "/LICENSE";
    QString fromLicenseFile = dictionary["QT_SOURCE_TREE"] + "/.LICENSE" + dictionary["LICENSE_EXTENSION"];
    if (licenseFeatures == 'G') //US
        fromLicenseFile += "-US";

    if (licenseFeatures == '5') //Floating
        dictionary["METERED LICENSE"] = "true";

    if (!CopyFileA(QDir::toNativeSeparators(fromLicenseFile).toLocal8Bit(),
        QDir::toNativeSeparators(toLicenseFile).toLocal8Bit(), FALSE)) {
        cout << "Failed to copy license file (" << fromLicenseFile << ")";
        dictionary["DONE"] = "error";
        return;
    }
    dictionary["LICENSE FILE"] = toLicenseFile;
    free(buffer);
}

