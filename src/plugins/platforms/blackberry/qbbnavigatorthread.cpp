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

//#define QBBNAVIGATORTHREAD_DEBUG


#include "qbbnavigatorthread.h"
#include "qbbscreen.h"

#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QWindowSystemInterface>
#include <QByteArray>
#include <QList>
#include <QDebug>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define NAV_CONTROL_PATH    "/pps/services/navigator/control"
#define PPS_BUFFER_SIZE     4096

QBBNavigatorThread::QBBNavigatorThread(QBBScreen& primaryScreen)
    : mPrimaryScreen(primaryScreen),
      mFd(-1),
      mQuit(false)
{
}

QBBNavigatorThread::~QBBNavigatorThread()
{
    // block until thread terminates
    shutdown();
}

void QBBNavigatorThread::run()
{
#if defined(QBBNAVIGATORTHREAD_DEBUG)
    qDebug() << "QBB: navigator thread started";
#endif

    // open connection to navigator
    errno = 0;
    mFd = open(NAV_CONTROL_PATH, O_RDWR);
    if (mFd == -1) {
        qWarning("QBB: failed to open navigator pps, errno=%d", errno);
        return;
    }

    // allocate buffer for pps data
    char buffer[PPS_BUFFER_SIZE];

    // loop indefinitely
    while (!mQuit) {

        // attempt to read pps data
        errno = 0;
        int bytes = read(mFd, buffer, PPS_BUFFER_SIZE - 1);
        if (bytes == -1) {
            qFatal("QBB: failed to read navigator pps, errno=%d", errno);
        }

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

        // yield
        msleep(5);
    }

    // close connection to navigator
    close(mFd);

#if defined(QBBNAVIGATORTHREAD_DEBUG)
    qDebug() << "QBB: navigator thread stopped";
#endif
}

void QBBNavigatorThread::shutdown()
{
    // signal thread to terminate
    mQuit = true;

#if defined(QBBNAVIGATORTHREAD_DEBUG)
    qDebug() << "QBB: navigator thread shutdown begin";
#endif

    // block until thread terminates
    wait();

#if defined(QBBNAVIGATORTHREAD_DEBUG)
    qDebug() << "QBB: navigator thread shutdown end";
#endif
}

void QBBNavigatorThread::parsePPS(const QByteArray &ppsData, QByteArray &msg, QByteArray &dat, QByteArray &id)
{
#if defined(QBBNAVIGATORTHREAD_DEBUG)
    qDebug() << "PPS: data=" << ppsData;
#endif

    // tokenize pps data into lines
    QList<QByteArray> lines = ppsData.split('\n');

    // validate pps object
    if (lines.size() == 0 || lines.at(0) != "@control") {
        qFatal("QBB: unrecognized pps object, data=%s", ppsData.constData());
    }

    // parse pps object attributes and extract values
    for (int i = 1; i < lines.size(); i++) {

        // tokenize current attribute
        const QByteArray &attr = lines.at(i);

#if defined(QBBNAVIGATORTHREAD_DEBUG)
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

#if defined(QBBNAVIGATORTHREAD_DEBUG)
        qDebug() << "PPS: key=" << key;
        qDebug() << "PPS: val=" << value;
#endif

        // save attribute value
        if (key == "msg") {
            msg = value;
        } else if (key == "dat") {
            dat = value;
        } else if (key == "id") {
            id = value;
        } else {
            qFatal("QBB: unrecognized pps attribute, attr=%s", key.constData());
        }
    }
}

void QBBNavigatorThread::replyPPS(const QByteArray &res, const QByteArray &id, const QByteArray &dat)
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

#if defined(QBBNAVIGATORTHREAD_DEBUG)
    qDebug() << "PPS reply=" << ppsData;
#endif

    // send pps message to navigator
    errno = 0;
    int bytes = write(mFd, ppsData.constData(), ppsData.size());
    if (bytes == -1) {
        qFatal("QBB: failed to write navigator pps, errno=%d", errno);
    }
}

void QBBNavigatorThread::handleMessage(const QByteArray &msg, const QByteArray &dat, const QByteArray &id)
{
#if defined(QBBNAVIGATORTHREAD_DEBUG)
    qDebug() << "PPS: msg=" << msg << ", dat=" << dat << ", id=" << id;
#endif

    // check message type
    if (msg == "orientationCheck") {

        // reply to navigator that (any) orientation is acceptable
#if defined(QBBNAVIGATORTHREAD_DEBUG)
        qDebug() << "PPS: orientation check, o=" << dat;
#endif
        replyPPS(msg, id, "true");

    } else if (msg == "orientation") {

        // update screen geometry and reply to navigator that we're ready
#if defined(QBBNAVIGATORTHREAD_DEBUG)
        qDebug() << "PPS: orientation, o=" << dat;
#endif
        mPrimaryScreen.setRotation( dat.toInt() );
        QWindowSystemInterface::handleScreenGeometryChange(0);
        replyPPS(msg, id, "");

    } else if (msg == "SWIPE_DOWN") {

        // simulate menu key press
#if defined(QBBNAVIGATORTHREAD_DEBUG)
        qDebug() << "PPS: menu";
#endif
        QWidget *w = QApplication::activeWindow();
        QWindowSystemInterface::handleKeyEvent(w, QEvent::KeyPress, Qt::Key_Menu, Qt::NoModifier);
        QWindowSystemInterface::handleKeyEvent(w, QEvent::KeyRelease, Qt::Key_Menu, Qt::NoModifier);

    } else if (msg == "exit") {

        // shutdown everything
#if defined(QBBNAVIGATORTHREAD_DEBUG)
        qDebug() << "PPS: exit";
#endif
        QApplication::quit();
    }
}
