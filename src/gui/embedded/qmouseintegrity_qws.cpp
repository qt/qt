/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT_NO_QWS_MOUSE_INTEGRITY

#include "qmouseintegrity_qws.h"
#include <qwindowsystem_qws.h>
#include <qapplication.h>
#include <qtimer.h>
#include <INTEGRITY.h>

#include <QThread>

#include <device/hiddriver.h>

QT_BEGIN_NAMESPACE

class QIntMouseListenThread;

class QIntMousePrivate : public QObject
{
    Q_OBJECT
    friend class QIntMouseListenTaskThread;
Q_SIGNALS:
    void mouseDataAvailable(int x, int y, int buttons);
public:
    QIntMousePrivate(QIntMouseHandler *handler);
    ~QIntMousePrivate();
    void dataReady(int x, int y, int buttons) { emit mouseDataAvailable(x, y, buttons); }
    bool calibrated;
    bool waitforread;
    bool suspended;
    QIntMouseListenThread *mousethread;
private:
    QIntMouseHandler *handler;
};

class QIntMouseListenThread : public QThread
{
protected:
    QIntMousePrivate *imp;
    bool loop;
    QList<HIDHandle> handleList;
    QList<Activity> actList;
    Semaphore loopsem;
public:
    QIntMouseListenThread(QIntMousePrivate *im) : QThread(), imp(im) {};
    ~QIntMouseListenThread() {};
    void run();
    bool setup(QString driverName, uint32_t index);
    void stoploop() { loop = false; ReleaseSemaphore(loopsem); };
};

QIntMouseHandler::QIntMouseHandler(const QString &driver, const QString &device)
    : QObject(), QWSCalibratedMouseHandler(driver, device)
{
    QPoint test(1,1);
    d = new QIntMousePrivate(this);
    connect(d, SIGNAL(mouseDataAvailable(int, int, int)), this, SLOT(readMouseData(int, int, int)));

    d->calibrated = (test != transform(test));

    d->mousethread->setup(QString(), 0);
    d->mousethread->start();
}

QIntMouseHandler::~QIntMouseHandler()
{
    disconnect(d, SIGNAL(mouseDataAvailable(int, int, int)), this, SLOT(readMouseData(int, int, int)));
    delete d;
}

void QIntMouseHandler::resume()
{
    d->suspended = true;
}

void QIntMouseHandler::suspend()
{
    d->suspended = false;
}

void QIntMouseHandler::readMouseData(int x, int y, int buttons)
{
    d->waitforread = false;
    if (d->suspended)
        return;
    if (d->calibrated)
        sendFiltered(QPoint(x, y), buttons);
    else {
        QPoint pos;
        pos = transform(QPoint(x, y));
        limitToScreen(pos);
        mouseChanged(pos, buttons, 0);
    }
}

void QIntMouseHandler::clearCalibration()
{
    QWSCalibratedMouseHandler::clearCalibration();
}

void QIntMouseHandler::calibrate(const QWSPointerCalibrationData *data)
{
    QWSCalibratedMouseHandler::calibrate(data);
}

bool QIntMouseListenThread::setup(QString driverName, uint32_t index)
{
    int i;
    int devices;
    Error driverError, deviceError;
    HIDDriver *driver;
    HIDHandle handle;
    /* FIXME : take a list of driver names/indexes for setup */
    devices = 0;
    i = 0;
    do {
    driverError = gh_hid_get_driver(i, &driver);
        if (driverError == Success) {
            int j = 0;
            do {
                deviceError = gh_hid_init_device(driver, j, &handle);
                if (deviceError == Success) {
                    int32_t type;
                    /* only accept pointing devices */
                    deviceError = gh_hid_get_setting(handle, HID_SETTING_CAPABILITIES, &type);
                    if ((deviceError == Success) && (type & HID_TYPE_AXIS)) {
                        handleList.append(handle);
                        devices++;
                    } else
                        gh_hid_close(handle);
                        j++;
                    }
            } while (deviceError == Success);
        i++;
        }
    } while (driverError == Success);
    return (devices > 0);
}

void QIntMouseListenThread::run(void)
{
    Value id;
    HIDEvent event;
    Activity loopact;
    QPoint currentpos(0,0);
    Qt::MouseButtons currentbutton = Qt::NoButton;
    Qt::KeyboardModifiers keymod;

    /* first create all Activities for the main loop.
     * We couldn't do this in setup() because this Task is different */
    Activity act;
    int i = 0;
    foreach (HIDHandle h, handleList) {
        CheckSuccess(CreateActivity(CurrentTask(), 2, false, i, &act));
        actList.append(act);
        i++;
        CheckSuccess(gh_hid_async_wait_for_event(h, act));
    }

    /* setup a Semaphore used to watch for a request for exit */
    CheckSuccess(CreateSemaphore(0, &loopsem));
    CheckSuccess(CreateActivity(CurrentTask(), 2, false, 0, &loopact));
    CheckSuccess(AsynchronousReceive(loopact, (Object)loopsem, NULL));

    loop = true;
    do {
        uint32_t num_events = 1;

        WaitForActivity(&id);
        if (loop) {
            while (gh_hid_get_event(handleList.at(id), &event, &num_events) == Success) {
                if (event.type == HID_TYPE_AXIS) {
                    switch (event.index) {
                        case HID_AXIS_ABSX:
                            currentpos.setX(event.value);
                            break;
                        case HID_AXIS_ABSY:
                            currentpos.setY(event.value);
                            break;
                        case HID_AXIS_RELX:
                            currentpos.setX(currentpos.x() + event.value);
                            break;
                        case HID_AXIS_RELY:
                            currentpos.setY(currentpos.y() + event.value);
                            break;
                        default:
                            break;
                    }
                } else if (event.type == HID_TYPE_KEY) {
                    switch (event.index) {
                        case HID_BUTTON_LEFT:
                            if (event.value)
                                currentbutton |= Qt::LeftButton;
                            else
                                currentbutton &= ~Qt::LeftButton;
                            break;
                        case HID_BUTTON_MIDDLE:
                            if (event.value)
                                currentbutton |= Qt::MiddleButton;
                            else
                                currentbutton &= ~Qt::MiddleButton;
                            break;
                        case HID_BUTTON_RIGHT:
                            if (event.value)
                                currentbutton |= Qt::RightButton;
                            else
                                currentbutton &= ~Qt::RightButton;
                            break;
                    }
                } else if (event.type == HID_TYPE_SYNC) {
                /* sync events are sent by mouses and not by keyboards.
                 * this should probably be changed */
                imp->dataReady(currentpos.x(), currentpos.y(), currentbutton);
                //QWindowSystemInterface::handleMouseEvent(0, currentpos, currentpos, currentbutton);
                }
            }
            CheckSuccess(gh_hid_async_wait_for_event(handleList.at(id), actList.at(id)));
        }
    } while (loop);
    CloseSemaphore(loopsem);
    QThread::exit(0);
}

QIntMousePrivate::QIntMousePrivate(QIntMouseHandler *handler) : QObject()
{
    this->handler = handler;
    suspended = false;
    mousethread = new QIntMouseListenThread(this);
}

QIntMousePrivate::~QIntMousePrivate()
{
    mousethread->stoploop();
    mousethread->wait();
    delete mousethread;
}

QT_END_NAMESPACE

#include "qmouseintegrity_qws.moc"

#endif // QT_NO_QWS_MOUSE_INTEGRITY
