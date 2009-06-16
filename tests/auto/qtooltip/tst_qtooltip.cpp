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
#include <qtooltip.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QToolTip : public QObject
{
    Q_OBJECT

public:
    tst_QToolTip() {}
    virtual ~tst_QToolTip() {}

public slots:
    void initTestCase() {}
    void cleanupTestCase() {}
    void init() {}
    void cleanup() {}

private slots:

    // task-specific tests below me
    void task183679_data();
    void task183679();
    void setPalette();
};

class Widget_task183679 : public QWidget
{
    Q_OBJECT
public:
    Widget_task183679(QWidget *parent = 0) : QWidget(parent) {}

    void showDelayedToolTip(int msecs)
    {
        QTimer::singleShot(msecs, this, SLOT(showToolTip()));
    }

private slots:
    void showToolTip()
    {
        QToolTip::showText(mapToGlobal(QPoint(0, 0)), "tool tip text", this);
    }
};

Q_DECLARE_METATYPE(Qt::Key)

void tst_QToolTip::task183679_data()
{
    QTest::addColumn<Qt::Key>("key");
    QTest::addColumn<bool>("visible");
#ifdef Q_WS_MAC
    const bool visibleAfterNonModifier = false;
#else
    const bool visibleAfterNonModifier = true;
#endif
    QTest::newRow("non-modifier") << Qt::Key_A << visibleAfterNonModifier;
    QTest::newRow("Shift") << Qt::Key_Shift << true;
    QTest::newRow("Control") << Qt::Key_Control << true;
    QTest::newRow("Alt") << Qt::Key_Alt << true;
    QTest::newRow("Meta") << Qt::Key_Meta << true;
}

void tst_QToolTip::task183679()
{
    QFETCH(Qt::Key, key);
    QFETCH(bool, visible);

    Widget_task183679 widget;
    widget.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&widget);
#endif

    widget.showDelayedToolTip(100);
    QTest::qWait(300);
    QVERIFY(QToolTip::isVisible());

    QTest::keyPress(&widget, key);

    // Important: the following delay must be larger than the duration of the timer potentially
    // initiated by the key press (currently 300 msecs), but smaller than the minimum
    // auto-close timeout (currently 10000 msecs)
    QTest::qWait(1500);

    QCOMPARE(QToolTip::isVisible(), visible);
}

void tst_QToolTip::setPalette()
{
    //the previous test may still have a tooltip pending for deletion
    QTest::qWait(100);
    QVERIFY(!QToolTip::isVisible());

    QToolTip::showText(QPoint(), "tool tip text", 0);
    QTest::qWait(100);

    QWidget *toolTip = 0;
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        if (widget->windowType() == Qt::ToolTip) {
            toolTip = widget;
            break;
        }
    }
    QVERIFY(toolTip);

    const QPalette oldPalette = toolTip->palette();
    QPalette newPalette = oldPalette;
    newPalette.setColor(QPalette::ToolTipBase, Qt::red);
    newPalette.setColor(QPalette::ToolTipText, Qt::blue);
    QToolTip::setPalette(newPalette);
    QCOMPARE(toolTip->palette(), newPalette);
}

QTEST_MAIN(tst_QToolTip)
#include "tst_qtooltip.moc"
