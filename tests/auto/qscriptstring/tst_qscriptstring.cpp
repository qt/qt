/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QtTest/QtTest>

#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptstring.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QScriptString : public QObject
{
    Q_OBJECT

public:
    tst_QScriptString();
    virtual ~tst_QScriptString();

private slots:
    void test();
};

tst_QScriptString::tst_QScriptString()
{
}

tst_QScriptString::~tst_QScriptString()
{
}

void tst_QScriptString::test()
{
    QScriptEngine eng;

    {
        QScriptString str;
        QVERIFY(!str.isValid());
        QVERIFY(str == str);
        QVERIFY(!(str != str));
        QVERIFY(str.toString().isNull());

        QScriptString str1(str);
        QVERIFY(!str1.isValid());

        QScriptString str2 = str;
        QVERIFY(!str2.isValid());
    }

    for (int x = 0; x < 2; ++x) {
        QString ciao = QString::fromLatin1("ciao");
        QScriptString str = eng.toStringHandle(ciao);
        QVERIFY(str.isValid());
        QVERIFY(str == str);
        QVERIFY(!(str != str));
        QCOMPARE(str.toString(), ciao);

        QScriptString str1(str);
        QCOMPARE(str, str1);

        QScriptString str2 = str;
        QCOMPARE(str, str2);

        QScriptString str3 = eng.toStringHandle(ciao);
        QVERIFY(str3.isValid());
        QCOMPARE(str, str3);

        eng.collectGarbage();

        QVERIFY(str.isValid());
        QCOMPARE(str.toString(), ciao);
        QVERIFY(str1.isValid());
        QCOMPARE(str1.toString(), ciao);
        QVERIFY(str2.isValid());
        QCOMPARE(str2.toString(), ciao);
        QVERIFY(str3.isValid());
        QCOMPARE(str3.toString(), ciao);
    }

    {
        QScriptEngine *eng2 = new QScriptEngine;
        QString one = QString::fromLatin1("one");
        QString two = QString::fromLatin1("two");
        QScriptString oneInterned = eng2->toStringHandle(one);
        QCOMPARE(oneInterned.toString(), one);
        QScriptString twoInterned = eng2->toStringHandle(two);
        QCOMPARE(twoInterned.toString(), two);
        QVERIFY(oneInterned != twoInterned);
        QVERIFY(!(oneInterned == twoInterned));

        delete eng2;

        QVERIFY(!oneInterned.isValid());
        QVERIFY(!twoInterned.isValid());
    }
}

QTEST_MAIN(tst_QScriptString)
#include "tst_qscriptstring.moc"
