/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "ossignalconverter_p.h"
#include <signal.h>
#include <QTimer>
#include <stdio.h>

Q_GLOBAL_STATIC(OsSignalConverter, osSignalConverter);

OsSignalConverter* OsSignalConverter::instance()
{
    return osSignalConverter();
}

OsSignalConverter::OsSignalConverter()
: d(new OsSignalConverterPrivate(this))
{
};

OsSignalConverter::~OsSignalConverter()
{
}

OsSignalConverterPrivate::OsSignalConverterPrivate(OsSignalConverter* owner)
: QObject(owner), q(owner), poller(new QTimer(this))
{
    trap();
    connect(poller, SIGNAL(timeout()), this, SLOT(poll()));
    poller->start(1000);
}

OsSignalConverterPrivate::~OsSignalConverterPrivate()
{
    untrap();
}

void OsSignalConverterPrivate::trap()
{
    signal(SIGINT, handler);
    signal(SIGTERM, handler);
#ifdef SIGBREAK
    signal(SIGBREAK, handler);
#endif
#ifdef SIGHUP
    signal(SIGHUP, handler);
#endif
#ifdef SIGQUIT
    signal(SIGQUIT, handler);
#endif
}

void OsSignalConverterPrivate::untrap()
{
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
#ifdef SIGBREAK
    signal(SIGBREAK, SIG_DFL);
#endif
#ifdef SIGHUP
    signal(SIGHUP, SIG_DFL);
#endif
#ifdef SIGQUIT
    signal(SIGQUIT, SIG_DFL);
#endif
}

void OsSignalConverterPrivate::handler(int sig)
{
    untrap(); //allow 2nd ctrl-c to really kill us
    terminateRequest = sig;
}

void OsSignalConverterPrivate::poll()
{
    if (terminateRequest) {
        fprintf(stderr, "\n*** caught signal %d, terminating ***\n", terminateRequest);
        poller->stop();
        emit q->terminate();
    }
}

sig_atomic_t OsSignalConverterPrivate::terminateRequest;
