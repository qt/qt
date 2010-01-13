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

#include <QDebug>
#include <QTextDocument>
#include <QTextLayout>
#include <QFile>
#include <qtest.h>

class tst_QText: public QObject
{
    Q_OBJECT
private slots:
    void loadHtml_data();
    void loadHtml();

    void shaping_data();
    void shaping();
};

void tst_QText::loadHtml_data()
{
    QTest::addColumn<QString>("source");
    QTest::newRow("empty") << QString();
    QTest::newRow("simple") << QString::fromLatin1("<html><b>Foo</b></html>");
    QTest::newRow("simple2") << QString::fromLatin1("<b>Foo</b>");

    QString parag = QString::fromLatin1("<p>Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. <b>Duis <i>autem</i> vel eum </b> iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.</p>");
    QString header = QString::fromLatin1("<html><head><title>test</title></head><body>");
    QTest::newRow("long") << QString::fromLatin1("<html><head><title>test</title></head><body>") + parag + parag + parag
        + parag + parag + parag + parag + parag + parag + parag + parag + parag + parag + parag + parag + parag + parag
        + QString::fromLatin1("</html>");
    QTest::newRow("table") <<  header + QLatin1String("<table border=\"1\"1><tr><td>xx</td></tr><tr><td colspan=\"2\">")
        + parag + QLatin1String("</td></tr></table></html");
    QTest::newRow("crappy") <<  header + QLatin1String("<table border=\"1\"1><tr><td>xx</td></tr><tr><td colspan=\"2\">")
        + parag;
}

void tst_QText::loadHtml()
{
    QFETCH(QString, source);
    QTextDocument doc;
    QBENCHMARK {
        doc.setHtml(source);
    }
}

void tst_QText::shaping_data()
{
    QTest::addColumn<QString>("parag");
    QTest::newRow("empty") << QString();
    QTest::newRow("lorem") << QString::fromLatin1("Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.");
    QTest::newRow("short") << QString::fromLatin1("Lorem ipsum dolor sit amet");

    QFile file(QString::fromLatin1(SRCDIR) + QLatin1String("/bidi.txt"));
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray data = file.readAll();
    QVERIFY(data.count() > 1000);
    QStringList list = QString::fromUtf8(data.data()).split(QLatin1Char('\n'), QString::SkipEmptyParts);
    QVERIFY(list.count() %2 == 0); // even amount as we have title and then content.
    for (int i=0; i < list.count(); i+=2) {
        QTest::newRow(list.at(i).toLatin1()) << list.at(i+1);
    }
}

void tst_QText::shaping()
{
    QFETCH(QString, parag);

    QTextLayout lay(parag);
    lay.setCacheEnabled(false);

    // do one run to make sure any fonts are loaded.
    lay.beginLayout();
    lay.createLine();
    lay.endLayout();

    QBENCHMARK {
        lay.beginLayout();
        lay.createLine();
        lay.endLayout();
    }
}

QTEST_MAIN(tst_QText)

#include "main.moc"
