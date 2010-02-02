/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <QtCore>
#include <QtTest/QtTest>
#include <QtXml/QXmlStreamReader>
#include <private/cycle_p.h>

class tst_Selftests: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void runSubTest_data();
    void runSubTest();
    void checkXML() const;
    void checkXML_data();
    void checkXunitxml() const;
    void checkXunitxml_data();

private:
    QStringList m_checkXMLBlacklist;
    QStringList m_checkXunitBlacklist;
    void doRunSubTest(QString &subdir, QStringList &arguments );
};

struct BenchmarkResult
{
    qint64  total;
    qint64  iterations;
    QString unit;

    inline QString toString() const
    { return QString("total:%1, unit:%2, iterations:%3").arg(total).arg(unit).arg(iterations); }

    static BenchmarkResult parse(QString const&, QString*);
};

QT_BEGIN_NAMESPACE
namespace QTest
{
template <>
inline bool qCompare
    (BenchmarkResult const &r1, BenchmarkResult const &r2,
     const char* actual, const char* expected, const char* file, int line)
{
    // First make sure the iterations and unit match.
    if (r1.iterations != r2.iterations || r1.unit != r2.unit) {
        /* Nope - compare whole string for best failure message */
        return qCompare(r1.toString(), r2.toString(), actual, expected, file, line);
    }

    /*
        Now check the value.  Some variance is allowed, and how much depends on the
        measured unit.
    */
    qreal variance = 0.;
    if (r1.unit == "msec") {
        variance = 0.1;
    }
    else if (r1.unit == "instr. loads") {
        variance = 0.001;
    }
    else if (r1.unit == "ticks") {
        variance = 0.001;
    }
    if (variance == 0.) {
        /* No variance allowed - compare whole string */
        return qCompare(r1.toString(), r2.toString(), actual, expected, file, line);
    }

    if (qAbs(qreal(r1.total) - qreal(r2.total)) <= qreal(r1.total)*variance) {
        return compare_helper(true, "COMPARE()", file, line);
    }

    /* Whoops, didn't match.  Compare the whole string for the most useful failure message. */
    return qCompare(r1.toString(), r2.toString(), actual, expected, file, line);
}
}
QT_END_NAMESPACE

static QList<QByteArray> splitLines(QByteArray ba)
{
    ba.replace('\r', "");
    return ba.split('\n');
}

static QList<QByteArray> expectedResult(const QString &subdir)
{
    QFile file(":/expected_" + subdir + ".txt");
    if (!file.open(QIODevice::ReadOnly))
        return QList<QByteArray>();
    return splitLines(file.readAll());
}

