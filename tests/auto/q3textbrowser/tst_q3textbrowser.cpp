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

#include <q3textbrowser.h>
#include <qapplication.h>
#include <qdatetime.h>
#include <q3mimefactory.h>
#include <qtextbrowser.h>
#include <qdesktopwidget.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3TextBrowser : public QObject
{
    Q_OBJECT
public:
    tst_Q3TextBrowser();
    virtual ~tst_Q3TextBrowser();

public slots:
    void initTestCase();
    void cleanupTestCase();
private slots:
    void setFont();
private:
    Q3TextBrowser *testWidget;
};

tst_Q3TextBrowser::tst_Q3TextBrowser()
{
    Q3MimeSourceFactory::defaultFactory()->addFilePath(".");
}

tst_Q3TextBrowser::~tst_Q3TextBrowser()
{
}

void tst_Q3TextBrowser::initTestCase()
{
    testWidget = new Q3TextBrowser;
    testWidget->setParent(0, Qt::WX11BypassWM);
    testWidget->move(QApplication::desktop()->availableGeometry().topLeft()+QPoint(5, 5));

    testWidget->show();
    QTest::qWaitForWindowShown(testWidget);
    qApp->processEvents();
}

void tst_Q3TextBrowser::cleanupTestCase()
{
    delete testWidget;
}

void tst_Q3TextBrowser::setFont()
{
    QFont f("Courier", 6);
    testWidget->setFont(f);
    f = f.resolve(testWidget->font());
    QVERIFY(testWidget->font() == f);
}

QTEST_MAIN(tst_Q3TextBrowser)
#include "tst_q3textbrowser.moc"

