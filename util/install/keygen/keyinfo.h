/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the utils of the Qt Toolkit.
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

#ifndef KEYINFO_H
#define KEYINFO_H

// Products
enum {
    // the first 10 bits are reserved for Qt editions
    QtUniversal     = 0x00000001,
    QtDesktop       = 0x00000002,
    QtDesktopLight  = 0x00000004,
    QtConsole       = 0x00000008,
    QtDesignerOnly  = 0x00000010,
    QtReserved1     = 0x00000020,
    QtReserved2     = 0x00000040,
    QtReserved3     = 0x00000080,
    QtReserved4     = 0x00000100,
    QtReserved5     = 0x00000200,
    QtProductMask   = 0x000003ff,

    // the next 10 bits are reserved for Qtopia editions
    QtopiaPDA       = 0x00000400,
    QtopiaPhone     = 0x00000800,
    QtopiaRefboard  = 0x00001000,
    QtopiaReserved1 = 0x00002000,
    QtopiaReserved2 = 0x00004000,
    QtopiaReserved3 = 0x00008000,
    QtopiaReserved4 = 0x00010000,
    QtopiaReserved5 = 0x00020000,
    QtopiaReserved6 = 0x00040000,
    QtopiaReserved7 = 0x00080000,

    // other products get the upper 12 bits
    Teambuilder     = 0x00100000,
    Solutions       = 0x00200000,
    QSA             = 0x00400000,
    OtherReserved1  = 0x00800000,
    OtherReserved2  = 0x01000000,
    OtherReserved3  = 0x02000000,
    OtherReserved4  = 0x04000000,
    OtherReserved5  = 0x08000000,
    OtherReserved6  = 0x10000000,
    OtherReserved7  = 0x20000000,
    OtherReserved8  = 0x40000000,
    OtherReserved9  = 0x80000000,
};

// Platforms
enum {
    PlatformX11 = 0x01,
    PlatformEmbedded = 0x02,
    PlatformWindows = 0x04,
    PlatformMac = 0x08
};

// LicenseSchema
enum {
    SupportedEvaluation = 0x01,
    UnsupportedEvaluation = 0x02,
    FullSourceEvaluation = 0x04,
    FullCommercial = 0x08
};

// LicenseFeatures
enum {
    USCustomer = 0x01
};

#include <qdatetime.h>
#include <qstring.h>

/*
    Decodes the \a licenseKey.  This function returns true if 1) decoding
    was successful and 2) the checksum matches, otherwise it returns false.
*/
bool decodeLicenseKey(const QString &licenseKey,
                      uint *products,
                      uint *platforms,
                      uint *licenseSchema,
                      uint *licenseFeatures,
                      uint *licenseID,
                      QDate *expiryDate);

#endif
