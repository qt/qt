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
#include <q3cstring.h>
#include <qregexp.h>
#include <qtextstream.h>


//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3CString : public QObject
{
    Q_OBJECT

public:
    tst_Q3CString();
    virtual ~tst_Q3CString();


public slots:
    void init();
    void cleanup();
private slots:
    void setExpand();
    void check_QTextStream();
    void check_QDataStream();
    void replace_uint_uint_data();
    void replace_uint_uint();
    void replace_string_data();
    void replace_string();
    void remove_uint_uint_data();
    void remove_uint_uint();
    void prepend();
    void append();
    void insert();
    void simplifyWhiteSpace();
    void stripWhiteSpace();
    void lower();
    void upper();
    void rightJustify();
    void leftJustify();
    void mid();
    void right();
    void left();
    void contains();
    void findRev();
    void find();
    void sprintf();
    void copy();
    void fill();
    void truncate();
    void constructor();
    void isEmpty();
    void isNull();
    void acc_01();
    void length_data();
    void length();

    // testfunctions?
    void remove_string_data();
    void remove_regexp_data();
};

Q_DECLARE_METATYPE(Q3CString)

tst_Q3CString::tst_Q3CString()
{
}

tst_Q3CString::~tst_Q3CString()
{
}

void tst_Q3CString::init()
{
}

void tst_Q3CString::cleanup()
{
}

void tst_Q3CString::remove_uint_uint_data()
{
    replace_uint_uint_data();
}

void tst_Q3CString::remove_string_data()
{
    replace_string_data();
}

void tst_Q3CString::length_data()
{
    QTest::addColumn<Q3CString>("s1");
    QTest::addColumn<int>("res");

    QTest::newRow( "data0" )  << Q3CString("Test") << 4;
    QTest::newRow( "data1" )  << Q3CString("The quick brown fox jumps over the lazy dog") << 43;
    QTest::newRow( "data2" )  << Q3CString(0) << 0;
    QTest::newRow( "data3" )  << Q3CString("A") << 1;
    QTest::newRow( "data4" )  << Q3CString("AB") << 2;
    QTest::newRow( "data5" )  << Q3CString("AB\n") << 3;
    QTest::newRow( "data6" )  << Q3CString("AB\nC") << 4;
    QTest::newRow( "data7" )  << Q3CString("\n") << 1;
    QTest::newRow( "data8" )  << Q3CString("\nA") << 2;
    QTest::newRow( "data9" )  << Q3CString("\nAB") << 3;
    QTest::newRow( "data10" )  << Q3CString("\nAB\nCDE") << 7;
    QTest::newRow( "data11" )  << Q3CString("shdnftrheid fhgnt gjvnfmd chfugkh bnfhg thgjf vnghturkf chfnguh bjgnfhvygh hnbhgutjfv dhdnjds dcjs d") << 100;
}

void tst_Q3CString::replace_uint_uint_data()
{
    QTest::addColumn<Q3CString>("string");
    QTest::addColumn<int>("index");
    QTest::addColumn<int>("len");
    QTest::addColumn<Q3CString>("after");
    QTest::addColumn<Q3CString>("result");

    QTest::newRow( "rem00" ) << Q3CString("-<>ABCABCABCABC>") << 0 << 3 << Q3CString("") << Q3CString("ABCABCABCABC>");
    QTest::newRow( "rem01" ) << Q3CString("ABCABCABCABC>") << 1 << 4 <<Q3CString("") <<Q3CString("ACABCABC>");
    QTest::newRow( "rem02" ) << Q3CString("ACABCABC>") << 999 << 4 << Q3CString("") << Q3CString("ACABCABC>");
    QTest::newRow( "rem03" ) << Q3CString("ACABCABC>") << 9 << 4 << Q3CString("") << Q3CString("ACABCABC>");
    QTest::newRow( "rem04" ) << Q3CString("ACABCABC>") << 8 << 4 << Q3CString("") << Q3CString("ACABCABC");
    QTest::newRow( "rem05" ) << Q3CString("ACABCABC") << 7 << 1 << Q3CString("") << Q3CString("ACABCAB");
    QTest::newRow( "rem06" ) << Q3CString("ACABCAB") << 4 << 0 << Q3CString("") << Q3CString("ACABCAB");

    QTest::newRow( "rep00" ) << Q3CString("ACABCAB") << 4 << 0 << Q3CString("X") << Q3CString("ACABXCAB");
    QTest::newRow( "rep01" ) << Q3CString("ACABXCAB") << 4 << 1 << Q3CString("Y") << Q3CString("ACABYCAB");
    QTest::newRow( "rep02" ) << Q3CString("ACABYCAB") << 4 << 1 << Q3CString("") << Q3CString("ACABCAB");
    QTest::newRow( "rep03" ) << Q3CString("ACABCAB") << 0 << 9999 << Q3CString("XX") << Q3CString("XX");
    QTest::newRow( "rep04" ) << Q3CString("XX") << 0 << 9999 << Q3CString("") << Q3CString("");
}

