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
#include <q3combobox.h>
#include <q3listbox.h>
#include <qlineedit.h>
#include <QValidator>

class tst_Q3ComboBox : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void q3combobox_data();
    void q3combobox();
    void autoCompletion_data();
    void autoCompletion();
    void autoResize_data();
    void autoResize();
    void changeItem_data();
    void changeItem();
    void clear_data();
    void clear();
    void clearEdit_data();
    void clearEdit();
    void count_data();
    void count();
    void currentItem_data();
    void currentItem();
    void currentText_data();
    void currentText();
    void duplicatesEnabled_data();
    void duplicatesEnabled();
    void editable_data();
    void editable();
    void insertItem_data();
    void insertItem();
    void insertStringList_data();
    void insertStringList();
    void insertStrList_data();
    void insertStrList();
    void maxCount_data();
    void maxCount();
    void pixmap();
    void removeItem_data();
    void removeItem();
    void setValidator();
    void keyPressEvent_data();
    void keyPressEvent();
    void wheelEvent_data();
    void wheelEvent();
    
    void task231724_clear();
};

// Subclass that exposes the protected functions.
class SubQ3ComboBox : public Q3ComboBox
{
public:
    void call_activated(int index)
        { return SubQ3ComboBox::activated(index); }

    void call_activated(QString const& str)
        { return SubQ3ComboBox::activated(str); }

    void call_focusInEvent(QFocusEvent* e)
        { return SubQ3ComboBox::focusInEvent(e); }

    void call_focusOutEvent(QFocusEvent* e)
        { return SubQ3ComboBox::focusOutEvent(e); }

    void call_highlighted(QString const& str)
        { return SubQ3ComboBox::highlighted(str); }

    void call_highlighted(int index)
        { return SubQ3ComboBox::highlighted(index); }

    void call_keyPressEvent(QKeyEvent* e)
        { return SubQ3ComboBox::keyPressEvent(e); }

    void call_mouseDoubleClickEvent(QMouseEvent* e)
        { return SubQ3ComboBox::mouseDoubleClickEvent(e); }

    void call_mouseMoveEvent(QMouseEvent* e)
        { return SubQ3ComboBox::mouseMoveEvent(e); }

    void call_mousePressEvent(QMouseEvent* e)
        { return SubQ3ComboBox::mousePressEvent(e); }

    void call_mouseReleaseEvent(QMouseEvent* e)
        { return SubQ3ComboBox::mouseReleaseEvent(e); }

    void call_paintEvent(QPaintEvent* e)
        { return SubQ3ComboBox::paintEvent(e); }

    void call_resizeEvent(QResizeEvent* e)
        { return SubQ3ComboBox::resizeEvent(e); }

    void call_styleChange(QStyle& style)
        { return SubQ3ComboBox::styleChange(style); }

    void call_textChanged(QString const& str)
        { return SubQ3ComboBox::textChanged(str); }

    void call_updateMask()
        { return SubQ3ComboBox::updateMask(); }

