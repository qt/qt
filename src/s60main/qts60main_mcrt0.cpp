/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Symbian application wrapper of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

// MCRT0.CPP
//
// Portions copyright (c) 2005-2006 Nokia Corporation.  All rights reserved.
// Copyright (c) Symbian Software Ltd 1997-2004.  All rights reserved.
//

// EPOC32 version of crt0.c for C programs which always want multi-threaded support

#include <e32std.h>
#include <exception> // must be before e32base.h so uncaught_exception gets defined
#include <e32base.h>
#include "estlib.h"
#include <string>


#ifdef __ARMCC__
__asm int CallMain(int argc, char *argv[], char *envp[])
{
    import main
    code32
    b main
}
#define CALLMAIN(argc, argv, envp) CallMain(argc, argv, envp)
#else
extern "C" int main(int argc, char *argv[], char *envp[]);
#define CALLMAIN(argc, argv, envp) main(argc, argv, envp)
#endif

// Dummy function to handle GCCE toolchain problem
extern "C" GLDEF_C int __GccGlueInit()
{
    return 0;
}

extern "C" IMPORT_C void exit(int ret);

namespace {
class QSymbianLeaveException : public std::exception
{
public:
    inline QSymbianLeaveException(int err) : error(err) {}
    inline const char* what() const throw() { return "Symbian leave exception"; }

public:
    int error;
};
}

GLDEF_C TInt QtMainWrapper()
{
    int argc = 0;
    char **argv = 0;
    char **envp = 0;
    // get args & environment
    __crt0(argc, argv, envp);
    //Call user(application)'s main

    TInt _err = KErrNone;
    TRAPD(ret,
        try {
            ret = CALLMAIN(argc, argv, envp);
        } catch (const std::exception &____ex) {
            _err = KErrGeneral;
            const std::type_info& type = typeid(____ex);

            if (type == typeid (std::bad_alloc)) {
                _err = KErrNoMemory;
            } else if (type == typeid(::QSymbianLeaveException)) {
                _err = static_cast<const ::QSymbianLeaveException&>(____ex).error;
            } else {
                if (type == typeid(std::invalid_argument))
                    _err =  KErrArgument;
                else if (type == typeid(std::out_of_range))
                    // std::out_of_range is of type logic_error which by definition means that it is
                    // "presumably detectable before the program executes".
                    // std::out_of_range is used to report an argument is not within the expected range.
                    // The description of KErrArgument says an argument is out of range. Hence the mapping.
                    _err =  KErrArgument;
                else if (type == typeid(std::overflow_error))
                    _err =  KErrOverflow;
                else if (type == typeid(std::underflow_error))
                    _err =  KErrUnderflow;
            }
        }
        User::LeaveIfError(_err);
    );

    delete[] argv;
    delete[] envp;
    return ret;
}


#ifdef __GCC32__

/* stub function inserted into main() by GCC */
extern "C" void __gccmain(void) {}

/* Default GCC entrypoint */
extern "C" TInt _mainCRTStartup(void)
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