void tst_Q3CString::replace_string_data()
{
    QTest::addColumn<Q3CString>("string");
    QTest::addColumn<Q3CString>("before");
    QTest::addColumn<Q3CString>("after");
    QTest::addColumn<Q3CString>("result");

    QTest::newRow( "rem00" ) << Q3CString("") << Q3CString("") << Q3CString("") << Q3CString("");
    QTest::newRow( "rem01" ) << Q3CString("A") << Q3CString("") << Q3CString("") << Q3CString("A");
    QTest::newRow( "rem02" ) << Q3CString("A") << Q3CString("A") << Q3CString("") << Q3CString("");
    QTest::newRow( "rem03" ) << Q3CString("A") << Q3CString("B") << Q3CString("") << Q3CString("A");
    QTest::newRow( "rem04" ) << Q3CString("AA") << Q3CString("A") << Q3CString("") << Q3CString("");
    QTest::newRow( "rem05" ) << Q3CString("AB") << Q3CString("A") << Q3CString("") << Q3CString("B");
    QTest::newRow( "rem06" ) << Q3CString("AB") << Q3CString("B") << Q3CString("") << Q3CString("A");
    QTest::newRow( "rem07" ) << Q3CString("AB") << Q3CString("C") << Q3CString("") << Q3CString("AB");
    QTest::newRow( "rem08" ) << Q3CString("ABA") << Q3CString("A") << Q3CString("") << Q3CString("B");
    QTest::newRow( "rem09" ) << Q3CString("ABA") << Q3CString("B") << Q3CString("") << Q3CString("AA");
    QTest::newRow( "rem10" ) << Q3CString("ABA") << Q3CString("C") << Q3CString("") << Q3CString("ABA");
    QTest::newRow( "rem11" ) << Q3CString("banana") << Q3CString("an") << Q3CString("") << Q3CString("ba");
    QTest::newRow( "rem12" ) << Q3CString("") << Q3CString("A") << Q3CString("") << Q3CString("");
    QTest::newRow( "rem13" ) << Q3CString("") << Q3CString("A") << Q3CString(0) << Q3CString("");
    QTest::newRow( "rem14" ) << Q3CString(0) << Q3CString("A") << Q3CString("") << Q3CString(0);
    QTest::newRow( "rem15" ) << Q3CString(0) << Q3CString("A") << Q3CString(0) << Q3CString(0);
    QTest::newRow( "rem17" ) << Q3CString(0) << Q3CString("") << Q3CString("") << Q3CString(0);
    // ### how should the one below behave in Q3CString????
//    QTest::newRow( "rem18" ) << Q3CString("") << Q3CString(0) << Q3CString("A") << Q3CString("A");
    QTest::newRow( "rem19" ) << Q3CString("") << Q3CString(0) << Q3CString("") << Q3CString("");

    QTest::newRow( "rep00" ) << Q3CString("ABC") << Q3CString("B") << Q3CString("-") << Q3CString("A-C");
    QTest::newRow( "rep01" ) << Q3CString("$()*+.?[\\]^{|}") << Q3CString("$()*+.?[\\]^{|}") << Q3CString("X") << Q3CString("X");
    QTest::newRow( "rep02" ) << Q3CString("ABCDEF") << Q3CString("") << Q3CString("X") << Q3CString("XAXBXCXDXEXFX");
    QTest::newRow( "rep03" ) << Q3CString("") << Q3CString("") << Q3CString("X") << Q3CString("X");
}

