/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <qmargins.h>

Q_DECLARE_METATYPE(QMargins)

//TESTED_CLASS=
//TESTED_FILES=

class tst_QMargins : public QObject
{
    Q_OBJECT

public:
    tst_QMargins();
    virtual ~tst_QMargins();


public slots:
    void init();
    void cleanup();
private slots:
    void getSetCheck();
};

// Testing get/set functions
void tst_QMargins::getSetCheck()
{
    QMargins margins;
    // int QMargins::width()
    // void QMargins::setWidth(int)
    margins.setLeft(0);
    QCOMPARE(0, margins.left());
    margins.setTop(INT_MIN);
    QCOMPARE(INT_MIN, margins.top());
    margins.setBottom(INT_MAX);
    QCOMPARE(INT_MAX, margins.bottom());
    margins.setRight(INT_MAX);
    QCOMPARE(INT_MAX, margins.right());

    margins = QMargins();
    QVERIFY(margins.isNull());
    margins.setLeft(5);
    margins.setRight(5);
    QVERIFY(!margins.isNull());
    QCOMPARE(margins, QMargins(5, 0, 5, 0));
} 

tst_QMargins::tst_QMargins()
{
}

tst_QMargins::~tst_QMargins()
{
}

void tst_QMargins::init()
{
}

void tst_QMargins::cleanup()
{
}



QTEST_APPLESS_MAIN(tst_QMargins)
#include "tst_qmargins.moc"
