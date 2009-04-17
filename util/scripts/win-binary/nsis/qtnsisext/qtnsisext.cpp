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
#include <windows.h>
#include "exdll.h"
#include "keydec.h"
#include "licensefinder.h"
#include "binpatch.h"
#include "mingw.h"
#include <stdio.h>
#include <time.h>

HINSTANCE g_hInstance;
HWND g_hwndParent;

#define EXPORT_NSIS_FUNCTION(funcName) \
extern "C" void __declspec(dllexport) funcName(HWND hwndParent, int string_size, \
                                               char *variables, stack_t **stacktop)

BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
    g_hInstance=static_cast<HINSTANCE>(hInst);
    return TRUE;
}

EXPORT_NSIS_FUNCTION(IsValidLicense)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    char isValid[2];
    char *key = (char *)LocalAlloc(LPTR, g_stringsize+1);
    popstring(key);

    KeyDecoder kdec(key);
    if (kdec.IsValid() && (
               kdec.getPlatforms() & KeyDecoder::Windows
            || kdec.getPlatforms() & KeyDecoder::Embedded
            || kdec.getPlatforms() & KeyDecoder::WinCE))
        strcpy(isValid, "1");
    else
        strcpy(isValid, "0");

    LocalFree(key);
    pushstring(isValid);
}

EXPORT_NSIS_FUNCTION(IsValidWinCELicense)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    char isValid[2];
    char *key = (char *)LocalAlloc(LPTR, g_stringsize+1);
    popstring(key);

    KeyDecoder kdec(key);
    if (kdec.IsValid() && (kdec.getPlatforms() & KeyDecoder::WinCE))
        strcpy(isValid, "1");
    else
        strcpy(isValid, "0");

    LocalFree(key);
    pushstring(isValid);
}

EXPORT_NSIS_FUNCTION(HasLicenseProduct)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    char retVal[2];
    char *key = (char *)LocalAlloc(LPTR, g_stringsize+1);
    char *product = (char *)LocalAlloc(LPTR, g_stringsize+1);
    uint qtproduct = 0;

    popstring(key);
    popstring(product);

    for (int i=0; i<KeyDecoder::NumberOfProducts; ++i) {
        if (KeyDecoder::Products[i] != 0 && stricmp(product, KeyDecoder::Products[i]) == 0) {
            qtproduct = (1 << i);
            break;
        }
    }

    KeyDecoder kdec(key);
    if (kdec.IsValid()
        && (qtproduct != 0)
        && (kdec.getProducts() & qtproduct)) {
        strcpy(retVal, "1");
    } else {
        strcpy(retVal, "0");
    }

    LocalFree(product);
    LocalFree(key);
    pushstring(retVal);
}

EXPORT_NSIS_FUNCTION(GetLicenseID)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    char *key = (char *)LocalAlloc(LPTR, g_stringsize+1);
    popstring(key);

    char lcnsid[512];
    lcnsid[0] = '\0';

    KeyDecoder kdec(key);
    if (kdec.IsValid()) {
        uint qtlcnsid = kdec.getLicenseID();
        sprintf(lcnsid, "%d", qtlcnsid);
    }

    LocalFree(key);
    pushstring(lcnsid);
}

EXPORT_NSIS_FUNCTION(GetLicenseProduct)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    char *key = (char *)LocalAlloc(LPTR, g_stringsize+1);
    popstring(key);

    char lcnsprod[512];
    lcnsprod[0] = '\0';

    KeyDecoder kdec(key);
    if (kdec.IsValid()) {
        uint qtschema = kdec.getLicenseSchema();
        if(qtschema & KeyDecoder::SupportedEvaluation)
            strcpy(lcnsprod, "SupportedEvaluation");
        else if(qtschema & KeyDecoder::UnsupportedEvaluation)
            strcpy(lcnsprod, "UnsupportedEvaluation");
        else if(qtschema & KeyDecoder::FullSourceEvaluation)
            strcpy(lcnsprod, "FullSourceEvaluation");
        else if(qtschema & KeyDecoder::Academic)
            strcpy(lcnsprod, "Academic");
        else if(qtschema & KeyDecoder::Educational)
            strcpy(lcnsprod, "Educational");
        else if(qtschema & KeyDecoder::FullCommercial)
        {
            uint qtproduct = kdec.getProducts();
            if (qtproduct & KeyDecoder::QtUniversal)
                strcpy(lcnsprod, "Universal");
            else if(qtproduct & KeyDecoder::QtDesktop)
                strcpy(lcnsprod, "Desktop");
            else if(qtproduct & KeyDecoder::QtDesktopLight)
                strcpy(lcnsprod, "DesktopLight");
            else if(qtproduct & KeyDecoder::QtConsole)
                strcpy(lcnsprod, "Console");
        }
    }

    LocalFree(key);
    pushstring(lcnsprod);
}

