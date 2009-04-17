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
#include <io.h>
#include <stdio.h>
#include "mingw.h"

HANDLE hChildStdoutWr, hChildStdoutRd;

#define VERSION_SIZE 30
#define WIN32_VERSION_STRING "__W32API_VERSION 3.2"

BOOL CreateChildProcess(char *command) 
{ 
    PROCESS_INFORMATION piProcInfo; 
    STARTUPINFOA siStartInfo;
    BOOL bFuncRetn = FALSE; 

    ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

    ZeroMemory( &siStartInfo, sizeof(STARTUPINFOA) );
    siStartInfo.cb = sizeof(STARTUPINFOA); 
    siStartInfo.hStdError = hChildStdoutWr;
    siStartInfo.hStdOutput = hChildStdoutWr;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    bFuncRetn = CreateProcessA(NULL, 
        command,
        NULL,            // process security attributes 
        NULL,            // thread security attributes 
        TRUE,            // inherit handles
        CREATE_NO_WINDOW,
        NULL,            // use environment 
        NULL,            // use current directory 
        &siStartInfo, 
        &piProcInfo);

    if (bFuncRetn == 0) 
        return 0;
    else 
    {
        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);
        return bFuncRetn;
    }
}

void getMinGWVersion(char *path, int *major, int *minor, int *patch)
{
    char command[MINGW_BUFFER_SIZE];
    char instr[VERSION_SIZE];

    strcpy(command, path);
    strcat(command, "\\bin\\gcc.exe --version");

    SECURITY_ATTRIBUTES saAttr; 
 
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL; 

    if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)) 
        return;;

    if (CreateChildProcess(command) == 0)
        return;

    DWORD nBytes = 0;
    ReadFile(hChildStdoutRd, instr, VERSION_SIZE, &nBytes, NULL);
    instr[VERSION_SIZE-1] = '\0'; 

    char *gcc = strstr(instr, "(GCC)");
    if (gcc == NULL)
        return;

   sscanf(gcc, "(GCC) %d.%d.%d ", major, minor, patch);
}

bool hasValidIncludeFiles(char *path)
{
    char filename[MINGW_BUFFER_SIZE];
    char buffer[MINGW_BUFFER_SIZE];

    strcpy(filename, path);
    strcat(filename, "\\include\\w32api.h");

    FILE *finc;
    if ((finc = fopen(filename, "rb")) == NULL)
        return false;

    memset(buffer, '\0', sizeof(char)*MINGW_BUFFER_SIZE);
    fread(buffer, sizeof(char), MINGW_BUFFER_SIZE-1, finc);

    if (strstr(buffer, WIN32_VERSION_STRING) != NULL)
        return true;

    return false;
}

bool shInEnvironment()
{
    char chpath[_MAX_PATH];
    char *env = getenv("PATH");
    char seps[] = ";";
    char *path;

    path = strtok(env, seps);
    while(path != NULL)
    {
        sprintf(chpath, "%s\\sh.exe", path);
        if(_access(chpath, 0) != -1)
            return true;

        path = strtok(NULL, seps);
    }

    return false;
}
