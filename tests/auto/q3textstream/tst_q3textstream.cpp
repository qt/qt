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


#include <QtCore/QtCore>
#include <QtTest/QtTest>
#include <Qt3Support/Q3TextStream>

Q_DECLARE_METATYPE(Q3CString)

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3TextStream : public QObject
{
    Q_OBJECT

public:
    tst_Q3TextStream();
    virtual ~tst_Q3TextStream();

public slots:
    void init();
    void cleanup();

private slots:
    void operator_shiftleft_data();
    void operator_shiftleft();
    void operator_shiftright_data();
    void operator_shiftright();

    void operator_shift_QChar_data();
    void operator_shift_QChar();
    void operator_shift_char_data();
    void operator_shift_char();
    void operator_shift_short_data();
    void operator_shift_short();
    void operator_shift_ushort_data();
    void operator_shift_ushort();
    void operator_shift_int_data();
    void operator_shift_int();
    void operator_shift_uint_data();
    void operator_shift_uint();
    void operator_shift_long_data();
    void operator_shift_long();
    void operator_shift_ulong_data();
    void operator_shift_ulong();
    void operator_shift_float_data();
    void operator_shift_float();
    void operator_shift_double_data();
    void operator_shift_double();
    void operator_shift_QString_data();
    void operator_shift_QString();
    void operator_shift_Q3CString_data();
    void operator_shift_Q3CString();

    void QTextCodecCodecForIndex() const;

    void atEnd();
    void eof();
    void precision();
    
    void task28319();

private:
    void createWriteStream( Q3TextStream *&os );
    void closeWriteStream( Q3TextStream *os );
    void createReadStream( Q3TextStream *&is );
    void closeReadStream( Q3TextStream *is );

    void read_QChar( Q3TextStream *s );
    void write_QChar( Q3TextStream *s );
    void read_char( Q3TextStream *s );
    void write_char( Q3TextStream *s );
    void read_short( Q3TextStream *s );
    void write_short( Q3TextStream *s );
    void read_ushort( Q3TextStream *s );
    void write_ushort( Q3TextStream *s );
    void read_int( Q3TextStream *s );
    void write_int( Q3TextStream *s );
    void read_uint( Q3TextStream *s );
    void write_uint( Q3TextStream *s );
    void read_long( Q3TextStream *s );
    void write_long( Q3TextStream *s );
    void read_ulong( Q3TextStream *s );
    void write_ulong( Q3TextStream *s );
    void read_float( Q3TextStream *s );
    void write_float( Q3TextStream *s );
    void read_double( Q3TextStream *s );
    void write_double( Q3TextStream *s );
    void read_QString( Q3TextStream *s );
    void write_QString( Q3TextStream *s );
    void read_Q3CString( Q3TextStream *s );
    void write_Q3CString( Q3TextStream *s );

    void operatorShift_data();

    void do_shiftleft( Q3TextStream *ts );
    Q3TextStream::Encoding toEncoding( const QString& );
    QString decodeString( QByteArray array, const QString& encoding );

    Q3TextStream *os;
    Q3TextStream *is;
    Q3TextStream *ts;
    QFile *inFile;
    QFile *outFile;
    QByteArray *inArray;
    QBuffer *inBuffer;
    QString *inString;
    bool file_is_empty;
};

tst_Q3TextStream::tst_Q3TextStream()
{
    ts = 0;
    os = 0;
    is = 0;
    outFile = 0;
    inFile = 0;
    inArray = 0;
    inBuffer = 0;
    inString = 0;
    file_is_empty = FALSE;
}

tst_Q3TextStream::~tst_Q3TextStream()
{
}

void tst_Q3TextStream::init()
{
    ts = 0;
    os = 0;
    is = 0;
    inFile = 0;
    outFile = 0;
    inArray = 0;
    inBuffer = 0;
    inString = 0;
    file_is_empty = FALSE;
}

void tst_Q3TextStream::cleanup()
{
    delete ts;
    ts = 0;
    delete os;
    os = 0;
    delete is;
    is = 0;
    delete inFile;
    inFile = 0;
    delete outFile;
    outFile = 0;
    delete inArray;
    inArray = 0;
    delete inBuffer;
    inBuffer = 0;
    delete inString;
    inString = 0;
}

void tst_Q3TextStream::operator_shiftright_data()
{
    operatorShift_data();
}

void tst_Q3TextStream::operator_shiftleft_data()
{
    operatorShift_data();
}

static const double doubleDummy = 567.89;
static const int intDummy = 1234;
static const QString stringDummy = "ABCD";

static const char * const devices[] = {
    "file",
    "bytearray",
    "buffer",
    "string",
    0
};

static const char * const encodings[] = {
    "Locale",
    "Latin1",
    "Unicode",
    "UnicodeNetworkOrder",
    "UnicodeReverse",
    "RawUnicode",
    "UnicodeUTF8",
    0
};

