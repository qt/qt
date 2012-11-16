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

#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QStringList>
#include <QTest>

class tst_MakeTestSelfTest: public QObject
{
    Q_OBJECT

private slots:
    void tests_pro_files();
    void tests_pro_files_data();
};

/* Verify that all tests are listed somewhere in one of the autotest .pro files */
void tst_MakeTestSelfTest::tests_pro_files()
{
    static QStringList lines;

    if (lines.isEmpty()) {
        QDir dir(SRCDIR "/..");
        QStringList proFiles = dir.entryList(QStringList() << "*.pro");
        foreach (QString const& proFile, proFiles) {
            QString filename = QString("%1/../%2").arg(SRCDIR).arg(proFile);
            QFile file(filename);
            if (!file.open(QIODevice::ReadOnly)) {
                QFAIL(qPrintable(QString("open %1: %2").arg(filename).arg(file.errorString())));
            }
            while (!file.atEnd()) {
                lines << file.readLine().trimmed();
            }
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
        "Subdir `%1' is missing from tests/auto/*.pro\n"
        "This means the test won't be compiled or run on any platform.\n"
        "If this is intentional, please put the test name in a comment in one of the .pro files.").arg(subdir))
    );

}

void tst_MakeTestSelfTest::tests_pro_files_data()
{
    QTest::addColumn<QString>("subdir");
    QDir dir(SRCDIR "/..");
    QStringList subdirs = dir.entryList(QDir::AllDirs|QDir::NoDotAndDotDot);

    foreach (const QString& subdir, subdirs) {
        if (subdir == QString::fromLatin1("tmp")
            || subdir.startsWith("."))
        {
            continue;
        }
        QTest::newRow(qPrintable(subdir)) << subdir;
    }
}

QTEST_MAIN(tst_MakeTestSelfTest)
#include "tst_maketestselftest.moc"
