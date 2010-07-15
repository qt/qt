#ifndef QPLATFORMEVENTLOOPINTEGRATION_QPA_H
#define QPLATFORMEVENTLOOPINTEGRATION_QPA_H

#include <qglobal.h>

class QPlatformEventLoopIntegration
{
public:
    virtual void processEvents( qint64 msec ) = 0;
    virtual void wakeup() = 0;
};

#endif // QPLATFORMEVENTLOOPINTEGRATION_QPA_H
