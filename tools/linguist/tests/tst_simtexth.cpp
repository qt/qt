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
#include <QtCore/QtCore>

//int getSimilarityScore(const QString &str1, const char* str2);
#include "../shared/simtexth.h"
#include "tst_linguist.h"

void tst_linguist::simtexth()
{
    QFETCH(QString, one);
    QFETCH(QString, two);
    QFETCH(int, expected);

    int measured = getSimilarityScore(one, two.toLatin1());
    QCOMPARE(measured, expected);
}


void tst_linguist::simtexth_data()
{
    using namespace QTest;

    addColumn<QString>("one");
    addColumn<QString>("two");
    addColumn<int>("expected");

    newRow("00") << "" << "" << 1024;
    newRow("01") << "a" << "a" << 1024;
    newRow("02") << "ab" << "ab" << 1024;
    newRow("03") << "abc" << "abc" << 1024;
    newRow("04") << "abcd" << "abcd" << 1024;
}
