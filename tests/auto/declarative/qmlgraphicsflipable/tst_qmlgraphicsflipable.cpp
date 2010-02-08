/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlgraphicsflipable_p.h>
#include <private/qmlvaluetype_p.h>
#include <QFontMetrics>
#include <private/qmlgraphicsrectangle_p.h>
#include <math.h>

class tst_qmlgraphicsflipable : public QObject
{
    Q_OBJECT
public:
    tst_qmlgraphicsflipable();

private slots:
    void create();
    void checkFrontAndBack();
    void setFrontAndBack();

private:
    QmlEngine engine;
};

tst_qmlgraphicsflipable::tst_qmlgraphicsflipable()
{
}

void tst_qmlgraphicsflipable::create()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/test-flipable.qml"));
    QmlGraphicsFlipable *obj = qobject_cast<QmlGraphicsFlipable*>(c.create());

    QVERIFY(obj != 0);
    delete obj;
}

void tst_qmlgraphicsflipable::checkFrontAndBack()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/test-flipable.qml"));
    QmlGraphicsFlipable *obj = qobject_cast<QmlGraphicsFlipable*>(c.create());

    QVERIFY(obj != 0);
    QVERIFY(obj->front() != 0);
    QVERIFY(obj->back() != 0);
    delete obj;
}

void tst_qmlgraphicsflipable::setFrontAndBack()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/test-flipable.qml"));
    QmlGraphicsFlipable *obj = qobject_cast<QmlGraphicsFlipable*>(c.create());

    QVERIFY(obj != 0);
    QVERIFY(obj->front() != 0);
    QVERIFY(obj->back() != 0);

    QString message = "QML Flipable (" + c.url().toString() + ":3:1) front is a write-once property";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(message));
    obj->setFront(new QmlGraphicsRectangle());

    message = "QML Flipable (" + c.url().toString() + ":3:1) back is a write-once property";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(message));
    obj->setBack(new QmlGraphicsRectangle());
    delete obj;
}

QTEST_MAIN(tst_qmlgraphicsflipable)

#include "tst_qmlgraphicsflipable.moc"
