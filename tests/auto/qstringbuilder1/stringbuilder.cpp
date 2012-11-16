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

#define LITERAL "some literal"

// "some literal", but replacing all vocals by their umlauted UTF-8 string :)
#define UTF8_LITERAL "s\xc3\xb6m\xc3\xab l\xc3\xaft\xc3\xabr\xc3\xa4l"

void runScenario()
{
    // set codec for C strings to 0, enforcing Latin1
    QTextCodec::setCodecForCStrings(0);
    QVERIFY(!QTextCodec::codecForCStrings());

    QLatin1Literal l1literal(LITERAL);
    QLatin1String l1string(LITERAL);
    QString string(l1string);
    QStringRef stringref(&string, 2, 10);
    QLatin1Char achar('c');
    QString r2(QLatin1String(LITERAL LITERAL));
    QString r;
    QByteArray ba(LITERAL);

    r = l1literal P l1literal;
    QCOMPARE(r, r2);
    r = string P string;
    QCOMPARE(r, r2);
    r = stringref P stringref;
    QCOMPARE(r, QString(stringref.toString() + stringref.toString()));
    r = string P l1literal;
    QCOMPARE(r, r2);
    r = string P l1string;
    QCOMPARE(r, r2);
    r = string + achar;
    QCOMPARE(r, QString(string P achar));
    r = achar + string;
    QCOMPARE(r, QString(achar P string));
#ifndef QT_NO_CAST_FROM_ASCII
    r = string P LITERAL;
    QCOMPARE(r, r2);
    r = LITERAL P string;
    QCOMPARE(r, r2);
    r = ba P string;
    QCOMPARE(r, r2);
    r = string P ba;
    QCOMPARE(r, r2);

    // now test with codec for C strings set
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QVERIFY(QTextCodec::codecForCStrings());
    QCOMPARE(QTextCodec::codecForCStrings()->name(), QByteArray("UTF-8"));

    string = QString::fromUtf8(UTF8_LITERAL);
    r2 = QString::fromUtf8(UTF8_LITERAL UTF8_LITERAL);
    ba = UTF8_LITERAL;

    r = string P UTF8_LITERAL;
    QCOMPARE(r.size(), r2.size());
    QCOMPARE(r, r2);
    r = UTF8_LITERAL P string;
    QCOMPARE(r, r2);
    r = ba P string;
    QCOMPARE(r, r2);
    r = string P ba;
    QCOMPARE(r, r2);
#endif
}
