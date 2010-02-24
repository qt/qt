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
#include <qtest.h>
#include <QtTest/QtTest>
#include <QtGui/QTextLayout>
#include <private/qmlstyledtext_p.h>

class tst_qmlstyledtext : public QObject
{
    Q_OBJECT
public:
    tst_qmlstyledtext()
    {
    }

private slots:
    void textOutput();
    void textOutput_data();
};

// For malformed input all we test is that we get the expected text out.
// 
void tst_qmlstyledtext::textOutput_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    QTest::newRow("bold") << "<b>bold</b>" << "bold";
    QTest::newRow("italic") << "<b>italic</b>" << "italic";
    QTest::newRow("missing >") << "<b>text</b" << "text";
    QTest::newRow("missing b>") << "<b>text</" << "text";
    QTest::newRow("missing /b>") << "<b>text<" << "text";
    QTest::newRow("missing </b>") << "<b>text" << "text";
    QTest::newRow("bad nest") << "<b>text <i>italic</b></i>" << "text italic";
    QTest::newRow("font color") << "<font color=\"red\">red text</font>" << "red text";
    QTest::newRow("font size") << "<font size=\"1\">text</font>" << "text";
    QTest::newRow("font empty") << "<font>text</font>" << "text";
    QTest::newRow("font bad 1") << "<font ezis=\"blah\">text</font>" << "text";
    QTest::newRow("font bad 2") << "<font size=\"1>text</font>" << "";
    QTest::newRow("extra close") << "<b>text</b></b>" << "text";
    QTest::newRow("empty") << "" << "";
}

void tst_qmlstyledtext::textOutput()
{
    QFETCH(QString, input);
    QFETCH(QString, output);

    QTextLayout layout;
    QmlStyledText::parse(input, layout);

    QCOMPARE(layout.text(), output);
}


QTEST_MAIN(tst_qmlstyledtext)

#include "tst_qmlstyledtext.moc"
