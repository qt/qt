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


#include <QApplication>
#include <Q3ButtonGroup>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QDebug>
#include <QLayout>
#include <QtTest/QtTest>

class tst_q3buttongroup : public QObject
{
Q_OBJECT
private slots:
    void exclusiveButtons();
    void nonExclusiveButtons();
    void buttonIds();
    void buttonId();
    void clickLock();
    void task198864_insert();
    void task200764_insert_data();
    void task200764_insert();
    void task238902_directChildrenOnly();
};

/*
    Test that check boxes created with a Q3ButtonGroup parent in exclusive
    mode really are exclusive.
*/
void tst_q3buttongroup::exclusiveButtons()
{
    Q3ButtonGroup group(1, Qt::Horizontal);
    group.setExclusive(true);

    QCheckBox *b1 = new QCheckBox("Hi", &group);
    QCheckBox *b2 = new QCheckBox("there", &group);
    QCheckBox *b3 = new QCheckBox("foo", &group);

    group.show();

    // Check b1 and verify that it stuck.
    b1->setCheckState(Qt::Checked);
    QCOMPARE(b1->checkState(), Qt::Checked);

    // Check b2 and verify that b1 is now unchecked.
    b2->setCheckState(Qt::Checked);
    QCOMPARE(b1->checkState(), Qt::Unchecked);

    // Check b3 and verify that b2 and b1 are now unchecked.
    b3->setCheckState(Qt::Checked);
    QCOMPARE(b1->checkState(), Qt::Unchecked);
    QCOMPARE(b2->checkState(), Qt::Unchecked);
}

/*
    Test that setting exclusive to false works.
*/
void tst_q3buttongroup::nonExclusiveButtons()
{
    Q3ButtonGroup group(1, Qt::Horizontal);

    QWidget parent;

    QCheckBox *b1 = new QCheckBox("Hi", &parent);
    group.insert(b1);
    QCheckBox *b2 = new QCheckBox("there", &parent);
    group.insert(b2);
    QCheckBox *b3 = new QCheckBox("foo", &parent);
    group.insert(b3);

    group.setExclusive(false);
    group.show();

    // Check b1 and verify that it stuck.
    b1->setCheckState(Qt::Checked);
    QCOMPARE(b1->checkState(), Qt::Checked);

    // Check b2 and verify that b1 is still checked.
    b2->setCheckState(Qt::Checked);
    QCOMPARE(b1->checkState(), Qt::Checked);

    // Check b3 and verify that b2 and b1 are still checked.
    b3->setCheckState(Qt::Checked);
    QCOMPARE(b1->checkState(), Qt::Checked);
    QCOMPARE(b2->checkState(), Qt::Checked);
}

/*
    Test that Ids get assigned
*/
void tst_q3buttongroup::buttonIds()
{
    Q3ButtonGroup group(0, Qt::Vertical, "ButtonGroup");
    group.setExclusive(true);
    QVERIFY(group.isExclusive());

    for (int i=0; i < 10; i++) {
        QRadioButton *button = new QRadioButton(QString("Button_%1").arg(i + 1) , &group);
        QCOMPARE(group.id(button) , i);
        int id = group.insert(button);
        QCOMPARE(id, i);
        group.setButton(id);
        QCOMPARE(group.selectedId(), id);
    }

    QCheckBox *button2 = new QCheckBox(QString("manuallyAdded"));
    int id = group.insert( button2 );
    QCOMPARE(id , 10 );

    button2->setChecked(true);
    QCOMPARE( group.selectedId() , id );

    group.remove(group.find(5));
    QCOMPARE(group.count() , 10);

    delete button2;
}

void tst_q3buttongroup::buttonId()
{
    Q3ButtonGroup bg;
    QPushButton *button = new QPushButton("Foo", &bg);
    int id = bg.insert(button, 1);
    QApplication::instance()->processEvents();
    QCOMPARE(id, bg.id(button));
}

void tst_q3buttongroup::clickLock()
{
    // Task 177677
    QProcess process;
    process.start(QLatin1String("clickLock/clickLock"));
    if (!process.waitForStarted(10000)) {
        QFAIL("Could not launch process.");
    }

    if (!process.waitForFinished(15000)) {
        process.terminate();
        QFAIL("Could not handle click events properly");
    }
}

