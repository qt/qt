/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
****************************************************************************/

#include <qtest.h>
#include "private/qguard_p.h"
#include <QWeakPointer>

class tst_pointers : public QObject
{
    Q_OBJECT

public:
    tst_pointers() {}

private slots:
    void guard();
    void weakPointer();
};

void tst_pointers::guard()
{
    QObject *obj = new QObject;
    QBENCHMARK {
        QGuard<QObject> guardedObject;
        guardedObject = obj;
    }
}

void tst_pointers::weakPointer()
{
    QObject *obj = new QObject;
    QBENCHMARK {
        QWeakPointer<QObject> guardedObject;
        guardedObject = obj;
    }
}

QTEST_MAIN(tst_pointers)
#include "tst_pointers.moc"
