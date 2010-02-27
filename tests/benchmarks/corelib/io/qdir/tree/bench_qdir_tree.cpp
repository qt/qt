/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite module of the Qt Toolkit.
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

#include <QtTest/QTest>

#include <QDirIterator>
#include <QFile>
#include <QString>
#include <QStack>

#include "../../../../../shared/filesystem.h"

class bench_QDir_tree
    : public QObject
{
    Q_OBJECT

public:
    bench_QDir_tree()
        : prefix("./test-tree/")
    {
    }

private:
    QByteArray prefix;

private slots:
    void initTestCase()
    {
        QFile list(":/4.6.0-list.txt");
        QVERIFY(list.open(QIODevice::ReadOnly | QIODevice::Text));

        QVERIFY(fs.createDirectory(prefix));

        QStack<QByteArray> stack;
        QByteArray line;
        Q_FOREVER {
            char ch;
            if (!list.getChar(&ch))
                break;
            if (ch != ' ') {
                line.append(ch);
                continue;
            }

            int pop = 1;
            if (!line.isEmpty())
                pop = line.toInt();

            while (pop) {
                stack.pop();
                --pop;
            }

            line = list.readLine();
            line.chop(1);
            stack.push(line);

            line = prefix;
            Q_FOREACH(const QByteArray &pathElement, stack)
                line += pathElement;

            if (line.endsWith('/'))
                QVERIFY(fs.createDirectory(line));
            else
                QVERIFY(fs.createFile(line));

            line.clear();
        }
    }

    void fileSearch_data() const
    {
        QTest::addColumn<QStringList>("nameFilters");
        QTest::addColumn<int>("filter");
        QTest::addColumn<int>("entryCount");

        QTest::newRow("*.cpp") << QStringList("*.cpp")
            << int(QDir::Files)
            << 3813;

        QTest::newRow("executables") << QStringList("*")
            << int(QDir::Executable | QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot)
            << 543;
    }

    void fileSearch() const
    {
        QFETCH(QStringList, nameFilters);
        QFETCH(int, filter);
        QFETCH(int, entryCount);

        int count = 0;
        QBENCHMARK {
            // Recursive directory iteration
            QDirIterator iterator(prefix, nameFilters, QDir::Filter(filter),
                QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

            count = 0;
            while (iterator.hasNext()) {
                iterator.next();
                ++count;
            }

            QCOMPARE(count, entryCount);
        }

        QCOMPARE(count, entryCount);
    }

    void traverseDirectory() const
    {
        int count = 0;
        QBENCHMARK {
            QDirIterator iterator(prefix,
                    QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System,
                    QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

            while (iterator.hasNext()) {
                iterator.next();
                ++count;
            }

            QCOMPARE(count, 11963);
        }

        QCOMPARE(count, 11963);
    }

private:
    FileSystem fs;
};

QTEST_MAIN(bench_QDir_tree)
#include "bench_qdir_tree.moc"
