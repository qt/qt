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

#define LITERAL "some literal"

// "some literal", but replacing all vocals by their umlauted UTF-8 string :)
#define UTF8_LITERAL "s\xc3\xb6m\xc3\xab l\xc3\xaft\xc3\xabr\xc3\xa4l"


//fix for gcc4.0: if the operator+ does not exist without QT_USE_FAST_OPERATOR_PLUS
#ifndef QT_USE_FAST_CONCATENATION
#define Q %
#else
#define Q P
#endif

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

    r = l1literal Q l1literal;
    QCOMPARE(r, r2);
    r = string P string;
    QCOMPARE(r, r2);
    r = stringref Q stringref;
    QCOMPARE(r, QString(stringref.toString() + stringref.toString()));
    r = string Q l1literal;
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
