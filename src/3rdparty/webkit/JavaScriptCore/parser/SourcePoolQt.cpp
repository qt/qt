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

#include "config.h"
#include "SourcePoolQt.h"


#ifdef QT_BUILD_SCRIPT_LIB

#include "SourceCode.h"
#include "Debugger.h"


namespace JSC {

    void SourcePool::startEvaluating(const SourceCode& source)
    {
        int id = source.provider()->asID();

        codes.insert(id,source.toString());

        currentScript.push(id);
        scriptRef.insert(id,ScriptActivCount());

        if (debug)
            debug->scriptLoad(id,source.toString(),source.provider()->url(),source.firstLine());
    }


    void SourcePool::stopEvaluating(const SourceCode& source)
    {
        int id = source.provider()->asID();
        currentScript.pop();

        if (scriptRef.contains(id)) {
            ScriptActivCount info = scriptRef.take(id);
            if (info.getCount()) {
                //we can't remove info from scriptRef
                info.isActive = false;
                scriptRef.insert(id,info);
            } else {
                //we are unloading source code
                if (debug)
                    debug->scriptUnload(id);
            }
        }
    }

    SourcePool::SourcePoolToken* SourcePool::objectRegister()
    {
        if (currentScript.isEmpty()) {
            return 0;
        }

        int id = currentScript.top();

        SourcePoolToken* token = new SourcePoolToken(id,this);

        ScriptActivCount info = scriptRef.take(id);

        info.incCount();
        scriptRef.insert(id,info);
        return token;
    }

    void SourcePool::objectUnregister(const SourcePool::SourcePoolToken *token)
    {
        int id = token->id;

        ScriptActivCount info = scriptRef.take(id);
        info.decCount();
        if (info.isActive) {
            scriptRef.insert(id,info);
        } else {
            if (info.getCount() == 0) {
                //remove from scriptRef (script is not active and there is no objects connected)
                if(debug)
                    debug->scriptUnload(id);
            } else {
                scriptRef.insert(id,info);
            }
        }

    }


    void SourcePool::setDebugger(JSC::Debugger *debugger) { this->debug = debugger; }
    Debugger* SourcePool::debugger() { return debug; }

} //namespace JSC


#endif //QT_BUILD_SCRIPT_LIB
