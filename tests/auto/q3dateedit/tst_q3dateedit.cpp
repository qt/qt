/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#include <q3datetimeedit.h>
#include <qapplication.h>
#include <qgroupbox.h>

//TESTED_CLASS=
//TESTED_FILES=qt3support/widgets/q3action.h qt3support/widgets/q3action.cpp

class tst_Q3DateEdit : public QObject
{
    Q_OBJECT

public:
    tst_Q3DateEdit();
    virtual ~tst_Q3DateEdit();



public slots:
    void initTestCase();
    void init();
    void cleanup();
private slots:
    void enabledPropagation();

private:
    Q3DateEdit* testWidget;
};

tst_Q3DateEdit::tst_Q3DateEdit()
{
}

tst_Q3DateEdit::~tst_Q3DateEdit()
{

}

void tst_Q3DateEdit::initTestCase()
{
    testWidget = new Q3DateEdit( 0, "testWidget" );
    testWidget->show();
    qApp->setActiveWindow(testWidget);
    qApp->setMainWidget( testWidget );
    QTest::qWait(100);
}

void tst_Q3DateEdit::init()
{
}

void tst_Q3DateEdit::cleanup()
{
}


void tst_Q3DateEdit::enabledPropagation()
{
    // Check a QDateEdit on its own
    testWidget->setEnabled(TRUE);
    QVERIFY(testWidget->isEnabled());
    testWidget->setEnabled(FALSE);
    QVERIFY(!testWidget->isEnabled());
    testWidget->setEnabled(TRUE);
    QVERIFY(testWidget->isEnabled());

    // Now check a QDateEdit on a QWidget
    QWidget w;
    Q3DateEdit *childOfW = new Q3DateEdit(&w, "childOfW");
    w.show();
    QVERIFY(childOfW->isEnabled());
    QObjectList children = childOfW->children();
    int i;
    for (i = 0; i < children.count(); ++i) {
        if (children.at(i)->isWidgetType())
	    QVERIFY(static_cast<QWidget *>(children.at(i))->isEnabled());
    }
    w.setEnabled(FALSE);
    QVERIFY(!childOfW->isEnabled());
    for (i = 0; i < children.count(); ++i) {
        if (children.at(i)->isWidgetType())
	    QVERIFY(!static_cast<QWidget *>(children.at(i))->isEnabled());
    }
    w.setEnabled(TRUE);
    QVERIFY(childOfW->isEnabled());
    for (i = 0; i < children.count(); ++i) {
        if (children.at(i)->isWidgetType())
	    QVERIFY(static_cast<QWidget *>(children.at(i))->isEnabled());
    }

    // Now check a QDateEdit on a non-checkable QGroupBox
    QGroupBox *gb = new QGroupBox(&w, "nonCheckGroupBox");
    Q3DateEdit *childOfGB = new Q3DateEdit(gb, "childOfGB");
    gb->show();
    QVERIFY(childOfGB->isEnabled());
    children = childOfGB->children();
    for (i = 0; i < children.count(); ++i) {
        if (children.at(i)->isWidgetType())
	    QVERIFY(static_cast<QWidget *>(children.at(i))->isEnabled());
    }

    gb->setEnabled(FALSE);
    QVERIFY(!childOfGB->isEnabled());
    for (i = 0; i < children.count(); ++i) {
        if (children.at(i)->isWidgetType())
	    QVERIFY(!static_cast<QWidget *>(children.at(i))->isEnabled());
    }
    gb->setEnabled(TRUE);
    QVERIFY(childOfGB->isEnabled());
    for (i = 0; i < children.count(); ++i) {
        if (children.at(i)->isWidgetType())
	    QVERIFY(static_cast<QWidget *>(children.at(i))->isEnabled());
    }

    // Now check a QDateEdit on a checkable QGroupBox
    QGroupBox *cgb = new QGroupBox(&w, "checkGroupBox");
    cgb->setCheckable(TRUE);
    Q3DateEdit *childOfCGB = new Q3DateEdit(cgb, "childOfCGB");
    cgb->show();
    QVERIFY(childOfCGB->isEnabled());
    children = childOfCGB->children();
    for (i = 0; i < children.count(); ++i) {
        if (children.at(i)->isWidgetType())
	    QVERIFY(static_cast<QWidget *>(children.at(i))->isEnabled());
    }
    cgb->setChecked(FALSE);
    QVERIFY(!childOfCGB->isEnabled());
    for (i = 0; i < children.count(); ++i) {
        if (children.at(i)->isWidgetType())
	    QVERIFY(!static_cast<QWidget *>(children.at(i))->isEnabled());
    }
    cgb->setChecked(TRUE);
    QVERIFY(childOfCGB->isEnabled());
    for (i = 0; i < children.count(); ++i) {
        if (children.at(i)->isWidgetType())
	    QVERIFY(static_cast<QWidget *>(children.at(i))->isEnabled());
    }
}


QTEST_MAIN(tst_Q3DateEdit)
#include "tst_q3dateedit.moc"