EXPORT_NSIS_FUNCTION(GetLicensePlatform)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    char *key = (char *)LocalAlloc(LPTR, g_stringsize+1);
    popstring(key);

    char lcnsplat[512];
    lcnsplat[0] = '\0';

    KeyDecoder kdec(key);
    if (kdec.getPlatforms() == KeyDecoder::AllOS)
        strcpy(lcnsplat, "AllOS");
    else if (kdec.getPlatforms() & KeyDecoder::Embedded)
        strcpy(lcnsplat, "Embedded");
    else if (kdec.getPlatforms() & KeyDecoder::WinCE)
        strcpy(lcnsplat, "Embedded");
    else if (kdec.getPlatforms() & KeyDecoder::Windows)
        strcpy(lcnsplat, "Windows");

    LocalFree(key);
    pushstring(lcnsplat);
}

EXPORT_NSIS_FUNCTION(UsesUSLicense)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    char isUSCustomer[2];
    char *key = (char *)LocalAlloc(LPTR, g_stringsize+1);
    popstring(key);

    KeyDecoder kdec(key);
    if (kdec.IsValid()
        && (kdec.getLicenseFeatures() & KeyDecoder::USCustomer))
        strcpy(isUSCustomer, "1");
    else
        strcpy(isUSCustomer, "0");

    LocalFree(key);
    pushstring(isUSCustomer);
}

EXPORT_NSIS_FUNCTION(IsValidDate)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    int year = 0;
    int month = 0;
    int day = 0;
    char isValid[2];
    char *key = (char *)LocalAlloc(LPTR, g_stringsize+1);
    char *pkgDate = (char *)LocalAlloc(LPTR, g_stringsize+1);

    popstring(key);
    popstring(pkgDate);

    if (strlen(pkgDate) > 0)
    {
        sscanf(pkgDate, "%d-%d-%d", &year, &month, &day);
    } else {
        //use current date
        char curDate[9];
        _strdate(curDate);
        sscanf(curDate, "%d/%d/%d", &month, &day, &year);
        year += 2000;
    }

    KeyDecoder kdec(key);
    CDate expiryDate = kdec.getExpiryDate();
    if (year == expiryDate.year()) {
        if (month == expiryDate.month()) {
            if (day <= expiryDate.day()) {
                strcpy(isValid, "1");
            } else {
                strcpy(isValid, "0");
            }
        } else if (month < expiryDate.month()) {
            strcpy(isValid, "1");
        } else {
            strcpy(isValid, "0");
        }
    } else if (year < expiryDate.year()) {
        strcpy(isValid, "1");
    } else {
        strcpy(isValid, "0");
    }

    LocalFree(pkgDate);
    LocalFree(key);
    pushstring(isValid);
}

EXPORT_NSIS_FUNCTION(IsFloatingLicense)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    char *key = (char *)LocalAlloc(LPTR, g_stringsize+1);
    popstring(key);
    char isFloatingLicense[2];

    KeyDecoder kdec(key);
    if (kdec.IsValid() &&
        kdec.getLicenseFeatures() & KeyDecoder::FloatingLicense)
            strcpy(isFloatingLicense, "1");
    else
        strcpy(isFloatingLicense, "0");

    LocalFree(key);
    pushstring(isFloatingLicense);
}

EXPORT_NSIS_FUNCTION(GetLicenseInfo)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    LicenseFinder f;
    pushstring(f.getLicenseKey());
    pushstring(f.getOldLicenseKey());
    pushstring(f.getLicensee());
    pushstring(f.getCustomerID());
    pushstring(f.getProducts());
    pushstring(f.getExpiryDate());
}