void tst_q3buttongroup::task198864_insert()
{
#if 0
    Q3ButtonGroup *group = new Q3ButtonGroup;
    QWidget *parent = new QWidget;

    const int id1 = group->insert(new QPushButton(parent));
    QCOMPARE(id1, 0);
    const int id2 = group->insert(new QPushButton(parent));
    QCOMPARE(id2, 1);

    delete parent;
    parent = new QWidget;

    const int id3 = group->insert(new QPushButton(parent));
    QCOMPARE(id3, 0);
#endif

    Q3ButtonGroup *group = new Q3ButtonGroup;

    QPushButton *button1 = new QPushButton;
    const int id1 = group->insert(button1);
    QCOMPARE(id1, 0);

    QPushButton *button2 = new QPushButton;
    const int id2 = group->insert(button2);
    QCOMPARE(id2, 1);

    delete button1;
    delete button2;

    QPushButton *button3 = new QPushButton;
    const int id3 = group->insert(button3);
    QCOMPARE(id3, 0);
}

typedef QList<int> IntList;
Q_DECLARE_METATYPE(IntList);

void tst_q3buttongroup::task200764_insert_data()
{
    QTest::addColumn<IntList >("ids");
    QTest::newRow("1") << (IntList() << 0 << 1 << 2 << 3);
    QTest::newRow("2") << (IntList() << 0 << 3 << 2 << 1);
    QTest::newRow("3") << (IntList() << 3 << 2 << 1 << 0);
    QTest::newRow("4") << (IntList() << 3 << 1 << 0 << 2);
}

class task200764_Widget : public QWidget
{
    Q_OBJECT
public:
    task200764_Widget(const IntList &ids)
    {
        Q3ButtonGroup *buttonGroup = new Q3ButtonGroup;
        buttonGroup->setExclusive(true);
        QVBoxLayout *layout = new QVBoxLayout(this);

        for (int i = 0; i < ids.size(); ++i) {
            QPushButton *button = new QPushButton(buttonGroup);
            const int id = buttonGroup->insert(button, ids.at(i));
            //button->setText(QString("%1 (id:%2->%3)").arg(i).arg(ids.at(i)).arg(id));
            layout->addWidget(button);
            buttons << button;
            actualIds << id;
        }

        QObject::connect(buttonGroup, SIGNAL(clicked(int)), SLOT(clicked(int)));
    }

    ~task200764_Widget()
    {
        foreach (QPushButton *button, buttons)
            delete button;
    }

    QList<QPushButton *> buttons;
    QList<int> actualIds;
    QList<int> clickedIds;

private slots:
    void clicked(int id) { clickedIds << id; }
};

void tst_q3buttongroup::task200764_insert()
{
    QFETCH(IntList, ids);

    task200764_Widget widget(ids);
    widget.show();

    for (int i = 0; i < ids.size(); ++i) {
        widget.clickedIds.clear();
        QTest::mouseClick(widget.buttons.at(i), Qt::LeftButton);
        QCOMPARE(widget.clickedIds.size(), 1);
        QCOMPARE(widget.clickedIds.first(), widget.actualIds.at(i));
    }
}

void tst_q3buttongroup::task238902_directChildrenOnly()
{
    Q3ButtonGroup *group = new Q3ButtonGroup;

    QFrame *frame = new QFrame(group);
    QPushButton *button = new QPushButton(frame);
    QSignalSpy spy(button, SIGNAL(clicked()));
    QSignalSpy spy2(group, SIGNAL(clicked(int)));
    group->show();
    QTest::qWait(500);
    QTest::mouseClick(button, Qt::LeftButton);
    QTest::qWait(500);
    QCOMPARE(spy.count(),1);
    QCOMPARE(spy2.count(),0);
    group->hide();

    //normal case, should work
    Q3ButtonGroup *group2 = new Q3ButtonGroup;

    QPushButton *button2 = new QPushButton(group2);
    QSignalSpy spy3(button2, SIGNAL(clicked()));
    QSignalSpy spy4(group2, SIGNAL(clicked(int)));
    group2->show();
    QTest::qWait(500);
    QTest::mouseClick(button2, Qt::LeftButton);
    QTest::qWait(500);
    QCOMPARE(spy3.count(),1);
    QCOMPARE(spy4.count(),1);
}

QTEST_MAIN(tst_q3buttongroup)
#include "tst_q3buttongroup.moc"
