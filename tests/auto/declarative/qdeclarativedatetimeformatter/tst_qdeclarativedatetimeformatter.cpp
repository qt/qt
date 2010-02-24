/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <qtest.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <private/qdeclarativedatetimeformatter_p.h>
#include <QDebug>

class tst_qdeclarativedatetimeformatter : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativedatetimeformatter() {}

private slots:
    void date();
    void time();
    void dateTime();
};

void tst_qdeclarativedatetimeformatter::date()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent formatterComponent(&engine);
    formatterComponent.setData(QByteArray("import Qt 4.6\n DateTimeFormatter { date: \"2008-12-24\" }"),
                               QUrl::fromLocalFile(""));
    QDeclarativeDateTimeFormatter *formatter = qobject_cast<QDeclarativeDateTimeFormatter*>(formatterComponent.create());
    if(formatterComponent.isError())
        qDebug() << formatterComponent.errors();
    QVERIFY(formatter != 0);

    QDate date(2008,12,24);
    QCOMPARE(formatter->date(), date);
    QCOMPARE(formatter->dateTime().date(), date);
    QCOMPARE(formatter->dateText(),date.toString(Qt::SystemLocaleShortDate));

    formatter->setLongStyle(true);
    QVERIFY(formatter->longStyle());
    QCOMPARE(formatter->dateText(),date.toString(Qt::SystemLocaleLongDate));

    formatter->setDateFormat("ddd MMMM d yy");
    QCOMPARE(formatter->dateFormat(), QLatin1String("ddd MMMM d yy"));
    QCOMPARE(formatter->dateText(),date.toString("ddd MMMM d yy"));

    QVERIFY(formatter->timeText().isEmpty());
    QVERIFY(formatter->dateTimeText().isEmpty());

    delete formatter;
}

void tst_qdeclarativedatetimeformatter::time()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent formatterComponent(&engine);
    formatterComponent.setData("import Qt 4.6\n DateTimeFormatter { time: \"14:15:38.200\" }", QUrl::fromLocalFile(""));
    QDeclarativeDateTimeFormatter *formatter = qobject_cast<QDeclarativeDateTimeFormatter*>(formatterComponent.create());
    if(formatterComponent.isError())
        qDebug() << formatterComponent.errors();
    QVERIFY(formatter != 0);

    QTime time(14,15,38,200);

    QCOMPARE(formatter->time(),time);
    QCOMPARE(formatter->dateTime().time(),time);

    QCOMPARE(formatter->timeText(),time.toString(Qt::SystemLocaleShortDate));

    formatter->setLongStyle(true);
    QCOMPARE(formatter->timeText(),time.toString(Qt::SystemLocaleLongDate));

    formatter->setTimeFormat("H:m:s a");
    QCOMPARE(formatter->timeFormat(), QLatin1String("H:m:s a"));
    QCOMPARE(formatter->timeText(),time.toString("H:m:s a"));

    formatter->setTimeFormat("hh:mm:ss.zzz");
    QCOMPARE(formatter->timeText(),time.toString("hh:mm:ss.zzz"));

    QVERIFY(formatter->dateText().isEmpty());
    QVERIFY(formatter->dateTimeText().isEmpty());

    delete formatter;
}

void tst_qdeclarativedatetimeformatter::dateTime()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent formatterComponent(&engine);
    formatterComponent.setData("import Qt 4.6\n DateTimeFormatter { dateTime: \"1978-03-04T09:13:54\" }", QUrl::fromLocalFile(""));
    QDeclarativeDateTimeFormatter *formatter = qobject_cast<QDeclarativeDateTimeFormatter*>(formatterComponent.create());
    if(formatterComponent.isError())
        qDebug() << formatterComponent.errors();
    QVERIFY(formatter != 0);

    QDateTime dateTime(QDate(1978,03,04),QTime(9,13,54));
    QCOMPARE(formatter->dateTime(),dateTime);
    QCOMPARE(formatter->date(),dateTime.date());
    QCOMPARE(formatter->time(),dateTime.time());
    QCOMPARE(formatter->dateTimeText(),dateTime.toString(Qt::SystemLocaleShortDate));

    formatter->setLongStyle(true);
    QCOMPARE(formatter->dateTimeText(),dateTime.toString(Qt::SystemLocaleLongDate));

    formatter->setDateTimeFormat("M/d/yy H:m:s a");
    QCOMPARE(formatter->dateTimeFormat(), QLatin1String("M/d/yy H:m:s a"));
    QCOMPARE(formatter->dateTimeText(),dateTime.toString("M/d/yy H:m:s a"));

    delete formatter;
}

QTEST_MAIN(tst_qdeclarativedatetimeformatter)

#include "tst_qdeclarativedatetimeformatter.moc"
