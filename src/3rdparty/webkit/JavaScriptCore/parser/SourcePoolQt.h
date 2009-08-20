/*
    Copyright (C) 2008, 2009 Nokia Corporation and/or its subsidiary(-ies)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
#ifndef SOURCEPOOL_H
#define SOURCEPOOL_H

#ifdef QT_BUILD_SCRIPT_LIB

#include "qhash.h"
#include "qstack.h"
#include "qdebug.h"
#include <stdint.h>

namespace JSC {

    class SourceCode;
    class Debugger;

    class SourcePool
    {
        class ScriptActivCount
        {
            int count;
        public:
            void incCount()
            {
                count++;
            };
            void decCount()
            {
                count--;
            };
            int getCount() const
            {
                return count;
            };
            bool isActive;
            ScriptActivCount() : count(0), isActive(true) {}
        };
        QStack<intptr_t> currentScript;
        QHash<unsigned, ScriptActivCount> scriptRef;
        QHash<int, QString> codes;  //debug
        Debugger *debug;

        friend class SourcePoolToken;
    public:
        class SourcePoolToken
        {
            unsigned id;
            SourcePool *ptr;
            SourcePoolToken(unsigned scriptId, SourcePool *scriptPool) : id(scriptId),ptr(scriptPool) {}
            SourcePoolToken(const SourcePoolToken&) : id(0), ptr(0) {}  //private - do not use - will crash
        public:
            ~SourcePoolToken() { ptr->objectUnregister(this); }
            friend class SourcePool;
        };

        SourcePool() : debug(0) {}

        void startEvaluating(const SourceCode& source);
        void stopEvaluating(const SourceCode& source);
        SourcePoolToken* objectRegister();

        void setDebugger(Debugger *debugger);
        Debugger* debugger();

    private:
        void objectUnregister(const SourcePoolToken *token);
    };

} //namespace JSC


#endif  //QT_BUILD_SCRIPT_LIB

#endif // SOURCEPOOL_H
