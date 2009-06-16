/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmousevr41xx_qws.h"

#ifndef QT_NO_QWS_MOUSE_VR41XX
#include "qwindowsystem_qws.h"
#include "qsocketnotifier.h"
#include "qtimer.h"
#include "qapplication.h"
#include "qscreen_qws.h"
#include <qstringlist.h>
#include <qvarlengtharray.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

QT_BEGIN_NAMESPACE

static const int defaultFilterSize = 3;

class QWSVr41xxMouseHandlerPrivate : public QObject
{
    Q_OBJECT
public:
    QWSVr41xxMouseHandlerPrivate(QWSVr41xxMouseHandler *, const QString &, const QString &);
    ~QWSVr41xxMouseHandlerPrivate();

    void resume();
    void suspend();

private slots:
    void sendRelease();
    void readMouseData();

private:
    bool getSample();
    ushort currSample[6];
    uint currLength;

    int mouseFD;
    int mouseIdx;
    QTimer *rtimer;
    QSocketNotifier *mouseNotifier;
    QWSVr41xxMouseHandler *handler;
    QPoint lastPos;
    bool isPressed;
    int filterSize;
    int pressLimit;
};

QWSVr41xxMouseHandler::QWSVr41xxMouseHandler(const QString &drv, const QString &dev)
    : QWSCalibratedMouseHandler(drv, dev)
{
    d = new QWSVr41xxMouseHandlerPrivate(this, drv, dev);
}

QWSVr41xxMouseHandler::~QWSVr41xxMouseHandler()
{
    delete d;
}

void QWSVr41xxMouseHandler::resume()
{
    d->resume();
}

void QWSVr41xxMouseHandler::suspend()
{
    d->suspend();
}

QWSVr41xxMouseHandlerPrivate::QWSVr41xxMouseHandlerPrivate(QWSVr41xxMouseHandler *h, const QString &, const QString &device)
    : currLength(0), handler(h)
{
    QStringList options = device.split(QLatin1String(":"));
    int index = -1;

    filterSize = defaultFilterSize;
    QRegExp filterRegExp(QLatin1String("filter=(\\d+)"));
    index = options.indexOf(filterRegExp);
    if (index != -1) {
        filterSize = qMax(1, filterRegExp.cap(1).toInt());
        options.removeAt(index);
    }
    handler->setFilterSize(filterSize);

    pressLimit = 750;
    QRegExp pressRegExp(QLatin1String("press=(\\d+)"));
    index = options.indexOf(pressRegExp);
    if (index != -1) {
        pressLimit = filterRegExp.cap(1).toInt();
        options.removeAt(index);
    }

    QString dev;
    if (options.isEmpty())
        dev = QLatin1String("/dev/vrtpanel");
    else
        dev = options.first();

    if ((mouseFD = open(dev.toLocal8Bit().constData(), O_RDONLY)) < 0) {
        qWarning("Cannot open %s (%s)", qPrintable(dev), strerror(errno));
        return;
    }
    sleep(1);

    if (fcntl(mouseFD, F_SETFL, O_NONBLOCK) < 0) {
        qWarning("Error initializing touch panel.");
        return;
    }

    mouseNotifier = new QSocketNotifier(mouseFD, QSocketNotifier::Read, this);
    connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));

    rtimer = new QTimer(this);
    rtimer->setSingleShot(true);
    connect(rtimer, SIGNAL(timeout()), this, SLOT(sendRelease()));
    mouseIdx = 0;
}

QWSVr41xxMouseHandlerPrivate::~QWSVr41xxMouseHandlerPrivate()
{
    if (mouseFD >= 0)
        close(mouseFD);
}

void QWSVr41xxMouseHandlerPrivate::suspend()
{
    mouseNotifier->setEnabled(false);
}


void QWSVr41xxMouseHandlerPrivate::resume()
{
    mouseIdx = 0;
    mouseNotifier->setEnabled(true);
}

void QWSVr41xxMouseHandlerPrivate::sendRelease()
{
    handler->sendFiltered(lastPos, Qt::NoButton);
    isPressed = false;
}

bool QWSVr41xxMouseHandlerPrivate::getSample()
{
    const int n = read(mouseFD,
                       reinterpret_cast<uchar*>(currSample) + currLength,
                       sizeof(currSample) - currLength);

    if (n > 0)
        currLength += n;

    if (currLength < sizeof(currSample))
        return false;

    currLength = 0;
    return true;
}

void QWSVr41xxMouseHandlerPrivate::readMouseData()
{
    const int sampleLength = sizeof(currSample) / sizeof(ushort);
    QVarLengthArray<ushort, sampleLength * defaultFilterSize> samples(sampleLength * filterSize);

    // Only return last 'filterSize' samples
    int head = 0;
    int tail = 0;
    int nSamples = 0;
    while (getSample()) {
        if (!(currSample[0] & 0x8000) || (currSample[5] < pressLimit))
            continue;

        ushort *data = samples.data() + head * sampleLength;
        memcpy(data, currSample, sizeof(currSample));
        ++nSamples;
        head = (head + 1) % filterSize;
        if (nSamples >= filterSize)
            tail = (tail + 1) % filterSize;
    }

    if (nSamples == 0)
        return;

    // send mouse events
    while (tail != head || filterSize == 1) {
        const ushort *data = samples.data() + tail * sampleLength;
        lastPos = QPoint(data[3] - data[4], data[2] - data[1]);
        handler->sendFiltered(lastPos, Qt::LeftButton);
        isPressed = true;
        tail = (tail + 1) % filterSize;
        if (filterSize == 1)
            break;
    }

    if (isPressed)
        rtimer->start(50); // release unreliable
}

QT_END_NAMESPACE

#include "qmousevr41xx_qws.moc"

#endif //QT_NO_QWS_MOUSE_VR41