void tst_Q3CString::remove_regexp_data()
{
    QTest::addColumn<Q3CString>("string");
    QTest::addColumn<Q3CString>("regexp");
    QTest::addColumn<Q3CString>("after");
    QTest::addColumn<Q3CString>("result");

    QTest::newRow( "rem00" ) << Q3CString("alpha") << Q3CString("a+") << Q3CString("") << Q3CString("lph");
    QTest::newRow( "rem01" ) << Q3CString("banana") << Q3CString("^.a") << Q3CString("") << Q3CString("nana");
    QTest::newRow( "rem02" ) << Q3CString("") << Q3CString("^.a") << Q3CString("") << Q3CString("");
    QTest::newRow( "rem03" ) << Q3CString("") << Q3CString("^.a") << Q3CString(0) << Q3CString("");
    QTest::newRow( "rem04" ) << Q3CString(0) << Q3CString("^.a") << Q3CString("") << Q3CString(0);
    QTest::newRow( "rem05" ) << Q3CString(0) << Q3CString("^.a") << Q3CString(0) << Q3CString(0);

    QTest::newRow( "rep00" ) << Q3CString("A <i>bon mot</i>.") << Q3CString("<i>([^<]*)</i>") << Q3CString("\\emph{\\1}") << Q3CString("A \\emph{bon mot}.");
    QTest::newRow( "rep01" ) << Q3CString("banana") << Q3CString("^.a()") << Q3CString("\\1") << Q3CString("nana");
    QTest::newRow( "rep02" ) << Q3CString("banana") << Q3CString("(ba)") << Q3CString("\\1X\\1") << Q3CString("baXbanana");
    QTest::newRow( "rep03" ) << Q3CString("banana") << Q3CString("(ba)(na)na") << Q3CString("\\2X\\1") << Q3CString("naXba");
}

void tst_Q3CString::length()
{
    QFETCH( Q3CString, s1 );
    QTEST( (int)s1.length(), "res" );
}

#include <qfile.h>

