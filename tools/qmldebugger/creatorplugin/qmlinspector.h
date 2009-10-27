#ifndef QMLINSPECTOR_H
#define QMLINSPECTOR_H

#include <QString>

namespace QmlInspector {
    namespace Constants {
        const char * const RUN = "QmlInspector.Run";
        const char * const STOP = "QmlInspector.Stop";

        const char * const C_INSPECTOR = "QmlInspector";
    };
    
    class StartParameters
    {
    public:
        StartParameters() : port(0) {}
        ~StartParameters() {}
        
        QString address;
        quint16 port;
    };
};

#endif
