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
            qDebug() << "PAINT EVENT:" << paintNumber;
            Q3ProgressBar::paintEvent(event);
        }
        int paintNumber;
};

/*
  Maybe this test should be redesigned.
 */
void tst_Q3ProgressBar::setProgress()
{
    MyCustomProgressBar * m_progressBar = new MyCustomProgressBar();
    m_progressBar->show();
    QApplication::processEvents();

    //case with total steps = 0
    m_progressBar->setTotalSteps(0);
    int oldValue = m_progressBar->progress();
    m_progressBar->paintNumber = 0;
    m_progressBar->setProgress(m_progressBar->progress() + 1);
    QCOMPARE(oldValue + 1,m_progressBar->progress());
    QApplication::processEvents();

    // It might be > 1 because it is animated.
    QVERIFY(m_progressBar->paintNumber >= 1);
    qDebug() << "Animation test: paintNumber =" << m_progressBar->paintNumber;
    
    //standard case
    m_progressBar->setTotalSteps(3);
    m_progressBar->setProgress(0);
    m_progressBar->paintNumber = 0;
    m_progressBar->setProgress(m_progressBar->progress() + 1);
    QApplication::processEvents();

    // It might be > 1 because other events might cause painting.
    QVERIFY(m_progressBar->paintNumber >= 1);
    qDebug() << "Standard test: paintNumber =" << m_progressBar->paintNumber;
}

QTEST_MAIN(tst_Q3ProgressBar)
#include "tst_q3progressbar.moc"
