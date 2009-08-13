/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
****************************************************************************/

#include <QtTest/QtTest>

#ifdef QTEST_XMLPATTERNS

#include "tst_suitetest.h"

/*!
 \internal
 \brief Test QXsdSchemaParser against W3C's XSD test suite.
 */
class tst_XmlPatternsSchemaTS : public tst_SuiteTest
{
    Q_OBJECT
public:
    tst_XmlPatternsSchemaTS();
protected:
    virtual void catalogPath(QString &write) const;
};

tst_XmlPatternsSchemaTS::tst_XmlPatternsSchemaTS()
    : tst_SuiteTest(tst_SuiteTest::XsdSuite)
{
}

void tst_XmlPatternsSchemaTS::catalogPath(QString &write) const
{
    write = QLatin1String("TESTSUITE/testSuites.xml");
}

QTEST_MAIN(tst_XmlPatternsSchemaTS)

#include "tst_xmlpatternsschemats.moc"
#else
QTEST_NOOP_MAIN
#endif

// vim: et:ts=4:sw=4:sts=4