    void call_wheelEvent(QWheelEvent* e)
        { return SubQ3ComboBox::wheelEvent(e); }
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_Q3ComboBox::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_Q3ComboBox::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_Q3ComboBox::init()
{
}

// This will be called after every test function.
void tst_Q3ComboBox::cleanup()
{
}

void tst_Q3ComboBox::q3combobox_data()
{
}

void tst_Q3ComboBox::q3combobox()
{
    SubQ3ComboBox box;
    box.autoCompletion();
    box.autoResize();
    box.changeItem(QString(), 0);
    box.changeItem(QPixmap(), 0);
    box.changeItem(QPixmap(), QString(), 0);
    box.clear();
    box.clearEdit();
    box.clearValidator();
    box.count();
    box.currentItem();
    box.currentText();
    box.duplicatesEnabled();
    box.editable();
    box.hide();
    box.insertionPolicy();
    box.insertItem(QString());
    box.insertItem(QPixmap(), -1);
    box.insertItem(QPixmap(), QString(), -1);
    box.insertStringList(QStringList());
    static const char* items[] = { "red", "green", "blue", 0 };
    box.insertStrList(items);
    box.lineEdit();
    box.listBox();
    box.maxCount();
    box.pixmap(0);
    box.popup();
    box.removeItem(0);
    box.setAutoCompletion(false);
    box.setAutoResize(false);
    box.setCurrentItem(0);
    box.setCurrentText(QString());
    box.setDuplicatesEnabled(true);
    box.setEditable(false);
    box.setEditText(false);
    box.setEnabled(false);
    box.setFont(QFont());
    box.setInsertionPolicy(Q3ComboBox::NoInsertion);
    box.setLineEdit(0);
    //box.setListBox(0);
    box.setMaxCount(0);
    box.setPalette(QPalette());
    box.setSizeLimit(0);
    box.setValidator(0);
    box.sizeHint();
    box.sizeLimit();
    box.text(0);
    box.validator();
    box.call_activated(0);
    box.call_activated(QString());
    box.call_highlighted(0);
    box.call_highlighted(QString());
    box.call_textChanged(QString());
    box.call_updateMask();
}

void tst_Q3ComboBox::autoCompletion_data()
{
    QTest::addColumn<bool>("autoCompletion");
    QTest::newRow("false") << false;
    QTest::newRow("true") << true;
}

// public bool autoCompletion() const
void tst_Q3ComboBox::autoCompletion()
{
    QFETCH(bool, autoCompletion);

    SubQ3ComboBox box;

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    box.setAutoCompletion(autoCompletion);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(spy4.count(), 0);

    QCOMPARE(box.autoCompletion(), autoCompletion);
}

void tst_Q3ComboBox::autoResize_data()
{
    QTest::addColumn<bool>("autoResize");
    QTest::newRow("false") << false;
    QTest::newRow("true") << true;
}

// public bool autoResize() const
void tst_Q3ComboBox::autoResize()
{
    QFETCH(bool, autoResize);

    SubQ3ComboBox box;

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    QSize oldSize = box.size();
    box.setAutoResize(autoResize);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(spy4.count(), 0);
    QCOMPARE(box.autoResize(), autoResize);

    if (autoResize)
        QVERIFY(oldSize != box.size());
    else
        QCOMPARE(oldSize, box.size());
}

void tst_Q3ComboBox::changeItem_data()
{
    QTest::addColumn<QPixmap>("pixmap");
    QTest::addColumn<QString>("text");
    QTest::addColumn<int>("index");
    QTest::newRow("null") << QPixmap() << QString("foo") << 0;
}

// public void changeItem(QPixmap const& pixmap, QString const& text, int index)
void tst_Q3ComboBox::changeItem()
{
    QFETCH(QPixmap, pixmap);
    QFETCH(QString, text);
    QFETCH(int, index);

    SubQ3ComboBox box;
    box.insertItem("stub");

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    box.changeItem(pixmap, text, index);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), box.listBox() ? 1 : 0);
    QCOMPARE(spy3.count(), box.listBox() ? 1 : 0);
    QCOMPARE(spy4.count(), 0);

    QCOMPARE(box.text(index), text);
    if (pixmap.isNull()) {
        QVERIFY(!box.pixmap(index) || box.pixmap(index)->isNull());
    } else {
        QCOMPARE(*box.pixmap(index), pixmap);
    }
}

void tst_Q3ComboBox::clear_data()
{
    QTest::addColumn<QStringList>("list");
    QTest::newRow("null") << QStringList();
    QTest::newRow("1") << (QStringList() << "x");
    QTest::newRow("2") << (QStringList() << "x" << "y");
    QTest::newRow("3") << (QStringList() << "x" << "y" << "z");
}

// public void clear()
void tst_Q3ComboBox::clear()
{
    QFETCH(QStringList, list);

    SubQ3ComboBox box;
    box.insertStringList(list);

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    box.clear();
    QCOMPARE(box.count(), 0);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(spy4.count(), 0);
}

void tst_Q3ComboBox::clearEdit_data()
{
    QTest::addColumn<bool>("editable");
    QTest::addColumn<QStringList>("list");
    QTest::newRow("non-editable") << false << QStringList();
    QTest::newRow("editable-empty") << true << QStringList();
    QTest::newRow("editable-3") << true << (QStringList() << "x" << "y" << "z");
}

// public void clearEdit()
void tst_Q3ComboBox::clearEdit()
{
    QFETCH(QStringList, list);
    QFETCH(bool, editable);

    SubQ3ComboBox box;
    box.setEditable(editable);
    box.insertStringList(list);

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    box.clearEdit();
    QCOMPARE(box.count(), list.count());
    for (int i = 0; i < list.count(); ++i)
        QCOMPARE(box.text(i), list.at(i));
    if (editable) {
        QVERIFY(box.lineEdit());
        QVERIFY(box.lineEdit()->text().isEmpty());
    }
    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(spy4.count(), (editable && !list.isEmpty())? 1 : 0);
}

