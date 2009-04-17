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
#ifndef LICENSEFINDER_H
#define LICENSEFINDER_H

#define MAX_KEY_LENGTH 512
#define MAX_LICENSEE_LENGTH 512
#define MAX_QT3INFO_LENGTH 512

typedef unsigned long ulong;

class LicenseFinder
{
public:
    LicenseFinder();
    char *getLicenseKey();
    char *getOldLicenseKey();
    char *getLicensee();
    char *getCustomerID();
    char *getProducts();
    char *getExpiryDate();

private:
    void searchLicense();
    bool lookInDirectory(const char* dir);
    char *findPattern(char *h, const char *n, ulong hlen);
    bool searched;
    char m_key[MAX_KEY_LENGTH];
    char m_oldkey[MAX_KEY_LENGTH];
    char licensee[MAX_LICENSEE_LENGTH];
    char m_customerId[MAX_QT3INFO_LENGTH];
    char m_products[MAX_QT3INFO_LENGTH];
    char m_expiryDate[MAX_QT3INFO_LENGTH];
};

#endif