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

#include <QtTest/QtTest>
#include <QtCore/QtCore>

//TESTED_CLASS=
//TESTED_FILES=

#include "tst_linguist.h"

void tst_linguist::fetchtr()
{
    // FIXME: This probably should use some yet-to-be-invented
    // binary interface to 'lupdate' instead of playing around
    // with the filesystem,

    QRegExp reg("\\s*");
    QString lupdate("lupdate");

    QFETCH(QString, input);

    QFETCH(QString, name);
    QFETCH(QString, file);
    QFETCH(QString, line);
    QFETCH(QString, src);

    QString result;

    QTemporaryFile profile("tst_lu_XXXXXX.pro");
    QTemporaryFile cppfile("tst_lu_XXXXXX.cpp");
    QTemporaryFile tsfile("tst_lu_XXXXXX.ts");

    profile.open();
    cppfile.open();
    tsfile.open();

#if 0
    profile.setAutoRemove(false);
    cppfile.setAutoRemove(false);
    tsfile.setAutoRemove(false);

    qDebug() << ".pro: " << profile.fileName();
    qDebug() << ".cpp: " << cppfile.fileName();
    qDebug() << ".ts:  " << tsfile.fileName();
#endif

    QTextStream prots(&profile);
    prots << "SOURCES += " << cppfile.fileName() << "\n";
    prots << "TRANSLATIONS += " << tsfile.fileName() << "\n";
    prots.flush();

    QTextStream cppts(&cppfile);
    cppts.setCodec("ISO 8859-1");
    cppts << input << '\n';
    cppts.flush();

    QProcess proc;
    proc.start(lupdate, QStringList() << profile.fileName());
    proc.waitForFinished();

    result = tsfile.readAll();

    static QRegExp re(
        "<name>(.+)</name>\\s*"
        "<message.*>\\s*"    // there might be a numerus="yes" attribiute
        "<location filename=\"(.+)\" line=\"(\\d+)\"/>\\s*"
        "<source>(.+)</source>\\s*"
        "<translation type=\"unfinished\">.*</translation>\\s*"
    );

    re.indexIn(result);
    QString resname = re.cap(1);
    //QString resfile = re.cap(2);
    QString resline = re.cap(3);
    QString ressrc  = re.cap(4);

    //qDebug() << "pattern:" << re.pattern();
    //qDebug() << "result:" << result;
    //qDebug() << "resname:" << resname;
    ////qDebug() << "resfile:" << resfile;
    //qDebug() << "resline:" << resline;
    //qDebug() << "ressource:" << ressrc;

    QCOMPARE(src + ":    " + resname, src + ":    " + name);
    QCOMPARE(src + ":    " + resline, src + ":    " + line);
    QCOMPARE(src + ":    " + ressrc,  src + ":    " + src);
}

void tst_linguist::fetchtr_data()
{
    using namespace QTest;

    addColumn<QString>("input");
    addColumn<QString>("name");
    addColumn<QString>("file");
    addColumn<QString>("line");
    addColumn<QString>("src");

    // plain stuff
    newRow("00") << "int main() { tr(\"foo\"); }"
        << "@default" << "XXXXXX" << "1" << "foo";

    // space at beginning of text
    newRow("01") << "int main() { tr(\" foo\"); }"
        << "@default" << "XXXXXX" << "1" << " foo";
    // space at end of text
    newRow("02") << "int main() { tr(\"foo \"); }"
        << "@default" << "XXXXXX" << "1" << "foo ";
    // space in the middle of the text
    newRow("03") << "int main() { tr(\"foo bar\"); }"
        << "@default" << "XXXXXX" << "1" << "foo bar";

    // tab at beginning of text
    newRow("04") << "int main() { tr(\"\tfoo\"); }"
        << "@default" << "XXXXXX" << "1" << "<byte value=\"x9\"/>foo";
    // tab at end of text
    newRow("05") << "int main() { tr(\"foo\t\"); }"
        << "@default" << "XXXXXX" << "1" << "foo<byte value=\"x9\"/>";
    // tab in the middle of the text
    newRow("06") << "int main() { tr(\"foo\tbar\"); }"
        << "@default" << "XXXXXX" << "1" << "foo<byte value=\"x9\"/>bar";

    // check for unicode
    newRow("07") << "int main() { tr(\"\32\"); }" // 26 dec
        << "@default" << "XXXXXX" << "1" << "<byte value=\"x1a\"/>";
    // check for unicode
    newRow("08") << "int main() { tr(\"\33\"); }" // 27 dec
        << "@default" << "XXXXXX" << "1" << "<byte value=\"x1b\"/>";
    // check for unicode
    newRow("09") << "int main() { tr(\"\176\"); }" // 124 dec
        << "@default" << "XXXXXX" << "1" << "~";
    // check for unicode
    newRow("10") << "int main() { tr(\"\301\"); }" // 193 dec
        << "@default" << "XXXXXX" << "1" << "&#xc1;";

    // Bug 162562: lupdate does not find QCoreApplication::translate() strings
    newRow("11") << "int main() { QString s = QCoreApplication::translate"
        "(\"mycontext\", \"msg\", \"\", QCoreApplication::CodecForTr, 2);"
        << "mycontext" << "XXXXXX" << "1" << "msg";

    // Bug 161504: lupdate produces wrong ts file with context "N::QObject"
    newRow("12") << "namespace N { QString foo() "
        "{ return QObject::tr(\"msg\"); }"
        << "QObject" << "XXXXXX" << "1" << "msg";

    // Correct example from 161504:
    newRow("13") << "namespace N { QString foo(); }"
        "QString N::anyfunc() { return QObject::tr(\"msg\"); }"
        << "QObject" << "XXXXXX" << "1" << "msg";

    // Bug 161106: When specifying ::QObject::tr() then lupdate will
    // take the previous word as being the namespace
    newRow("14") << " std::cout << ::QObject::tr(\"msg\");"
        << "QObject" << "XXXXXX" << "1" << "msg";

}