void tst_Q3CString::acc_01()
{
    Q3CString a;
    Q3CString b; //b(10);
    Q3CString bb; //bb((int)0);
    Q3CString c("String C");
    char tmp[10];
    tmp[0] = 'S';
    tmp[1] = 't';
    tmp[2] = 'r';
    tmp[3] = 'i';
    tmp[4] = 'n';
    tmp[5] = 'g';
    tmp[6] = ' ';
    tmp[7] = 'D';
    tmp[8] = 'X';
    tmp[9] = '\0';
    Q3CString d(tmp,8);
    Q3CString ca(a);
    Q3CString cb(b);
    Q3CString cc(c);
    Q3CString n;
    Q3CString e("String E");
    Q3CString f;
    f = e;
    f.detach();
    f[7]='F';
    QCOMPARE(e,(Q3CString)"String E");
    char text[]="String f";
    f = text;
    text[7]='!';
    QCOMPARE(f,(Q3CString)"String f");
    f[7]='F';
    QCOMPARE(text[7],'!');

#if 0
    a="";
    a[0]='A';
    Q3CString res = "A";
    QCOMPARE(a,res);
    QCOMPARE(a.length(),(uint)1);
    compare(a.length(),(uint)1);
    a[1]='B';
    QCOMPARE(a,(Q3CString)"AB");
    QCOMPARE(a.length(),(uint)2);
    a[2]='C';
    QCOMPARE(a,(Q3CString)"ABC");
    QCOMPARE(a.length(),(uint)3);
    a = Q3CString();
    QVERIFY(a.isNull());
    a[0]='A';
    QCOMPARE(a,(Q3CString)"A");
    QCOMPARE(a.length(),(uint)1);
    a[1]='B';
    QCOMPARE(a,(Q3CString)"AB");
    QCOMPARE(a.length(),(uint)2);
    a[2]='C';
    QCOMPARE(a,(Q3CString)"ABC");
    QCOMPARE(a.length(),(uint)3);
#endif
    a="123";
    b="456";
    a[0]=a[1];
    QCOMPARE(a,(Q3CString)"223");
    a[1]=b[1];
    QCOMPARE(b,(Q3CString)"456");
    QCOMPARE(a,(Q3CString)"253");

    char t[]="TEXT";
    a="A";
    a=t;
    QCOMPARE(a,(Q3CString)"TEXT");
    QCOMPARE(a,(Q3CString)t);
    a[0]='X';
    QCOMPARE(a,(Q3CString)"XEXT");
    QCOMPARE(t[0],'T');
    t[0]='Z';
    QCOMPARE(a,(Q3CString)"XEXT");

    a="ABC";
    QCOMPARE(((const char*)a)[1],'B');
    QCOMPARE(strcmp(a,(Q3CString)"ABC"),0);
    a += "DEF";
    QCOMPARE(a, (Q3CString)"ABCDEF");
    a += 'G';
    QCOMPARE(a, (Q3CString)"ABCDEFG");
    a += ((const char*)(0));
    QCOMPARE(a, (Q3CString)"ABCDEFG");

    // non-member operators

    a="ABC";
    b="ABC";
    c="ACB";
    d="ABCD";
    QVERIFY(a==b);
    QVERIFY(!(a==d));
    QVERIFY(!(a!=b));
    QVERIFY(a!=d);
    QVERIFY(!(a<b));
    QVERIFY(a<c);
    QVERIFY(a<d);
    QVERIFY(!(d<a));
    QVERIFY(!(c<a));
    QVERIFY(a<=b);
    QVERIFY(a<=d);
    QVERIFY(a<=c);
    QVERIFY(!(c<=a));
    QVERIFY(!(d<=a));
    QCOMPARE(a+b,(Q3CString)"ABCABC");
    QCOMPARE(a	+"XXXX",(Q3CString)"ABCXXXX");
    QCOMPARE(a+'X',(Q3CString)"ABCX");
    QCOMPARE("XXXX"+a,(Q3CString)"XXXXABC");
    QCOMPARE('X'+a,(Q3CString)"XABC");
    a = (const char*)0;
    QVERIFY(a.isNull());
    QVERIFY(*((const char *)a) == 0);

    {
	QFile f("COMPARE.txt");
	f.open( QIODevice::WriteOnly );
	QTextStream ts( &f );
	ts.setEncoding(QTextStream::Unicode);
	ts << "Abc";
    }
}

void tst_Q3CString::isNull()
{
    Q3CString a;
    QVERIFY( a.isNull() );

    const char *str = "foo";
    a.sprintf( str );
    QVERIFY( !a.isNull() );
}

void tst_Q3CString::isEmpty()
{
    Q3CString a;
    QVERIFY(a.isEmpty());
    Q3CString c("Not empty");
    QVERIFY(!c.isEmpty());
}

