/****************************************************************************
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_3RDPARTY_LICENSE$
**
****************************************************************************/

// MCRT0.CPP
//
// © Portions copyright (c) 2005-2006 Nokia Corporation.  All rights reserved.
// Copyright (c) Symbian Software Ltd 1997-2004.  All rights reserved.
//

// EPOC32 version of crt0.c for C programs which always want multi-threaded support

#include <e32std.h>
#include <e32base.h>
#include "estlib.h"

#ifdef __ARMCC__
__asm int CallMain(int argc, char *argv[], char *envp[])
{
       import main
       code32
       b main
}
#define CALLMAIN(argc, argv, envp) CallMain(argc, argv, envp)
#else
extern "C" int main (int argc, char *argv[], char *envp[]);
#define CALLMAIN(argc, argv, envp) main(argc, argv, envp)
#endif

// Dummy function to handle GCCE toolchain problem
extern "C" GLDEF_C int __GccGlueInit()
{
    return 0;
}

extern "C" IMPORT_C void exit (int ret);

GLDEF_C TInt QtMainWrapper()
{
    int argc=0;
    char **argv=0;
    char **envp=0;
    // get args & environment
    __crt0(argc,argv,envp);
    CleanupArrayDelete<char*>::PushL(argv);
    CleanupArrayDelete<char*>::PushL(envp);
    //Call user(application)'s main
    int ret = CALLMAIN(argc,argv,envp);
    CleanupStack::PopAndDestroy(2,argv);
    return ret;
}


#ifdef __GCC32__

/* stub function inserted into main() by GCC */
extern "C" void __gccmain (void) {}

/* Default GCC entrypoint */
extern "C" TInt _mainCRTStartup (void) 
{
    extern TInt _E32Startup();
    return _E32Startup();
}

#endif /* __GCC32__ */

#ifdef __EABI__

// standard entrypoint for C runtime, expected by some linkers
// Symbian OS does not currently use this function
extern "C" void __main() {}
#endif
