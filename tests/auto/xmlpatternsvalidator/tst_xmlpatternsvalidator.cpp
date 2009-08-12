/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
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

    QCOMPARE(process.exitStatus(), QProcess::NormalExit);
    QVERIFY(process.waitForFinished());

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
}

QTEST_MAIN(tst_XmlPatternsValidator)

#include "tst_xmlpatternsvalidator.moc"
#else
QTEST_NOOP_MAIN
#endif

// vim: et:ts=4:sw=4:sts=4
