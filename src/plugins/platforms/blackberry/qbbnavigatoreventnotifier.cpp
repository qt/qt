/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//#define QBBNAVIGATOREVENTNOTIFIER_DEBUG


#include "qbbnavigatoreventnotifier.h"
#include "qbbnavigatoreventhandler.h"

#include <QtCore/private/qcore_unix_p.h>
#include <QByteArray>
#include <QList>
#include <QDebug>
#include <QSocketNotifier>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define NAV_CONTROL_PATH    "/pps/services/navigator/control"
#define PPS_BUFFER_SIZE     4096

QBBNavigatorEventNotifier::QBBNavigatorEventNotifier(QBBNavigatorEventHandler *eventHandler, QObject *parent)
    : QObject(parent),
      mEventHandler(eventHandler),
      mFd(-1),
      mReadNotifier(0)
{
}

QBBNavigatorEventNotifier::~QBBNavigatorEventNotifier()
{
    delete mReadNotifier;

    if (mFd != -1)
        qt_safe_close(mFd);

#if defined(QBBNAVIGATOREVENTNOTIFIER_DEBUG)
    qDebug() << "QBB: navigator event notifier stopped";
#endif
}

void QBBNavigatorEventNotifier::start()
{
#if defined(QBBNAVIGATOREVENTNOTIFIER_DEBUG)
    qDebug() << "QBB: navigator event notifier started";
#endif

    // open connection to navigator
    errno = 0;
    mFd = qt_safe_open(NAV_CONTROL_PATH, O_RDWR);
    if (mFd == -1) {
        qWarning("QBB: failed to open navigator pps, errno=%d", errno);
        return;
    }

    mReadNotifier = new QSocketNotifier(mFd, QSocketNotifier::Read);
    connect(mReadNotifier, SIGNAL(activated(int)), this, SLOT(readData()));
}

void QBBNavigatorEventNotifier::parsePPS(const QByteArray &ppsData, QByteArray &msg, QByteArray &dat, QByteArray &id)
{
#if defined(QBBNAVIGATOREVENTNOTIFIER_DEBUG)
    qDebug() << "PPS: data=" << ppsData;
#endif

    // tokenize pps data into lines
    QList<QByteArray> lines = ppsData.split('\n');

    // validate pps object
    if (lines.size() == 0 || lines.at(0) != "@control")
        qFatal("QBB: unrecognized pps object, data=%s", ppsData.constData());

    // parse pps object attributes and extract values
    for (int i = 1; i < lines.size(); i++) {

        // tokenize current attribute
        const QByteArray &attr = lines.at(i);

#if defined(QBBNAVIGATOREVENTNOTIFIER_DEBUG)
        qDebug() << "PPS: attr=" << attr;
#endif

        int firstColon = attr.indexOf(':');
        if (firstColon == -1) {
            // abort - malformed attribute
            continue;
        }

        int secondColon = attr.indexOf(':', firstColon + 1);
        if (secondColon == -1) {
            // abort - malformed attribute
            continue;
        }

        QByteArray key = attr.left(firstColon);
        QByteArray value = attr.mid(secondColon + 1);

#if defined(QBBNAVIGATOREVENTNOTIFIER_DEBUG)
        qDebug() << "PPS: key=" << key;
        qDebug() << "PPS: val=" << value;
#endif

        // save attribute value
        if (key == "msg")
            msg = value;
        else if (key == "dat")
            dat = value;
        else if (key == "id")
            id = value;
        else
            qFatal("QBB: unrecognized pps attribute, attr=%s", key.constData());
    }
}

void QBBNavigatorEventNotifier::replyPPS(const QByteArray &res, const QByteArray &id, const QByteArray &dat)
{
    // construct pps message
    QByteArray ppsData = "res::";
    ppsData += res;
    ppsData += "\nid::";
    ppsData += id;
    if (!dat.isEmpty()) {
        ppsData += "\ndat::";
        ppsData += dat;
    }
    ppsData += "\n";

#if defined(QBBNAVIGATOREVENTNOTIFIER_DEBUG)
    qDebug() << "PPS reply=" << ppsData;
#endif

    // send pps message to navigator
    errno = 0;
    int bytes = write(mFd, ppsData.constData(), ppsData.size());
    if (bytes == -1)
        qFatal("QBB: failed to write navigator pps, errno=%d", errno);
}

void QBBNavigatorEventNotifier::handleMessage(const QByteArray &msg, const QByteArray &dat, const QByteArray &id)
{
#if defined(QBBNAVIGATOREVENTNOTIFIER_DEBUG)
    qDebug() << "PPS: msg=" << msg << ", dat=" << dat << ", id=" << id;
#endif

    // check message type
    if (msg == "orientationCheck") {
        const bool result = mEventHandler->handleOrientationCheck(dat.toInt());
        replyPPS(msg, id, result ? "true": "false");
    } else if (msg == "orientation") {
        mEventHandler->handleOrientationChange(dat.toInt());
        replyPPS(msg, id, "");
    } else if (msg == "SWIPE_DOWN") {
        mEventHandler->handleSwipeDown();
    } else if (msg == "exit") {
        mEventHandler->handleExit();
    }
}

void QBBNavigatorEventNotifier::readData()
{
#if defined(QBBNAVIGATOREVENTNOTIFIER_DEBUG)
    qDebug() << "QBB: reading navigator data";
#endif

    // allocate buffer for pps data
    char buffer[PPS_BUFFER_SIZE];

    // attempt to read pps data
    errno = 0;
    int bytes = qt_safe_read(mFd, buffer, PPS_BUFFER_SIZE - 1);
    if (bytes == -1)
        qFatal("QBB: failed to read navigator pps, errno=%d", errno);

    // check if pps data was received
    if (bytes > 0) {

        // ensure data is null terminated
        buffer[bytes] = '\0';

        // process received message
        QByteArray ppsData(buffer);
        QByteArray msg;
        QByteArray dat;
        QByteArray id;
        parsePPS(ppsData, msg, dat, id);
        handleMessage(msg, dat, id);
    }

}
