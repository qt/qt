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

#include "qwssignalhandler_p.h"

#ifndef QT_NO_QWS_SIGNALHANDLER

#include "qlock_p.h"
#include "qwslock_p.h"

#include <sys/types.h>
#include <signal.h>

QT_BEGIN_NAMESPACE

class QWSSignalHandlerPrivate : public QWSSignalHandler
{
public:
    QWSSignalHandlerPrivate() : QWSSignalHandler() {}
};


Q_GLOBAL_STATIC(QWSSignalHandlerPrivate, signalHandlerInstance);


QWSSignalHandler* QWSSignalHandler::instance()
{
    return signalHandlerInstance();
}

QWSSignalHandler::QWSSignalHandler()
{
    const int signums[] = { SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGFPE,
                            SIGSEGV, SIGTERM, SIGBUS };
    const int n = sizeof(signums)/sizeof(int);

    for (int i = 0; i < n; ++i) {
        const int signum = signums[i];
        qt_sighandler_t old = signal(signum, handleSignal);
        if (old == SIG_IGN) // don't remove shm and semaphores when ignored
            signal(signum, old);
        else
            oldHandlers[signum] = (old == SIG_ERR ? SIG_DFL : old);
    }
}

QWSSignalHandler::~QWSSignalHandler()
{
    clear();
}

void QWSSignalHandler::clear()
{
#if !defined(QT_NO_QWS_MULTIPROCESS)
    // it is safe to call d-tors directly here since, on normal exit,
    // lists should be empty; otherwise, we don't care about semi-alive objects
    // and the only important thing here is to unregister the system semaphores.
    while (!locks.isEmpty())
        locks.takeLast()->~QLock();
    while (!wslocks.isEmpty())
        wslocks.takeLast()->~QWSLock();
#endif
    objects.clear();
}

void QWSSignalHandler::handleSignal(int signum)
{
    QWSSignalHandler *h = instance();
    if (h) {
        signal(signum, h->oldHandlers[signum]);
        h->clear();
    }
    raise(signum);
}

QT_END_NAMESPACE

#endif // QT_NO_QWS_SIGNALHANDLER
