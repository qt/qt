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

#include "tst_suitetest.h"

/*!
 \class tst_XmlPatternsXQTS
 \internal
 \since 4.4
 \brief Tests the actual engine by running W3C's conformance test suite.
 */
class tst_XmlPatternsXQTS : public tst_SuiteTest
{
    Q_OBJECT
public:
    tst_XmlPatternsXQTS();
public:
    virtual void catalogPath(QString &write) const;
};

tst_XmlPatternsXQTS::tst_XmlPatternsXQTS() : tst_SuiteTest(tst_SuiteTest::XQuerySuite)
{
}

void tst_XmlPatternsXQTS::catalogPath(QString &write) const
{
    if(dontRun())
        QSKIP("This test takes too long time to run on the majority of platforms.", SkipAll);

    QProcess p4;

    QStringList arguments;
    arguments << QLatin1String("where")
              << QLatin1String("//depot/autotests/4.4/tests/auto/xmlpatternsxqts/XQTS/XQTSCatalog.xml");
    p4.start(QLatin1String("p4"), arguments);
    QVERIFY(p4.waitForFinished());
    QCOMPARE(p4.exitCode(), 0);
    QCOMPARE(p4.exitStatus(), QProcess::NormalExit);

    /* `p4 where' prints for instance:
     *
     *    //depot/qt/4.4/tests/auto/xmlpatternsxqts/... //fenglich-englich/qt-4.4/tests/auto/xmlpatternsxqts/... /home/fenglich/dev/autotests/4.4/tests/auto/xmlpatternsxqts/XQTS/XQTSCatalog.xml
     *
     * so we want the last string.
     */
    write = QString::fromLocal8Bit(p4.readAllStandardOutput()).split(QLatin1Char(' ')).last().trimmed();

    if(write.isEmpty() || !QFile::exists(write))
    {
        QEXPECT_FAIL("", "//depot/autotests/4.4/tests/auto/xmlpatternsxqts/XQTS/ must be part of the perforce client spec, "
                         "checked out at an arbitrary location, for this test to run. The test suite will now be skipped.", Abort);
        QVERIFY(false);
    }
}

QTEST_MAIN(tst_XmlPatternsXQTS)

#include "tst_xmlpatternsxqts.moc"
#else
QTEST_NOOP_MAIN
#endif

// vim: et:ts=4:sw=4:sts=4