void tst_Q3CString::constructor()
{
    Q3CString a;
    Q3CString b; //b(10);
    Q3CString c("String C");
    char tmp[10];
    tmp[0] = 'S';
    tmp[1] = 't';
    tmp[2] = 'r';
    tmp[3] = 'i';
    tmp[4] = 'n';
    tmp[5] = 'g';
    tmp[6] = ' ';
    tmp[7] = 'D';
    tmp[8] = 'X';
    tmp[9] = '\0';
    Q3CString d(tmp,9);
    Q3CString ca(a);
    Q3CString cb(b);
    Q3CString cc(c);

    QCOMPARE(a,ca);
    QVERIFY(a.isNull());
    QVERIFY(a == Q3CString(""));
    QCOMPARE(b,cb);
    QCOMPARE(c,cc);
    QCOMPARE(d,(Q3CString)"String D");

    Q3CString null(0);
    QVERIFY( null.isNull() );
    QVERIFY( null.isEmpty() );
    Q3CString empty("");
    QVERIFY( !empty.isNull() );
    QVERIFY( empty.isEmpty() );
}

void tst_Q3CString::truncate()
{
    Q3CString e("String E");
    e.truncate(4);
    QCOMPARE(e,(Q3CString)"Stri");

    e = "String E";
    e.truncate(0);
    QCOMPARE(e,(Q3CString)"");
    QVERIFY(e.isEmpty());
    QVERIFY(!e.isNull());

}

void tst_Q3CString::fill()
{
    Q3CString e;
    e.fill('e',1);
    QCOMPARE(e,(Q3CString)"e");
    Q3CString f;
    f.fill('f',3);
    QCOMPARE(f,(Q3CString)"fff");
    f.fill('F');
    QCOMPARE(f,(Q3CString)"FFF");
}

void tst_Q3CString::copy()
{
    Q3CString e;
    e = "String E";
    Q3CString ce = e.copy();
    QCOMPARE(ce,(Q3CString)"String E");
    e = "XXX";
    QCOMPARE(ce,(Q3CString)"String E");
    QCOMPARE(e,(Q3CString)"XXX");
}

void tst_Q3CString::sprintf()
{
    Q3CString a;
    a.sprintf("COMPARE");
    QCOMPARE(a,(Q3CString)"COMPARE");
    a.sprintf("%%%d",1);
    QCOMPARE(a,(Q3CString)"%1");
    QCOMPARE(a.sprintf("X%dY",2),(Q3CString)"X2Y");
    //QCOMPARE(a.sprintf("X%sY",(Q3CString)"hello"),"XhelloY");
    // QCOMPARE(a.sprintf("X%9sY","hello"),(Q3CString)"X    helloY");
    QCOMPARE(a.sprintf("X%9iY", 50000 ),(Q3CString)"X    50000Y");
    QCOMPARE(a.sprintf("X%-9sY","hello"),(Q3CString)"Xhello    Y");
    QCOMPARE(a.sprintf("X%-9iY", 50000 ),(Q3CString)"X50000    Y");
    //Q3CString fmt("X%-10SY");
    //Q3CString txt("hello");
    //QCOMPARE(a.sprintf(fmt,(Q3CString)&txt),"X     helloY");
}

void tst_Q3CString::find()
{
    Q3CString a;
    a="";
    QCOMPARE(a.find('A'),-1);
    a=Q3CString();
    QCOMPARE(a.find('A'),-1);
    a="ABCDEFGHIEfGEFG"; // 15 chars
    QCOMPARE(a.find('A'),0);
    QCOMPARE(a.find('C'),2);
    QCOMPARE(a.find('Z'),-1);
    QCOMPARE(a.find('E'),4);
    QCOMPARE(a.find('E',4),4);
    QCOMPARE(a.find('E',5),9);
    //QCOMPARE(a.find('G',-1),14);    // -ve does what?  Parameter should be uint?
    //QCOMPARE(a.find('G',-2),11);    // -ve does what?  Parameter should be uint?
    QCOMPARE(a.find('f'),10);
//  QCOMPARE(a.find("efg",-1,FALSE),12);    // -ve does what?  Parameter should be uint?
//  QCOMPARE(a.find("efg",-2,FALSE),12);    // -ve does what?  Parameter should be uint?
//  QCOMPARE(a.find("efg",-3,FALSE),12);    // -ve does what?  Parameter should be uint?
//  QCOMPARE(a.find("efg",-4,FALSE),9);    // -ve does what?  Parameter should be uint?
//  QCOMPARE(a.find(QRegExp("[EFG][EFG]"),0),4);
//  QCOMPARE(a.find(QRegExp("[EFG][EFG]"),4),4);
//  QCOMPARE(a.find(QRegExp("[EFG][EFG]"),5),5);
//  QCOMPARE(a.find(QRegExp("[EFG][EFG]"),6),11);
//  QCOMPARE(a.find(QRegExp("G"),14),14);
}

