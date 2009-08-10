#ifndef SOURCEPOOL_H
#define SOURCEPOOL_H

#ifdef QT_BUILD_SCRIPT_LIB

#include "qhash.h"
#include "qstack.h"
#include "qdebug.h"

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
