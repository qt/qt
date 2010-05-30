/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#ifdef QTEST_XMLPATTERNS
#include <QtXmlPatterns/QXmlItem>
#include <QtXmlPatterns/QXmlQuery>
#include <QtXmlPatterns/QXmlResultItems>

#include "../qxmlquery/MessageSilencer.h"
/*!
 \class tst_QXmlResultItems
 \internal
 \since 4.4
 \brief Tests class QXmlResultItems.

 */
class tst_QXmlResultItems : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void defaultConstructor() const;
    void next() const;
    void current() const;
    void hasError() const;
    void objectSize() const;
    void constCorrectness() const;

    void evalateWithInstantError() const;
    void evalateWithQueryError() const;
    void evaluate() const;
    void evaluate_data() const;
};

void tst_QXmlResultItems::defaultConstructor() const
{
    {
        QXmlResultItems result;
    }

    {
        QXmlResultItems result1;
        QXmlResultItems result2;
    }

    {
        QXmlResultItems result1;
        QXmlResultItems result2;
        QXmlResultItems result3;
    }
}

void tst_QXmlResultItems::next() const
{
    /* Check default value. */
    {
        QXmlResultItems result;
        QVERIFY(result.next().isNull());
    }

    /* Stress it on a default constructed value. */
    {
        QXmlResultItems result;
        QVERIFY(result.next().isNull());
        QVERIFY(result.next().isNull());
        QVERIFY(result.next().isNull());
    }
}

void tst_QXmlResultItems::current() const
{
    /* Check default value. */
    {
        QXmlResultItems result;
        QVERIFY(result.current().isNull());
    }

    /* Stress it on a default constructed value. */
    {
        QXmlResultItems result;
        QVERIFY(result.current().isNull());
        QVERIFY(result.current().isNull());
        QVERIFY(result.current().isNull());
    }
}

void tst_QXmlResultItems::hasError() const
{
    /* Check default value. */
    {
        QXmlResultItems result;
        QVERIFY(!result.hasError());
    }

    /* Stress it on a default constructed value. */
    {
        QXmlResultItems result;
        QVERIFY(!result.hasError());
        QVERIFY(!result.hasError());
        QVERIFY(!result.hasError());
    }
}

void tst_QXmlResultItems::objectSize() const
{
    /* A d-pointer plus a vtable pointer. */
    QCOMPARE(sizeof(QXmlResultItems), sizeof(void *) * 2);
}

void tst_QXmlResultItems::constCorrectness() const
{
    const QXmlResultItems result;

    /* These functions should be const. */
    result.current();
    result.hasError();
}

/*!
 What's special about this is that it's not the QAbstractXmlForwardIterator::next()
 that triggers the error, it's QPatternist::Expression::evaluateSingleton() directly.
 */
void tst_QXmlResultItems::evalateWithInstantError() const
{
    QXmlQuery query;
    MessageSilencer silencer;
    query.setMessageHandler(&silencer);
    query.setQuery(QLatin1String("fn:error()"));

    QXmlResultItems result;
    query.evaluateTo(&result);

    /* Check the values, and stress it. */
    for(int i = 0; i < 3; ++i)
    {
        QVERIFY(result.current().isNull());
        QVERIFY(result.next().isNull());
        QVERIFY(result.hasError());
    }
}

void tst_QXmlResultItems::evaluate() const
{
    QFETCH(QString, queryString);

    QXmlQuery query;
    query.setQuery(queryString);

    QVERIFY(query.isValid());

    QXmlResultItems result;
    query.evaluateTo(&result);
    QXmlItem item(result.next());

    while(!item.isNull())
    {
        QVERIFY(!result.current().isNull());
        QVERIFY(!result.hasError());
        item = result.next();
    }

    /* Now, stress beyond the end. */
    for(int i = 0; i < 3; ++i)
    {
        QVERIFY(result.current().isNull());
        QVERIFY(result.next().isNull());
    }
}

void tst_QXmlResultItems::evaluate_data() const
{
    QTest::addColumn<QString>("queryString");

    QTest::newRow("one atomic value") << QString::fromLatin1("1");
    QTest::newRow("two atomic values") << QString::fromLatin1("1, xs:hexBinary('FF')");
    QTest::newRow("one node") << QString::fromLatin1("attribute name {'body'}");
    QTest::newRow("one node") << QString::fromLatin1("attribute name {'body'}");
    QTest::newRow("two nodes") << QString::fromLatin1("attribute name {'body'}, <!-- comment -->");
}

void tst_QXmlResultItems::evalateWithQueryError() const
{
    /* This query is invalid. */
    const QXmlQuery query;

    QXmlResultItems result;
    query.evaluateTo(&result);

    QVERIFY(result.hasError());
    QVERIFY(result.next().isNull());
}

QTEST_MAIN(tst_QXmlResultItems)

#include "tst_qxmlresultitems.moc"
#else
QTEST_NOOP_MAIN
#endif
// vim: et:ts=4:sw=4:sts=4