void tst_Q3ComboBox::count_data()
{
    QTest::addColumn<bool>("useListBox");
    QTest::addColumn<QStringList>("list");
    QTest::newRow("popup-empty") << false << QStringList();
    QTest::newRow("popup-3") << false << (QStringList() << "x" << "y" << "z");
    QTest::newRow("listbox-empty") << true << QStringList();
    QTest::newRow("listbox-3") << true << (QStringList() << "x" << "y" << "z");
}

// public int count() const
void tst_Q3ComboBox::count()
{
    QFETCH(bool, useListBox);
    QFETCH(QStringList, list);

    SubQ3ComboBox box;
    box.insertStringList(list);
    Q3ListBox *listBox = new Q3ListBox(&box);
    if (useListBox) {
        box.setListBox(listBox);
        QCOMPARE(box.listBox(), listBox);
        QCOMPARE(box.count(), 0);
        listBox->insertStringList(list);
        QCOMPARE(box.count(), list.count());
    }
    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    QCOMPARE(box.count(), list.count());

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(spy4.count(), 0);
}

void tst_Q3ComboBox::currentItem_data()
{
    QTest::addColumn<QStringList>("list");
    QTest::addColumn<int>("setCurrentItem");
    QTest::addColumn<int>("currentItem");
    QTest::addColumn<bool>("signal");
    QStringList list = (QStringList() << "x" << "y" << "z");
    QTest::newRow("3 0,0") << list << 0 << 0 << false;
    QTest::newRow("3 1,1") << list << 1 << 1 << true;
    QTest::newRow("3 2,2") << list << 2 << 2 << true;
    QTest::newRow("3 3,0") << list << 3 << 0 << false;
    QTest::newRow("- 1,0") << QStringList() << 1 << 0 << false;
}

// public int currentItem() const
void tst_Q3ComboBox::currentItem()
{
    QFETCH(QStringList, list);
    QFETCH(int, setCurrentItem);
    QFETCH(int, currentItem);
    QFETCH(bool, signal);

    SubQ3ComboBox box;
    box.insertStringList(list);

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    box.setCurrentItem(setCurrentItem);
    QCOMPARE(box.currentItem(), currentItem);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), signal ? 1 : 0);
    QCOMPARE(spy3.count(), signal ? 1 : 0);
    QCOMPARE(spy4.count(), 0);
}

void tst_Q3ComboBox::currentText_data()
{
    QTest::addColumn<QStringList>("list");
    QTest::addColumn<int>("currentItem");
    QTest::addColumn<QString>("currentText");
    QTest::addColumn<int>("expectedCurrentItem");
    QTest::addColumn<bool>("signal");
    QTest::newRow("null") << QStringList() << 0 << QString("foo") << 0 << false;
    QStringList list = (QStringList() << "x" << "y" << "z");
    QTest::newRow("3,0") << list << 0 << QString("x") << 0 << false;
    QTest::newRow("3,1") << list << 0 << QString("y") << 1 << true;
    QTest::newRow("3,2") << list << 0 << QString("z") << 2 << true;
}

// public QString currentText() const
void tst_Q3ComboBox::currentText()
{
    QFETCH(QStringList, list);
    QFETCH(int, currentItem);
    QFETCH(QString, currentText);
    QFETCH(int, expectedCurrentItem);
    QFETCH(bool, signal);

    SubQ3ComboBox box;
    QVERIFY(box.currentText().isEmpty());
    box.insertStringList(list);
    box.setCurrentItem(currentItem);

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    box.setCurrentText(currentText);
    if (list.isEmpty())
        QVERIFY(box.currentText().isEmpty());
    else {
        QCOMPARE(box.currentText(), currentText);
        QCOMPARE(box.currentItem(), expectedCurrentItem);
    }
    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), signal ? 1 : 0);
    QCOMPARE(spy3.count(), signal ? 1 : 0);
    QCOMPARE(spy4.count(), 0);
}

