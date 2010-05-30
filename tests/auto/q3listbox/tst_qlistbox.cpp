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


#include <qapplication.h>
#include <q3listbox.h>
#include <qvariant.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3ListBox : public QObject
{
Q_OBJECT

public:
    tst_Q3ListBox();
    virtual ~tst_Q3ListBox();


protected slots:
    void selectionChanged_helper();
    void currentChanged_helper(Q3ListBoxItem *);
    void highlighted_helper(Q3ListBoxItem *);

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void count();
    void itemAt();
    void selectionChangedSingleSelection();
    void selectionChangedExtendedSelection();
    void selectionChangedMultiSelection();
    void currentChangedSingleSelection();
    void currentChangedExtendedSelection();
    void currentChangedMultiSelection();
    void highlightedChangedSingleSelection();
    void highlightedChangedExtendedSelection();
    void highlightedChangedMultiSelection();
private:
    Q3ListBox *testWidget;
    Q3ListBoxItem *selectedItem;
    bool selectionChangedSignal;
    Q3ListBoxItem *currentItem;
    bool currentChangedSignal;
    Q3ListBoxItem *highlightedItem;
    bool highlightedSignal;
    int currentChangedCount, selectionChangedCount, highlightedCount, itemHeight;
};
 



tst_Q3ListBox::tst_Q3ListBox()
{
}

tst_Q3ListBox::~tst_Q3ListBox()
{
}

void tst_Q3ListBox::initTestCase()
{
  // Create the test class
    testWidget = new Q3ListBox(0,"testObject");
    testWidget->resize(200,200);
    qApp->setMainWidget(testWidget);
    testWidget->show();

    // Connect things here, so they aren't done more than once
    connect(testWidget, SIGNAL(currentChanged(Q3ListBoxItem *)), this, SLOT(currentChanged_helper(Q3ListBoxItem *)));
    connect(testWidget, SIGNAL(selectionChanged()), this, SLOT(selectionChanged_helper()));
    connect(testWidget, SIGNAL(highlighted(Q3ListBoxItem *)), this, SLOT(highlighted_helper(Q3ListBoxItem *)));
}

void tst_Q3ListBox::cleanupTestCase()
{
    delete testWidget;
}

void tst_Q3ListBox::init()
{
    testWidget->insertItem("This is a test");
    testWidget->insertItem("And this is another");
    testWidget->insertItem("Radiohead kicks ass!");
    testWidget->insertItem("As do Sigur Ros!");

    itemHeight = testWidget->itemHeight(0); // Safe to assume its the same for the standard tests

    selectionChangedCount = 0;
    selectionChangedSignal = false;
    selectedItem = 0;
    currentChangedCount = 0;
    currentChangedSignal = false;
    currentItem = 0;
    highlightedCount = 0;
    highlightedSignal = false;
    highlightedItem = 0;
}

void tst_Q3ListBox::cleanup()
{
    testWidget->clear();
}

void tst_Q3ListBox::count()
{
    testWidget->clear();
    QCOMPARE( testWidget->count(), (uint)0);
    testWidget->insertItem("1");
    QCOMPARE( testWidget->count(), (uint)1);
    testWidget->insertItem("2");
    QCOMPARE( testWidget->count(), (uint)2);
}

void tst_Q3ListBox::itemAt()
{
    QStringList itemText;
    itemText << "This is a test"
             << "And this is another"
             << "Radiohead kicks ass!"
             << "As do Sigur Ros!";

    // insert items
    for (int i=0; i<(int)itemText.count(); ++i)
        testWidget->insertItem(itemText[i]);

    QPoint itemPos = testWidget->viewport()->pos();
    // test itemAt
    for (int i=0; i<(int)itemText.count(); ++i) {
        QCOMPARE(testWidget->item(i)->text(), itemText[i]);
        Q3ListBoxItem *item = 0;
        item = testWidget->itemAt(itemPos);
        if (item) {
            QCOMPARE(testWidget->itemAt(itemPos)->text(), itemText[i]);
        } else {
            QFAIL(QString("No item at QPoint(%1, %2)").
                 arg(itemPos.x()).
                 arg(itemPos.y()));
        }
        itemPos += QPoint(0, testWidget->itemHeight(i));
    }
}