EXPORT_NSIS_FUNCTION(PatchVC6Binary)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    char *fileName = (char *)LocalAlloc(LPTR, g_stringsize+1);
    char *oldStr = (char *)LocalAlloc(LPTR, g_stringsize+1);
    char *newStr = (char *)LocalAlloc(LPTR, g_stringsize+1);

    popstring(fileName);
    popstring(oldStr);
    popstring(newStr);

    // remove last separator...
    int oldLen = (int)strlen(oldStr);
    int newLen = (int)strlen(newStr);
    if (oldStr[oldLen-1] == '\\')
        oldStr[oldLen-1] = '\0';
    if (newStr[newLen-1] == '\\')
        newStr[newLen-1] = '\0';

    BinPatch binFile(fileName);
    binFile.enableInsertReplace(true);
    binFile.enableUseLength(true);
    binFile.setEndTokens(".cpp;.h;.moc;.pdb");
    binFile.patch(oldStr, newStr);

    //patch also with path sep. the other way since
    //vc60 in some cases uses different path separators :|
    char *reverse = (char *)malloc(sizeof(char)*(oldLen+1));
    for (int i=0; i<oldLen; ++i) {
        if (oldStr[i] == '\\')
            reverse[i] = '/';
        else
            reverse[i] = oldStr[i];
    }
    reverse[oldLen] = '\0';
    binFile.patch(reverse, newStr);

    LocalFree(newStr);
    LocalFree(oldStr);
    LocalFree(fileName);
}

EXPORT_NSIS_FUNCTION(PatchVC7Binary)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    char *fileName = (char *)LocalAlloc(LPTR, g_stringsize+1);
    char *oldStr = (char *)LocalAlloc(LPTR, g_stringsize+1);
    char *newStr = (char *)LocalAlloc(LPTR, g_stringsize+1);

    popstring(fileName);
    popstring(oldStr);
    popstring(newStr);

    BinPatch binFile(fileName);
    binFile.enableInsertReplace(true);
    binFile.setEndTokens(".cpp;.h;.moc;.pdb");
    binFile.patch(oldStr, newStr);

    LocalFree(newStr);
    LocalFree(oldStr);
    LocalFree(fileName);
}

EXPORT_NSIS_FUNCTION(PatchBinary)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    char *fileName = (char *)LocalAlloc(LPTR, g_stringsize+1);
    char *oldStr = (char *)LocalAlloc(LPTR, g_stringsize+1);
    char *newStr = (char *)LocalAlloc(LPTR, g_stringsize+1);

    popstring(fileName);
    popstring(oldStr);
    popstring(newStr);

    BinPatch binFile(fileName);
    binFile.patch(oldStr, newStr);

    LocalFree(newStr);
    LocalFree(oldStr);
    LocalFree(fileName);
}

EXPORT_NSIS_FUNCTION(ShowLicenseFile)
{
    char licenseBuffer[60000];
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    void *hCtrl = 0;
    char *strCtrl = (char *)LocalAlloc(LPTR, g_stringsize+1);
    char *strLicenseFile = (char *)LocalAlloc(LPTR, g_stringsize+1);
    popstring(strCtrl);
    popstring(strLicenseFile);

    if (sscanf(strCtrl, "%d", &hCtrl) == 1) {
        FILE *fIn = fopen(strLicenseFile, "rb");
        if (fIn) {
            size_t r = fread(licenseBuffer, sizeof(char), 59999, fIn);
            licenseBuffer[r] = '\0';
            fclose(fIn);
            SendMessage((HWND)hCtrl, (UINT)WM_SETTEXT, 0, (LPARAM)licenseBuffer);
        }
    }

    LocalFree(strLicenseFile);
    LocalFree(strCtrl);
}

EXPORT_NSIS_FUNCTION(HasValidWin32Library)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    char isValid[2];
    char *path = (char *)LocalAlloc(LPTR, g_stringsize+1);
    popstring(path);

    if (hasValidIncludeFiles(path))
        strcpy(isValid, "1");
    else
        strcpy(isValid, "0");

    LocalFree(path);
    pushstring(isValid);
}

EXPORT_NSIS_FUNCTION(GetMinGWVersion)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();

    char *path = (char *)LocalAlloc(LPTR, g_stringsize+1);
    popstring(path);

    int major = 0, minor = 0, patch = 0;
    char versionstr[MINGW_BUFFER_SIZE];

    getMinGWVersion(path, &major, &minor, &patch);
    sprintf(versionstr, "%d.%d.%d", major, minor, patch);

    LocalFree(path);
    pushstring(versionstr);
}

EXPORT_NSIS_FUNCTION(ShInPath)
{
    g_hwndParent = hwndParent;
    EXDLL_INIT();
    char res[2];

    if (shInEnvironment())
        res[0] = '1';
    else
        res[0] = '0';

    res[1] = '\0';

    pushstring(res);
}
