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

#include <QProcess>
#include "TestSuite.h"
#include "TestSuiteResult.h"
#include "XMLWriter.h"
#include "ExitCode.h"
#include "Worker.h"
#include "private/qautoptr_p.h"
#include "tst_suitetest.h"

using namespace QPatternistSDK;

tst_SuiteTest::tst_SuiteTest(const SuiteType suiteType,
                             const bool alwaysRun) : m_existingBaseline(inputFile(QLatin1String("Baseline.xml")))
                                                   , m_candidateBaseline(inputFile(QLatin1String("CandidateBaseline.xml")))
                                                   , m_abortRun(!alwaysRun && !QFile::exists(QLatin1String("runTests")))
                                                   , m_suiteType(suiteType)
{
}

/*!
 Returns an absolute path to the XQTS catalog, or flags a failure using
 QTestLib's mechanisms.

 Finding the location of the catalog is done with `p4 where` such that we don't have
 to care about where it is checked out.
 */
void tst_SuiteTest::initTestCase()
{
    catalogPath(m_catalogPath);
}

/*!
  Just runs the test suite and writes the result to m_candidateBaseline.
 */
void tst_SuiteTest::runTestSuite() const
{
    if(m_abortRun)
        QSKIP("The test suite is not available, no tests are run.", SkipAll);

    QString errMsg;
    const QFileInfo fi(m_catalogPath);
    const QUrl catalogPath(QUrl::fromLocalFile(fi.absoluteFilePath()));

    TestSuite::SuiteType suiteType;
    switch (m_suiteType) {
    case XQuerySuite:
        suiteType = TestSuite::XQuerySuite;
        break;
    case XsltSuite:
        suiteType = TestSuite::XsltSuite;
        break;
    case XsdSuite:
        suiteType = TestSuite::XsdSuite;
        break;
    default:
        break;
    }

    TestSuite *const ts = TestSuite::openCatalog(catalogPath, errMsg, true, suiteType);

    QVERIFY2(ts, qPrintable(QString::fromLatin1("Failed to open the catalog, maybe it doesn't exist or is broken: %1").arg(errMsg)));

    /* Run the tests, and serialize the result(as according to XQTSResult.xsd) to standard out. */
    TestSuiteResult *const result = ts->runSuite();
    Q_ASSERT(result);

    QFile out(m_candidateBaseline);
    QVERIFY(out.open(QIODevice::WriteOnly));

    XMLWriter serializer(&out);
    result->toXML(serializer);

    delete result;
    delete ts;
}

void tst_SuiteTest::checkTestSuiteResult() const
{
    if(m_abortRun)
        QSKIP("This test takes too long time to run on the majority of platforms.", SkipAll);

    typedef QList<QFileInfo> QFileInfoList;

    const QFileInfo baseline(m_existingBaseline);
    const QFileInfo result(m_candidateBaseline);
    QFileInfoList list;
    list.append(baseline);
    list.append(result);

    const QFileInfoList::const_iterator end(list.constEnd());

    QEventLoop eventLoop;
    const QPatternist::AutoPtr<Worker> worker(new Worker(eventLoop, m_existingBaseline, result));

    /* Passed to ResultThreader so it knows what kind of file it is handling. */
    ResultThreader::Type type = ResultThreader::Baseline;

    for(QFileInfoList::const_iterator it(list.constBegin()); it != end; ++it)
    {
        QFileInfo i(*it);
        i.makeAbsolute();

        QVERIFY2(i.exists(), qPrintable(QString::fromLatin1("File %1 does not exist.")
                                                            .arg(i.fileName())));

        QFile *const file = new QFile(i.absoluteFilePath(), worker.data());

        QVERIFY2(file->open(QIODevice::ReadOnly), qPrintable(QString::fromLatin1("Could not open file %1 for reading.")
                                                             .arg(i.fileName())));

        ResultThreader *handler = new ResultThreader(eventLoop, file, type, worker.data());

        QObject::connect(handler, SIGNAL(finished()), worker.data(), SLOT(threadFinished()));

        handler->start(); /* Start the thread. It now parses the file
                             and emits threadFinished() when done. */
        type = ResultThreader::Result;
    }

    const int exitCode = eventLoop.exec();

    QCOMPARE(exitCode, 0);
}

bool tst_SuiteTest::dontRun() const
{
    return m_abortRun;
}
#endif


// vim: et:ts=4:sw=4:sts=4
