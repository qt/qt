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
#include <QDebug>
#include <QRegExp>
#include <QString>

#include <qtest.h>


class tst_qregexp : public QObject
{
    Q_OBJECT
private slots:
    void escape_old();
    void escape_old_data() { escape_data(); }
    void escape_new1();
    void escape_new1_data() { escape_data(); }
    void escape_new2();
    void escape_new2_data() { escape_data(); }
    void escape_new3();
    void escape_new3_data() { escape_data(); }
    void escape_new4();
    void escape_new4_data() { escape_data(); }
private:
    void escape_data();
};


static void verify(const QString &quoted, const QString &expected)
{
    if (quoted != expected)
        qDebug() << "ERROR:" << quoted << expected;
}

void tst_qregexp::escape_data()
{
    QTest::addColumn<QString>("pattern");
    QTest::addColumn<QString>("expected");

    QTest::newRow("escape 0") << "Hello world" << "Hello world";
    QTest::newRow("escape 1") << "(Hello world)" << "\\(Hello world\\)";
    { 
        QString s;
        for (int i = 0; i < 10; ++i)
            s += "(escape)";
        QTest::newRow("escape 10") << s << QRegExp::escape(s);
    }
    { 
        QString s;
        for (int i = 0; i < 100; ++i)
            s += "(escape)";
        QTest::newRow("escape 100") << s << QRegExp::escape(s);
    }
}

void tst_qregexp::escape_old()
{
    QFETCH(QString, pattern);
    QFETCH(QString, expected);

    QBENCHMARK {
        static const char meta[] = "$()*+.?[\\]^{|}";
        QString quoted = pattern;
        int i = 0;

        while (i < quoted.length()) {
            if (strchr(meta, quoted.at(i).toLatin1()) != 0)
                quoted.insert(i++, QLatin1Char('\\'));
            ++i;
        }

        verify(quoted, expected);
    }
}

void tst_qregexp::escape_new1()
{
    QFETCH(QString, pattern);
    QFETCH(QString, expected);

    QBENCHMARK {
        QString quoted;
        const int count = pattern.count();
        quoted.reserve(count * 2);
        const QLatin1Char backslash('\\');
        for (int i = 0; i < count; i++) {
            switch (pattern.at(i).toLatin1()) {
            case '$':
            case '(':
            case ')':
            case '*':
            case '+':
            case '.':
            case '?':
            case '[':
            case '\\':
            case ']':
            case '^':
            case '{':
            case '|':
            case '}':
                quoted.append(backslash);
            }
            quoted.append(pattern.at(i));
        }
        verify(quoted, expected);
    }
}

void tst_qregexp::escape_new2()
{
    QFETCH(QString, pattern);
    QFETCH(QString, expected);

    QBENCHMARK {
        int count = pattern.count();
        const QLatin1Char backslash('\\');
        QString quoted(count * 2, backslash);
        const QChar *patternData = pattern.data();
        QChar *quotedData = quoted.data();
        int escaped = 0;
        for ( ; --count >= 0; ++patternData) {
            const QChar c = *patternData;
            switch (c.unicode()) {
            case '$':
            case '(':
            case ')':
            case '*':
            case '+':
            case '.':
            case '?':
            case '[':
            case '\\':
            case ']':
            case '^':
            case '{':
            case '|':
            case '}':
                ++escaped;
                ++quotedData;
            }
            *quotedData = c;
            ++quotedData;
        }
        quoted.resize(pattern.size() + escaped); 

        verify(quoted, expected);
    }
}

void tst_qregexp::escape_new3()
{
    QFETCH(QString, pattern);
    QFETCH(QString, expected);

    QBENCHMARK {
        QString quoted;
        const int count = pattern.count();
        quoted.reserve(count * 2);
        const QLatin1Char backslash('\\');
        for (int i = 0; i < count; i++) {
            switch (pattern.at(i).toLatin1()) {
            case '$':
            case '(':
            case ')':
            case '*':
            case '+':
            case '.':
            case '?':
            case '[':
            case '\\':
            case ']':
            case '^':
            case '{':
            case '|':
            case '}':
                quoted += backslash;
            }
            quoted += pattern.at(i);
        }

        verify(quoted, expected);
    }
}


static inline bool needsEscaping(int c)
{
    switch (c) {
    case '$':
    case '(':
    case ')':
    case '*':
    case '+':
    case '.':
    case '?':
    case '[':
    case '\\':
    case ']':
    case '^':
    case '{':
    case '|':
    case '}':
        return true;
    }
    return false;
}

void tst_qregexp::escape_new4()
{
    QFETCH(QString, pattern);
    QFETCH(QString, expected);

    QBENCHMARK {
        const int n = pattern.size();
        const QChar *patternData = pattern.data();
        // try to prevent copy if no escape is needed
        int i = 0;
        for (int i = 0; i != n; ++i) {
            const QChar c = patternData[i];
            if (needsEscaping(c.unicode()))
                break;
        }
        if (i == n) {
            verify(pattern, expected);
            // no escaping needed, "return pattern" should be done here.
            return;
        }
        const QLatin1Char backslash('\\');
        QString quoted(n * 2, backslash);
        QChar *quotedData = quoted.data();
        for (int j = 0; j != i; ++j) 
            *quotedData++ = *patternData++;
        int escaped = 0;
        for (; i != n; ++i) {
            const QChar c = *patternData;
            if (needsEscaping(c.unicode())) {
                ++escaped;
                ++quotedData;
            }
            *quotedData = c;
            ++quotedData;
            ++patternData;
        }
        quoted.resize(n + escaped); 
        verify(quoted, expected);
        // "return quoted"
    }
}
QTEST_MAIN(tst_qregexp)

#include "main.moc"