void tst_Q3TextStream::operatorShift_data()
{
    QTest::addColumn<QString>( "device" );
    QTest::addColumn<QString>( "encoding" );
    QTest::addColumn<QString>( "type" );
    QTest::addColumn<double>( "doubleVal" );
    QTest::addColumn<int>( "intVal" );
    QTest::addColumn<QString>( "stringVal" );
    QTest::addColumn<QByteArray>( "encoded" );

    for ( int i=0; devices[i] != 0; i++ ) {
	QString dev = devices[i];

	/*
	  We first test each type at least once.
	*/
	QTest::newRow( dev + "0" ) << dev << QString("UnicodeUTF8") << QString("QChar")
		<< doubleDummy << (int) 'Z' << stringDummy
		<< QByteArray( Q3CString("Z") );
	QTest::newRow( dev + "1" ) << dev << QString("UnicodeUTF8") << QString("char")
		<< doubleDummy << (int) 'Z' << stringDummy
		<< QByteArray( Q3CString("Z") );
	QTest::newRow( dev + "2" ) << dev << QString("UnicodeUTF8") << QString("signed short")
		<< doubleDummy << 12345 << stringDummy
		<< QByteArray( Q3CString("12345") );
	QTest::newRow( dev + "3" ) << dev << QString("UnicodeUTF8") << QString("unsigned short")
		<< doubleDummy << 12345 << stringDummy
		<< QByteArray( Q3CString("12345") );
	QTest::newRow( dev + "4" ) << dev << QString("UnicodeUTF8") << QString("signed int")
		<< doubleDummy << 12345 << stringDummy
		<< QByteArray( Q3CString("12345") );
	QTest::newRow( dev + "5" ) << dev << QString("UnicodeUTF8") << QString("unsigned int")
		<< doubleDummy << 12345 << stringDummy
		<< QByteArray( Q3CString("12345") );
	QTest::newRow( dev + "6" ) << dev << QString("UnicodeUTF8") << QString("signed long")
		<< doubleDummy << 12345 << stringDummy
		<< QByteArray( Q3CString("12345") );
	QTest::newRow( dev + "7" ) << dev << QString("UnicodeUTF8") << QString("unsigned long")
		<< doubleDummy << 12345 << stringDummy
		<< QByteArray( Q3CString("12345") );
	QTest::newRow( dev + "8" ) << dev << QString("UnicodeUTF8") << QString("float")
		<< (double)3.1415f << intDummy << stringDummy
		<< QByteArray( Q3CString("3.1415") );
	QTest::newRow( dev + "9" ) << dev << QString("UnicodeUTF8") << QString("double")
		<< 3.1415 << intDummy << stringDummy
		<< QByteArray( Q3CString("3.1415") );
	QTest::newRow( dev + "10" ) << dev << QString("UnicodeUTF8") << QString("char *")
		<< doubleDummy << intDummy << QString("I-am-a-string")
		<< QByteArray( Q3CString("I-am-a-string") );
	QTest::newRow( dev + "11" ) << dev << QString("UnicodeUTF8") << QString("QString")
		<< doubleDummy << intDummy << QString("I-am-a-string")
		<< QByteArray( Q3CString("I-am-a-string") );
	QTest::newRow( dev + "12" ) << dev << QString("UnicodeUTF8") << QString("Q3CString")
		<< doubleDummy << intDummy << QString("I-am-a-string")
		<< QByteArray( Q3CString("I-am-a-string") );

	/*
	  Then we test some special cases that have caused problems in the past.
	*/
	QTest::newRow( dev + "20" ) << dev << QString("UnicodeUTF8") << QString("QChar")
		<< doubleDummy << 0xff8c << stringDummy
		<< QByteArray( Q3CString("\xef\xbe\x8c") );
	QTest::newRow( dev + "21" ) << dev << QString("UnicodeUTF8") << QString("QChar")
		<< doubleDummy << 0x8cff << stringDummy
		<< QByteArray( Q3CString("\xe8\xb3\xbf") );
    }
}

void tst_Q3TextStream::do_shiftleft( Q3TextStream *ts )
{
    QFETCH( QString, encoding );
    QFETCH( QString, type );
    QFETCH( double,  doubleVal );
    QFETCH( int, intVal );
    QFETCH( QString,  stringVal );

    ts->setEncoding( toEncoding(encoding) );

    if ( type == "QChar" ) {
	if ( intVal >= 0 && intVal <= 0xffff )
	    *ts << QChar( intVal );
    } else if ( type == "char" ) {
	*ts << (char) intVal;
    } else if ( type == "signed short" ) {
	*ts << (signed short) intVal;
    } else if ( type == "unsigned short" ) {
	*ts << (unsigned short) intVal;
    } else if ( type == "signed int" ) {
	*ts << (signed int) intVal;
    } else if ( type == "unsigned int" ) {
	*ts << (unsigned int) intVal;
    } else if ( type == "signed long" ) {
	*ts << (signed long) intVal;
    } else if ( type == "unsigned long" ) {
	*ts << (unsigned long) intVal;
    } else if ( type == "float" ) {
	*ts << (float) doubleVal;
    } else if ( type == "double" ) {
	*ts << (double) doubleVal;
    } else if ( type == "char *" ) {
	*ts << stringVal.latin1();
    } else if ( type == "QString" ) {
	*ts << stringVal;
    } else if ( type == "Q3CString" ) {
	*ts << Q3CString( stringVal.latin1() );
    } else {
	qWarning( "Unknown type: %s" + type );
    }
}