void tst_Q3CString::findRev()
{
    Q3CString a;
    a="ABCDEFGHIEfGEFG"; // 15 chars
    QCOMPARE(a.findRev('G'),14);
//     QCOMPARE(a.findRev('G',-3),11);
//     QCOMPARE(a.findRev('G',-5),6);
    QCOMPARE(a.findRev('G',14),14);
    QCOMPARE(a.findRev('G',13),11);
    QCOMPARE(a.findRev('B'),1);
    QCOMPARE(a.findRev('B',1),1);
    QCOMPARE(a.findRev('B',0),-1);
//    QCOMPARE(a.findRev(QRegExp("[EFG][EFG]"),14),13);
//    QCOMPARE(a.findRev(QRegExp("[EFG][EFG]"),11),11);
}

void tst_Q3CString::contains()
{
    Q3CString a;
    a="ABCDEFGHIEfGEFG"; // 15 chars
    QVERIFY(a.contains('A'));
    QVERIFY(!a.contains('Z'));
    QVERIFY(a.contains('E'));
    QVERIFY(a.contains('F'));
    QVERIFY(a.contains("FG"));
    QCOMPARE(a.count('A'),1);
    QCOMPARE(a.count('Z'),0);
    QCOMPARE(a.count('E'),3);
    QCOMPARE(a.count('F'),2);
    QCOMPARE(a.count("FG"),2);
//    QCOMPARE(a.contains(QRegExp("[FG][HI]")),1);
//    QCOMPARE(a.contains(QRegExp("[G][HE]")),2);
}

void tst_Q3CString::left()
{
    Q3CString a;
    a="ABCDEFGHIEfGEFG"; // 15 chars
    QCOMPARE(a.left(3),(Q3CString)"ABC");
    QVERIFY(!a.left(0).isNull());
    QCOMPARE(a.left(0),(Q3CString)"");

    Q3CString n;
    QVERIFY(n.left(3).isNull());
    QVERIFY(n.left(0).isNull());
    QVERIFY(n.left(0).isNull());
}

void tst_Q3CString::right()
{
    Q3CString a;
    a="ABCDEFGHIEfGEFG"; // 15 chars
    QCOMPARE(a.right(3),(Q3CString)"EFG");
    QCOMPARE(a.right(0),(Q3CString)"");

    Q3CString n;
    QVERIFY(n.right(3).isNull());
    QVERIFY(n.right(0).isNull());
}

void tst_Q3CString::mid()
{
    Q3CString a;
    a="ABCDEFGHIEfGEFG"; // 15 chars

    QCOMPARE(a.mid(3,3),(Q3CString)"DEF");
    QCOMPARE(a.mid(0,0),(Q3CString)"");
    QVERIFY(a.mid(9999).isNull());
    QVERIFY(a.mid(9999,1).isNull());

    Q3CString n;
    QVERIFY(n.mid(3,3).isNull());
    QVERIFY(n.mid(0,0).isNull());
    QVERIFY(n.mid(9999,0).isNull());
    QVERIFY(n.mid(9999,1).isNull());
}