void tst_Selftests::runSubTest_data()
{
    QTest::addColumn<QString>("subdir");
    QTest::addColumn<QStringList>("arguments");

    QTest::newRow("subtest") << "subtest" << QStringList();
    QTest::newRow("warnings") << "warnings" << QStringList();
    QTest::newRow("maxwarnings") << "maxwarnings" << QStringList();
    QTest::newRow("cmptest") << "cmptest" << QStringList();
//    QTest::newRow("alive") << "alive" << QStringList(); // timer dependent
    QTest::newRow("globaldata") << "globaldata" << QStringList();
    QTest::newRow("skipglobal") << "skipglobal" << QStringList();
    QTest::newRow("skip") << "skip" << QStringList();
    QTest::newRow("strcmp") << "strcmp" << QStringList();
    QTest::newRow("expectfail") << "expectfail" << QStringList();
    QTest::newRow("sleep") << "sleep" << QStringList();
    QTest::newRow("fetchbogus") << "fetchbogus" << QStringList();
    QTest::newRow("crashes") << "crashes" << QStringList();
    QTest::newRow("multiexec") << "multiexec" << QStringList();
    QTest::newRow("failinit") << "failinit" << QStringList();
    QTest::newRow("failinitdata") << "failinitdata" << QStringList();
    QTest::newRow("skipinit") << "skipinit" << QStringList();
    QTest::newRow("skipinitdata") << "skipinitdata" << QStringList();
    QTest::newRow("datetime") << "datetime" << QStringList();
    QTest::newRow("singleskip") << "singleskip" << QStringList();

    //on windows assert does nothing in release mode and blocks execution with a popup window in debug mode
#if !defined(Q_OS_WIN)
    QTest::newRow("assert") << "assert" << QStringList();
#endif

    QTest::newRow("waitwithoutgui") << "waitwithoutgui" << QStringList();
    QTest::newRow("differentexec") << "differentexec" << QStringList();
#ifndef QT_NO_EXCEPTIONS
    // The machine that run the intel autotests will popup a dialog
    // with a warning that an uncaught exception was thrown.
    // This will time out and falsely fail, therefore we disable the test for that platform.
# if !defined(Q_CC_INTEL) || !defined(Q_OS_WIN)
    QTest::newRow("exceptionthrow") << "exceptionthrow" << QStringList();
# endif
#endif
    QTest::newRow("qexecstringlist") << "qexecstringlist" << QStringList();
    QTest::newRow("datatable") << "datatable" << QStringList();
    QTest::newRow("commandlinedata") << "commandlinedata" << QString("fiveTablePasses fiveTablePasses:fiveTablePasses_data1 -v2").split(' ');

#if defined(__GNUC__) && defined(__i386) && defined(Q_OS_LINUX)
    QTest::newRow("benchlibcallgrind") << "benchlibcallgrind" << QStringList("-callgrind");
#endif
    QTest::newRow("benchlibeventcounter") << "benchlibeventcounter" << QStringList("-eventcounter");
    QTest::newRow("benchliboptions") << "benchliboptions" << QStringList("-eventcounter");

    //### These tests are affected by timing and whether the CPU tick counter is
    //### monotonically increasing. They won't work on some machines so leave them off by default.
    //### Feel free to uncomment for your own testing.
#if 0
    QTest::newRow("benchlibwalltime") << "benchlibwalltime" << QStringList();
    QTest::newRow("benchlibtickcounter") << "benchlibtickcounter" << QStringList("-tickcounter");
#endif

    QTest::newRow("xunit") << "xunit" << QStringList("-xunitxml");
    QTest::newRow("longstring") << "longstring" << QStringList();

}

void tst_Selftests::doRunSubTest(QString &subdir, QStringList &arguments )
{
    QProcess proc;
    proc.setEnvironment(QStringList(""));
    proc.start(subdir + "/" + subdir, arguments);
    QVERIFY2(proc.waitForFinished(), qPrintable(proc.errorString()));

    const QByteArray out(proc.readAllStandardOutput());
    const QByteArray err(proc.readAllStandardError());

    /* Some platforms decides to output a message for uncaught exceptions. For instance,
     * this is what windows platforms says:
     * "This application has requested the Runtime to terminate it in an unusual way.
     * Please contact the application's support team for more information." */
    if(subdir != QLatin1String("exceptionthrow") && subdir != QLatin1String("fetchbogus")
        && subdir != QLatin1String("xunit"))
        QVERIFY2(err.isEmpty(), err.constData());

    QList<QByteArray> res = splitLines(out);
    QList<QByteArray> exp = expectedResult(subdir);

    if (exp.count() == 0) {
        QList<QList<QByteArray> > expArr;
        int i = 1;
        do {
            exp = expectedResult(subdir + QString("_%1").arg(i++));
            if (exp.count())
            expArr += exp;
        } while(exp.count());

        for (int j = 0; j < expArr.count(); ++j) {
            if (res.count() == expArr.at(j).count()) {
                exp = expArr.at(j);
                break;
            }
        }
    } else {
        QCOMPARE(res.count(), exp.count());
    }

    bool benchmark = false;
    for (int i = 0; i < res.count(); ++i) {
        QByteArray line = res.at(i);
        if (line.startsWith("Config: Using QTest"))
            continue;
        // the __FILE__ __LINE__ output is compiler dependent, skip it
        if (line.startsWith("   Loc: [") && line.endsWith(")]"))
            continue;
        if (line.endsWith(" : failure location"))
            continue;

        const QString output(QString::fromLatin1(line));
        const QString expected(QString::fromLatin1(exp.at(i)).replace("<INSERT_QT_VERSION_HERE>", QT_VERSION_STR));

        if (line.contains("ASSERT") && output != expected)
            QEXPECT_FAIL("assert", "QTestLib prints out the absolute path.", Continue);

        /* On some platforms we compile without RTTI, and as a result we never throw an exception. */
        if(expected.startsWith(QLatin1String("FAIL!  : tst_Exception::throwException() Caught unhandled exce")) && expected != output)
            QCOMPARE(output.simplified(), QString::fromLatin1("tst_Exception::throwException()").simplified());
        else
        {
            if(output != expected && qstrcmp(QTest::currentDataTag(), "subtest") == 0)
            {
            /* The floating point formatting differs between platforms, so let's just skip it. */
            continue;
            }
            else {
                /*
                   Are we expecting this line to be a benchmark result?
                   If so, don't do a literal comparison, since results have some natural variance.
                */
                if (benchmark) {
                    QString error;

                    BenchmarkResult actualResult = BenchmarkResult::parse(output, &error);
                    QVERIFY2(error.isEmpty(), qPrintable(QString("Actual line didn't parse as benchmark result: %1\nLine: %2").arg(error).arg(output)));

                    BenchmarkResult expectedResult = BenchmarkResult::parse(expected, &error);
                    QVERIFY2(error.isEmpty(), qPrintable(QString("Expected line didn't parse as benchmark result: %1\nLine: %2").arg(error).arg(expected)));

                    QCOMPARE(actualResult, expectedResult);
                }
                else {
                    QCOMPARE(output, expected);
                }
            }
        }

        benchmark = line.startsWith("RESULT : ");
    }
}

