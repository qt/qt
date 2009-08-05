/*
 *  Copyright (C) 2005, 2008 Apple Inc. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
#ifdef QT_BUILD_SCRIPT_LIB
#include "ExceptionHelpers.h"
#include "Interpreter.h"
#endif
#include "PropertySlot.h"

#include "JSFunction.h"
#include "JSGlobalObject.h"
#include "JSObject.h"

namespace JSC {

JSValue PropertySlot::functionGetter(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    // Prevent getter functions from observing execution if an exception is pending.
    if (exec->hadException())
        return exec->exception();

    CallData callData;
    CallType callType = slot.m_data.getterFunc->getCallData(callData);
    if (callType == CallTypeHost) {
#ifdef QT_BUILD_SCRIPT_LIB
        ScopeChainNode* scopeChain = exec->scopeChain();
        Interpreter *interp = exec->interpreter();
        Register *oldEnd = interp->registerFile().end();
        int argc = 1; // implicit "this" parameter
        if (!interp->registerFile().grow(oldEnd + argc + RegisterFile::CallFrameHeaderSize))
            return createStackOverflowError(exec);
        JSC::CallFrame* newCallFrame = JSC::CallFrame::create(oldEnd);
        newCallFrame[0] = slot.slotBase(); // this
        newCallFrame += argc + JSC::RegisterFile::CallFrameHeaderSize;
        newCallFrame->init(0, /*vPC=*/0, scopeChain, exec, 0, argc, slot.m_data.getterFunc);
        JSValue result = callData.native.function(newCallFrame, slot.m_data.getterFunc, slot.slotBase(), exec->emptyList());
        interp->registerFile().shrink(oldEnd);
        return result;
#else
        return callData.native.function(exec, slot.m_data.getterFunc, slot.slotBase(), exec->emptyList());
#endif
    }
    ASSERT(callType == CallTypeJS);
    // FIXME: Can this be done more efficiently using the callData?
    return static_cast<JSFunction*>(slot.m_data.getterFunc)->call(exec, slot.slotBase(), exec->emptyList());
}

} // namespace JSC