void tst_Q3ListBox::selectionChanged_helper()
{
    selectionChangedCount++;
    selectionChangedSignal = true;
    selectedItem = testWidget->selectedItem();
}

void tst_Q3ListBox::selectionChangedSingleSelection()
{
    testWidget->setSelectionMode(Q3ListBox::Single);
    testWidget->setSelected(testWidget->item(0), true);

    QVERIFY(selectionChangedSignal);
    QVERIFY(selectionChangedCount == 1);
    QVERIFY(testWidget->index(selectedItem) == 0);

    selectionChangedSignal = false;
    selectionChangedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ControlModifier, QPoint(5, itemHeight*2-5));

    QVERIFY(selectionChangedSignal);
    QVERIFY(selectionChangedCount == 1);
    QVERIFY(testWidget->index(selectedItem) == 1);
    QCOMPARE(selectedItem->text(), QString::fromLatin1("And this is another"));

    selectionChangedSignal = false;
    selectionChangedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ShiftModifier, QPoint(5,itemHeight*3-5));

    QVERIFY(selectionChangedSignal);
    QVERIFY(selectionChangedCount == 1);
    QVERIFY(testWidget->index(selectedItem) == 2);
    QCOMPARE(selectedItem->text(), QString::fromLatin1("Radiohead kicks ass!"));
}

void tst_Q3ListBox::selectionChangedExtendedSelection()
{
    testWidget->setSelectionMode(Q3ListBox::Extended);
    testWidget->setSelected(testWidget->item(0), true);

    QVERIFY(selectionChangedSignal);
    QVERIFY(selectionChangedCount == 1);
    QVERIFY(!selectedItem); // In this selection mode, it should be 0

    selectionChangedSignal = false;
    selectionChangedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ControlModifier, QPoint(5,itemHeight*2-5));

    QVERIFY(selectionChangedSignal);
    QVERIFY(selectionChangedCount == 1);
    QVERIFY(!selectedItem);

    selectionChangedSignal = false;
    selectionChangedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ShiftModifier, QPoint(5,itemHeight*3-5));

    QVERIFY(selectionChangedSignal);
    QVERIFY(selectionChangedCount == 1);
    QVERIFY(!selectedItem);
}

void tst_Q3ListBox::selectionChangedMultiSelection()
{
    testWidget->setSelectionMode(Q3ListBox::Multi);
    testWidget->setSelected(testWidget->item(0), true);

    QVERIFY(selectionChangedSignal);
    QVERIFY(selectionChangedCount == 1);
    QVERIFY(!selectedItem); // In this selection mode, it should be 0

    selectionChangedSignal = false;
    selectionChangedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ControlModifier, QPoint(5,itemHeight*2-5));

    QVERIFY(selectionChangedSignal);
    QVERIFY(selectionChangedCount == 1);
    QVERIFY(!selectedItem);

    selectionChangedSignal = false;
    selectionChangedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ShiftModifier, QPoint(5,itemHeight*3-5));

    QVERIFY(selectionChangedSignal);
    QVERIFY(selectionChangedCount == 1);
    QVERIFY(!selectedItem);
}

void tst_Q3ListBox::currentChanged_helper(Q3ListBoxItem *item)
{
    currentChangedCount++;
    currentChangedSignal = true;
    currentItem = item;
}

void tst_Q3ListBox::currentChangedSingleSelection()
{
    testWidget->setSelectionMode(Q3ListBox::Single);

    testWidget->setSelected(3, true);

    QVERIFY(currentChangedSignal);
    QVERIFY(currentChangedCount == 1);
    QVERIFY(currentItem);
    QCOMPARE(currentItem->text(), QString::fromLatin1("As do Sigur Ros!"));

    currentChangedSignal = false;
    currentChangedCount = 0;
    testWidget->setCurrentItem(2);

    QVERIFY(currentChangedSignal);
    QVERIFY(currentChangedCount == 1);
    QVERIFY(currentItem);
    QCOMPARE(currentItem->text(), QString::fromLatin1("Radiohead kicks ass!"));

    currentChangedSignal = false;
    currentChangedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoModifier, QPoint(5,itemHeight-5));
    
    QVERIFY(currentChangedSignal);
    QVERIFY(currentChangedCount == 1);
    QVERIFY(currentItem);
    QCOMPARE(currentItem->text(), QString::fromLatin1("This is a test"));

    currentChangedSignal = false;
    currentChangedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ControlModifier, QPoint(5,itemHeight*2-5));

    QVERIFY(currentChangedSignal);
    QVERIFY(currentChangedCount == 1);
    QVERIFY(currentItem);
    QCOMPARE(currentItem->text(), QString::fromLatin1("And this is another"));

    currentChangedSignal = false;
    currentChangedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ShiftModifier, QPoint(5,itemHeight*3-5));

    QVERIFY(currentChangedSignal);
    QVERIFY(currentChangedCount == 1);
    QVERIFY(currentItem);
    QCOMPARE(currentItem->text(), QString::fromLatin1("Radiohead kicks ass!"));
}