Q_DECLARE_METATYPE(Q3ComboBox::Policy)
void tst_Q3ComboBox::duplicatesEnabled_data()
{
    QTest::addColumn<QStringList>("list");
    QTest::addColumn<bool>("duplicatesEnabled");
    QTest::addColumn<int>("count");
    QTest::addColumn<QString>("string");
    QTest::addColumn<int>("postCount");
    QTest::addColumn<Q3ComboBox::Policy>("policy");
    QTest::addColumn<bool>("activatedSignal");
    QTest::addColumn<bool>("highlightedSignal");

    QStringList list = (QStringList() << "x" << "y" << "z");

    QTest::newRow("null") << QStringList() << false << 0 << QString() << 0 << Q3ComboBox::AtTop << false << false;
    QTest::newRow("3, no-dup-3") << list << false << list.count() << QString() << list.count() << Q3ComboBox::AtTop << false << false;
    QTest::newRow("3, dup-3") << list << true << list.count() << QString() << list.count() << Q3ComboBox::AtTop << false << false;
    QStringList duplist = list;
    duplist << list[0];

    int c = duplist.count();
    QTest::newRow("4, no-dup-wdup-4") << duplist << false << c << "x" << c << Q3ComboBox::AtTop << true << true;

    QTest::newRow("4, dup-wdup-4 noIn") << duplist << true << c << "x" << c << Q3ComboBox::NoInsertion << true << false;
    QTest::newRow("4, dup-wdup-4 atTop") << duplist << true << c << "x" << c + 1 << Q3ComboBox::AtTop << true << true;
}

// public bool duplicatesEnabled() const
void tst_Q3ComboBox::duplicatesEnabled()
{
    QFETCH(QStringList, list);
    QFETCH(bool, duplicatesEnabled);
    QFETCH(int, count);
    QFETCH(QString, string);
    QFETCH(int, postCount);
    QFETCH(Q3ComboBox::Policy, policy);
    QFETCH(bool, activatedSignal);
    QFETCH(bool, highlightedSignal);

    SubQ3ComboBox box;
    box.setEditable(true);
    box.insertStringList(list);
    box.setInsertionPolicy(policy);
    box.setCurrentItem(1);
    QCOMPARE(box.insertionPolicy(), policy);

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    box.setDuplicatesEnabled(duplicatesEnabled);
    QCOMPARE(box.duplicatesEnabled(), duplicatesEnabled);
    QCOMPARE(box.count(), count);

    box.setEditText(string);
    QCOMPARE(box.lineEdit()->text(), string);
    QTest::keyPress(box.lineEdit(), Qt::Key_Return);
    QCOMPARE(box.count(), postCount);
    if (postCount != list.count()) {
        switch (policy) {
        case Q3ComboBox::InsertBeforeCurrent:
            QCOMPARE(box.text(0), string);
            break;
        case Q3ComboBox::InsertAtCurrent:
            QCOMPARE(box.text(1), string);
            break;
        case Q3ComboBox::InsertAfterCurrent:
            QCOMPARE(box.text(2), string);
            break;
        case Q3ComboBox::InsertAtTop:
            QCOMPARE(box.text(0), string);
            break;
        case Q3ComboBox::InsertAtBottom:
            QCOMPARE(box.text(box.count() - 1), string);
            break;
        case Q3ComboBox::NoInsert:
            break;
        }
    }

    //QCOMPARE(spy0.count(), activatedSignal ? 1 : 0);
    //QCOMPARE(spy1.count(), activatedSignal ? 1 : 0);
    QCOMPARE(spy2.count(), highlightedSignal ? 1 : 0);
    QCOMPARE(spy3.count(), highlightedSignal ? 1 : 0);
    QCOMPARE(spy4.count(), !list.isEmpty() ? 1 : 0);
}

void tst_Q3ComboBox::editable_data()
{
    QTest::addColumn<QStringList>("list");
    QTest::addColumn<bool>("editable");
    QTest::newRow("empty-noneditable") << QStringList() << false;
    QTest::newRow("empty-editable") << QStringList() << true;
}

// public bool editable() const
void tst_Q3ComboBox::editable()
{
    QFETCH(QStringList, list);
    QFETCH(bool, editable);

    SubQ3ComboBox box;
    box.insertStringList(list);

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    box.setEditable(editable);
    QCOMPARE(box.editable(), editable);
    if (box.editable()) {
        QVERIFY(box.lineEdit());
        if (!list.empty())
            QCOMPARE(box.lineEdit()->text(), list[box.currentItem()]);

        box.setEditable(false);
        QVERIFY(!box.lineEdit());
    }

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(spy4.count(), 0);
}

