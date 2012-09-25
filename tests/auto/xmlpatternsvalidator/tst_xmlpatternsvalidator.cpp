/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite module of the Qt Toolkit.
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

#include <QFile>
#include <QtTest/QtTest>

#ifdef QTEST_XMLPATTERNS

#include "../qxmlquery/TestFundament.h"
#include "../network-settings.h"

/*!
 \class tst_XmlPatterns
 \internal
 \since 4.6
 \brief Tests the command line interface, \c xmlpatternsvalidator, for the XML validation code.
 */
class tst_XmlPatternsValidator : public QObject
                               , private TestFundament
{
    Q_OBJECT

public:
    tst_XmlPatternsValidator();

private Q_SLOTS:
    void initTestCase();
    void xsdSupport();
    void xsdSupport_data() const;

private:
    const QString   m_command;
    bool            m_dontRun;
};

tst_XmlPatternsValidator::tst_XmlPatternsValidator()
    : m_command(QLatin1String("xmlpatternsvalidator"))
    , m_dontRun(false)
{
}

void tst_XmlPatternsValidator::initTestCase()
{
    QProcess process;
    process.start(m_command);

    if(!process.waitForFinished())
    {
        m_dontRun = true;
        QEXPECT_FAIL("", "The command line tool is not in the path, most likely because Qt "
                         "has been partically built, such as only the sub-src rule. No tests will be run.", Abort);
        QVERIFY(false);
    }
}

void tst_XmlPatternsValidator::xsdSupport()
{
    if(m_dontRun)
        QSKIP("The command line utility is not in the path.", SkipAll);

#ifdef Q_OS_WINCE
    QSKIP("WinCE: This test uses unsupported WinCE functionality", SkipAll);
#endif

    QFETCH(int,         expectedExitCode);
    QFETCH(QStringList, arguments);
    QFETCH(QString,     cwd);

    QProcess process;

    if(!cwd.isEmpty())
        process.setWorkingDirectory(inputFile(cwd));

    process.start(m_command, arguments);

    QVERIFY(process.waitForFinished());
    QCOMPARE(process.exitStatus(), QProcess::NormalExit);

    if(process.exitCode() != expectedExitCode)
        QTextStream(stderr) << "foo:" << process.readAllStandardError();

    QCOMPARE(process.exitCode(), expectedExitCode);
}

void tst_XmlPatternsValidator::xsdSupport_data() const
{
#ifdef Q_OS_WINCE
    return;
#endif

    QTest::addColumn<int>("expectedExitCode");
    QTest::addColumn<QStringList>("arguments");
    QTest::addColumn<QString>("cwd");

    QTest::newRow("No arguments")
        << 2
        << QStringList()
        << QString();

    QTest::newRow("A valid schema")
        << 0
        << QStringList(QLatin1String("files/valid_schema.xsd"))
        << QString();

    QTest::newRow("An invalid schema")
        << 1
        << QStringList(QLatin1String("files/invalid_schema.xsd"))
        << QString();

    QTest::newRow("An instance and valid schema")
        << 0
        << (QStringList() << QLatin1String("files/instance.xml")
                          << QLatin1String("files/valid_schema.xsd"))
        << QString();

    QTest::newRow("An instance and invalid schema")
        << 1
        << (QStringList() << QLatin1String("files/instance.xml")
                          << QLatin1String("files/invalid_schema.xsd"))
        << QString();

    QTest::newRow("An instance and not matching schema")
        << 1
        << (QStringList() << QLatin1String("files/instance.xml")
                          << QLatin1String("files/other_valid_schema.xsd"))
        << QString();

    QTest::newRow("Two instance documents")
        << 1
        << (QStringList() << QLatin1String("files/instance.xml")
                          << QLatin1String("files/instance.xml"))
        << QString();

    QTest::newRow("Three instance documents")
        << 2
        << (QStringList() << QLatin1String("files/instance.xml")
                          << QLatin1String("files/instance.xml")
                          << QLatin1String("files/instance.xml"))
        << QString();

    QTest::newRow("Two schema documents")
        << 1
        << (QStringList() << QLatin1String("files/valid_schema.xsd")
                          << QLatin1String("files/valid_schema.xsd"))
        << QString();

    QTest::newRow("A schema aware valid instance document")
        << 0
        << (QStringList() << QLatin1String("files/sa_valid_instance.xml"))
        << QString();

    QTest::newRow("A schema aware invalid instance document")
        << 1
        << (QStringList() << QLatin1String("files/sa_invalid_instance.xml"))
        << QString();

    QTest::newRow("A non-schema aware instance document")
        << 1
        << (QStringList() << QLatin1String("files/instance.xml"))
        << QString();

    QTest::newRow("QTBUG-8394 A schema with an indirectly included type")
        << 0
        << (QStringList() << QLatin1String("files/indirect-include-a.xsd"))
        << QString();

    QTest::newRow("QTBUG-8394 A schema with an indirectly imported type")
        << 0
        << (QStringList() << QLatin1String("files/indirect-import-a.xsd"))
        << QString();

    QTest::newRow("QTBUG-8394 A schema with an indirectly redefined type")
        << 0
        << (QStringList() << QLatin1String("files/indirect-redefine-a.xsd"))
        << QString();

    QTest::newRow("QTBUG-8920 A schema with a complex type that indirectly includes an anonymous type")
        << 0
        << (QStringList() << QLatin1String("files/complex-type-including-anonymous-type.xsd"))
        << QString();

    QTest::newRow("QTBUG-11559 A schema and instance with a dateTime containing microseconds")
        << 0
        << (QStringList() << QLatin1String("files/dateTime-with-microseconds.xml")
                          << QLatin1String("files/dateTime-with-microseconds.xsd"))
        << QString();

    QTest::newRow("QTBUG-21375 A schema with a xs:double based simple type with non-numeric restrictions")
        << 0
        << (QStringList() << QLatin1String("files/schema-with-restrictions.xsd"))
        << QString();
}

QTEST_MAIN(tst_XmlPatternsValidator)

#include "tst_xmlpatternsvalidator.moc"
#else
QTEST_NOOP_MAIN
#endif

// vim: et:ts=4:sw=4:sts=4
