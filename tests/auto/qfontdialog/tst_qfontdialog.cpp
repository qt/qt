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


#include <qapplication.h>
#include <qfontinfo.h>
#include <qtimer.h>
#include <qmainwindow.h>
#include "qfontdialog.h"

//TESTED_CLASS=
//TESTED_FILES=

QT_FORWARD_DECLARE_CLASS(QtTestEventThread)

class tst_QFontDialog : public QObject
{
    Q_OBJECT

public:
    tst_QFontDialog();
    virtual ~tst_QFontDialog();


public slots:
    void postKeyReturn();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void defaultOkButton();
    void setFont();
};

tst_QFontDialog::tst_QFontDialog()
{
}

tst_QFontDialog::~tst_QFontDialog()
{
}

void tst_QFontDialog::initTestCase()
{
}

void tst_QFontDialog::cleanupTestCase()
{
}

void tst_QFontDialog::init()
{
}

void tst_QFontDialog::cleanup()
{
}


void tst_QFontDialog::postKeyReturn() {
#ifndef Q_WS_MAC
    QWidgetList list = QApplication::topLevelWidgets();
    for (int i=0; i<list.count(); ++i) {
	QFontDialog *dialog = qobject_cast<QFontDialog*>(list[i]);
	if (dialog) {
	    QTest::keyClick( list[i], Qt::Key_Return, Qt::NoModifier );
	    return;
	}
    }
#else
    extern void click_cocoa_button();
    click_cocoa_button();
#endif
}

void tst_QFontDialog::defaultOkButton()
{
    bool ok = FALSE;
    QTimer::singleShot(2000, this, SLOT(postKeyReturn()));
    QFontDialog::getFont(&ok);
    QVERIFY(ok == TRUE);
}


void tst_QFontDialog::setFont()
{
    /* The font should be the same before as it is after if nothing changed
              while the font dialog was open.
	      Task #27662
    */
    bool ok = FALSE;
#if defined Q_OS_HPUX
    QString fontName = "Courier";
    int fontSize = 25;
#elif defined Q_OS_AIX
    QString fontName = "Charter";
    int fontSize = 13;
#else
    QString fontName = "Arial";
    int fontSize = 24;
#endif
    QFont f1(fontName, fontSize);
    f1.setPixelSize(QFontInfo(f1).pixelSize());
    QTimer::singleShot(2000, this, SLOT(postKeyReturn()));
    QFont f2 = QFontDialog::getFont(&ok, f1);
    QCOMPARE(QFontInfo(f2).pointSize(), QFontInfo(f1).pointSize());
}



QTEST_MAIN(tst_QFontDialog)
#include "tst_qfontdialog.moc"