void tst_Q3ComboBox::insertItem_data()
{
    QTest::addColumn<QStringList>("init");
    QTest::addColumn<QString>("string");
    QTest::addColumn<int>("index");
    QTest::addColumn<QStringList>("expected");
    QTest::newRow("null") << QStringList() << QString() << 0 << (QStringList() << QString());
    QTest::newRow("front") << (QStringList() << "x") << QString() << 0 << (QStringList() << QString() << "x");
    QTest::newRow("back") << (QStringList() << "x") << QString() << 1 << (QStringList() << "x" << QString());
}

// public void insertItem(QPixmap const& pixmap, int index = -1)
void tst_Q3ComboBox::insertItem()
{
    QFETCH(QStringList, init);
    QFETCH(QString, string);
    QFETCH(int, index);
    QFETCH(QStringList, expected);

    SubQ3ComboBox box;
    box.insertStringList(init);

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    box.insertItem(string, index);
    for (int i = 0; i < expected.count(); ++i)
        QCOMPARE(box.text(i), expected.at(i));

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(spy4.count(), 0);
}

Q_DECLARE_METATYPE(QStringList)
void tst_Q3ComboBox::insertStringList_data()
{
    QTest::addColumn<QStringList>("init");
    QTest::addColumn<QStringList>("stringlist");
    QTest::addColumn<int>("index");
    QTest::addColumn<QStringList>("expected");
    QTest::newRow("null") << QStringList() << QStringList() << 0 << (QStringList() << QString());
    QTest::newRow("front") << (QStringList() << "x") << (QStringList() << "y") << 0 << (QStringList() << "y" << "x");
    QTest::newRow("back") << (QStringList() << "x") << (QStringList() << "y") << 1 << (QStringList() << "x" << "y");
}

// public void insertStringList(QStringList const& , int index = -1)
void tst_Q3ComboBox::insertStringList()
{
    QFETCH(QStringList, init);
    QFETCH(QStringList, stringlist);
    QFETCH(int, index);
    QFETCH(QStringList, expected);

    SubQ3ComboBox box;
    box.insertStringList(init);

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    box.insertStringList(stringlist, index);
    for (int i = 0; i < expected.count(); ++i)
        QCOMPARE(box.text(i), expected.at(i));

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(spy4.count(), 0);
}

void tst_Q3ComboBox::insertStrList_data()
{
    QTest::addColumn<QStringList>("init");
    QTest::addColumn<int>("index");
    QTest::addColumn<QStringList>("expected");
    QTest::newRow("front") << (QStringList() << "x") << 0 << (QStringList() << "y" << "x");
    QTest::newRow("back") << (QStringList() << "x") << 1 << (QStringList() << "x" << "y");
}

// public void insertStrList(Q3StrList const* , int index = -1)
void tst_Q3ComboBox::insertStrList()
{
    QFETCH(QStringList, init);
    static const char* strings[] = { "y", 0 };
    QFETCH(int, index);
    QFETCH(QStringList, expected);

    SubQ3ComboBox box;
    box.insertStringList(init);

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    box.insertStrList(strings, -1, index);
    for (int i = 0; i < expected.count(); ++i)
        QCOMPARE(box.text(i), expected.at(i));

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(spy4.count(), 0);
}

void tst_Q3ComboBox::maxCount_data()
{
    QTest::addColumn<QStringList>("init");
    QTest::addColumn<int>("maxCount");
    QTest::addColumn<QStringList>("expected");
    QStringList empty;
    QTest::newRow("remove-all-1") << (QStringList(empty) << "x") << 0 << (QStringList(empty));
    QTest::newRow("remove-all-2") << (QStringList(empty) << "x" << "y") << 0 << (QStringList(empty));
    QTest::newRow("remove-one") << (QStringList(empty) << "x" << "y") << 1 << (QStringList(empty) << "x");
    QTest::newRow("remove-two") << (QStringList(empty) << "x" << "y" << "z") << 1 << (QStringList(empty) << "x");
    QTest::newRow("do-nothing-1") << (QStringList(empty) << "x") << 1 << (QStringList(empty) << "x");
    QTest::newRow("do-nothing-2") << (QStringList(empty) << "x") << 2 << (QStringList(empty) << "x");
    QTest::newRow("do-nothing-3") << (QStringList(empty) << "x" << "y") << 2 << (QStringList(empty) << "x" << "y");
}