void tst_Q3ListBox::currentChangedExtendedSelection()
{
    testWidget->setSelectionMode(Q3ListBox::Extended);
    testWidget->setSelected(3, true);

    QVERIFY(currentChangedSignal);
    QVERIFY(currentChangedCount == 1);
    QVERIFY(currentItem);
    QCOMPARE(currentItem->text(), QString::fromLatin1("As do Sigur Ros!"));

    currentChangedSignal = false;
    currentChangedCount = 0;
    testWidget->setCurrentItem(2);

    QVERIFY(currentChangedSignal);
    QVERIFY(currentChangedCount == 1);
    QVERIFY(currentItem);
    QCOMPARE(currentItem->text(), QString::fromLatin1("Radiohead kicks ass!"));

    currentChangedSignal = false;
    currentChangedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoModifier, QPoint(5,itemHeight-5));
    
    QVERIFY(currentChangedSignal);
    QVERIFY(currentChangedCount == 1);
    QVERIFY(currentItem);
    QCOMPARE(currentItem->text(), QString::fromLatin1("This is a test"));

    currentChangedSignal = false;
    currentChangedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ControlModifier, QPoint(5,itemHeight*2-5));

    QVERIFY(currentChangedSignal);
    QVERIFY(currentChangedCount == 1);
    QVERIFY(currentItem);
    QCOMPARE(currentItem->text(), QString::fromLatin1("And this is another"));

    currentChangedSignal = false;
    currentChangedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ShiftModifier, QPoint(5,itemHeight*3-5));

    QVERIFY(currentChangedSignal);
    QVERIFY(currentChangedCount == 1);
    QVERIFY(currentItem);
    QCOMPARE(currentItem->text(), QString::fromLatin1("Radiohead kicks ass!"));
}

void tst_Q3ListBox::currentChangedMultiSelection()
{
    testWidget->setSelectionMode(Q3ListBox::Multi);
    testWidget->setSelected(3, true);

    QVERIFY(currentChangedSignal);
    QVERIFY(currentChangedCount == 1);
    QVERIFY(currentItem);
    QCOMPARE(currentItem->text(), QString::fromLatin1("As do Sigur Ros!"));

    currentChangedSignal = false;
    currentChangedCount = 0;
    testWidget->setCurrentItem(2);

    QVERIFY(currentChangedSignal);
    QVERIFY(currentChangedCount == 1);
    QVERIFY(currentItem);
    QCOMPARE(currentItem->text(), QString::fromLatin1("Radiohead kicks ass!"));

    currentChangedSignal = false;
    currentChangedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoModifier, QPoint(5,itemHeight-5));
    
    QVERIFY(currentChangedSignal);
    QVERIFY(currentChangedCount == 1);
    QVERIFY(currentItem);
    QCOMPARE(currentItem->text(), QString::fromLatin1("This is a test"));

    currentChangedSignal = false;
    currentChangedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ControlModifier, QPoint(5,itemHeight*2-5));

    QVERIFY(currentChangedSignal);
    QVERIFY(currentChangedCount == 1);
    QVERIFY(currentItem);
    QCOMPARE(currentItem->text(), QString::fromLatin1("And this is another"));

    currentChangedSignal = false;
    currentChangedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ShiftModifier, QPoint(5,itemHeight*3-5));

    QVERIFY(currentChangedSignal);
    QVERIFY(currentChangedCount == 1);
    QVERIFY(currentItem);
    QCOMPARE(currentItem->text(), QString::fromLatin1("Radiohead kicks ass!"));
}

