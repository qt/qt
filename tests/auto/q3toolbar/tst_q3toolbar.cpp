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
#include <q3toolbar.h>
#include <q3mainwindow.h>
#include <qaction.h>
#include <qapplication.h>
#include <QToolButton>
#include <q3action.h>
#include <qmenu.h>

//TESTED_CLASS=
//TESTED_FILES=

#if defined Q_CC_MSVC && _MSC_VER <= 1200
#define NOFINDCHILDRENMETHOD
#endif

class tst_Q3ToolBar : public QObject
{
    Q_OBJECT

public:
    tst_Q3ToolBar();
    virtual ~tst_Q3ToolBar();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void toggled();
    void actionGroupPopup();

    // task-specific tests below me
    void task182657();

private:
    Q3ToolBar* testWidget;
};

tst_Q3ToolBar::tst_Q3ToolBar()
{
}

tst_Q3ToolBar::~tst_Q3ToolBar()
{

}

void tst_Q3ToolBar::initTestCase()
{
    testWidget = new Q3ToolBar(0, "testWidget");
    testWidget->show();
    qApp->setMainWidget(testWidget);

    QTest::qWait(100);
}

void tst_Q3ToolBar::cleanupTestCase()
{
    delete testWidget;
}

void tst_Q3ToolBar::init()
{
}

void tst_Q3ToolBar::cleanup()
{
}

void tst_Q3ToolBar::toggled()
{
    // When clicking on a toggled action it should emit a signal
    QAction *action = new QAction( this, "action" );
    action->setToggleAction( true );
    action->addTo(testWidget);
    testWidget->show();
    QSignalSpy spy(action, SIGNAL(toggled(bool)));
#ifndef NOFINDCHILDRENMETHOD
    QList<QToolButton *> list = testWidget->findChildren<QToolButton *>();
#else
    QList<QToolButton *> list = qFindChildren<QToolButton *>(testWidget, QString());

#endif
    for (int i = 0; i < list.size(); ++i)
        QTest::mouseClick(list.at(i), Qt::LeftButton);
    QCOMPARE(spy.count(), 1);

    // Also try the othe case (a toggled action will emit the toolbuttons toggled)
    QSignalSpy spy2(list.at(1), SIGNAL(toggled(bool)));
    action->setChecked(!action->isChecked());
    QCOMPARE(spy2.count(), 1);

}

class MenuEventFilter : public QObject
{
public:
    MenuEventFilter(QObject *parent = 0) : QObject(parent), menuShown(false) {}
    bool wasMenuShown() const { return menuShown; }
    void setMenuShown(bool b) { menuShown = b; }
protected:
    bool eventFilter(QObject *o, QEvent *e)
    {
        if (e->type() == QEvent::Show) {
            menuShown = true;
            QTimer::singleShot(0, o, SLOT(hide()));
        }
        return false;
    }
private:
    bool menuShown;
};

void tst_Q3ToolBar::actionGroupPopup()
{
    Q3ActionGroup* ag = new Q3ActionGroup(testWidget);
    ag->setText("Group");
    ag->setUsesDropDown(true);
    ag->setExclusive(false);
    Q3Action *a = new Q3Action(QIcon(), "ActionA", QKeySequence(), ag);
    a->setToggleAction(true);
    Q3Action *b = new Q3Action(QIcon(), "ActionB", QKeySequence(), ag);
    b->setToggleAction(true);
    ag->addTo(testWidget);
    QTest::qWait(100);
#ifndef NOFINDCHILDRENMETHOD
    QList<QToolButton *> list = testWidget->findChildren<QToolButton *>();
#else
    QList<QToolButton *> list = qFindChildren<QToolButton *>(testWidget, QString());
#endif
    QToolButton *tb = 0;
    for (int i=0;i<list.size();i++) {
        if (list.at(i)->menu()) {
            tb = list.at(i);
            break;
        }
    }
    MenuEventFilter mef;
    tb->menu()->installEventFilter(&mef);
    QTest::mouseClick(tb, Qt::LeftButton, 0, QPoint(5,5));
    QVERIFY(!mef.wasMenuShown());
    QTest::mouseClick(tb, Qt::LeftButton, 0, QPoint(tb->rect().right() - 5, tb->rect().bottom() - 5));
    QVERIFY(mef.wasMenuShown());
}

class Q3MainWindow_task182657 : public Q3MainWindow
{
    Q3ToolBar *toolbar;

public:
    Q3MainWindow_task182657(QWidget *parent = 0)
        : Q3MainWindow(parent)
    {
	toolbar = new Q3ToolBar(this);
    }

    void rebuild()
    {
	toolbar->clear();
 	new QToolButton(toolbar, "b");
 	new QToolButton(toolbar, "a");
    }
};

void tst_Q3ToolBar::task182657()
{
    Q3MainWindow_task182657 *window = new Q3MainWindow_task182657;
    window->show();
    qApp->processEvents();
    window->rebuild();
    qApp->processEvents();
    window->rebuild();
    qApp->processEvents();
}

QTEST_MAIN(tst_Q3ToolBar)
#include "tst_q3toolbar.moc"
