/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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

#include "QtCore/qpauseanimation.h"
#include "private/qobject_p.h"
#include "qmltimer.h"
#include "qdebug.h"

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(QmlTimer,Timer)

class QmlTimerPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlTimer)
public:
    QmlTimerPrivate() : interval(1000) {}
    int interval;
    QPauseAnimation pause;
};

QmlTimer::QmlTimer(QObject *parent)
    : QObject(*(new QmlTimerPrivate), parent)
{
    Q_D(QmlTimer);
    connect(&d->pause, SIGNAL(currentLoopChanged(int)), this, SLOT(ticked()));
    d->pause.setLoopCount(-1);
    d->pause.setDuration(d->interval);
}

void QmlTimer::setInterval(int interval)
{
    Q_D(QmlTimer);
    if (interval != d->interval) {
        d->interval = interval;
        d->pause.setDuration(d->interval);
        d->pause.start();
    }
}

int QmlTimer::interval() const
{
    Q_D(const QmlTimer);
    return d->interval;
}

void QmlTimer::componentComplete()
{
    Q_D(QmlTimer);
    if (d->pause.state() != QAbstractAnimation::Running)
        d->pause.start();
}

void QmlTimer::ticked()
{
    emit timeout();
}

QT_END_NAMESPACE
