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

#ifndef QPOPEN_H
#define QPOPEN_H

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>

class QPopen
{
public:
    QPopen();
    ~QPopen();
    int fread(char* buffer, int maxBytes);
    int fwrite(char* buffer, int maxBytes);
    bool init(const char* command, const char* mode);

private:
    // This pair of handles represent the "Write" pipe
    // ie: Parent -> Child
    HANDLE childStdInR;
    HANDLE childStdInW;

    // This pair of handles represent the "Read" pipe
    // ie: Child -> Parent
    HANDLE childStdOutR;
    HANDLE childStdOutW;
    PROCESS_INFORMATION processInfo;
    STARTUPINFO siStartInfo;
};

#endif
