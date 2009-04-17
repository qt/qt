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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "keydec.h"

#define MAX_STRSIZE 256
#define NUMBER_OF_PARTS 7

const ushort KeyDecoder::crc_tbl[] = {
    0x0000, 0x1081, 0x2102, 0x3183,
    0x4204, 0x5285, 0x6306, 0x7387,
    0x8408, 0x9489, 0xa50a, 0xb58b,
    0xc60c, 0xd68d, 0xe70e, 0xf78f
};

const char KeyDecoder::Xalphabet[] = "WX9MUEC7AJH3KS6DB4YFG2L5PQRT8VNZ";
const int KeyDecoder::XAlphabetSize = sizeof(Xalphabet) - 1;
const CDate KeyDecoder::StartDate = CDate(2001, 1, 1);
const uint KeyDecoder::MaxDays = 4000;

const char *KeyDecoder::Products[] = {
    "Qt Universal",
    "Qt Desktop",
    "Qt Desktop Light",
    "Qt Console",
    "Qt Designer Only",
    0,
    0,
    0,
    0,
    0,

    "Qtopia PDA",
    "Qtopia Phone",
    "Qtopia Reference Board",
    0,
    0,
    0,
    0,
    0,
    0,
    0,

    "Teambuilder",
    "Solutions",
    "QSA",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

const char *KeyDecoder::Platforms[] = {
    "X11",
    "Embedded",
    "Windows",
    "Mac",
    "Windows CE"
};

const char *KeyDecoder::LicenseSchemas[] = {
    "Supported Evaluation",
    "Unsupported Evaluation",
    "Full Source Evaluation",
    "Full Commercial",
    "Academic",
    "Educational"
};

const char *KeyDecoder::LicenseFeatures[] = {
    "US Customer",
    "Floating License",
    0,
    0
};

const int KeyDecoder::NumberOfProducts = sizeof(Products) / sizeof(char *);
const int KeyDecoder::NumberOfPlatforms = sizeof(Platforms) / sizeof(char *);
const int KeyDecoder::NumberOfLicenseSchemas = sizeof(LicenseSchemas) / sizeof(char *);
const int KeyDecoder::NumberOfLicenseFeatures = sizeof(LicenseFeatures) / sizeof(char *);

CDate::CDate() : m_jd(0)
{
}

CDate::CDate(int y, int m, int d)
{
    m_jd = gregorianToJulian(y, m, d);
}

CDate::CDate(CDate *d)
{
    m_jd = d->m_jd;
}

CDate::CDate(uint julianDays)
{
    m_jd = julianDays;
}

int CDate::year() const
{
    int y, m, d;
    julianToGregorian(m_jd, y, m, d);
    return y;
}

int CDate::month() const
{
    int y, m, d;
    julianToGregorian(m_jd, y, m, d);
    return m;
}

int CDate::day() const
{
    int y, m, d;
    julianToGregorian(m_jd, y, m, d);
    return d;
}

uint CDate::gregorianToJulian(int y, int m, int d) const
{
    uint c, ya;
    if (y <= 99)
        y += 1900;
    if (m > 2) {
        m -= 3;
    } else {
        m += 9;
        y--;
    }
    c = y;                                        // NOTE: Sym C++ 6.0 bug
    c /= 100;
    ya = y - 100*c;
    return 1721119 + d + (146097*c)/4 + (1461*ya)/4 + (153*m+2)/5;
}

void CDate::julianToGregorian(uint jd, int &y, int &m, int &d) const
{
    uint x;
    uint j = jd - 1721119;
    y = (j*4 - 1)/146097;
    j = j*4 - 146097*y - 1;
    x = j/4;
    j = (x*4 + 3) / 1461;
    y = 100*y + j;
    x = (x*4) + 3 - 1461*j;
    x = (x + 4)/4;
    m = (5*x - 3)/153;
    x = 5*x - 3 - 153*m;
    d = (x + 5)/5;
    if (m < 10) {
        m += 3;
    } else {
        m -= 9;
        y++;
    }
}

void KeyDecoder::encodeBaseX(uint k, char *str)
{
    memset(str, 0, MAX_STRSIZE);

    do {
        str[strlen(str)] = Xalphabet[ k % XAlphabetSize ];
	k /= XAlphabetSize;
    } while (k > 0u);
}

uint KeyDecoder::decodeBaseX(const char *str)
{
    uint k = 0;
    int i = (int)strlen(str);
    while (i > 0) {
	i--;
	const char *p = strchr(Xalphabet, str[i]);
	if (p == 0) {
	    return 0;
	} else {
	    k = (k * XAlphabetSize) + (p - Xalphabet);
	}
    }
    return k;
}

void KeyDecoder::encodedExpiryDate(const CDate &date, char *str)
{
    encodeBaseX(date.julianDate() ^ ExpiryDateMagic, str);
}

CDate KeyDecoder::decodedExpiryDate(const char *encodedDate)
{
    uint y = decodeBaseX(encodedDate);
    uint x = y ^ ExpiryDateMagic;

    CDate date(x);

    char str[MAX_STRSIZE];
    encodedExpiryDate(date, str);

    if (strcmp(encodedDate, str) != 0)
        return CDate();
    return date;
}

ushort KeyDecoder::qChecksum(const char *data, uint len)
{
    ushort crc = 0xffff;
    uchar c;
    const uchar *p = reinterpret_cast<const uchar *>(data);
    while (len--) {
        c = *p++;
        crc = ((crc >> 4) & 0x0fff) ^ crc_tbl[((crc ^ c) & 15)];
        c >>= 4;
        crc = ((crc >> 4) & 0x0fff) ^ crc_tbl[((crc ^ c) & 15)];
    }
    return ~crc & 0xffff;
}

KeyDecoder::KeyDecoder(const char *clicenseKey)
    : products(0), platforms(0), licenseSchema(0), licenseFeatures(0), licenseID(0), m_valid(false)
{
    static const int MAXCHARS = 7;
    static const char * const SEP = "-";

#ifndef _WIN32_WCE
    char *buffer = strdup(clicenseKey);
#else
    char *buffer = (char*) malloc(strlen(clicenseKey) + 1);
    strcpy(buffer, clicenseKey);
#endif
    char *licenseParts[NUMBER_OF_PARTS];
    int partNumber = 0;

    for (char *part = strtok(buffer, SEP); part != 0; part = strtok(0, SEP))
        licenseParts[partNumber++] = part;

    if (partNumber < (NUMBER_OF_PARTS-1)) {
        free(buffer);
        return; //invalid key
    }

    int crcPeices = NUMBER_OF_PARTS - 1;
    char *crcCheckKey = (char*)calloc(crcPeices * MAXCHARS + crcPeices, sizeof(char));
    for (int i = 0; i < crcPeices; ++i) {
        if (i != 0)
            strncat(crcCheckKey, SEP, 1);
        strncat(crcCheckKey, licenseParts[i], MAXCHARS);
    }

    int crc = qChecksum(crcCheckKey, (uint)strlen(crcCheckKey));

    char checksumVerification[5];
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
    _snprintf(checksumVerification, 5, "%.2X%.2X", (crc & 0xff), (crc >> 8 & 0xff));
#else
    snprintf(checksumVerification, 5, "%.2X%.2X", (crc & 0xff), (crc >> 8 & 0xff));
#endif

    if (strncmp(checksumVerification, licenseParts[6], 5) != 0) {
        free(buffer);
        free(crcCheckKey);
        return; //invalid checksum
    }

    products = decodeBaseX(licenseParts[0]) ^ ProductMagic;
    platforms = decodeBaseX(licenseParts[1]) ^ PlatformMagic;
    licenseSchema = decodeBaseX(licenseParts[2]) ^ LicenseSchemaMagic;
    licenseFeatures = decodeBaseX(licenseParts[3]) ^ FeatureMagic;
    licenseID = decodeBaseX(licenseParts[4]) ^ LicenseIDMagic;
    expiryDate = decodedExpiryDate(licenseParts[5]);

    m_valid = true;
    free(buffer);
    free(crcCheckKey);
}
