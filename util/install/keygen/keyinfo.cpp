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

#include <qregexp.h>

#include "keyinfo.h"

// Magic values
enum {
    ProductMagic = 0xB292,
    PlatformMagic = 0x5C7E,
    LicenseSchemaMagic = 0xE3B,
    LicenseIDMagic = 0xCE57,
    FeatureMagic = 0x4D5
};

static const char Xalphabet[] = "WX9MUEC7AJH3KS6DB4YFG2L5PQRT8VNZ";
static const int XAlphabetSize = sizeof(Xalphabet) - 1;

static QString encodeBaseX(uint k)
{
    QString str;
    do {
	str += QChar(Xalphabet[ k % XAlphabetSize ]);
	k /= XAlphabetSize;
    } while (k > 0u);
    return str;
}

static uint decodeBaseX(const QString &str)
{
    uint k = 0;
    int i = str.length();
    while (i > 0) {
	i--;
	const char *p = strchr(Xalphabet, str[i].unicode());
	if (p == 0) {
	    return 0;
	} else {
	    k = (k * XAlphabetSize) + (p - Xalphabet);
	}
    }
    return k;
}

static const QDate StartDate(2001, 1, 1);
static const uint MaxDays = 4000;
    
static QString encodedExpiryDate(const QDate &date)
{
    uint days = StartDate.daysTo(date);
    if (days >= MaxDays)
	days = MaxDays - 1;
    uint x = (days << 7) ^ days;
    return encodeBaseX(x ^ 0x0000beef);
}

static QDate decodedExpiryDate(const QString &encodedDate)
{
    QDate date;
    uint y = decodeBaseX(encodedDate);
    uint x = y ^ 0x0000beef;
    uint days = ( (x >> 7) ^ x ) >> 7;
    if (days >= MaxDays)
        return QDate();
    date = StartDate.addDays( days );
    if (encodedExpiryDate(date) != encodedDate)
        return QDate();
    return date;
}

bool decodeLicenseKey(const QString &licenseKey,
                      uint *products,
                      uint *platforms,
                      uint *licenseSchema,
                      uint *licenseFeatures,
                      uint *licenseID,
                      QDate *expiryDate)
{
    QStringList licenseParts;
    int dash = -1;
    do {
        int start = dash + 1;
        dash = licenseKey.indexOf('-', start);
        licenseParts.append(licenseKey.mid(start, dash - start));
    } while (dash != -1);

    // license keys have 7 fields
    if (licenseParts.size() != 7) {
        // invalid key
        return false;
    }
    QString productPart = licenseParts.at(0);
    QString platformPart = licenseParts.at(1);
    QString licenseSchemaPart = licenseParts.at(2);
    QString licenseFeaturesPart = licenseParts.at(3);
    QString licenseIDPart = licenseParts.at(4);
    QString expiryDatePart = licenseParts.at(5);
    QString checksumPart = licenseParts.at(6);
    QString keyPart = productPart
                      + '-' + platformPart
                      + '-' + licenseSchemaPart
                      + '-' + licenseFeaturesPart
                      + '-' + licenseIDPart
                      + '-' + expiryDatePart;

    // verify the crc
    QByteArray ba = keyPart.toLatin1();
    int crc = qChecksum(ba.constData(), ba.size());
    QString checksumVerification = QString("%1%2")
                                   .arg((crc & 0xff),2,16,QChar('0'))
                                   .arg((crc >> 8 & 0xff),2,16,QChar('0'))
                                   .toUpper();
    if (checksumPart != checksumVerification) {
        // invalid checksum
        return false;
    }

    *products = decodeBaseX(productPart) ^ ProductMagic;
    *platforms = decodeBaseX(platformPart) ^ PlatformMagic;
    *licenseSchema = decodeBaseX(licenseSchemaPart) ^ LicenseSchemaMagic;
    *licenseFeatures = decodeBaseX(licenseFeaturesPart) ^ FeatureMagic;
    *licenseID = decodeBaseX(licenseIDPart) ^ LicenseIDMagic;
    *expiryDate = decodedExpiryDate(expiryDatePart);
    return true;
}
