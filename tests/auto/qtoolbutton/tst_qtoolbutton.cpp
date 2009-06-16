/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qtoolbutton.h>
#include <qmenu.h>
#include <qaction.h>
#include <qwindowsstyle.h>
#include <qstyleoption.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QToolButton : public QObject
{
Q_OBJECT

public:
    tst_QToolButton();
    virtual ~tst_QToolButton();

private slots:
    void getSetCheck();
    void triggered();
    void task230994_iconSize();

protected slots:
    void sendMouseClick();
private:
    QWidget *w;
};

tst_QToolButton::tst_QToolButton()
{
}

tst_QToolButton::~tst_QToolButton()
{
}

// Testing get/set functions
void tst_QToolButton::getSetCheck()
{
    QToolButton obj1;
    // QMenu* QToolButton::menu()
    // void QToolButton::setMenu(QMenu*)
    QMenu *var1 = new QMenu;
    obj1.setMenu(var1);
    QCOMPARE(var1, obj1.menu());
    obj1.setMenu((QMenu *)0);
    QCOMPARE((QMenu *)0, obj1.menu());
    delete var1;

    // ToolButtonPopupMode QToolButton::popupMode()
    // void QToolButton::setPopupMode(ToolButtonPopupMode)
    obj1.setPopupMode(QToolButton::ToolButtonPopupMode(QToolButton::DelayedPopup));
    QCOMPARE(QToolButton::ToolButtonPopupMode(QToolButton::DelayedPopup), obj1.popupMode());
    obj1.setPopupMode(QToolButton::ToolButtonPopupMode(QToolButton::MenuButtonPopup));
    QCOMPARE(QToolButton::ToolButtonPopupMode(QToolButton::MenuButtonPopup), obj1.popupMode());
    obj1.setPopupMode(QToolButton::ToolButtonPopupMode(QToolButton::InstantPopup));
    QCOMPARE(QToolButton::ToolButtonPopupMode(QToolButton::InstantPopup), obj1.popupMode());

    // bool QToolButton::autoRaise()
    // void QToolButton::setAutoRaise(bool)
    obj1.setAutoRaise(false);
    QCOMPARE(false, obj1.autoRaise());
    obj1.setAutoRaise(true);
    QCOMPARE(true, obj1.autoRaise());

    // QAction * QToolButton::defaultAction()
    // void QToolButton::setDefaultAction(QAction *)
    QAction *var4 = new QAction(0);
    obj1.setDefaultAction(var4);
    QCOMPARE(var4, obj1.defaultAction());
    obj1.setDefaultAction((QAction *)0);
    QCOMPARE((QAction *)0, obj1.defaultAction());
    delete var4;

#ifdef QT_HAS_QT3SUPPORT
    //ensure that popup delay is not reset on style change
    obj1.setPopupDelay(5);
    obj1.setStyle(new QWindowsStyle);
    QCOMPARE(obj1.popupDelay(), 5);
#endif
}

Q_DECLARE_METATYPE(QAction*)

void tst_QToolButton::triggered()
{
    qRegisterMetaType<QAction *>("QAction *");
    QToolButton tb;
    tb.show();
    QSignalSpy spy(&tb,SIGNAL(triggered(QAction*)));
    QMenu *menu = new QMenu("Menu");
    QAction *one = menu->addAction("one");
    menu->addAction("two");
    QAction *def = new QAction("def", this);

    tb.setMenu(menu);
    tb.setDefaultAction(def);

#ifdef Q_WS_QWS
    QApplication::processEvents(); //wait for the window system to show the tool button
#endif

    def->trigger();
    QCOMPARE(spy.count(),1);
    QCOMPARE(qvariant_cast<QAction *>(spy.at(0).at(0)), def);

    w = menu;
    QTimer::singleShot(30, this, SLOT(sendMouseClick()));
    tb.showMenu();
    QTest::qWait(20);
    QCOMPARE(spy.count(),2);
    QCOMPARE(qvariant_cast<QAction *>(spy.at(1).at(0)), one);
    delete menu;
}

void tst_QToolButton::task230994_iconSize()
{
    //we check that the iconsize returned bu initStyleOption is valid
    //when the toolbutton has no parent
    class MyToolButton : public QToolButton
    {
        friend class tst_QToolButton;
    public:
        void initStyleOption(QStyleOptionToolButton *option)
        {
            QToolButton::initStyleOption(option);
        }
    };

    MyToolButton button;
    QStyleOptionToolButton option;
    button.initStyleOption(&option);
    QVERIFY(option.iconSize.isValid());
}


void tst_QToolButton::sendMouseClick()
{
    QTest::mouseClick(w, Qt::LeftButton, 0, QPoint(7,7));
}

QTEST_MAIN(tst_QToolButton)
#include "tst_qtoolbutton.moc"
