/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "qpopen.h"


QPopen::QPopen()
{
}

bool QPopen::init(const char *command, const char* /* mode */)
{
    SECURITY_ATTRIBUTES attributes;

    attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    attributes.bInheritHandle = TRUE;
    attributes.lpSecurityDescriptor = NULL;

    if (! CreatePipe(&childStdOutR, &childStdOutW, &attributes, 0)) {
        return false;
    }

    // Ensure that the read handle to the child process's pipe for STDOUT is not inherited.
    SetHandleInformation( childStdOutR, HANDLE_FLAG_INHERIT, 0);

    // Create a pipe for the child process's STDIN.
    if (! CreatePipe(&childStdInR, &childStdInW, &attributes, 0)) {
        return false;
    }

    // Ensure that the write handle to the child process's pipe for STDIN is not inherited.
    SetHandleInformation( childStdInW, HANDLE_FLAG_INHERIT, 0);

    //TCHAR szCmdline[strlen(command)]=TEXT(command);
    TCHAR *szCmdLine = new TCHAR[strlen(command)+1];
    strcpy(szCmdLine, command);

    // Set up members of the PROCESS_INFORMATION structure.

    ZeroMemory( &processInfo, sizeof(PROCESS_INFORMATION) );

    // Set up members of the STARTUPINFO structure.

    ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = childStdOutW;
    siStartInfo.hStdOutput = childStdOutW;
    siStartInfo.hStdInput = childStdInR;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Create the child process.

    bool success = CreateProcess(NULL,
            szCmdLine,     // command line
            NULL,          // process security attributes
            NULL,          // primary thread security attributes
            TRUE,          // handles are inherited
            0,             // creation flags
            NULL,          // use parent's environment
            NULL,          // use parent's current directory
            &siStartInfo,  // STARTUPINFO pointer
            &processInfo);  // receives PROCESS_INFORMATION

    delete szCmdLine;

    return success;
}

QPopen::~QPopen()
{
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    CloseHandle(childStdInR);
    CloseHandle(childStdInW);
    CloseHandle(childStdOutR);
    CloseHandle(childStdOutW);
}

int QPopen::fwrite(char* buffer, int maxBytes)
{
    DWORD bytesWritten;

    bool success = WriteFile(childStdInW, buffer, maxBytes, &bytesWritten, NULL);
    if(success) {
        return bytesWritten;
    }

    return 0;
}


int QPopen::fread(char* buffer, int maxBytes)
{
    DWORD bytesRead;

    if( !CloseHandle(childStdOutW) )
        return 0;

    bool success = ReadFile(childStdOutR, buffer, maxBytes, &bytesRead, NULL);
    if(success)
        return bytesRead;

    return 0;
}