void tst_Q3ListBox::highlighted_helper(Q3ListBoxItem *item)
{
    highlightedCount++;
    highlightedSignal = true;
    highlightedItem = item;
}

void tst_Q3ListBox::highlightedChangedSingleSelection()
{
    QVERIFY(highlightedCount == 0);
    testWidget->setSelectionMode(Q3ListBox::Single);
    QSignalSpy highlightedIndexSpy(testWidget, SIGNAL(highlighted(int)));
    QSignalSpy highlightedTextSpy(testWidget, SIGNAL(highlighted(const QString &)));

    testWidget->setSelected(3, true);

    QCOMPARE(highlightedIndexSpy.count(), 1); 
    QList<QVariant> list = highlightedIndexSpy.takeFirst();
    QCOMPARE(list.at(0).toInt(), 3);
    QCOMPARE(highlightedTextSpy.count(), 1); 
    list = highlightedTextSpy.takeFirst();
    QCOMPARE(list.at(0).toString(), QString::fromLatin1("As do Sigur Ros!"));
    QVERIFY(highlightedSignal);
    QVERIFY(highlightedCount == 1);
    QVERIFY(highlightedItem);
    QCOMPARE(highlightedItem->text(), QString::fromLatin1("As do Sigur Ros!"));

    highlightedSignal = false;
    highlightedCount = 0;
    testWidget->setCurrentItem(2);

    QCOMPARE(highlightedIndexSpy.count(), 1); 
    list = highlightedIndexSpy.takeFirst();
    QCOMPARE(list.at(0).toInt(), 2);
    QCOMPARE(highlightedTextSpy.count(), 1); 
    list = highlightedTextSpy.takeFirst();
    QCOMPARE(list.at(0).toString(), QString::fromLatin1("Radiohead kicks ass!"));
    QVERIFY(highlightedSignal);
    QVERIFY(highlightedCount == 1);
    QVERIFY(highlightedItem);
    QCOMPARE(highlightedItem->text(), QString::fromLatin1("Radiohead kicks ass!"));

    highlightedSignal = false;
    highlightedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoModifier, QPoint(5,itemHeight-5));

    QCOMPARE(highlightedIndexSpy.count(), 1); 
    list = highlightedIndexSpy.takeFirst();
    QCOMPARE(list.at(0).toInt(), 0);
    QCOMPARE(highlightedTextSpy.count(), 1); 
    list = highlightedTextSpy.takeFirst();
    QCOMPARE(list.at(0).toString(), QString::fromLatin1("This is a test"));
    QVERIFY(highlightedSignal);
    QVERIFY(highlightedCount == 1);
    QVERIFY(highlightedItem);
    QCOMPARE(highlightedItem->text(), QString::fromLatin1("This is a test"));

    highlightedSignal = false;
    highlightedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ControlModifier, QPoint(5,itemHeight*2-5));
    
    QCOMPARE(highlightedIndexSpy.count(), 1); 
    list = highlightedIndexSpy.takeFirst();
    QCOMPARE(list.at(0).toInt(), 1);
    QCOMPARE(highlightedTextSpy.count(), 1); 
    list = highlightedTextSpy.takeFirst();
    QCOMPARE(list.at(0).toString(), QString::fromLatin1("And this is another"));
    QVERIFY(highlightedSignal);
    QVERIFY(highlightedCount == 1);
    QVERIFY(highlightedItem);
    QCOMPARE(highlightedItem->text(), QString::fromLatin1("And this is another"));

    highlightedSignal = false;
    highlightedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ShiftModifier, QPoint(5,itemHeight*3-5));
    
    QCOMPARE(highlightedIndexSpy.count(), 1); 
    list = highlightedIndexSpy.takeFirst();
    QCOMPARE(list.at(0).toInt(), 2);
    QCOMPARE(highlightedTextSpy.count(), 1); 
    list = highlightedTextSpy.takeFirst();
    QCOMPARE(list.at(0).toString(), QString::fromLatin1("Radiohead kicks ass!"));
    QVERIFY(highlightedSignal);
    QVERIFY(highlightedCount == 1);
    QVERIFY(highlightedItem);
    QCOMPARE(highlightedItem->text(), QString::fromLatin1("Radiohead kicks ass!"));
}