// public int maxCount() const
void tst_Q3ComboBox::maxCount()
{
    QFETCH(QStringList, init);
    QFETCH(int, maxCount);
    QFETCH(QStringList, expected);

    SubQ3ComboBox box;
    box.insertStringList(init);

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    box.setMaxCount(maxCount);
    QCOMPARE(box.maxCount(), maxCount);
    //QCOMPARE(box.count(), expected.count());
    for (int i = 0; i < expected.count(); ++i)
        QCOMPARE(box.text(i), expected.at(i));

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(spy4.count(), 0);

    //box.setEditable(true);
    //box.lineEdit()->setText("foo");
    //QTest::keyPress(box.lineEdit(), Qt::Key_Return);
    //QVERIFY(box.count() <= maxCount);
}

// public QPixmap const* pixmap(int index) const
void tst_Q3ComboBox::pixmap()
{
    SubQ3ComboBox box;
    QCOMPARE(box.pixmap(0), static_cast<const QPixmap*>(0));
    QPixmap pixmap(1, 1);
    box.insertItem(pixmap, "foo", 0);
    QVERIFY(box.pixmap(0) != 0);
}

void tst_Q3ComboBox::removeItem_data()
{
    QTest::addColumn<QStringList>("init");
    QTest::addColumn<int>("removeItem");
    QTest::addColumn<QStringList>("expected");
    QStringList empty;
    QTest::newRow("remove-1") << (QStringList(empty) << "x") << 0 << (QStringList(empty));
    QTest::newRow("remove-2") << (QStringList(empty) << "x" << "y") << 0 << (QStringList(empty) << "y");
    QTest::newRow("remove-3") << (QStringList(empty) << "x" << "y") << 1 << (QStringList(empty) << "x");
    QTest::newRow("remove-4") << (QStringList(empty) << "x" << "y" << "z") << 1 << (QStringList(empty) << "x" << "z");
    QTest::newRow("do-nothing") << (QStringList(empty) << "x" << "y") << -2 << (QStringList(empty) << "x" << "y");
}

// public void removeItem(int index)
void tst_Q3ComboBox::removeItem()
{
    QFETCH(QStringList, init);
    QFETCH(int, removeItem);
    QFETCH(QStringList, expected);

    SubQ3ComboBox box;
    box.insertStringList(init);

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    box.removeItem(removeItem);
    QCOMPARE(box.count(), expected.count());
    for (int i = 0; i < expected.count(); ++i)
        QCOMPARE(box.text(i), expected.at(i));

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(spy4.count(), 0);
}

// public void setValidator(QValidator const* )
void tst_Q3ComboBox::setValidator()
{
    SubQ3ComboBox box;
    const QValidator *v = new QRegExpValidator(&box);
    box.setValidator(v);
    QCOMPARE(box.validator(), static_cast<const QValidator*>(0));
    box.setEditable(true);
    box.setValidator(v);
    QCOMPARE(box.validator(), v);
}

Q_DECLARE_METATYPE(Qt::Key)
void tst_Q3ComboBox::keyPressEvent_data()
{
    QTest::addColumn<QStringList>("init");
    QTest::addColumn<int>("currentItem");
    QTest::addColumn<Qt::Key>("key");
    QTest::addColumn<int>("expectedCurrent");
    QTest::newRow("go-nowhere-1") << (QStringList() << "x") << 0 << Qt::Key_Up << 0;
    QTest::newRow("go-nowhere-1") << (QStringList() << "x") << 0 << Qt::Key_Down << 0;

    QTest::newRow("move-1") << (QStringList() << "x" << "y") << 1 << Qt::Key_Up << 0;
    QTest::newRow("move-2") << (QStringList() << "x" << "y") << 0 << Qt::Key_Down << 1;
    QTest::newRow("move-3") << (QStringList() << "x" << "y") << 1 << Qt::Key_Home << 0;
    QTest::newRow("move-4") << (QStringList() << "x" << "y") << 0 << Qt::Key_End << 1;

    QTest::newRow("move-let-1") << (QStringList() << "x" << "y") << 0 << Qt::Key_X << 0;
    QTest::newRow("move-let-2") << (QStringList() << "x" << "y") << 0 << Qt::Key_Y << 1;
    QTest::newRow("move-let-3") << (QStringList() << "x" << "y") << 1 << Qt::Key_X << 0;
    QTest::newRow("move-let-4") << (QStringList() << "x" << "y") << 1 << Qt::Key_Y << 1;
}