void tst_Selftests::runSubTest()
{
    QFETCH(QString, subdir);
    QFETCH(QStringList, arguments);

    doRunSubTest(subdir, arguments);
}

void tst_Selftests::initTestCase()
{
#if !defined(Q_OS_UNIX) || defined(Q_WS_MAC)
    m_checkXMLBlacklist.append("crashes"); // This test crashes (XML valid on Unix only)
#endif
    m_checkXMLBlacklist.append("waitwithoutgui"); // This test is not a QTestLib test.

    /* Output from several tests is broken with the XML output method,
     * and it's quite heavy in the design. See task 155001. */
    m_checkXMLBlacklist.append("multiexec");
    m_checkXMLBlacklist.append("differentexec");
    m_checkXMLBlacklist.append("qexecstringlist");
    m_checkXMLBlacklist.append("benchliboptions");

    /* These tests use printf and therefore corrupt the testlog */
    m_checkXMLBlacklist.append("subtest");
    m_checkXMLBlacklist.append("globaldata");
    m_checkXMLBlacklist.append("warnings");

    m_checkXunitBlacklist = m_checkXMLBlacklist;
}

void tst_Selftests::checkXML() const
{
    QFETCH(QString, subdir);
    QFETCH(QStringList, arguments);

    if(m_checkXMLBlacklist.contains(subdir))
        return;

    QStringList args;
    /* Test both old (-flush) and new XML logger implementation */
    for (int i = 0; i < 2; ++i) {
        bool flush = i;
        args = arguments;
        args.prepend("-xml");
        if (flush) args.prepend("-flush");

        QProcess proc;
        proc.setEnvironment(QStringList(""));
        proc.start(subdir + "/" + subdir, args);
        QVERIFY(proc.waitForFinished());

        QByteArray out(proc.readAllStandardOutput());
        QByteArray err(proc.readAllStandardError());

        /* Some platforms decides to output a message for uncaught exceptions. For instance,
         * this is what windows platforms says:
         * "This application has requested the Runtime to terminate it in an unusual way.
         * Please contact the application's support team for more information." */
        if(subdir != QLatin1String("exceptionthrow") && subdir != QLatin1String("fetchbogus"))
            QVERIFY2(err.isEmpty(), err.constData());

        QXmlStreamReader reader(out);

        while(!reader.atEnd())
            reader.readNext();

        QVERIFY2(!reader.error(), qPrintable(QString("(flush %0) line %1, col %2: %3")
            .arg(flush)
            .arg(reader.lineNumber())
            .arg(reader.columnNumber())
            .arg(reader.errorString())
        ));
    }
}

void tst_Selftests::checkXunitxml() const
{
    QFETCH(QString, subdir);
    QFETCH(QStringList, arguments);

    if(m_checkXunitBlacklist.contains(subdir))
        return;

    arguments.prepend("-xunitxml");
    arguments.prepend("-flush");

    QProcess proc;
    proc.setEnvironment(QStringList(""));
    proc.start(subdir + "/" + subdir, arguments);
    QVERIFY(proc.waitForFinished());

    QByteArray out(proc.readAllStandardOutput());
    QByteArray err(proc.readAllStandardError());

//    qDebug()<<out;

    /* Some platforms decides to output a message for uncaught exceptions. For instance,
     * this is what windows platforms says:
     * "This application has requested the Runtime to terminate it in an unusual way.
     * Please contact the application's support team for more information." */
    if(subdir != QLatin1String("exceptionthrow") && subdir != QLatin1String("fetchbogus"))
        QVERIFY2(err.isEmpty(), err.constData());

    QXmlStreamReader reader(out);

    while(!reader.atEnd())
        reader.readNext();

    QVERIFY2(!reader.error(), qPrintable(QString("line %1, col %2: %3")
        .arg(reader.lineNumber())
        .arg(reader.columnNumber())
        .arg(reader.errorString())
    ));
}

