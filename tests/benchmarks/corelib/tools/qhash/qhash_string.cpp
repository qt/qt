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

/*

////////////////////////////////////////////////////////////////////

This benchmark serves as reality check on the idea that hashing the complete
string is a good idea.

  Executive summary: It is not a good idea.

////////////////////////////////////////////////////////////////////

********* Start testing of tst_QHash *********
Config: Using QTest library 4.8.0, Qt 4.8.0
PASS   : tst_QHash::initTestCase()
RESULT : tst_QHash::qhash_qt4():
     0.041 msecs per iteration (total: 85, iterations: 2048)
PASS   : tst_QHash::qhash_qt4()
RESULT : tst_QHash::qhash_faster():
     0.0122 msecs per iteration (total: 100, iterations: 8192)
PASS   : tst_QHash::qhash_faster()
PASS   : tst_QHash::cleanupTestCase()
Totals: 4 passed, 0 failed, 0 skipped

////////////////////////////////////////////////////////////////////

*/

#include "qhash_string.h"

#include <QFile>
#include <QHash>
#include <QString>
#include <QStringList>

#include <QTest>


class tst_QHash : public QObject
{
    Q_OBJECT

private slots:
    void qhash_qt4();
    void qhash_faster();

private:
    QString data();
};

const int N = 1000000;
extern double s;

///////////////////// QHash /////////////////////

QString tst_QHash::data()
{
    QFile file("data.txt");
    file.open(QIODevice::ReadOnly);
    return QString::fromLatin1(file.readAll());
}

void tst_QHash::qhash_qt4()
{
    QStringList items = data().split(QLatin1Char('\n'));
    QHash<QString, int> hash;
    
    QBENCHMARK {
        for (int i = 0, n = items.size(); i != n; ++i) {
            hash[items.at(i)] = i;
        }
    }
}

void tst_QHash::qhash_faster()
{
    QList<String> items;
    foreach (const QString &s, data().split(QLatin1Char('\n')))
        items.append(s);
    QHash<String, int> hash;
    
    QBENCHMARK {
        for (int i = 0, n = items.size(); i != n; ++i) {
            hash[items.at(i)] = i;
        }
    }
}

QTEST_MAIN(tst_QHash)

#include "qhash_string.moc"
