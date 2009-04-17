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
#include "binpatch.h"
#include <stdio.h>

bool patchBinaryWithQtPathes(const char *fileName, const char *baseQtPath)
{
    bool result = true;

    static const struct
    {
        const char *variable;
        const char *subDirectory;
    } variables[] = {
        {"qt_prfxpath=", ""},
        {"qt_docspath=", "/doc"},
        {"qt_hdrspath=", "/include"},
        {"qt_libspath=", "/lib"},
        {"qt_binspath=", "/bin"},
        {"qt_plugpath=", "/plugins"},
        {"qt_datapath=", ""},
        {"qt_trnspath=", "/translations"},
        {"qt_xmplpath=", "/examples"},
        {"qt_demopath=", "/demos"}
    };

    for (int i = 0; i < sizeof(variables)/sizeof(variables[0]); i++) {
        char newStr[256] = "";
        strncpy(newStr, variables[i].variable, sizeof(newStr));
        newStr[sizeof(newStr) - 1] = 0;
        strncat(newStr, baseQtPath, sizeof(newStr) - strlen(newStr) - 1);
        newStr[sizeof(newStr) - 1] = 0;
        strncat(newStr, variables[i].subDirectory, sizeof(newStr) - strlen(newStr) - 1);
        newStr[sizeof(newStr) - 1] = 0;
        BinPatch binFile(fileName);
        if (!binFile.patch(variables[i].variable, newStr)) {
            result = false;
            break;
        }
    }

    return result;
}

bool patchBinariesWithQtPathes(const char *baseQtPath)
{
    bool result = true;

    static const char *filesToPatch[] = {
        "/bin/qmake.exe",
        "/bin/QtCore4.dll",
        "/bin/QtCored4.dll"
    };

    for (int i = 0; i < sizeof(filesToPatch)/sizeof(filesToPatch[0]); i++) {
        char fileName[FILENAME_MAX] = "";
        strncpy(fileName, baseQtPath, sizeof(fileName));
        fileName[sizeof(fileName)-1] = '\0';
        strncat(fileName, filesToPatch[i], sizeof(fileName) - strlen(fileName) - 1);
        fileName[sizeof(fileName)-1] = '\0';
        if (!patchBinaryWithQtPathes(fileName, baseQtPath)) {
            result = false;
            break;
        }
    }

    return result;
}

bool patchDebugLibrariesWithQtPath(const char *baseQtPath)
{
    bool result = true;

    static const struct
    {
        const char *fileName;
        const char *sourceLocation;
    } libraries[] = {
        {"/bin/Qt3Supportd4.dll", "/src/qt3support/"},
        {"/bin/QtCored4.dll", "/src/corelib/"},
        {"/bin/QtGuid4.dll", "/src/gui/"},
        {"/bin/QtHelpd4.dll", "/tools/assistant/lib/"},
        {"/bin/QtNetworkd4.dll", "/src/network/"},
        {"/bin/QtOpenGLd4.dll", "/src/opengl/"},
        {"/bin/QtScriptd4.dll", "/src/script/"},
        {"/bin/QtSqld4.dll", "/src/sql/"},
        {"/bin/QtSvgd4.dll", "/src/svg/"},
        {"/bin/QtTestd4.dll", "/src/testlib/"},
        {"/bin/QtWebKitd4.dll", "/src/3rdparty/webkit/WebCore/"},
        {"/bin/QtXmld4.dll", "/src/xml/"},
        {"/bin/QtXmlPatternsd4.dll", "/src/xmlpatterns/"},
        {"/plugins/accessible/qtaccessiblecompatwidgetsd4.dll", "/src/plugins/accessible/compat/"},
        {"/plugins/accessible/qtaccessiblewidgetsd4.dll", "/src/plugins/accessible/widgets/"},
        {"/plugins/codecs/qcncodecsd4.dll", "/src/plugins/codecs/cn/"},
        {"/plugins/codecs/qjpcodecsd4.dll", "/src/plugins/codecs/jp/"},
        {"/plugins/codecs/qkrcodecsd4.dll", "/src/plugins/codecs/kr/"},
        {"/plugins/codecs/qtwcodecsd4.dll", "/src/plugins/codecs/tw/"},
        {"/plugins/iconengines/qsvgicond4.dll", "/src/plugins/iconengines/svgiconengine/"},
        {"/plugins/imageformats/qgifd4.dll", "/src/plugins/imageformats/gif/"},
        {"/plugins/imageformats/qjpegd4.dll", "/src/plugins/imageformats/jpeg/"},
        {"/plugins/imageformats/qmngd4.dll", "/src/plugins/imageformats/mng/"},
        {"/plugins/imageformats/qsvgd4.dll", "/src/plugins/imageformats/svg/"},
        {"/plugins/imageformats/qtiffd4.dll", "/src/plugins/imageformats/tiff/"},
        {"/plugins/sqldrivers/qsqlited4.dll", "/src/plugins/sqldrivers/sqlite/"},
//        {"/plugins/sqldrivers/qsqlodbcd4.dll", "/src/plugins/sqldrivers/odbc/"}
    };

    for (int i = 0; i < sizeof(libraries)/sizeof(libraries[0]); i++) {
        char fileName[FILENAME_MAX] = "";
        strncpy(fileName, baseQtPath, sizeof(fileName));
        fileName[sizeof(fileName)-1] = '\0';
        strncat(fileName, libraries[i].fileName, sizeof(fileName) - strlen(fileName) - 1);
        fileName[sizeof(fileName)-1] = '\0';

        char oldSourcePath[FILENAME_MAX] =
            "C:/depot/qt-workbench/Trolltech/Code_less_create_more/Trolltech/Code_less_create_more/Trolltech";
        strncat(oldSourcePath, libraries[i].sourceLocation, sizeof(oldSourcePath) - strlen(oldSourcePath) - 1);
        oldSourcePath[sizeof(oldSourcePath)-1] = '\0';

        char newSourcePath[FILENAME_MAX] = "";
        strncpy(newSourcePath, baseQtPath, sizeof(newSourcePath));
        newSourcePath[sizeof(newSourcePath)-1] = '\0';
        strncat(newSourcePath, libraries[i].sourceLocation, sizeof(newSourcePath) - strlen(newSourcePath) - 1);
        newSourcePath[sizeof(newSourcePath)-1] = '\0';

        BinPatch binFile(fileName);
        if (!binFile.patch(oldSourcePath, newSourcePath)) {
            result = false;
            break;
        }
    }

    return result;
}

int main(int argc, char *args[])
{
    if (argc != 2) {
        printf("Please provide a QTDIR value as parameter.\n"
               "This is also the location where the binaries are expected\n"
               "in the \"/bin\" and \"/plugins\" subdirectories.\n");
        return 1;
    }

    char baseQtPath[FILENAME_MAX] = "";
    strncpy(baseQtPath, args[1], sizeof(baseQtPath));
    baseQtPath[sizeof(baseQtPath)-1] = '\0';

    // Convert backslash to slash
    for (char *p = baseQtPath; *p != '\0'; p++)
        if (*p == '\\')
            *p = '/';

    // Remove trailing slash(es)
    for (char *p = baseQtPath + strlen(baseQtPath) - 1; p != baseQtPath; p--)
        if (*p == '/')
            *p = '\0';
        else
            break;

    return patchBinariesWithQtPathes(baseQtPath) && patchDebugLibrariesWithQtPath(baseQtPath)?0:1;
}
