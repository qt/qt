/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtDeclarative/QDeclarativeExtensionPlugin>
#include <QtDeclarative/qdeclarative.h>
#include <qdebug.h>
#include <qdatetime.h>
#include <qbasictimer.h>
#include <qapplication.h>

// Implements a "TimeModel" class with hour and minute properties
// that change on-the-minute yet efficiently sleep the rest
// of the time.

class MinuteTimer : public QObject
{
    Q_OBJECT
public:
    MinuteTimer(QObject *parent) : QObject(parent)
    {
    }

    void start()
    {
        if (!timer.isActive()) {
            time = QTime::currentTime();
            timer.start(60000-time.second()*1000, this);
        }
    }

    void stop()
    {
        timer.stop();
    }

    int hour() const { return time.hour(); }
    int minute() const { return time.minute(); }

signals:
    void timeChanged();

protected:
    void timerEvent(QTimerEvent *)
    {
        QTime now = QTime::currentTime();
        if (now.second() == 59 && now.minute() == time.minute() && now.hour() == time.hour()) {
            // just missed time tick over, force it, wait extra 0.5 seconds
            time.addSecs(60);
            timer.start(60500, this);
        } else {
            time = now;
            timer.start(60000-time.second()*1000, this);
        }
        emit timeChanged();
    }

private:
    QTime time;
    QBasicTimer timer;
};

class TimeModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int hour READ hour NOTIFY timeChanged)
    Q_PROPERTY(int minute READ minute NOTIFY timeChanged)

public:
    TimeModel(QObject *parent=0) : QObject(parent)
    {
        if (++instances == 1) {
            if (!timer)
                timer = new MinuteTimer(qApp);
            connect(timer, SIGNAL(timeChanged()), this, SIGNAL(timeChanged()));
            timer->start();
        }
    }

    ~TimeModel()
    {
        if (--instances == 0) {
            timer->stop();
        }
    }

    int minute() const { return timer->minute(); }
    int hour() const { return timer->hour(); }

signals:
    void timeChanged();

private:
    QTime t;
    static MinuteTimer *timer;
    static int instances;
};

int TimeModel::instances=0;
MinuteTimer *TimeModel::timer=0;

class QExampleQmlPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("com.nokia.TimeExample"));
        qmlRegisterType<TimeModel>(uri, 1, 0, "Time");
    }
};

#include "plugin.moc"

Q_EXPORT_PLUGIN2(qtimeexampleqmlplugin, QExampleQmlPlugin);