void tst_Q3TextStream::operator_shiftleft()
{
    QFETCH( QString, device );
    QFETCH( QString, encoding );
    QFETCH( QByteArray, encoded );

    if ( device == "file" ) {
	QFile outFile( "qtextstream.out" );
	QVERIFY( outFile.open( IO_WriteOnly ) );
	Q3TextStream ts( &outFile );

	do_shiftleft( &ts );
	outFile.close();

	QFile inFile( "qtextstream.out" );
	QVERIFY( inFile.open( IO_ReadOnly ) );
	QCOMPARE( inFile.readAll(), encoded );
    } else if ( device == "bytearray" ) {
	QByteArray outArray;
	Q3TextStream ts( outArray, IO_WriteOnly );

	do_shiftleft( &ts );

	QCOMPARE( outArray, encoded );
    } else if ( device == "buffer" ) {
	QByteArray outArray;
	QBuffer outBuffer;
        outBuffer.setBuffer( &outArray );
	QVERIFY( outBuffer.open(IO_WriteOnly) );
	Q3TextStream ts( &outBuffer );

	do_shiftleft( &ts );

	QCOMPARE( outArray, encoded );
    } else if ( device == "string" ) {
	QString outString;
	Q3TextStream ts( &outString, IO_WriteOnly );

	do_shiftleft( &ts );

	QString decodedString = decodeString( encoded, encoding );
	QCOMPARE( outString, decodedString );
    } else {
	qWarning( "Unknown device type: " + device );
    }
}

void tst_Q3TextStream::operator_shiftright()
{
    QFETCH( QString, device );
    QFETCH( QString, encoding );
    QFETCH( QString, type );
    QFETCH( double,  doubleVal );
    QFETCH( int, intVal );
    QFETCH( QString,  stringVal );
    QFETCH( QByteArray, encoded );

    if ( device == "file" ) {
	QFile outFile( "qtextstream.out" );
	QVERIFY( outFile.open( IO_WriteOnly ) );
	QCOMPARE( (int) outFile.writeBlock(encoded), encoded.size() );
	outFile.close();

	inFile = new QFile( "qtextstream.out" );
	QVERIFY( inFile->open( IO_ReadOnly ) );
	ts = new Q3TextStream( inFile );
    } else if ( device == "bytearray" ) {
	ts = new Q3TextStream( encoded, IO_ReadOnly );
    } else if ( device == "buffer" ) {
	inBuffer = new QBuffer;
        inBuffer->setData( encoded );
	QVERIFY( inBuffer->open(IO_ReadOnly) );
	ts = new Q3TextStream( inBuffer );
    } else if ( device == "string" ) {
	inString = new QString( decodeString(encoded, encoding) );
	ts = new Q3TextStream( inString, IO_ReadOnly );
    } else {
	qWarning( "Unknown device type: " + device );
    }

    ts->setEncoding( toEncoding(encoding) );

    if ( type == "QChar" ) {
	QChar c;
	if ( intVal >= 0 && intVal <= 0xffff )
	    *ts >> c;
	QCOMPARE( c, QChar(intVal) );
    } else if ( type == "char" ) {
	char c;
	*ts >> c;
	QVERIFY( c == (char) intVal );
    } else if ( type == "signed short" ) {
	signed short h;
	*ts >> h;
	QVERIFY( h == (signed short) intVal );
    } else if ( type == "unsigned short" ) {
	unsigned short h;
	*ts >> h;
	QVERIFY( h == (unsigned short) intVal );
    } else if ( type == "signed int" ) {
	signed int i;
	*ts >> i;
	QVERIFY( i == (signed int) intVal );
    } else if ( type == "unsigned int" ) {
	unsigned int i;
	*ts >> i;
	QVERIFY( i == (unsigned int) intVal );
    } else if ( type == "signed long" ) {
	signed long ell;
	*ts >> ell;
	QVERIFY( ell == (signed long) intVal );
    } else if ( type == "unsigned long" ) {
	unsigned long ell;
	*ts >> ell;
	QVERIFY( ell == (unsigned long) intVal );
    } else if ( type == "float" ) {
	float f;
	*ts >> f;
	QVERIFY( f == (float) doubleVal );
    } else if ( type == "double" ) {
	double d;
	*ts >> d;
	QVERIFY( d == (double) doubleVal );
    } else if ( type == "char *" ) {
	char *cp = new char[2048];
	*ts >> cp;
	QVERIFY( qstrcmp(cp, stringVal.latin1()) == 0 );
	delete[] cp;
    } else if ( type == "QString" ) {
	QString s;
	*ts >> s;
	QCOMPARE( s, stringVal );
    } else if ( type == "Q3CString" ) {
	Q3CString s;
	*ts >> s;
	QCOMPARE( QString::fromLatin1(s), stringVal );
    } else {
	qWarning( "Unknown type: %s" + type );
    }
}

Q3TextStream::Encoding tst_Q3TextStream::toEncoding( const QString &str )
{
    if ( str == "Locale" )
	return Q3TextStream::Locale;
    else if ( str == "Latin1" )
	return Q3TextStream::Latin1;
    else if ( str == "Unicode" )
	return Q3TextStream::Unicode;
    else if ( str == "UnicodeNetworkOrder" )
	return Q3TextStream::UnicodeNetworkOrder;
    else if ( str == "UnicodeReverse" )
	return Q3TextStream::UnicodeReverse;
    else if ( str == "RawUnicode" )
	return Q3TextStream::RawUnicode;
    else if ( str == "UnicodeUTF8" )
	return Q3TextStream::UnicodeUTF8;

    qWarning( "No such encoding " + str );
    return Q3TextStream::Latin1;
}

