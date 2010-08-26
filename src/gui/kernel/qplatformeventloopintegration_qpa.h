#ifndef QPLATFORMEVENTLOOPINTEGRATION_QPA_H
#define QPLATFORMEVENTLOOPINTEGRATION_QPA_H

#include <QtCore/qglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QPlatformEventLoopIntegration
{
public:
    virtual void processEvents( qint64 msec ) = 0;
    virtual void wakeup() = 0;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPLATFORMEVENTLOOPINTEGRATION_QPA_H
