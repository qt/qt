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

#include <qbaselinetest.h>
#include <QPushButton>

class tst_BaselineExample : public QObject
{
    Q_OBJECT

public:
    tst_BaselineExample();

private Q_SLOTS:
    void testBasicUsage();
    void testMultipleImages();
    void testDataDriven_data();
    void testDataDriven();
    void testDataDrivenMultiple_data();
    void testDataDrivenMultiple();
    void testChecksum_data();
    void testChecksum();
};


tst_BaselineExample::tst_BaselineExample()
{
}


void tst_BaselineExample::testBasicUsage()
{
    // Generate an image:
    QPushButton b("Press me!");
    b.resize(100, 50);
    b.show();
    QTest::qWaitForWindowShown(&b);
    QImage img1 = QPixmap::grabWidget(&b).toImage();
    QVERIFY(!img1.isNull());

    // Compare it to baseline on server:
    QBASELINE_CHECK(img1, "button");
}


void tst_BaselineExample::testMultipleImages()
{
    QPushButton b("Press me!");
    b.resize(100, 50);
    b.show();
    QTest::qWaitForWindowShown(&b);
    QBASELINE_CHECK(QPixmap::grabWidget(&b).toImage(), "text1");

    b.setText("Kick me!");
    QTest::qWait(50);
    QBASELINE_CHECK(QPixmap::grabWidget(&b).toImage(), "text2");
}


void tst_BaselineExample::testDataDriven_data()
{
    QTest::addColumn<QString>("label");
    QTest::newRow("short") << "Ok!";
    QTest::newRow("long") << "A really long button text that just does not seem to end";
    QTest::newRow("empty") << "";
}


void tst_BaselineExample::testDataDriven()
{
    QFETCH(QString, label);
    QPushButton b(label);
    b.resize(100, 50);
    b.show();
    QTest::qWaitForWindowShown(&b);
    QBASELINE_CHECK(QPixmap::grabWidget(&b).toImage(), 0);
}


void tst_BaselineExample::testDataDrivenMultiple_data()
{
    testDataDriven_data();
}


void tst_BaselineExample::testDataDrivenMultiple()
{
    QFETCH(QString, label);
    QPushButton b(label);
    b.resize(100, 50);
    b.show();
    QTest::qWaitForWindowShown(&b);
    QBASELINE_CHECK(QPixmap::grabWidget(&b).toImage(), "normal");

    b.setText(label.prepend('&'));
    QTest::qWait(50);
    QBASELINE_CHECK(QPixmap::grabWidget(&b).toImage(), "shortcut");
}


void tst_BaselineExample::testChecksum_data()
{
    testDataDriven_data();
}


void tst_BaselineExample::testChecksum()
{
    QFETCH(QString, label);
    QPushButton b(label);
    b.resize(100, 50);
    b.show();
    QTest::qWaitForWindowShown(&b);
    QBASELINE_CHECK_SUM(QPixmap::grabWidget(&b).toImage(), 0, quint16(qHash(label)));
}


QTEST_MAIN(tst_BaselineExample);

#include "tst_baselineexample.moc"
