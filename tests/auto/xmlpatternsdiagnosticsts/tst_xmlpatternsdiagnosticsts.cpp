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
