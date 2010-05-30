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

