#include "qopenkodeeventloopintegration.h"

#include <KD/kd.h>

QT_BEGIN_NAMESPACE

QOpenKODEEventLoopIntegration::QOpenKODEEventLoopIntegration()
{
    m_kdThread = kdThreadSelf();
}

void QOpenKODEEventLoopIntegration::processEvents(qint64 msec)
{
    if (msec == 0)
        msec = -1;
    const KDEvent *event = kdWaitEvent(msec*1000);
    if (event) {
        kdDefaultEvent(event);
        while ((event = kdWaitEvent(0)) != 0) {
            kdDefaultEvent(event);
        }
    }
}

void QOpenKODEEventLoopIntegration::wakeup()
{
    KDEvent *event = kdCreateEvent();
    kdPostThreadEvent(event,m_kdThread);
}

QT_END_NAMESPACE