QString tst_Q3TextStream::decodeString( QByteArray array, const QString& encoding )
{
    switch ( toEncoding(encoding) ) {
    case Q3TextStream::Locale:
	return QString::fromLocal8Bit( array.data(), array.size() );
    case Q3TextStream::Latin1:
	return QString::fromLatin1( array.data(), array.size() );
    case Q3TextStream::Unicode:
    case Q3TextStream::UnicodeNetworkOrder:
    case Q3TextStream::UnicodeReverse:
    case Q3TextStream::RawUnicode:
	qWarning( "Unicode not implemented ###" );
	return QString();
    case Q3TextStream::UnicodeUTF8:
	return QString::fromUtf8( array.data(), array.size() );
    default:
	return QString();
    }
}

// ************************************************

void tst_Q3TextStream::createWriteStream( Q3TextStream *&os )
{
    QFETCH( QString, device );

    if ( device == "file" ) {
	outFile = new QFile( "qtextstream.out" );
	QVERIFY( outFile->open( IO_WriteOnly ) );
	os = new Q3TextStream( outFile );
    } else if ( device == "bytearray" ) {
	inArray = new QByteArray;
	os = new Q3TextStream( *inArray, IO_WriteOnly );
    } else if ( device == "buffer" ) {
	inBuffer = new QBuffer;
	QVERIFY( inBuffer->open(IO_WriteOnly) );
	os = new Q3TextStream( inBuffer );
    } else if ( device == "string" ) {
	inString = new QString;
	os = new Q3TextStream( inString, IO_WriteOnly );
    } else {
	qWarning( "Error creating write stream: Unknown device type '" + device + "'" );
    }

    QFETCH( QString, encoding );
    os->setEncoding( toEncoding( encoding ));
}

void tst_Q3TextStream::closeWriteStream( Q3TextStream *os )
{
    QFETCH( QString, device );

    if ( os->device() )
	os->device()->close();
}

void tst_Q3TextStream::createReadStream( Q3TextStream *&is )
{
    QFETCH( QString, device );

    if ( device == "file" ) {
	inFile = new QFile( "qtextstream.out" );
	QVERIFY( inFile->open( IO_ReadOnly ) );
	is = new Q3TextStream( inFile );
    } else if ( device == "bytearray" ) {
	is = new Q3TextStream( *inArray, IO_ReadOnly );
    } else if ( device == "buffer" ) {
	QVERIFY( inBuffer->open(IO_ReadOnly) );
	is = new Q3TextStream( inBuffer );
    } else if ( device == "string" ) {
	is = new Q3TextStream( inString, IO_ReadOnly );
    } else {
	qWarning( "Error creating read stream: Unknown device type '" + device + "'" );
    }

    QFETCH( QString, encoding );
    is->setEncoding( toEncoding( encoding ));

    if (!file_is_empty) {
	QVERIFY( !is->atEnd() );
	QVERIFY( !is->eof() );
    }
}

void tst_Q3TextStream::closeReadStream( Q3TextStream *is )
{
    QVERIFY( is->atEnd() );
    QVERIFY( is->eof() );

    if ( is->device() )
	is->device()->close();
}

void tst_Q3TextStream::precision()
{
}

void tst_Q3TextStream::atEnd()
{
    // atEnd is actually tested in all the operator_shift*functions
    // but it's sufficient to mention one here.
}

void tst_Q3TextStream::eof()
{
    // eof is obsolete, but as long as it's supported it should work.
    // eof is actually tested in all the operator_shift*functions
    // but it's sufficient to mention one here.
}

// **************** QChar ****************

void tst_Q3TextStream::operator_shift_QChar_data()
{
    QTest::addColumn<QString>( "device" );
    QTest::addColumn<QString>( "encoding" );
    QTest::addColumn<ushort>( "qchar" );

    for ( int d=0; devices[d] != 0; d++ ) {
	QString device = devices[d];
	for ( int e=0; encodings[e] != 0; e++ ) {
	    QString encoding = encodings[e];

	    QString tag = device + "_" + encoding + "_";
	    QTest::newRow( tag + "0" ) << device << encoding << QChar( 'A' ).unicode();
	    QTest::newRow( tag + "1" ) << device << encoding << QChar( 'B' ).unicode();
	    QTest::newRow( tag + "2" ) << device << encoding << QChar( 'Z' ).unicode();
	    QTest::newRow( tag + "3" ) << device << encoding << QChar( 'z' ).unicode();
	    QTest::newRow( tag + "4" ) << device << encoding << QChar( '@' ).unicode();
	}
    }
}

void tst_Q3TextStream::operator_shift_QChar()
{
// Uncomment the qDebug and you'll see all the combinations we actually run :-))
//    qDebug( data()->dataTag() );

    createWriteStream( os );
    write_QChar( os );
    closeWriteStream( os );

    createReadStream( is );
    read_QChar( is );
    closeReadStream( is );
}

void tst_Q3TextStream::read_QChar( Q3TextStream *s )
{
    QFETCH( ushort, qchar );
    QChar expected( qchar );
    QChar actual;
    *s >> actual;
    QVERIFY( actual == expected );
}

void tst_Q3TextStream::write_QChar( Q3TextStream *s )
{
    QFETCH( ushort, qchar );
    QChar actual( qchar );
    *s << actual;
}

// **************** char ****************

