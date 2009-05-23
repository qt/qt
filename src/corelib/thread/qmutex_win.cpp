/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <windows.h>

#include "qmutex.h"
#include <qatomic.h>
#include "qmutex_p.h"

QT_BEGIN_NAMESPACE

QMutexPrivate::QMutexPrivate(QMutex::RecursionMode mode)
    : recursive(mode == QMutex::Recursive), contenders(0), lastSpinCount(0), owner(0), count(0)
{
    if (QSysInfo::WindowsVersion == 0) {
        // mutex was created before initializing WindowsVersion. this
        // can happen when creating the resource file engine handler,
        // for example. try again with just the A version
#ifdef Q_OS_WINCE
        event = CreateEventW(0, FALSE, FALSE, 0);
#else
        event = CreateEventA(0, FALSE, FALSE, 0);
#endif
    } else {
        event = QT_WA_INLINE(CreateEventW(0, FALSE, FALSE, 0),
                             CreateEventA(0, FALSE, FALSE, 0));
    }
    if (!event)
        qWarning("QMutexPrivate::QMutexPrivate: Cannot create event");
}

QMutexPrivate::~QMutexPrivate()
{ CloseHandle(event); }

bool QMutexPrivate::wait(int timeout)
{
    return WaitForSingleObject(event, timeout < 0 ? INFINITE : timeout) ==  WAIT_OBJECT_0;
}

void QMutexPrivate::wakeUp()
{ SetEvent(event); }

QT_END_NAMESPACE
