#ifndef QOPENKODEEVENTLOOPINTEGRATION_H
#define QOPENKODEEVENTLOOPINTEGRATION_H

#include <QtGui/QPlatformEventLoopIntegration>

class KDThread;
class QOpenKODEEventLoopIntegration : public QPlatformEventLoopIntegration
{
public:
    QOpenKODEEventLoopIntegration();
    void processEvents(qint64 msec);
    void wakeup();
private:
    KDThread *m_kdThread;
};

#endif // QOPENKODEEVENTLOOPINTEGRATION_H
