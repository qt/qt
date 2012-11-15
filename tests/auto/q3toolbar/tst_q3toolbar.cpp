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
#include <q3toolbar.h>
#include <q3mainwindow.h>
#include <qaction.h>
#include <qapplication.h>
#include <QToolButton>

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
