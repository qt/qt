/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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
