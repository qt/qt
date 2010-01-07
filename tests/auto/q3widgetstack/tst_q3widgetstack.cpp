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


#include <QtTest/QtTest>

#include "q3widgetstack.h"
#include <qapplication.h>
#include <qboxlayout.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qdialog.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3WidgetStack : public QObject
{
    Q_OBJECT

public:
    tst_Q3WidgetStack();
    virtual ~tst_Q3WidgetStack();

protected slots:
    void aboutToShow_helper(int);
    void aboutToShow_helper(QWidget *);
public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void aboutToShow();
    void sizeHint();
    void addWidget();

private:
    Q3WidgetStack *testWidget;
    QWidget *widgetOne;
    QWidget *widgetTwo;

    // Helpers for aboutToShow() test
    QWidget *currentVisibleWidgetOne;
    int aboutToShowId;
    bool aboutToShowSignalOne;
    QWidget *currentVisibleWidgetTwo;
    QWidget *aboutToShowWidget;
    bool aboutToShowSignalTwo;

};


const QSizePolicy ignored(QSizePolicy::Ignored, QSizePolicy::Ignored);
const QSizePolicy preferred(QSizePolicy::Preferred, QSizePolicy::Preferred);

tst_Q3WidgetStack::tst_Q3WidgetStack()
{
    testWidget = 0;
}

tst_Q3WidgetStack::~tst_Q3WidgetStack()
{
}


void tst_Q3WidgetStack::initTestCase()
{
    testWidget = new Q3WidgetStack( 0 );
    qApp->setMainWidget(testWidget);
    testWidget->show();
    widgetOne = new QWidget( testWidget );
    testWidget->addWidget( widgetOne );
    widgetTwo = new QWidget( testWidget );
    testWidget->addWidget( widgetTwo );

}

void tst_Q3WidgetStack::cleanupTestCase()
{
    delete testWidget;
    testWidget = 0;
}

void tst_Q3WidgetStack::init()
{
   testWidget->raiseWidget( widgetOne );
}

void tst_Q3WidgetStack::cleanup()
{
}

void tst_Q3WidgetStack::aboutToShow_helper(int id)
{
    currentVisibleWidgetOne = testWidget->visibleWidget();
    aboutToShowId = id;
    aboutToShowSignalOne = TRUE;
}

void tst_Q3WidgetStack::aboutToShow_helper(QWidget *widget)
{
    currentVisibleWidgetTwo = testWidget->visibleWidget();
    aboutToShowWidget = widget;
    aboutToShowSignalTwo = TRUE;
}

void tst_Q3WidgetStack::aboutToShow()
{
    currentVisibleWidgetOne = 0;
    aboutToShowId = 1000; // The id is not likely to be 1000
    aboutToShowSignalOne = FALSE;
    currentVisibleWidgetTwo = 0;
    aboutToShowWidget = 0;
    aboutToShowSignalTwo = FALSE;

    connect(testWidget, SIGNAL(aboutToShow(int)), this, SLOT(aboutToShow_helper(int)));
    connect(testWidget, SIGNAL(aboutToShow(QWidget *)), this, SLOT(aboutToShow_helper(QWidget *)));
    testWidget->raiseWidget(widgetTwo);
    for (int a = 0;a < 10;a++) {
	qApp->processEvents();
	if (aboutToShowSignalOne || aboutToShowSignalTwo)
	    break;
    }
    QVERIFY(aboutToShowSignalOne);
    QVERIFY(aboutToShowSignalTwo);
    QVERIFY(currentVisibleWidgetOne == widgetOne);
    QVERIFY(currentVisibleWidgetTwo == widgetOne);
    QCOMPARE(aboutToShowId, testWidget->id(widgetTwo));
    QCOMPARE(aboutToShowWidget, widgetTwo);
    QCOMPARE(testWidget->visibleWidget(), widgetTwo);
}

void tst_Q3WidgetStack::sizeHint()
{
    QDialog dialog(0);

    QHBoxLayout *layout = new QHBoxLayout(&dialog);
    Q3WidgetStack *stack = new Q3WidgetStack(&dialog);
    layout->addWidget(stack);

    QWidget *pageA = new QWidget(stack);
    (new QVBoxLayout(pageA))->addWidget(new QLineEdit(pageA));
    stack->addWidget(pageA);
    pageA->setSizePolicy(ignored);

    QWidget *pageB = new QWidget(stack);
    (new QVBoxLayout(pageB))->addWidget(new QRadioButton(pageB));
    stack->addWidget(pageB);
    pageB->setSizePolicy(ignored);

    stack->raiseWidget(pageA);
    dialog.show();

    QSize before(dialog.size());
    pageB->setSizePolicy(preferred);
    stack->raiseWidget(pageB);

    layout->activate();
    dialog.setFixedSize(dialog.minimumSizeHint());
    qApp->processEvents();
    QVERIFY(before != dialog.minimumSizeHint());
}

void tst_Q3WidgetStack::addWidget()
{
    QDialog dialog(0);
    Q3WidgetStack *stack = new Q3WidgetStack(&dialog);

    // The widget should now be added to the stack 
    QWidget *widget = new QWidget(stack);
    QCOMPARE(stack->id(widget), -1);

    // The widget should get a positive ID
    int id = stack->addWidget(widget);
    QVERIFY(id >= 0);
    QCOMPARE(stack->id(widget), id);
    QCOMPARE(stack->widget(id), widget);

    QWidget *widget2 = new QWidget(stack);
    QCOMPARE(stack->id(widget2), -1);

    // The widget should get a negative ID different from -1
    id = stack->addWidget(widget2, -2);
    QVERIFY(id < -1);
    QCOMPARE(stack->id(widget2), id);
    QVERIFY(stack->id(widget2) != stack->id(widget));
    QCOMPARE(stack->widget(id), widget2);

    // The widget should be removed when it's deleted
    delete widget2;
    QCOMPARE(stack->id(widget2), -1);
    QCOMPARE(stack->widget(id), (QWidget*)0);
    
    // Create a unique ID
    int uid = stack->id(widget) + 100;
    QWidget *widget3 = new QWidget(widget);

    // Test of reparenting
    id = stack->addWidget(widget3, uid);
    QCOMPARE(id, uid);
    QCOMPARE(stack->id(widget3), id);
    QCOMPARE(widget3->parentWidget(), (QWidget*)stack);
    QCOMPARE(stack->widget(id), widget3);

    delete widget3;
    QCOMPARE(stack->id(widget3), -1);
    QCOMPARE(stack->widget(id), (QWidget*)0);

    delete widget;
    QCOMPARE(stack->id(widget), -1);

    delete stack;

}

QTEST_MAIN(tst_Q3WidgetStack)
#include "tst_q3widgetstack.moc"
