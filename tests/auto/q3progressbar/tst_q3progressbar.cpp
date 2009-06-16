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
#include <qdebug.h>
#include <q3progressbar.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3ProgressBar : public QObject
{
Q_OBJECT

public:
    tst_Q3ProgressBar();
    virtual ~tst_Q3ProgressBar();

private slots:
    void getSetCheck();
    void setProgress();
};

tst_Q3ProgressBar::tst_Q3ProgressBar()
{
}

tst_Q3ProgressBar::~tst_Q3ProgressBar()
{
}

// Testing get/set functions
void tst_Q3ProgressBar::getSetCheck()
{
    Q3ProgressBar obj1;
    // bool Q3ProgressBar::centerIndicator()
    // void Q3ProgressBar::setCenterIndicator(bool)
    obj1.setCenterIndicator(false);
    QCOMPARE(false, obj1.centerIndicator());
    obj1.setCenterIndicator(true);
    QCOMPARE(true, obj1.centerIndicator());
}

class MyCustomProgressBar : public Q3ProgressBar
{
    public :
        MyCustomProgressBar() : Q3ProgressBar()
        {
            paintNumber = 0;
        }

        void paintEvent(QPaintEvent * event)
        {
            paintNumber++;
            Q3ProgressBar::paintEvent(event);
        }
        int paintNumber;
};

void tst_Q3ProgressBar::setProgress()
{
    MyCustomProgressBar * m_progressBar = new MyCustomProgressBar();
    m_progressBar->show();
    QTest::qWait(500);

    //case with total steps = 0
    m_progressBar->setTotalSteps(0);
    int oldValue = m_progressBar->progress();
    m_progressBar->paintNumber = 0;
    m_progressBar->setProgress(m_progressBar->progress() + 1);
    QCOMPARE(oldValue + 1,m_progressBar->progress());
    QCOMPARE(m_progressBar->paintNumber,1);

    //standard case
    m_progressBar->setTotalSteps(3);
    m_progressBar->setProgress(0);
    m_progressBar->paintNumber = 0;
    m_progressBar->setProgress(m_progressBar->progress() + 1);
    QCOMPARE(m_progressBar->paintNumber,1);

}

QTEST_MAIN(tst_Q3ProgressBar)
#include "tst_q3progressbar.moc"