void tst_Q3CString::leftJustify()
{
    Q3CString a;
    a="ABC";
    QCOMPARE(a.leftJustify(5,'-'),(Q3CString)"ABC--");
    QCOMPARE(a.leftJustify(4,'-'),(Q3CString)"ABC-");
    QCOMPARE(a.leftJustify(4),(Q3CString)"ABC ");
    QCOMPARE(a.leftJustify(3),(Q3CString)"ABC");
    QCOMPARE(a.leftJustify(2),(Q3CString)"ABC");
    QCOMPARE(a.leftJustify(1),(Q3CString)"ABC");
    QCOMPARE(a.leftJustify(0),(Q3CString)"ABC");

    Q3CString n;
    QVERIFY(!n.leftJustify(3).isNull());    // I expected TRUE
    QCOMPARE(a.leftJustify(4,' ',TRUE),(Q3CString)"ABC ");
    QCOMPARE(a.leftJustify(3,' ',TRUE),(Q3CString)"ABC");
    QCOMPARE(a.leftJustify(2,' ',TRUE),(Q3CString)"AB");
    QCOMPARE(a.leftJustify(1,' ',TRUE),(Q3CString)"A");
    QCOMPARE(a.leftJustify(0,' ',TRUE),(Q3CString)"");
}

void tst_Q3CString::rightJustify()
{
    Q3CString a;
    a="ABC";
    QCOMPARE(a.rightJustify(5,'-'),(Q3CString)"--ABC");
    QCOMPARE(a.rightJustify(4,'-'),(Q3CString)"-ABC");
    QCOMPARE(a.rightJustify(4),(Q3CString)" ABC");
    QCOMPARE(a.rightJustify(3),(Q3CString)"ABC");
    QCOMPARE(a.rightJustify(2),(Q3CString)"ABC");
    QCOMPARE(a.rightJustify(1),(Q3CString)"ABC");
    QCOMPARE(a.rightJustify(0),(Q3CString)"ABC");

    Q3CString n;
    QVERIFY(!n.rightJustify(3).isNull());  // I expected TRUE
    QCOMPARE(a.rightJustify(4,'-',TRUE),(Q3CString)"-ABC");
    QCOMPARE(a.rightJustify(4,' ',TRUE),(Q3CString)" ABC");
    QCOMPARE(a.rightJustify(3,' ',TRUE),(Q3CString)"ABC");
    QCOMPARE(a.rightJustify(2,' ',TRUE),(Q3CString)"AB");
    QCOMPARE(a.rightJustify(1,' ',TRUE),(Q3CString)"A");
    QCOMPARE(a.rightJustify(0,' ',TRUE),(Q3CString)"");
    QCOMPARE(a,(Q3CString)"ABC");
}

void tst_Q3CString::upper()
{
    Q3CString a;
    a="Text";
    QCOMPARE(a.upper(),(Q3CString)"TEXT");
}

void tst_Q3CString::lower()
{
    Q3CString a;
    a="Text";
    QCOMPARE(a.lower(),(Q3CString)"text");
}

void tst_Q3CString::stripWhiteSpace()
{
    Q3CString a;
    a="Text";
    QCOMPARE(a,(Q3CString)"Text");
    QCOMPARE(a.stripWhiteSpace(),(Q3CString)"Text");
    QCOMPARE(a,(Q3CString)"Text");
    a=" ";
    QCOMPARE(a.stripWhiteSpace(),(Q3CString)"");
    QCOMPARE(a,(Q3CString)" ");
    a=" a   ";
    QCOMPARE(a.stripWhiteSpace(),(Q3CString)"a");
}

void tst_Q3CString::simplifyWhiteSpace()
{
    Q3CString j;
    j.simplifyWhiteSpace();

    Q3CString a;
    a = "a ";
    QCOMPARE(a.simplifyWhiteSpace(),(Q3CString)"a");
    a=" a   b ";
    QCOMPARE(a.simplifyWhiteSpace(),(Q3CString)"a b");
}

