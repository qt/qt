#include "SourcePoolQt.h"


#ifdef QT_BUILD_SCRIPT_LIB

#include "SourceCode.h"
#include "Debugger.h"


namespace JSC {

    void SourcePool::startEvaluating(const SourceCode& source)
    {
        int id = source.provider()->asID();

        codes.insert(id,source.toString().toQString());

        currentScript.push(id);
        scriptRef.insert(id,ScriptActivCount());

        if (debug)
            debug->scriptLoad(id,source.toString(),source.provider()->url(),source.firstLine());
    }


    void SourcePool::stopEvaluating(const SourceCode& source)
    {
        int id = source.provider()->asID();
        int cid = currentScript.pop();

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