void tst_Q3TextStream::operator_shift_char_data()
{
    QTest::addColumn<QString>( "device" );
    QTest::addColumn<QString>( "encoding" );
    QTest::addColumn<int>( "ch" );

    for ( int d=0; devices[d] != 0; d++ ) {
	QString device = devices[d];
	for ( int e=0; encodings[e] != 0; e++ ) {
	    QString encoding = encodings[e];

	    QString tag = device + "_" + encoding + "_";
	    QTest::newRow( tag + "0" ) << device << encoding << int('A');
	    QTest::newRow( tag + "1" ) << device << encoding << int('B');
	    QTest::newRow( tag + "2" ) << device << encoding << int('Z');
	    QTest::newRow( tag + "3" ) << device << encoding << int(14);
	    QTest::newRow( tag + "4" ) << device << encoding << int('0');
	}
    }
}

void tst_Q3TextStream::operator_shift_char()
{
    createWriteStream( os );
    write_char( os );
    closeWriteStream( os );

    createReadStream( is );
    read_char( is );
    closeReadStream( is );
}

void tst_Q3TextStream::read_char( Q3TextStream *s )
{
    QFETCH( int, ch );
    char c(ch);
    char exp;
    *s >> exp;
    QCOMPARE( exp, c );
}

void tst_Q3TextStream::write_char( Q3TextStream *s )
{
    QFETCH( int, ch );
    char c(ch);
    *s << c;
}

// **************** short ****************

void tst_Q3TextStream::operator_shift_short_data()
{
    QTest::addColumn<QString>( "device" );
    QTest::addColumn<QString>( "encoding" );
    QTest::addColumn<short>( "ss" );

    for ( int d=0; devices[d] != 0; d++ ) {
	QString device = devices[d];
	for ( int e=0; encodings[e] != 0; e++ ) {
	    QString encoding = encodings[e];

	    QString tag = device + "_" + encoding + "_";
	    QTest::newRow( tag + "0" ) << device << encoding << short(0);
	    QTest::newRow( tag + "1" ) << device << encoding << short(-1);
	    QTest::newRow( tag + "2" ) << device << encoding << short(1);
	    QTest::newRow( tag + "3" ) << device << encoding << short(255);
	    QTest::newRow( tag + "4" ) << device << encoding << short(-254);
	}
    }
}

void tst_Q3TextStream::operator_shift_short()
{
    createWriteStream( os );
    write_short( os );
    closeWriteStream( os );

    createReadStream( is );
    read_short( is );
    closeReadStream( is );
}

void tst_Q3TextStream::read_short( Q3TextStream *s )
{
    QFETCH( short, ss );
    short exp;
    QString A, B;
    *s >> A >> exp >> B;
    QCOMPARE( A, QString("A") );
    QCOMPARE( B, QString("B") );
    QCOMPARE( exp, ss );
    s->skipWhiteSpace();
}

void tst_Q3TextStream::write_short( Q3TextStream *s )
{
    QFETCH( short, ss );
    *s << " A " << ss << " B ";
}

// **************** ushort ****************

void tst_Q3TextStream::operator_shift_ushort_data()
{
    QTest::addColumn<QString>( "device" );
    QTest::addColumn<QString>( "encoding" );
    QTest::addColumn<ushort>( "us" );

    for ( int d=0; devices[d] != 0; d++ ) {
	QString device = devices[d];
	for ( int e=0; encodings[e] != 0; e++ ) {
	    QString encoding = encodings[e];

	    QString tag = device + "_" + encoding + "_";
	    QTest::newRow( tag + "0" ) << device << encoding << ushort(0);
	    QTest::newRow( tag + "1" ) << device << encoding << ushort(1);
	    QTest::newRow( tag + "2" ) << device << encoding << ushort(10);
	    QTest::newRow( tag + "3" ) << device << encoding << ushort(255);
	    QTest::newRow( tag + "4" ) << device << encoding << ushort(512);
	}
    }
}

void tst_Q3TextStream::operator_shift_ushort()
{
    createWriteStream( os );
    write_ushort( os );
    closeWriteStream( os );

    createReadStream( is );
    read_ushort( is );
    closeReadStream( is );
}

void tst_Q3TextStream::read_ushort( Q3TextStream *s )
{
    QFETCH( ushort, us );
    ushort exp;
    QString A, B;
    *s >> A >> exp >> B;
    QCOMPARE( A, QString("A") );
    QCOMPARE( B, QString("B") );
    QCOMPARE( exp, us );
    s->skipWhiteSpace();
}

void tst_Q3TextStream::write_ushort( Q3TextStream *s )
{
    QFETCH( ushort, us );
    *s << " A " << us << " B ";
}

// **************** int ****************

void tst_Q3TextStream::operator_shift_int_data()
{
    QTest::addColumn<QString>( "device" );
    QTest::addColumn<QString>( "encoding" );
    QTest::addColumn<int>( "si" );

    for ( int d=0; devices[d] != 0; d++ ) {
	QString device = devices[d];
	for ( int e=0; encodings[e] != 0; e++ ) {
	    QString encoding = encodings[e];

	    QString tag = device + "_" + encoding + "_";
	    QTest::newRow( tag + "0" ) << device << encoding << int(0);
	    QTest::newRow( tag + "1" ) << device << encoding << int(1);
	    QTest::newRow( tag + "2" ) << device << encoding << int(10);
	    QTest::newRow( tag + "3" ) << device << encoding << int(255);
	    QTest::newRow( tag + "4" ) << device << encoding << int(512);
	    QTest::newRow( tag + "5" ) << device << encoding << int(-1);
	    QTest::newRow( tag + "6" ) << device << encoding << int(-10);
	    QTest::newRow( tag + "7" ) << device << encoding << int(-255);
	    QTest::newRow( tag + "8" ) << device << encoding << int(-512);
	}
    }
}

