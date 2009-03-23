/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
#if defined(_WIN32_WCE)
    // no environment in Windows CE
    return 0;
#else
    if (argc == 1)
        return 1;

    char *env = getenv(argv[1]);
    if (env) {
        printf("%s", env);
        return 0;
    }
    return 1;
#endif
}
