#include "qopenkodeeventloopintegration.h"

#include <QDebug>

#include <KD/kd.h>
#include <KD/ATX_keyboard.h>

QT_BEGIN_NAMESPACE

static const int QT_EVENT_WAKEUP_EVENTLOOP = KD_EVENT_USER + 1;

void kdprocessevent( const KDEvent *event)
{
    switch (event->type) {
    case KD_EVENT_INPUT:
        qDebug() << "KD_EVENT_INPUT";
        break;
    case KD_EVENT_INPUT_POINTER:
        qDebug() << "KD_EVENT_INPUT_POINTER";
        break;
    case KD_EVENT_WINDOW_CLOSE:
        qDebug() << "KD_EVENT_WINDOW_CLOSE";
        break;
    case KD_EVENT_WINDOWPROPERTY_CHANGE:
        qDebug() << "KD_EVENT_WINDOWPROPERTY_CHANGE";
        qDebug() << event->data.windowproperty.pname;
        break;
    case KD_EVENT_WINDOW_FOCUS:
        qDebug() << "KD_EVENT_WINDOW_FOCUS";
        break;
    case KD_EVENT_WINDOW_REDRAW:
        qDebug() << "KD_EVENT_WINDOW_REDRAW";
        break;
    case KD_EVENT_USER:
        qDebug() << "KD_EVENT_USER";
        break;
    case KD_EVENT_INPUT_KEY_ATX:
        qDebug() << "KD_EVENT_INPUT_KEY_ATX";
        break;
    case QT_EVENT_WAKEUP_EVENTLOOP:
//        qDebug() << "QT_EVENT_WAKEUP_EVENTLOOP";
        break;
    default:
        break;
    }

    kdDefaultEvent(event);

}

QOpenKODEEventLoopIntegration::QOpenKODEEventLoopIntegration()
{
    m_kdThread = kdThreadSelf();
    kdInstallCallback(&kdprocessevent,QT_EVENT_WAKEUP_EVENTLOOP,this);
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
    event->type = QT_EVENT_WAKEUP_EVENTLOOP;
    event->userptr = this;
    kdPostThreadEvent(event,m_kdThread);
}

QT_END_NAMESPACE