void tst_Q3ListBox::highlightedChangedExtendedSelection()
{
    QVERIFY(highlightedCount == 0);
    testWidget->setSelectionMode(Q3ListBox::Extended);
    QSignalSpy highlightedIndexSpy(testWidget, SIGNAL(highlighted(int)));
    QSignalSpy highlightedTextSpy(testWidget, SIGNAL(highlighted(const QString &)));

    testWidget->setSelected(3, true);

    QCOMPARE(highlightedIndexSpy.count(), 1); 
    QList<QVariant> list = highlightedIndexSpy.takeFirst();
    QCOMPARE(list.at(0).toInt(), 3);
    QCOMPARE(highlightedTextSpy.count(), 1); 
    list = highlightedTextSpy.takeFirst();
    QCOMPARE(list.at(0).toString(), QString::fromLatin1("As do Sigur Ros!"));
    QVERIFY(highlightedSignal);
    QVERIFY(highlightedCount == 1);
    QVERIFY(highlightedItem);
    QCOMPARE(highlightedItem->text(), QString::fromLatin1("As do Sigur Ros!"));

    highlightedSignal = false;
    highlightedCount = 0;
    testWidget->setCurrentItem(2);

    QCOMPARE(highlightedIndexSpy.count(), 1); 
    list = highlightedIndexSpy.takeFirst();
    QCOMPARE(list.at(0).toInt(), 2);
    QCOMPARE(highlightedTextSpy.count(), 1); 
    list = highlightedTextSpy.takeFirst();
    QCOMPARE(list.at(0).toString(), QString::fromLatin1("Radiohead kicks ass!"));
    QVERIFY(highlightedSignal);
    QVERIFY(highlightedCount == 1);
    QVERIFY(highlightedItem);
    QCOMPARE(highlightedItem->text(), QString::fromLatin1("Radiohead kicks ass!"));

    highlightedSignal = false;
    highlightedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoModifier, QPoint(5,itemHeight-5));
    
    QCOMPARE(highlightedIndexSpy.count(), 1); 
    list = highlightedIndexSpy.takeFirst();
    QCOMPARE(list.at(0).toInt(), 0);
    QCOMPARE(highlightedTextSpy.count(), 1); 
    list = highlightedTextSpy.takeFirst();
    QCOMPARE(list.at(0).toString(), QString::fromLatin1("This is a test"));
    QVERIFY(highlightedSignal);
    QVERIFY(highlightedCount == 1);
    QVERIFY(highlightedItem);
    QCOMPARE(highlightedItem->text(), QString::fromLatin1("This is a test"));

    highlightedSignal = false;
    highlightedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ControlModifier, QPoint(5,itemHeight*2-5));
    
    QCOMPARE(highlightedIndexSpy.count(), 1); 
    list = highlightedIndexSpy.takeFirst();
    QCOMPARE(list.at(0).toInt(), 1);
    QCOMPARE(highlightedTextSpy.count(), 1); 
    list = highlightedTextSpy.takeFirst();
    QCOMPARE(list.at(0).toString(), QString::fromLatin1("And this is another"));
    QVERIFY(highlightedSignal);
    QVERIFY(highlightedCount == 1);
    QVERIFY(highlightedItem);
    QCOMPARE(highlightedItem->text(), QString::fromLatin1("And this is another"));

    highlightedSignal = false;
    highlightedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ShiftModifier, QPoint(5,itemHeight*3-5));
    
    QCOMPARE(highlightedIndexSpy.count(), 1); 
    list = highlightedIndexSpy.takeFirst();
    QCOMPARE(list.at(0).toInt(), 2);
    QCOMPARE(highlightedTextSpy.count(), 1); 
    list = highlightedTextSpy.takeFirst();
    QCOMPARE(list.at(0).toString(), QString::fromLatin1("Radiohead kicks ass!"));
    QVERIFY(highlightedSignal);
    QVERIFY(highlightedCount == 1);
    QVERIFY(highlightedItem);
    QCOMPARE(highlightedItem->text(), QString::fromLatin1("Radiohead kicks ass!"));
}

