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
 \class tst_XmlPatternsDiagnosticsTS
 \internal
 \since 4.5
 \brief Test QtXmlPatterns test suite driver in tests/auto/xmlpatternsxqts/lib/.
 */
class tst_XmlPatternsDiagnosticsTS : public tst_SuiteTest
{
    Q_OBJECT
public:
    tst_XmlPatternsDiagnosticsTS();
protected:
    virtual void catalogPath(QString &write) const;
};

tst_XmlPatternsDiagnosticsTS::tst_XmlPatternsDiagnosticsTS() : tst_SuiteTest(tst_SuiteTest::XQuerySuite, true)
{
}

void tst_XmlPatternsDiagnosticsTS::catalogPath(QString &write) const
{
    write = QLatin1String("TestSuite/DiagnosticsCatalog.xml");
}

QTEST_MAIN(tst_XmlPatternsDiagnosticsTS)

#include "tst_xmlpatternsdiagnosticsts.moc"
#else
QTEST_NOOP_MAIN
#endif

// vim: et:ts=4:sw=4:sts=4
