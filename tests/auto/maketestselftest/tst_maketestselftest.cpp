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

#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QStringList>
#include <QTest>

class tst_MakeTestSelfTest: public QObject
{
    Q_OBJECT

private slots:
    void auto_dot_pro();
    void auto_dot_pro_data();
};

/* Verify that all tests are listed somewhere in auto.pro */
void tst_MakeTestSelfTest::auto_dot_pro()
{
    static QStringList lines;

    if (lines.isEmpty()) {
        QString filename = QString::fromLatin1(SRCDIR "/../auto.pro");
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            QFAIL(qPrintable(QString("open %1: %2").arg(filename).arg(file.errorString())));
        }
        while (!file.atEnd()) {
            lines << file.readLine().trimmed();
        }
    }

    QFETCH(QString, subdir);
    QRegExp re(QString("( |=|^|#)%1( |\\\\|$)").arg(QRegExp::escape(subdir)));
    foreach (const QString& line, lines) {
        if (re.indexIn(line) != -1) {
            return;
        }
    }

    QFAIL(qPrintable(QString(
        "Subdir `%1' is missing from tests/auto/auto.pro\n"
        "This means the test won't be compiled or run on any platform.\n"
        "If this is intentional, please put the test name in a comment in auto.pro.").arg(subdir))
    );
}

void tst_MakeTestSelfTest::auto_dot_pro_data()
{
    QTest::addColumn<QString>("subdir");
    QDir dir(SRCDIR "/..");
    QStringList subdirs = dir.entryList(QDir::AllDirs|QDir::NoDotAndDotDot);

    foreach (const QString& subdir, subdirs) {
        QTest::newRow(qPrintable(subdir)) << subdir;
    }
}

QTEST_MAIN(tst_MakeTestSelfTest)
#include "tst_maketestselftest.moc"
