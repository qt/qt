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

    write = QLatin1String("TESTSUITE/XQTSCatalog.xml");
    return;
}

QTEST_MAIN(tst_XmlPatternsXQTS)

#include "tst_xmlpatternsxqts.moc"
#else
QTEST_NOOP_MAIN
#endif

// vim: et:ts=4:sw=4:sts=4
