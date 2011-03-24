/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtDeclarative/qsgview.h>
#include <private/qsgflipable_p.h>
#include <private/qdeclarativevaluetype_p.h>
#include <QFontMetrics>
#include <private/qsgrectangle_p.h>
#include <math.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qsgflipable : public QObject
{
    Q_OBJECT
public:
    tst_qsgflipable();

private slots:
    void create();
    void checkFrontAndBack();
    void setFrontAndBack();

    // below here task issues
    void QTBUG_9161_crash();
    void QTBUG_8474_qgv_abort();

private:
    QDeclarativeEngine engine;
};

tst_qsgflipable::tst_qsgflipable()
{
}

void tst_qsgflipable::create()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/test-flipable.qml"));
    QSGFlipable *obj = qobject_cast<QSGFlipable*>(c.create());

    QVERIFY(obj != 0);
    delete obj;
}

void tst_qsgflipable::checkFrontAndBack()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/test-flipable.qml"));
    QSGFlipable *obj = qobject_cast<QSGFlipable*>(c.create());

    QVERIFY(obj != 0);
    QVERIFY(obj->front() != 0);
    QVERIFY(obj->back() != 0);
    delete obj;
}

void tst_qsgflipable::setFrontAndBack()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/test-flipable.qml"));
    QSGFlipable *obj = qobject_cast<QSGFlipable*>(c.create());

    QVERIFY(obj != 0);
    QVERIFY(obj->front() != 0);
    QVERIFY(obj->back() != 0);

    QString message = c.url().toString() + ":3:1: QML Flipable: front is a write-once property";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(message));
    obj->setFront(new QSGRectangle());

    message = c.url().toString() + ":3:1: QML Flipable: back is a write-once property";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(message));
    obj->setBack(new QSGRectangle());
    delete obj;
}

void tst_qsgflipable::QTBUG_9161_crash()
{
    QSGView *canvas = new QSGView;
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/crash.qml"));
    QSGItem *root = canvas->rootObject();
    QVERIFY(root != 0);
    canvas->show();
    delete canvas;
}

void tst_qsgflipable::QTBUG_8474_qgv_abort()
{
    QSGView *canvas = new QSGView;
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/flipable-abort.qml"));
    QSGItem *root = canvas->rootObject();
    QVERIFY(root != 0);
    canvas->show();
    delete canvas;
}

QTEST_MAIN(tst_qsgflipable)

#include "tst_qsgflipable.moc"
