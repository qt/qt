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
#include <private/qmlbind_p.h>
#include <private/qmlgraphicsrectangle_p.h>
#include "../../../shared/util.h"

class tst_qmlbinding : public QObject

{
    Q_OBJECT
public:
    tst_qmlbinding();

private slots:
    void binding();
    void whenAfterValue();

private:
    QmlEngine engine;
};

tst_qmlbinding::tst_qmlbinding()
{
}

void tst_qmlbinding::binding()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/test-binding.qml"));
    QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());

    QVERIFY(rect != 0);
    QCOMPARE(rect->color(), QColor("yellow"));
    QCOMPARE(rect->property("text").toString(), QString("Hello"));

    rect->setProperty("changeColor", true);
    QCOMPARE(rect->color(), QColor("red"));

    QmlBind *binding = qobject_cast<QmlBind*>(rect->findChild<QmlBind*>("binding1"));
    QVERIFY(binding != 0);
    QCOMPARE(binding->object(), rect);
    QCOMPARE(binding->property(), QLatin1String("text"));
    QCOMPARE(binding->value().toString(), QLatin1String("Hello"));

    delete rect;
}

void tst_qmlbinding::whenAfterValue()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/test-binding2.qml"));
    QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());

    QVERIFY(rect != 0);
    QCOMPARE(rect->color(), QColor("yellow"));
    QCOMPARE(rect->property("text").toString(), QString("Hello"));

    rect->setProperty("changeColor", true);
    QCOMPARE(rect->color(), QColor("red"));

    delete rect;
}

QTEST_MAIN(tst_qmlbinding)

#include "tst_qmlbinding.moc"