void tst_Q3ListBox::highlightedChangedMultiSelection()
{
    QVERIFY(highlightedCount == 0);
    testWidget->setSelectionMode(Q3ListBox::Multi);
    QSignalSpy highlightedIndexSpy(testWidget, SIGNAL(highlighted(int)));
    QSignalSpy highlightedTextSpy(testWidget, SIGNAL(highlighted(const QString &)));

    testWidget->setSelected(3, true);

    QCOMPARE(highlightedIndexSpy.count(), 1); 
    QList<QVariant> list = highlightedIndexSpy.takeFirst();
    QCOMPARE(list.at(0).toInt(), 3);
    QCOMPARE(highlightedTextSpy.count(), 1); 
    list = highlightedTextSpy.takeFirst();
    QCOMPARE(list.at(0).toString(), QString::fromLatin1("As do Sigur Ros!"));
    QVERIFY(highlightedSignal);
    QVERIFY(highlightedCount == 1);
    QVERIFY(highlightedItem);
    QCOMPARE(highlightedItem->text(), QString::fromLatin1("As do Sigur Ros!"));

    highlightedSignal = false;
    highlightedCount = 0;
    testWidget->setCurrentItem(2);

    QCOMPARE(highlightedIndexSpy.count(), 1); 
    list = highlightedIndexSpy.takeFirst();
    QCOMPARE(list.at(0).toInt(), 2);
    QCOMPARE(highlightedTextSpy.count(), 1); 
    list = highlightedTextSpy.takeFirst();
    QCOMPARE(list.at(0).toString(), QString::fromLatin1("Radiohead kicks ass!"));
    QVERIFY(highlightedSignal);
    QVERIFY(highlightedCount == 1);
    QVERIFY(highlightedItem);
    QCOMPARE(highlightedItem->text(), QString::fromLatin1("Radiohead kicks ass!"));

    highlightedSignal = false;
    highlightedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoModifier, QPoint(5,itemHeight-5));
    
    QCOMPARE(highlightedIndexSpy.count(), 1); 
    list = highlightedIndexSpy.takeFirst();
    QCOMPARE(list.at(0).toInt(), 0);
    QCOMPARE(highlightedTextSpy.count(), 1); 
    list = highlightedTextSpy.takeFirst();
    QCOMPARE(list.at(0).toString(), QString::fromLatin1("This is a test"));
    QVERIFY(highlightedSignal);
    QVERIFY(highlightedCount == 1);
    QVERIFY(highlightedItem);
    QCOMPARE(highlightedItem->text(), QString::fromLatin1("This is a test"));

    highlightedSignal = false;
    highlightedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ControlModifier, QPoint(5,itemHeight*2-5));
    
    QCOMPARE(highlightedIndexSpy.count(), 1); 
    list = highlightedIndexSpy.takeFirst();
    QCOMPARE(list.at(0).toInt(), 1);
    QCOMPARE(highlightedTextSpy.count(), 1); 
    list = highlightedTextSpy.takeFirst();
    QCOMPARE(list.at(0).toString(), QString::fromLatin1("And this is another"));
    QVERIFY(highlightedSignal);
    QVERIFY(highlightedCount == 1);
    QVERIFY(highlightedItem);
    QCOMPARE(highlightedItem->text(), QString::fromLatin1("And this is another"));

    highlightedSignal = false;
    highlightedCount = 0;
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::ShiftModifier, QPoint(5,itemHeight*3-5));
    
    QCOMPARE(highlightedIndexSpy.count(), 1); 
    list = highlightedIndexSpy.takeFirst();
    QCOMPARE(list.at(0).toInt(), 2);
    QCOMPARE(highlightedTextSpy.count(), 1); 
    list = highlightedTextSpy.takeFirst();
    QCOMPARE(list.at(0).toString(), QString::fromLatin1("Radiohead kicks ass!"));
    QVERIFY(highlightedSignal);
    QVERIFY(highlightedCount == 1);
    QVERIFY(highlightedItem);
    QCOMPARE(highlightedItem->text(), QString::fromLatin1("Radiohead kicks ass!"));
}

QTEST_MAIN(tst_Q3ListBox)
#include "tst_qlistbox.moc"