void tst_Selftests::checkXunitxml_data()
{
    checkXML_data();
}

void tst_Selftests::checkXML_data()
{
    runSubTest_data();
    QTest::newRow("badxml 1") << "badxml" << QStringList();
    QTest::newRow("badxml 2") << "badxml" << (QStringList() << "-badstring" << "0");
    QTest::newRow("badxml 3") << "badxml" << (QStringList() << "-badstring" << "1");
    QTest::newRow("badxml 4") << "badxml" << (QStringList() << "-badstring" << "2");
    QTest::newRow("badxml 5") << "badxml" << (QStringList() << "-badstring" << "3");
}

/* Parse line into the BenchmarkResult it represents. */
BenchmarkResult BenchmarkResult::parse(QString const& line, QString* error)
{
    if (error) *error = QString();
    BenchmarkResult out;

    QString remaining = line.trimmed();

    if (remaining.isEmpty()) {
        if (error) *error = "Line is empty";
        return out;
    }

    /* This code avoids using a QRegExp because QRegExp might be broken. */

    /* Sample format: 4,000 msec per iteration (total: 4000, iterations: 1) */

    QString sFirstNumber;
    while (!remaining.isEmpty() && !remaining.at(0).isSpace()) {
        sFirstNumber += remaining.at(0);
        remaining.remove(0,1);
    }
    remaining = remaining.trimmed();

    /* 4,000 -> 4000 */
    sFirstNumber.remove(',');

    /* Should now be parseable as floating point */
    bool ok;
    double firstNumber = sFirstNumber.toDouble(&ok);
    if (!ok) {
        if (error) *error = sFirstNumber + " (at beginning of line) is not a valid number";
        return out;
    }

    /* Remaining: msec per iteration (total: 4000, iterations: 1) */
    static const char periterbit[] = " per iteration (total: ";
    QString unit;
    while (!remaining.startsWith(periterbit) && !remaining.isEmpty()) {
        unit += remaining.at(0);
        remaining.remove(0,1);
    }
    if (remaining.isEmpty()) {
        if (error) *error = "Could not find pattern: '<unit> per iteration (total: '";
        return out;
    }

    remaining = remaining.mid(sizeof(periterbit)-1);

    /* Remaining: 4000, iterations: 1) */
    static const char itersbit[] = ", iterations: ";
    QString sTotal;
    while (!remaining.startsWith(itersbit) && !remaining.isEmpty()) {
        sTotal += remaining.at(0);
        remaining.remove(0,1);
    }
    if (remaining.isEmpty()) {
        if (error) *error = "Could not find pattern: '<number>, iterations: '";
        return out;
    }

    remaining = remaining.mid(sizeof(itersbit)-1);

    qint64 total = sTotal.toLongLong(&ok);
    if (!ok) {
        if (error) *error = sTotal + " (total) is not a valid integer";
        return out;
    }

    /* Remaining: 1) */
    QString sIters;
    while (remaining != QLatin1String(")") && !remaining.isEmpty()) {
        sIters += remaining.at(0);
        remaining.remove(0,1);
    }
    if (remaining.isEmpty()) {
        if (error) *error = "Could not find pattern: '<num>)'";
        return out;
    }
    qint64 iters = sIters.toLongLong(&ok);
    if (!ok) {
        if (error) *error = sIters + " (iterations) is not a valid integer";
        return out;
    }

    double calcFirstNumber = double(total)/double(iters);
    if (!qFuzzyCompare(firstNumber, calcFirstNumber)) {
        if (error) *error = QString("total/iters is %1, but benchlib output result as %2").arg(calcFirstNumber).arg(firstNumber);
        return out;
    }

    out.total = total;
    out.unit = unit;
    out.iterations = iters;
    return out;
}

QTEST_MAIN(tst_Selftests)

#include "tst_selftests.moc"