void tst_Q3CString::insert()
{
    Q3CString a;
    a = "Ys";
    QCOMPARE(a.insert(1,'e'),(Q3CString)"Yes");
    QCOMPARE(a.insert(3,'!'),(Q3CString)"Yes!");
    QCOMPARE(a.insert(5,'?'),(Q3CString)"Yes! ?");

    a="ABC";
    QCOMPARE(a.insert(5,"DEF"),(Q3CString)"ABC  DEF");
    a="ABC";
    QCOMPARE(a.insert(0,"ABC"),(Q3CString)"ABCABC");
    QCOMPARE(a,(Q3CString)"ABCABC");

    // ######### Q3CString::insert is not safe against self insertion...
//     Q3CString res = "ABCABCABCABC";
//     QCOMPARE(a.insert(0,a),res);
    a += "ABCABC";
    Q3CString res = "ABCABCABCABC";
    QCOMPARE(a, res);
    res = "<ABCABCABCABC";
    QCOMPARE(a.insert(0,'<'),res );
    res = "<>ABCABCABCABC";
    QCOMPARE(a.insert(1,'>'),res );
}

void tst_Q3CString::append()
{
    Q3CString a;
    a = "<>ABCABCABCABC";
    QCOMPARE(a.append(">"),(Q3CString)"<>ABCABCABCABC>");
}

void tst_Q3CString::prepend()
{
    Q3CString a;
    a = "<>ABCABCABCABC>";
    QCOMPARE(a.prepend("-"),(Q3CString)"-<>ABCABCABCABC>");
}

void tst_Q3CString::replace_uint_uint()
{
    QFETCH( Q3CString, string );
    QFETCH( int, index );
    QFETCH( int, len );
    QFETCH( Q3CString, after );

    Q3CString s1 = string;
    s1.replace( (uint) index, (int) len, after );
    QFETCH( Q3CString, result );
    QVERIFY( s1 == result );

    Q3CString s2 = string;
    s2.replace( (uint) index, (uint) len, after );
    QVERIFY( s2 == result );

}

void tst_Q3CString::replace_string()
{
    QFETCH( Q3CString, string );
    QFETCH( Q3CString, before );
    QFETCH( Q3CString, after );
    QFETCH( Q3CString, result );

    if ( before.length() == 1 ) {
	char ch = before[0];

	Q3CString s1 = string;
	s1.replace( ch, after );
	QCOMPARE( s1, result );
    }

    Q3CString s3 = string;
    s3.replace( before, after );
    QCOMPARE( s3, result );
}

void tst_Q3CString::remove_uint_uint()
{
    QFETCH( Q3CString, string );
    QFETCH( int, index );
    QFETCH( int, len );
    QFETCH( Q3CString, after );
    QFETCH( Q3CString, result );

    if ( after.length() == 0 ) {
	Q3CString s1 = string;
	s1.remove( (uint) index, (uint) len );
	QVERIFY( s1 == result );
    } else {
	QSKIP("Test data applies only to replace_uint_uint(), not remove_uint_uint()", SkipSingle);
    }
}

void tst_Q3CString::check_QDataStream()
{
    Q3CString a;
    QByteArray ar;
    {
	QDataStream out(&ar,QIODevice::WriteOnly);
	out << Q3CString("COMPARE Text");
    }
    {
        QDataStream in(&ar,QIODevice::ReadOnly);
        in >> a;
        QCOMPARE(a,(Q3CString)"COMPARE Text");
    }
}

void tst_Q3CString::check_QTextStream()
{
    Q3CString a;
    QByteArray ar;
    {
	QTextStream out(&ar,QIODevice::WriteOnly);
	out << Q3CString("This is COMPARE Text");
    }
    {
	QTextStream in(&ar,QIODevice::ReadOnly);
	in >> a;
	QCOMPARE(a,(Q3CString)"This");
    }
}

void tst_Q3CString::setExpand()
{
    Q3CString a;
    a="ABC";
    a.setExpand(0,'X');
    QCOMPARE(a,(Q3CString)"XBC");
    a.setExpand(4,'Z');
    QCOMPARE(a,(Q3CString)"XBC Z");
    a.setExpand(3,'Y');
    QCOMPARE(a,(Q3CString)"XBCYZ");
}


QTEST_APPLESS_MAIN(tst_Q3CString)
#include "tst_q3cstring.moc"