// protected void keyPressEvent(QKeyEvent* e)
void tst_Q3ComboBox::keyPressEvent()
{
    QFETCH(QStringList, init);
    QFETCH(int, currentItem);
    QFETCH(Qt::Key, key);
    QFETCH(int, expectedCurrent);

    SubQ3ComboBox box;
    box.insertStringList(init);
    box.setCurrentItem(currentItem);

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    QTest::keyPress(&box, key);
    QCOMPARE(box.currentItem(), expectedCurrent);

    // Signal behavior is inconsistent :( so only check that the two signals
    // match if it ever emits, sometimes it does, sometimes it doesn't.
    QCOMPARE(spy0.count(), spy1.count());

    int spyCount = (currentItem == expectedCurrent) ? 0 : 1;
    QCOMPARE(spy2.count(), spyCount);
    QCOMPARE(spy3.count(), spyCount);
    QCOMPARE(spy4.count(), 0);
}

void tst_Q3ComboBox::wheelEvent_data()
{
    QTest::addColumn<QStringList>("init");
    QTest::addColumn<int>("currentItem");
    QTest::addColumn<int>("delta");
    QTest::addColumn<int>("expectedCurrent");
    QTest::newRow("go-nowhere-1") << (QStringList() << "x") << 0 << 10 << 0;
    QTest::newRow("go-nowhere-1") << (QStringList() << "x") << 0 << -10 << 0;

    QTest::newRow("move-1") << (QStringList() << "x" << "y") << 1 << 10 << 0;
    QTest::newRow("move-2") << (QStringList() << "x" << "y") << 0 << -10 << 1;
}

// protected void wheelEvent(QWheelEvent* e)
void tst_Q3ComboBox::wheelEvent()
{
    QFETCH(QStringList, init);
    QFETCH(int, currentItem);
    QFETCH(int, delta);
    QFETCH(int, expectedCurrent);

    SubQ3ComboBox box;
    box.insertStringList(init);
    box.setCurrentItem(currentItem);

    QSignalSpy spy0(&box, SIGNAL(activated(int)));
    QSignalSpy spy1(&box, SIGNAL(activated(QString)));
    QSignalSpy spy2(&box, SIGNAL(highlighted(int)));
    QSignalSpy spy3(&box, SIGNAL(highlighted(QString)));
    QSignalSpy spy4(&box, SIGNAL(textChanged(QString)));

    QWheelEvent event(QPoint(0,0), delta, Qt::LeftButton, Qt::NoModifier);
    box.call_wheelEvent(&event);
    QCOMPARE(box.currentItem(), expectedCurrent);

    int spyCount = (currentItem == expectedCurrent) ? 0 : 1;
    QCOMPARE(spy0.count(), spyCount);
    QCOMPARE(spy1.count(), spyCount);
    QCOMPARE(spy2.count(), spyCount);
    QCOMPARE(spy3.count(), spyCount);
    QCOMPARE(spy4.count(), 0);
}

void tst_Q3ComboBox::task231724_clear()
{
    Q3ComboBox box;
    
    for ( int i = 0; i <50; i++ ) {
        box.insertItem(QString( "value %1" ).arg( i ));
    }
    
    box.show();
    QTest::qWait(20);
    QTest::mouseClick (&box, Qt::LeftButton);
    QTest::qWait(500);
    Q3ListBox *popup = box.findChild<Q3ListBox *>("in-combo");
    QVERIFY(popup);
    QVERIFY(popup->isVisible());
    QVERIFY(!popup->testAttribute(Qt::WA_OutsideWSRange));
    QTest::qWait(20);
    
    box.clear();
    for ( int i = 0; i <50; i++ ) {
        box.insertItem(QString( "value %1" ).arg( i ));
    }
    
    QTest::qWait(20);
    QTest::mouseClick (&box, Qt::LeftButton);
    QTest::qWait(500);

    popup = box.findChild<Q3ListBox *>("in-combo");
    QVERIFY(popup);
    QVERIFY(popup->isVisible());
    QVERIFY(!popup->testAttribute(Qt::WA_OutsideWSRange));

}

QTEST_MAIN(tst_Q3ComboBox)
#include "tst_q3combobox.moc"