void tst_Q3TextStream::operator_shift_int()
{
    createWriteStream( os );
    write_int( os );
    closeWriteStream( os );

    createReadStream( is );
    read_int( is );
    closeReadStream( is );
}

void tst_Q3TextStream::read_int( Q3TextStream *s )
{
    QFETCH( int, si );
    int exp;
    QString A, B;
    *s >> A >> exp >> B;
    QCOMPARE( A, QString("A") );
    QCOMPARE( B, QString("B") );
    QCOMPARE( exp, si );
    s->skipWhiteSpace();
}

void tst_Q3TextStream::write_int( Q3TextStream *s )
{
    QFETCH( int, si );
    *s << " A " << si << " B ";
}

// **************** uint ****************

void tst_Q3TextStream::operator_shift_uint_data()
{
    QTest::addColumn<QString>( "device" );
    QTest::addColumn<QString>( "encoding" );
    QTest::addColumn<uint>( "ui" );

    for ( int d=0; devices[d] != 0; d++ ) {
	QString device = devices[d];
	for ( int e=0; encodings[e] != 0; e++ ) {
	    QString encoding = encodings[e];

	    QString tag = device + "_" + encoding + "_";
	    QTest::newRow( tag + "0" ) << device << encoding << uint(0);
	    QTest::newRow( tag + "1" ) << device << encoding << uint(1);
	    QTest::newRow( tag + "2" ) << device << encoding << uint(10);
	    QTest::newRow( tag + "3" ) << device << encoding << uint(255);
	    QTest::newRow( tag + "4" ) << device << encoding << uint(512);
	}
    }
}

void tst_Q3TextStream::operator_shift_uint()
{
    createWriteStream( os );
    write_uint( os );
    closeWriteStream( os );

    createReadStream( is );
    read_uint( is );
    closeReadStream( is );
}

void tst_Q3TextStream::read_uint( Q3TextStream *s )
{
    QFETCH( uint, ui );
    uint exp;
    QString A, B;
    *s >> A >> exp >> B;
    QCOMPARE( A, QString("A") );
    QCOMPARE( B, QString("B") );
    QCOMPARE( exp, ui );
    s->skipWhiteSpace();
}

void tst_Q3TextStream::write_uint( Q3TextStream *s )
{
    QFETCH( uint, ui );
    *s << " A " << ui << " B ";
}

// **************** long ****************

void tst_Q3TextStream::operator_shift_long_data()
{
    QTest::addColumn<QString>( "device" );
    QTest::addColumn<QString>( "encoding" );
    QTest::addColumn<long>( "sl" );

    for ( int d=0; devices[d] != 0; d++ ) {
	QString device = devices[d];
	for ( int e=0; encodings[e] != 0; e++ ) {
	    QString encoding = encodings[e];

	    QString tag = device + "_" + encoding + "_";
	    QTest::newRow( tag + "0" ) << device << encoding << long(0);
	    QTest::newRow( tag + "1" ) << device << encoding << long(1);
	    QTest::newRow( tag + "2" ) << device << encoding << long(10);
	    QTest::newRow( tag + "3" ) << device << encoding << long(255);
	    QTest::newRow( tag + "4" ) << device << encoding << long(65535);
	    QTest::newRow( tag + "5" ) << device << encoding << long(-1);
	    QTest::newRow( tag + "6" ) << device << encoding << long(-10);
	    QTest::newRow( tag + "7" ) << device << encoding << long(-255);
	    QTest::newRow( tag + "8" ) << device << encoding << long(-65534);
	}
    }
}

void tst_Q3TextStream::operator_shift_long()
{
    createWriteStream( os );
    write_long( os );
    closeWriteStream( os );

    createReadStream( is );
    read_long( is );
    closeReadStream( is );
}

void tst_Q3TextStream::read_long( Q3TextStream *s )
{
    QFETCH( long, sl );
    long exp;
    QString A, B;
    *s >> A >> exp >> B;
    QCOMPARE( A, QString("A") );
    QCOMPARE( B, QString("B") );
    QCOMPARE( exp, sl );
    s->skipWhiteSpace();
}

void tst_Q3TextStream::write_long( Q3TextStream *s )
{
    QFETCH( long, sl );
    *s << " A " << sl << " B ";
}

// **************** long ****************

void tst_Q3TextStream::operator_shift_ulong_data()
{
    QTest::addColumn<QString>( "device" );
    QTest::addColumn<QString>( "encoding" );
    QTest::addColumn<ulong>( "ul" );

    for ( int d=0; devices[d] != 0; d++ ) {
	QString device = devices[d];
	for ( int e=0; encodings[e] != 0; e++ ) {
	    QString encoding = encodings[e];

	    QString tag = device + "_" + encoding + "_";
	    QTest::newRow( tag + "0" ) << device << encoding << ulong(0);
	    QTest::newRow( tag + "1" ) << device << encoding << ulong(1);
	    QTest::newRow( tag + "2" ) << device << encoding << ulong(10);
	    QTest::newRow( tag + "3" ) << device << encoding << ulong(255);
	    QTest::newRow( tag + "4" ) << device << encoding << ulong(65535);
	}
    }
}

