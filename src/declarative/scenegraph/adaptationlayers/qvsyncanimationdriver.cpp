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

// #define VSYNC_TIME_DEBUG


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

    d->window->update();
}

bool QVSyncAnimationDriver::eventFilter(QObject *object, QEvent *event) {
    Q_D(QVSyncAnimationDriver);

    if (isRunning()) {

        if (object == QApplication::instance() && event->type() == QEvent::Quit) {
            // If we get a close event while running, we are actually inside the processEvents()
            // block and need to exit an extra level to exit the final exec().
            d->aborted = true;
            QApplication::quit();
        } else if (object == d->window && event->type() == QEvent::Paint) {

#ifdef VSYNC_TIME_DEBUG
            QTime time;
            time.start();
#endif

            while (isRunning() && !d->aborted && !d->threadData->quitNow) {

#ifdef VSYNC_TIME_DEBUG
                time.start();
#endif

                QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);

#ifdef VSYNC_TIME_DEBUG
                int t1 = time.elapsed();
#endif

                QApplication::processEvents(QEventLoop::AllEvents, 1);

#ifdef VSYNC_TIME_DEBUG
                int t2 = time.elapsed();
#endif

                advance();

#ifdef VSYNC_TIME_DEBUG
                int t3 = time.elapsed();
#endif

                ((WidgetAccessor *) d->window)->paint();

#ifdef VSYNC_TIME_DEBUG
                int t4 = time.elapsed();
                printf("VSync breakdown: deletes=%d, events=%d, animations=%d, painting=%d, totoal=%d %s\n",
                       t1,
                       t2 - t1,
                       t3 - t2,
                       t4 - t3,
                       t4,
                       t4 > 19 || t4 < 12 ? "** BAD **" : "");
#endif
            }
            return true;
        }
    }

    return QObject::eventFilter(object, event);
}

void QVSyncAnimationDriver::stopped()
{
    // I don't need to do nothing as baseclass sets isRunning to false for us..
}
