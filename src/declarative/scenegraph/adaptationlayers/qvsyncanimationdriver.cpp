#include "qvsyncanimationdriver_p.h"

#include "private/qabstractanimation_p.h"
#include <private/qthread_p.h>

#include <QtGui/qevent.h>
#include <QtGui/qwidget.h>
#include <QtGui/qapplication.h>

#ifdef Q_WS_QPA
#include <qplatformglcontext_qpa.h>
#endif

#include <qdebug.h>

class QVSyncAnimationDriverPrivate : public QAnimationDriverPrivate
{
public:
    QWidget *window;
    bool aborted;
};


class WidgetAccessor : public QWidget
{
public:
    void paint() {
        QPaintEvent e(rect());
        paintEvent(&e);
    }
};


QVSyncAnimationDriver::QVSyncAnimationDriver(QObject *parent)
    : QAnimationDriver(*(new QVSyncAnimationDriverPrivate), parent)
{
    Q_D(QVSyncAnimationDriver);
    d->window = 0;
    d->aborted = false;

    qApp->installEventFilter(this);
}

void QVSyncAnimationDriver::setWidget(QWidget *window)
{
    d_func()->window = window;
}

QWidget *QVSyncAnimationDriver::widget() const
{
    return d_func()->window;
}

void QVSyncAnimationDriver::started()
{
    Q_D(QVSyncAnimationDriver);

    if (!d->window) {
        qWarning("VSyncDriver: no window to drive animation on...\n");
        return;
    }

    QEvent *event = new QEvent(QEvent::Type(QEvent::User + 1));
    QApplication::postEvent(this, event);
}

bool QVSyncAnimationDriver::event(QEvent *e)
{
    Q_D(QVSyncAnimationDriver);

    if (e->type() == QEvent::User + 1) {
        while (isRunning() && !d->aborted && !d->threadData->quitNow) {
            QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
            QApplication::processEvents(QEventLoop::AllEvents, 1);
            advance();
            ((WidgetAccessor *) d->window)->paint();
        }
    }

    return QAnimationDriver::event(e);
}

bool QVSyncAnimationDriver::eventFilter(QObject *object, QEvent *event) {
    Q_D(QVSyncAnimationDriver);

    if (isRunning()) {

#ifdef Q_WS_QPA
        if (object == d->window && event->type() == QEvent::UpdateRequest)
            return true;
        else
#endif

        if (object == QApplication::instance() && event->type() == QEvent::Quit) {
            // If we get a close event while running, we are actually inside the processEvents()
            // block and need to exit an extra level to exit the final exec().
            d->aborted = true;
            QApplication::quit();
        } else if (object == d->window && event->type() == QEvent::Paint) {
            // Since we are now blocking on vsync and we do a single fullscreen update,
            // want to only respond to a single update pr 16.66 ms increment, which is
            // the one we explicitly send to the window. All others we reject.
            return true;
        }
    }

    return QObject::eventFilter(object, event);
}

void QVSyncAnimationDriver::stopped()
{
    // I don't need to do nothing as baseclass sets isRunning to false for us..
}