void tst_Q3TextStream::operator_shift_ulong()
{
    createWriteStream( os );
    write_ulong( os );
    closeWriteStream( os );

    createReadStream( is );
    read_ulong( is );
    closeReadStream( is );
}

void tst_Q3TextStream::read_ulong( Q3TextStream *s )
{
    QFETCH( ulong, ul );
    ulong exp;
    QString A, B;
    *s >> A >> exp >> B;
    QCOMPARE( A, QString("A") );
    QCOMPARE( B, QString("B") );
    QCOMPARE( exp, ul );
    s->skipWhiteSpace();
}

void tst_Q3TextStream::write_ulong( Q3TextStream *s )
{
    QFETCH( ulong, ul );
    *s << " A " << ul << " B ";
}

// **************** float ****************

void tst_Q3TextStream::operator_shift_float_data()
{
    QTest::addColumn<QString>( "device" );
    QTest::addColumn<QString>( "encoding" );
    QTest::addColumn<float>( "f" );

    for ( int d=0; devices[d] != 0; d++ ) {
	QString device = devices[d];
	for ( int e=0; encodings[e] != 0; e++ ) {
	    QString encoding = encodings[e];

	    QString tag = device + "_" + encoding + "_";
	    QTest::newRow( tag + "0" ) << device << encoding << float(0.0);
	    QTest::newRow( tag + "1" ) << device << encoding << float(0.0001);
	    QTest::newRow( tag + "2" ) << device << encoding << float(-0.0001);
	    QTest::newRow( tag + "3" ) << device << encoding << float(3.45678);
	    QTest::newRow( tag + "4" ) << device << encoding << float(-3.45678);
	}
    }
}

void tst_Q3TextStream::operator_shift_float()
{
    createWriteStream( os );
    write_float( os );
    closeWriteStream( os );

    createReadStream( is );
    read_float( is );
    closeReadStream( is );
}

void tst_Q3TextStream::read_float( Q3TextStream *s )
{
    QFETCH( float, f );
    float exp;
    QString A, B;
    *s >> A >> exp >> B;
    QCOMPARE( A, QString("A") );
    QCOMPARE( B, QString("B") );
    QCOMPARE( exp, f );
    s->skipWhiteSpace();
}

void tst_Q3TextStream::write_float( Q3TextStream *s )
{
    QFETCH( float, f );
    *s << " A " << f << " B ";
}

// **************** double ****************

void tst_Q3TextStream::operator_shift_double_data()
{
    QTest::addColumn<QString>( "device" );
    QTest::addColumn<QString>( "encoding" );
    QTest::addColumn<double>( "d" );

    for ( int d=0; devices[d] != 0; d++ ) {
	QString device = devices[d];
	for ( int e=0; encodings[e] != 0; e++ ) {
	    QString encoding = encodings[e];

	    QString tag = device + "_" + encoding + "_";
	    QTest::newRow( tag + "0" ) << device << encoding << double(0.0);
	    QTest::newRow( tag + "1" ) << device << encoding << double(0.0001);
	    QTest::newRow( tag + "2" ) << device << encoding << double(-0.0001);
	    QTest::newRow( tag + "3" ) << device << encoding << double(3.45678);
	    QTest::newRow( tag + "4" ) << device << encoding << double(-3.45678);
	    QTest::newRow( tag + "5" ) << device << encoding << double(1.23456789);
	    QTest::newRow( tag + "6" ) << device << encoding << double(-1.23456789);
	}
    }
}

void tst_Q3TextStream::operator_shift_double()
{
    createWriteStream( os );
    os->precision( 10 );
    write_double( os );
    closeWriteStream( os );

    createReadStream( is );
    is->precision( 10 );
    read_double( is );
    closeReadStream( is );
}

void tst_Q3TextStream::read_double( Q3TextStream *s )
{
    QFETCH( double, d );
    double exp;
    QString A, B;
    *s >> A >> exp >> B;
    QCOMPARE( A, QString("A") );
    QCOMPARE( B, QString("B") );
    QCOMPARE( exp, d );
    s->skipWhiteSpace();
}

void tst_Q3TextStream::write_double( Q3TextStream *s )
{
    QFETCH( double, d );
    *s << " A " << d << " B ";
}

// **************** QString ****************

void tst_Q3TextStream::operator_shift_QString_data()
{
    QTest::addColumn<QString>( "device" );
    QTest::addColumn<QString>( "encoding" );
    QTest::addColumn<QString>( "str" );
    QTest::addColumn<bool>( "multi_str" );
    QTest::addColumn<bool>( "zero_length" );

    for ( int d=0; devices[d] != 0; d++ ) {
	QString device = devices[d];
	for ( int e=0; encodings[e] != 0; e++ ) {
	    QString encoding = encodings[e];

	    QString tag = device + "_" + encoding + "_";
	    QTest::newRow( tag + "0" ) << device << encoding << QString("") << bool(FALSE) << bool(TRUE);
	    QTest::newRow( tag + "1" ) << device << encoding << QString() << bool(FALSE) << bool(TRUE);
	    QTest::newRow( tag + "2" ) << device << encoding << QString("foo") << bool(FALSE) << bool(FALSE);
	    QTest::newRow( tag + "3" ) << device << encoding << QString("foo\nbar") << bool(TRUE) << bool(FALSE);
	    QTest::newRow( tag + "4" ) << device << encoding << QString("cjacka ckha cka ckah ckac kahckadhcbkgdk vkzdfbvajef vkahv") << bool(TRUE) << bool(FALSE);
	}
    }
}

