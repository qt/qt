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
#include <QtXmlPatterns/QXmlFormatter>
#include <QtXmlPatterns/QXmlQuery>

/*!
 \class tst_QXmlFormatter
 \internal
 \since 4.4
 \brief Tests class QXmlFormatter.

 This test is not intended for testing the engine, but the functionality specific
 to the QXmlFormatter class.

 In other words, if you have an engine bug; don't add it here because it won't be
 tested properly. Instead add it to the test suite.

 */
class tst_QXmlFormatter : public QObject
{
    Q_OBJECT

public:
    tst_QXmlFormatter();

private Q_SLOTS:
    void indentationDepth() const;
    void setIndentationDepth() const;
    void constCorrectness() const;
    void objectSize() const;
    void format();
    void format_data() const;
    void cleanupTestCase() const;
private:
    enum
    {
        ExpectedTestCount = 19
    };

    int m_generatedBaselines;
};

tst_QXmlFormatter::tst_QXmlFormatter() : m_generatedBaselines(0)
{
}

void tst_QXmlFormatter::indentationDepth() const
{
    /* Check default value. */
    {
        QXmlQuery query;
        QByteArray out;
        QBuffer buffer(&out);
        QVERIFY(buffer.open(QIODevice::WriteOnly));

        QXmlFormatter formatter(query, &buffer);
        QCOMPARE(formatter.indentationDepth(), 4);
    }
}

void tst_QXmlFormatter::setIndentationDepth() const
{
    QXmlQuery query;
    QByteArray out;
    QBuffer buffer(&out);
    QVERIFY(buffer.open(QIODevice::WriteOnly));

    QXmlFormatter formatter(query, &buffer);

    formatter.setIndentationDepth(1);
    QCOMPARE(formatter.indentationDepth(), 1);

    formatter.setIndentationDepth(654987);
    QCOMPARE(formatter.indentationDepth(), 654987);
}

void tst_QXmlFormatter::constCorrectness() const
{
    QXmlQuery query;
    QByteArray out;
    QBuffer buffer(&out);
    QVERIFY(buffer.open(QIODevice::WriteOnly));

    const QXmlFormatter formatter(query, &buffer);

    /* These functions should be const. */
    formatter.indentationDepth();
}

void tst_QXmlFormatter::objectSize() const
{
    /* We shouldn't add something. */
    QCOMPARE(sizeof(QXmlFormatter), sizeof(QXmlSerializer));
}

void tst_QXmlFormatter::format()
{
    QFETCH(QString, testName);

    const QString location(QLatin1String("input/") + testName);
    QFile queryFile(location);
    QVERIFY(queryFile.open(QIODevice::ReadOnly));

    QXmlQuery query;
    query.setQuery(&queryFile, QUrl::fromLocalFile(location));

    QByteArray formatted;
    QBuffer bridge(&formatted);
    QVERIFY(bridge.open(QIODevice::WriteOnly));

    QXmlFormatter formatter(query, &bridge);

    QVERIFY(query.evaluateTo(&formatter));

    QFile expectedFile(QLatin1String("baselines/") + testName.left(testName.length() - 2) + QString::fromLatin1("xml"));

    if(expectedFile.exists())
    {
        QVERIFY(expectedFile.open(QIODevice::ReadOnly));
        const QByteArray expectedOutput(expectedFile.readAll());
        QCOMPARE(formatted, expectedOutput);
    }
    else
    {
        ++m_generatedBaselines;
        expectedFile.close();
        QVERIFY(expectedFile.open(QIODevice::WriteOnly));
        QCOMPARE(expectedFile.write(formatted), qint64(formatted.size()));
    }
}

void tst_QXmlFormatter::format_data() const
{
    // TODO test with pis and document nodes commencing indentaiton.
    // TODO atomic values doesn't trigger characters, it seems.
    QTest::addColumn<QString>("testName");

    QDir dir;
    dir.cd(QLatin1String("input"));

    const QStringList entries(dir.entryList(QStringList(QLatin1String("*.xq"))));
    for(int i = 0; i < entries.count(); ++i)
    {
        const QString &at = entries.at(i);
        QTest::newRow(at.toUtf8().constData()) << at;
    }

    QCOMPARE(int(ExpectedTestCount), entries.count());
}

void tst_QXmlFormatter::cleanupTestCase() const
{
    QCOMPARE(m_generatedBaselines, 0);
}

QTEST_MAIN(tst_QXmlFormatter)

#include "tst_qxmlformatter.moc"
#else
QTEST_NOOP_MAIN
#endif
// vim: et:ts=4:sw=4:sts=4
