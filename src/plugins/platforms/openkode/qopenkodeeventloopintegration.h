#ifndef QOPENKODEEVENTLOOPINTEGRATION_H
#define QOPENKODEEVENTLOOPINTEGRATION_H

#include <QtGui/QPlatformEventLoopIntegration>

class KDThread;

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QOpenKODEEventLoopIntegration : public QPlatformEventLoopIntegration
{
public:
    QOpenKODEEventLoopIntegration();
    void processEvents(qint64 msec);
    void wakeup();
private:
    KDThread *m_kdThread;
};

QT_END_NAMESPACE
QT_END_HEADER

#endif // QOPENKODEEVENTLOOPINTEGRATION_H