void tst_Q3TextStream::operator_shift_QString()
{
    QFETCH( bool, zero_length );
    file_is_empty = zero_length;

    createWriteStream( os );
    write_QString( os );
    closeWriteStream( os );

    createReadStream( is );
    read_QString( is );
    closeReadStream( is );
}

void tst_Q3TextStream::read_QString( Q3TextStream *s )
{
    QString exp;
    QFETCH( QString, str );
    if (str.isNull())
	str = "";

    QFETCH( bool, multi_str );
    if (!multi_str) {
	*s >> exp;
	QCOMPARE( exp, str );
    } else {
	QStringList l;
	l = QStringList::split( " ", str );
	if (l.count() < 2)
	    l = QStringList::split( "\n", str );
	for (uint i=0; i<uint(l.count()); i++) {
	    *s >> exp;
	    QCOMPARE( exp, l[i] );
	}
    }
}

void tst_Q3TextStream::write_QString( Q3TextStream *s )
{
    QFETCH( QString, str );
    *s << str;
}

// **************** Q3CString ****************

void tst_Q3TextStream::operator_shift_Q3CString_data()
{
    QTest::addColumn<QString>( "device" );
    QTest::addColumn<QString>( "encoding" );
    QTest::addColumn<Q3CString>( "cs" );
    QTest::addColumn<bool>( "multi_str" );
    QTest::addColumn<bool>( "zero_length" );

    for ( int d=0; devices[d] != 0; d++ ) {
	QString device = devices[d];
	for ( int e=0; encodings[e] != 0; e++ ) {
	    QString encoding = encodings[e];

	    QString tag = device + "_" + encoding + "_";
	    QTest::newRow( tag + "0" ) << device << encoding << Q3CString("") << bool(FALSE) << bool(TRUE);
	    QTest::newRow( tag + "1" ) << device << encoding << Q3CString(0) << bool(FALSE) << bool(TRUE);
	    QTest::newRow( tag + "2" ) << device << encoding << Q3CString("foo") << bool(FALSE) << bool(FALSE);
	    QTest::newRow( tag + "3" ) << device << encoding << Q3CString("foo\nbar") << bool(TRUE) << bool(FALSE);
	    QTest::newRow( tag + "4" ) << device << encoding << Q3CString("cjacka ckha cka ckah ckac kahckadhcbkgdk vkzdfbvajef vkahv") << bool(TRUE) << bool(FALSE);
	}
    }
}

void tst_Q3TextStream::operator_shift_Q3CString()
{
    QFETCH( bool, zero_length );
    file_is_empty = zero_length;

    createWriteStream( os );
    write_Q3CString( os );
    closeWriteStream( os );

    createReadStream( is );
    read_Q3CString( is );
    closeReadStream( is );
}

void tst_Q3TextStream::read_Q3CString( Q3TextStream *s )
{
/*
    QFETCH( Q3CString, cs );
    Q3CString exp;
    *s >> exp;
    QCOMPARE( exp, cs );
*/
    Q3CString exp;
    QFETCH( Q3CString, cs );
    if (cs.isNull())
	cs = "";

    QFETCH( bool, multi_str );
    if (!multi_str) {
	*s >> exp;
	QCOMPARE( exp, cs );
    } else {
	QStringList l;
	l = QStringList::split( " ", cs );
	if (l.count() < 2)
	    l = QStringList::split( "\n", cs );
	for (uint i=0; i<uint(l.count()); i++) {
	    *s >> exp;
	    QCOMPARE( exp, Q3CString(l[i].toLatin1()) );
	}
    }
}

void tst_Q3TextStream::write_Q3CString( Q3TextStream *s )
{
    QFETCH( Q3CString, cs );
    *s << cs;
}

void tst_Q3TextStream::task28319()
{
    /*
    // Specific test for task 28319
    QFile f("28319.txt");
    if (f.open(IO_WriteOnly)) {
	// First we write some text
	Q3TextStream writeStream(&f);
	writeStream.setEncoding(Q3TextStream::Unicode);
	writeStream << "This is a";
	f.close();
    } else {
	QVERIFY(FALSE);
    }
    if (f.open(IO_WriteOnly | IO_Append)) {
	// Now we append some text
	Q3TextStream appendStream(&f);
	appendStream.setEncoding(Q3TextStream::Unicode);
	appendStream << " test";
	f.close();
    } else {
	QVERIFY(FALSE);
    }
    if (f.open(IO_ReadOnly)) {
	// Now we read in the text
	Q3TextStream readStream(&f);
	QString text = readStream.read();
	QCOMPARE(text, QString("This is a test"));
    } else {
	QVERIFY(FALSE);
    }
    */
}

/*!
 \since 4.5

 This code worked in Qt 3 and should also in Qt 4. It should not crash due to
 out of bounds.

 */
void tst_Q3TextStream::QTextCodecCodecForIndex() const
{
    for(int i = 0; QTextCodec::codecForIndex(i); i++)
        ;
}

QTEST_MAIN(tst_Q3TextStream)
#include "tst_q3textstream.moc"
