/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qtextcodec.h>
#include <qfile.h>
#include <qtextdocument.h>
#include <time.h>
#include <qprocess.h>

#ifdef Q_OS_SYMBIAN
#define SRCDIR ""
#endif


class tst_QTextCodec : public QObject
{
    Q_OBJECT

private slots:
    void toUnicode_data();
    void toUnicode();
    void codecForName_data();
    void codecForName();
    void fromUnicode_data();
    void fromUnicode();
    void toUnicode_codecForHtml();
    void toUnicode_incremental();
    void codecForLocale();

    void asciiToIscii() const;
    void flagCodepointFFFF() const;
    void flagF7808080() const;
    void flagEFBFBF() const;
    void decode0D() const;
    void codecForIndex() const;
    void aliasForUTF16() const;
    void mibForTSCII() const;

    void utf8Codec_data();
    void utf8Codec();

    void utf8bom_data();
    void utf8bom();

    void utfHeaders_data();
    void utfHeaders();

    void codecForHtml();

    void codecForUtfText_data();
    void codecForUtfText();

#ifdef Q_OS_UNIX
    void toLocal8Bit();
#endif
};

void tst_QTextCodec::toUnicode_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QString>("codecName");

    QTest::newRow( "korean-eucKR" ) << SRCDIR "korean.txt" << "eucKR";
    QTest::newRow( "UTF-8" ) << SRCDIR "utf8.txt" << "UTF-8";
}

void tst_QTextCodec::toUnicode()
{
    QFETCH( QString, fileName );
    QFETCH( QString, codecName );

    QFile file( fileName );

    if ( file.open( QIODevice::ReadOnly ) ) {
        QByteArray ba = file.readAll();
        QTextCodec *c = QTextCodec::codecForName( codecName.toLatin1() );
        QVERIFY(c != 0);
        QString uniString = c->toUnicode( ba );
        if (codecName == QLatin1String("UTF-8")) {
            QCOMPARE(uniString, QString::fromUtf8(ba));
            QCOMPARE(ba, uniString.toUtf8());
        }
        QCOMPARE( ba, c->fromUnicode( uniString ) );

        if (codecName == QLatin1String("eucKR")) {
            char ch = '\0';
            QVERIFY(c->toUnicode(&ch, 1).isEmpty());
            QVERIFY(c->toUnicode(&ch, 1).isNull());
        }
    } else {
        QFAIL(qPrintable("File could not be opened: " + file.errorString()));
    }
}

void tst_QTextCodec::codecForName_data()
{
    QTest::addColumn<QString>("hint");
    QTest::addColumn<QString>("actualCodecName");

    QTest::newRow("data1") << "iso88591" << "ISO-8859-1";
    QTest::newRow("data2") << "iso88592" << "ISO-8859-2";
    QTest::newRow("data3") << " IsO(8)8/5*9-2 " << "ISO-8859-2";
    QTest::newRow("data4") << " IsO(8)8/5*2-9 " << "";
    QTest::newRow("data5") << "latin2" << "ISO-8859-2";
}

void tst_QTextCodec::codecForName()
{
    QFETCH(QString, hint);
    QFETCH(QString, actualCodecName);

    QTextCodec *codec = QTextCodec::codecForName(hint.toLatin1());
    if (actualCodecName.isEmpty()) {
        QVERIFY(codec == 0);
    } else {
        QVERIFY(codec != 0);
        QCOMPARE(QString(codec->name()), actualCodecName);
    }
}

void tst_QTextCodec::fromUnicode_data()
{
    QTest::addColumn<QString>("codecName");
    QTest::addColumn<bool>("eightBit");

    QTest::newRow("data1") << "ISO-8859-1" << true;
    QTest::newRow("data2") << "ISO-8859-2" << true;
    QTest::newRow("data3") << "ISO-8859-3" << true;
    QTest::newRow("data4") << "ISO-8859-4" << true;
    QTest::newRow("data5") << "ISO-8859-5" << true;
    QTest::newRow("data6") << "ISO-8859-6" << true;
    QTest::newRow("data7") << "ISO-8859-7" << true;
    QTest::newRow("data8") << "ISO-8859-8" << true;
    QTest::newRow("data9") << "ISO-8859-9" << true;
    QTest::newRow("data10") << "ISO-8859-10" << true;
    QTest::newRow("data13") << "ISO-8859-13" << true;
    QTest::newRow("data14") << "ISO-8859-14" << true;
    QTest::newRow("data15") << "ISO-8859-15" << true;
    QTest::newRow("data16") << "ISO-8859-16" << true;

    QTest::newRow("data18") << "IBM850" << true;
    QTest::newRow("data19") << "IBM874" << true;
    QTest::newRow("data20") << "IBM866" << true;

    QTest::newRow("data21") << "windows-1250" << true;
    QTest::newRow("data22") << "windows-1251" << true;
    QTest::newRow("data23") << "windows-1252" << true;
    QTest::newRow("data24") << "windows-1253" << true;
    QTest::newRow("data25") << "windows-1254" << true;
    QTest::newRow("data26") << "windows-1255" << true;
    QTest::newRow("data27") << "windows-1256" << true;
    QTest::newRow("data28") << "windows-1257" << true;
    QTest::newRow("data28") << "windows-1258" << true;

    QTest::newRow("data29") << "Apple Roman" << true;
    QTest::newRow("data29") << "WINSAMI2" << true;
    QTest::newRow("data30") << "TIS-620" << true;
    QTest::newRow("data31") << "roman8" << true;

    QTest::newRow("data32") << "SJIS" << false;
    QTest::newRow("data33") << "EUC-KR" << false;
}

void tst_QTextCodec::fromUnicode()
{
    QFETCH(QString, codecName);
    QFETCH(bool, eightBit);

    QTextCodec *codec = QTextCodec::codecForName(codecName.toLatin1());
    QVERIFY(codec != 0);

    // Check if the reverse lookup is what we expect
    if (eightBit) {
        char chars[128];
        for (int i = 0; i < 128; ++i)
            chars[i] = i + 128;
        QString s = codec->toUnicode(chars, 128);
        QByteArray c = codec->fromUnicode(s);

        int numberOfQuestionMarks = 0;
        for (int i = 0; i < 128; ++i) {
            if (c.at(i) == '?')
                ++numberOfQuestionMarks;
            else
                QCOMPARE(c.at(i), char(i + 128));
        }
        QVERIFY(numberOfQuestionMarks != 128);
    }

    /*
        If the encoding is a superset of ASCII, test that the byte
        array is correct (no off by one, no trailing '\0').
    */
    QByteArray result = codec->fromUnicode(QString("abc"));
    if (result.startsWith("a")) {
        QCOMPARE(result.size(), 3);
        QCOMPARE(result, QByteArray("abc"));
    } else {
        QVERIFY(true);
    }
}

void tst_QTextCodec::toUnicode_codecForHtml()
{
    QFile file(QString(SRCDIR "QT4-crashtest.txt"));
    QVERIFY(file.open(QFile::ReadOnly));

    QByteArray data = file.readAll();
    QTextCodec *codec = Qt::codecForHtml(data);
    codec->toUnicode(data); // this line crashes
}


void tst_QTextCodec::toUnicode_incremental()
{
    QByteArray ba;
    ba += char(0xf0);
    ba += char(0x90);
    ba += char(0x80);
    ba += char(0x80);
    ba += char(0xf4);
    ba += char(0x8f);
    ba += char(0xbf);
    ba += char(0xbd);

    QString expected = QString::fromUtf8(ba);

    QString incremental;
    QTextDecoder *utf8Decoder = QTextCodec::codecForMib(106)->makeDecoder();

    QString actual;
    for (int i = 0; i < ba.size(); ++i)
        utf8Decoder->toUnicode(&actual, ba.constData() + i, 1);

    QCOMPARE(actual, expected);


    delete utf8Decoder;
}

void tst_QTextCodec::codecForLocale()
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    QVERIFY(codec != 0);

#if defined(Q_OS_UNIX)
    // get a time string that is locale-encoded
    QByteArray originalLocaleEncodedTimeString;
    originalLocaleEncodedTimeString.resize(1024);
    time_t t;
    time(&t);
    int r = strftime(originalLocaleEncodedTimeString.data(),
                     originalLocaleEncodedTimeString.size(),
                     "%A%a%B%b%Z",
                     localtime(&t));
    if (r == 0)
        QSKIP("strftime() failed", SkipAll);
    originalLocaleEncodedTimeString.resize(r);

    QString unicodeTimeString = codec->toUnicode(originalLocaleEncodedTimeString);
    QByteArray localeEncodedTimeString = codec->fromUnicode(unicodeTimeString);
    QCOMPARE(localeEncodedTimeString, originalLocaleEncodedTimeString);
#else
    QSKIP("This test is not implemented on Windows", SkipAll);
#endif

    // find a codec that is not the codecForLocale()
    QTextCodec *codec2 = 0;
    foreach (int mib, QTextCodec::availableMibs()) {
        if (mib != codec->mibEnum()) {
            codec2 = QTextCodec::codecForMib(mib);
            if (codec2)
                break;
        }
    }
    if (!codec2) {
        QSKIP("Could not find a codec that is not already the codecForLocale()", SkipAll);
    }

    // set it, codecForLocale() should return it now
    QTextCodec::setCodecForLocale(codec2);
    QCOMPARE(QTextCodec::codecForLocale(), codec2);

    // reset back to the default
    QTextCodec::setCodecForLocale(0);
    QCOMPARE(QTextCodec::codecForLocale(), codec);
}

void tst_QTextCodec::asciiToIscii() const
{
    /* Add all low, 7-bit ASCII characters. */
    QString ascii;
    const int len = 0xA0 - 1;
    ascii.resize(len);

    for(int i = 0; i < len; ++i)
        ascii[i] = QChar(i + 1);

    static const char *const isciiCodecs[] =
    {
        "Iscii-Mlm",
        "Iscii-Knd",
        "Iscii-Tlg",
        "Iscii-Tml",
        "Iscii-Ori",
        "Iscii-Gjr",
        "Iscii-Pnj",
        "Iscii-Bng",
        "Iscii-Dev"
    };
    const int isciiCodecsLen = sizeof(isciiCodecs) / sizeof(const char *);

    for(int i = 0; i < isciiCodecsLen; ++i) {
        /* For each codec. */

        const QTextCodec *const textCodec = QTextCodec::codecForName(isciiCodecs[i]);
        Q_ASSERT(textCodec);

        for(int i2 = 0; i2 < len; ++i2) {
            /* For each character in ascii. */
            const QChar c(ascii[i2]);
            QVERIFY2(textCodec->canEncode(c), qPrintable(QString::fromLatin1("Failed to encode %1 with encoding %2")
                                                         .arg(QString::number(c.unicode()), QString::fromLatin1(textCodec->name().constData()))));
        }

        QVERIFY2(textCodec->canEncode(ascii), qPrintable(QString::fromLatin1("Failed for full string with encoding %1")
                                                         .arg(QString::fromLatin1(textCodec->name().constData()))));
    }
}

void tst_QTextCodec::flagCodepointFFFF() const
{
    // This is an invalid Unicode codepoint.
    const QChar ch(0xFFFF);
    QString input(ch);

    QTextCodec *const codec = QTextCodec::codecForMib(106); // UTF-8
    Q_ASSERT(codec);

    const QByteArray asDecoded(codec->fromUnicode(input));
    QTextCodec::ConverterState state(QTextCodec::ConvertInvalidToNull);
    QVERIFY(codec->toUnicode(asDecoded.constData(), asDecoded.length(), &state) == QChar(0));
    QVERIFY(codec->toUnicode(asDecoded) == QChar(0xfffd));
}

void tst_QTextCodec::flagF7808080() const
{
    /* This test case stems from test not-wf-sa-170, tests/qxmlstream/XML-Test-Suite/xmlconf/xmltest/not-wf/sa/166.xml,
     * whose description reads:
     *
     * "Four byte UTF-8 encodings can encode UCS-4 characters
     *  which are beyond the range of legal XML characters
     *  (and can't be expressed in Unicode surrogate pairs).
     *  This document holds such a character."
     *
     *  In binary, this is:
     *  11110111100000001000000010000000
     *  *       *       *       *
     *  11110www10xxxxxx10yyyyyy10zzzzzz
     *
     *  With multibyte logic removed it is the codepoint 0x1C0000.
     */
    QByteArray input;
    input.resize(4);
    input[0] = char(0xF7);
    input[1] = char(0x80);
    input[2] = char(0x80);
    input[3] = char(0x80);


    QTextCodec *const codec = QTextCodec::codecForMib(106); // UTF-8
    Q_ASSERT(codec);

    //QVERIFY(!codec->canEncode(QChar(0x1C0000)));

    QTextCodec::ConverterState state(QTextCodec::ConvertInvalidToNull);
    QVERIFY(codec->toUnicode(input.constData(), input.length(), &state) == QChar(0));
}

void tst_QTextCodec::flagEFBFBF() const
{
    QByteArray invalidInput;
    invalidInput.resize(3);
    invalidInput[0] = char(0xEF);
    invalidInput[1] = char(0xBF);
    invalidInput[2] = char(0xBF);

    const QTextCodec *const codec = QTextCodec::codecForMib(106); // UTF-8
    Q_ASSERT(codec);

    {
        //QVERIFY(!codec->canEncode(QChar(0xFFFF)));
        QTextCodec::ConverterState state(QTextCodec::ConvertInvalidToNull);
        QVERIFY(codec->toUnicode(invalidInput.constData(), invalidInput.length(), &state) == QChar(0));

        QByteArray start("<?pi ");
        start.append(invalidInput);
        start.append("?>");
    }

    /* When 0xEFBFBF is preceeded by what seems to be an arbitrary character,
     * QTextCodec fails to flag it. */
    {
        QEXPECT_FAIL("", "This is a bug and needs to be fixed.", Continue);
        QByteArray start("B");
        start.append(invalidInput);

        QTextCodec::ConverterState state(QTextCodec::ConvertInvalidToNull);
        QVERIFY(codec->toUnicode(start.constData(), start.length(), &state) == QChar(0));
    }
}

void tst_QTextCodec::decode0D() const
{
    QByteArray input;
    input.resize(3);
    input[0] = 'A';
    input[1] = '\r';
    input[2] = 'B';

    QCOMPARE(QString::fromUtf8(input.constData()).toUtf8(), input);
}

void tst_QTextCodec::codecForIndex() const
{
}

void tst_QTextCodec::aliasForUTF16() const
{
    QVERIFY(QTextCodec::codecForName("UTF-16")->aliases().isEmpty());
}

void tst_QTextCodec::mibForTSCII() const
{
    QCOMPARE(QTextCodec::codecForName("TSCII")->mibEnum(), 2107);
}

static QString fromInvalidUtf8Sequence(const QByteArray &ba)
{
    return QString().fill(QChar::ReplacementCharacter, ba.size());
}

// copied from tst_QString::fromUtf8_data()
void tst_QTextCodec::utf8Codec_data()
{
    QTest::addColumn<QByteArray>("utf8");
    QTest::addColumn<QString>("res");
    QTest::addColumn<int>("len");
    QString str;

    QTest::newRow("str0") << QByteArray("abcdefgh") << QString("abcdefgh") << -1;
    QTest::newRow("str0-len") << QByteArray("abcdefgh") << QString("abc") << 3;
    QTest::newRow("str1") << QByteArray("\303\266\303\244\303\274\303\226\303\204\303\234\303\270\303\246\303\245\303\230\303\206\303\205") << QString("\366\344\374\326\304\334\370\346\345\330\306\305") << -1;
    QTest::newRow("str1-len") << QByteArray("\303\266\303\244\303\274\303\226\303\204\303\234\303\270\303\246\303\245\303\230\303\206\303\205") << QString("\366\344\374\326\304") << 10;

    str += QChar(0x05e9);
    str += QChar(0x05d3);
    str += QChar(0x05d2);
    QTest::newRow("str2") << QByteArray("\327\251\327\223\327\222") << str << -1;

    str = QChar(0x05e9);
    QTest::newRow("str2-len") << QByteArray("\327\251\327\223\327\222") << str << 2;

    str = QChar(0x20ac);
    str += " some text";
    QTest::newRow("str3") << QByteArray("\342\202\254 some text") << str << -1;

    str = QChar(0x20ac);
    str += " some ";
    QTest::newRow("str3-len") << QByteArray("\342\202\254 some text") << str << 9;

    str = "hello";
    str += QChar::ReplacementCharacter;
    str += QChar(0x68);
    str += QChar::ReplacementCharacter;
    str += QChar::ReplacementCharacter;
    str += QChar::ReplacementCharacter;
    str += QChar::ReplacementCharacter;
    str += QChar(0x61);
    str += QChar::ReplacementCharacter;
    QTest::newRow("invalid utf8") << QByteArray("hello\344h\344\344\366\344a\304") << str << -1;
    QTest::newRow("invalid utf8-len") << QByteArray("hello\344h\344\344\366\344a\304") << QString("hello") << 5;

    str = "Prohl";
    str += QChar::ReplacementCharacter;
    str += QChar::ReplacementCharacter;
    str += "e";
    str += QChar::ReplacementCharacter;
    str += " plugin";
    str += QChar::ReplacementCharacter;
    str += " Netscape";

    QTest::newRow("task28417") << QByteArray("Prohl\355\276e\350 plugin\371 Netscape") << str << -1;
    QTest::newRow("task28417-len") << QByteArray("Prohl\355\276e\350 plugin\371 Netscape") << QString("") << 0;

    QTest::newRow("null-1") << QByteArray() << QString() << -1;
    QTest::newRow("null0") << QByteArray() << QString() << 0;
    // QTest::newRow("null5") << QByteArray() << QString() << 5;
    QTest::newRow("empty-1") << QByteArray("\0abcd", 5) << QString() << -1;
    QTest::newRow("empty0") << QByteArray() << QString() << 0;
    QTest::newRow("empty5") << QByteArray("\0abcd", 5) << QString::fromAscii("\0abcd", 5) << 5;
    QTest::newRow("other-1") << QByteArray("ab\0cd", 5) << QString::fromAscii("ab") << -1;
    QTest::newRow("other5") << QByteArray("ab\0cd", 5) << QString::fromAscii("ab\0cd", 5) << 5;

    str = "Old Italic: ";
    str += QChar(0xd800);
    str += QChar(0xdf00);
    str += QChar(0xd800);
    str += QChar(0xdf01);
    str += QChar(0xd800);
    str += QChar(0xdf02);
    str += QChar(0xd800);
    str += QChar(0xdf03);
    str += QChar(0xd800);
    str += QChar(0xdf04);
    QTest::newRow("surrogate") << QByteArray("Old Italic: \360\220\214\200\360\220\214\201\360\220\214\202\360\220\214\203\360\220\214\204") << str << -1;

    QTest::newRow("surrogate-len") << QByteArray("Old Italic: \360\220\214\200\360\220\214\201\360\220\214\202\360\220\214\203\360\220\214\204") << str.left(16) << 20;

    // from http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html

    // 2.1.1 U+00000000
    QByteArray utf8;
    utf8 += char(0x00);
    str = QChar(QChar::Null);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.1.1") << utf8 << str << 1;

    // 2.1.2 U+00000080
    utf8.clear();
    utf8 += char(0xc2);
    utf8 += char(0x80);
    str = QChar(0x80);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.1.2") << utf8 << str << -1;

    // 2.1.3 U+00000800
    utf8.clear();
    utf8 += char(0xe0);
    utf8 += char(0xa0);
    utf8 += char(0x80);
    str = QChar(0x800);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.1.3") << utf8 << str << -1;

    // 2.1.4 U+00010000
    utf8.clear();
    utf8 += char(0xf0);
    utf8 += char(0x90);
    utf8 += char(0x80);
    utf8 += char(0x80);
    str.clear();
    str += QChar(0xd800);
    str += QChar(0xdc00);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.1.4") << utf8 << str << -1;

    // 2.1.5 U+00200000 (not a valid Unicode character)
    utf8.clear();
    utf8 += char(0xf8);
    utf8 += char(0x88);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.1.5") << utf8 << str << -1;

    // 2.1.6 U+04000000 (not a valid Unicode character)
    utf8.clear();
    utf8 += char(0xfc);
    utf8 += char(0x84);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.1.6") << utf8 << str << -1;

    // 2.2.1 U+0000007F
    utf8.clear();
    utf8 += char(0x7f);
    str = QChar(0x7f);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.2.1") << utf8 << str << -1;

    // 2.2.2 U+000007FF
    utf8.clear();
    utf8 += char(0xdf);
    utf8 += char(0xbf);
    str = QChar(0x7ff);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.2.2") << utf8 << str << -1;

    // 2.2.3 U+000FFFF
    utf8.clear();
    utf8 += char(0xef);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str.clear();
    str += QChar::ReplacementCharacter;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.2.3") << utf8 << str << -1;

    // 2.2.4 U+001FFFFF
    utf8.clear();
    utf8 += char(0xf7);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str.clear();
    str += QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.2.4") << utf8 << str << -1;

    // 2.2.5 U+03FFFFFF (not a valid Unicode character)
    utf8.clear();
    utf8 += char(0xfb);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.2.5") << utf8 << str << -1;

    // 2.2.6 U+7FFFFFFF
    utf8.clear();
    utf8 += char(0xfd);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.2.6") << utf8 << str << -1;

    // 2.3.1 U+0000D7FF
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0x9f);
    utf8 += char(0xbf);
    str = QChar(0xd7ff);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.3.1") << utf8 << str << -1;

    // 2.3.2 U+0000E000
    utf8.clear();
    utf8 += char(0xee);
    utf8 += char(0x80);
    utf8 += char(0x80);
    str = QChar(0xe000);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.3.2") << utf8 << str << -1;

    // 2.3.3 U+0000FFFD
    utf8.clear();
    utf8 += char(0xef);
    utf8 += char(0xbf);
    utf8 += char(0xbd);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.3.3") << utf8 << str << -1;

    // 2.3.4 U+0010FFFF
    utf8.clear();
    utf8 += char(0xf4);
    utf8 += char(0x8f);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str.clear();
    str += QChar(0xdbff);
    str += QChar(0xdfff);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.3.4") << utf8 << str << -1;

    // 2.3.5 U+00110000
    utf8.clear();
    utf8 += char(0xf4);
    utf8 += char(0x90);
    utf8 += char(0x80);
    utf8 += char(0x80);
    str.clear();
    str += QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 2.3.5") << utf8 << str << -1;

    // 3.1.1
    utf8.clear();
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.1.1") << utf8 << str << -1;

    // 3.1.2
    utf8.clear();
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.1.2") << utf8 << str << -1;

    // 3.1.3
    utf8.clear();
    utf8 += char(0x80);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.1.3") << utf8 << str << -1;

    // 3.1.4
    utf8.clear();
    utf8 += char(0x80);
    utf8 += char(0xbf);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.1.4") << utf8 << str << -1;

    // 3.1.5
    utf8.clear();
    utf8 += char(0x80);
    utf8 += char(0xbf);
    utf8 += char(0x80);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.1.5") << utf8 << str << -1;

    // 3.1.6
    utf8.clear();
    utf8 += char(0x80);
    utf8 += char(0xbf);
    utf8 += char(0x80);
    utf8 += char(0xbf);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.1.6") << utf8 << str << -1;

    // 3.1.7
    utf8.clear();
    utf8 += char(0x80);
    utf8 += char(0xbf);
    utf8 += char(0x80);
    utf8 += char(0xbf);
    utf8 += char(0x80);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.1.7") << utf8 << str << -1;

    // 3.1.8
    utf8.clear();
    utf8 += char(0x80);
    utf8 += char(0xbf);
    utf8 += char(0x80);
    utf8 += char(0xbf);
    utf8 += char(0x80);
    utf8 += char(0xbf);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.1.8") << utf8 << str << -1;

    // 3.1.9
    utf8.clear();
    for (uint i = 0x80; i<= 0xbf; ++i)
        utf8 += i;
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.1.9") << utf8 << str << -1;

    // 3.2.1
    utf8.clear();
    str.clear();
    for (uint i = 0xc8; i <= 0xdf; ++i) {
        utf8 += i;
        utf8 += char(0x20);

        str += QChar::ReplacementCharacter;
        str += QChar(0x0020);
    }
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.2.1") << utf8 << str << -1;

    // 3.2.2
    utf8.clear();
    str.clear();
    for (uint i = 0xe0; i <= 0xef; ++i) {
        utf8 += i;
        utf8 += char(0x20);

        str += QChar::ReplacementCharacter;
        str += QChar(0x0020);
    }
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.2.2") << utf8 << str << -1;

    // 3.2.3
    utf8.clear();
    str.clear();
    for (uint i = 0xf0; i <= 0xf7; ++i) {
        utf8 += i;
        utf8 += 0x20;

        str += QChar::ReplacementCharacter;
        str += QChar(0x0020);
    }
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.2.3") << utf8 << str << -1;

    // 3.2.4
    utf8.clear();
    str.clear();
    for (uint i = 0xf8; i <= 0xfb; ++i) {
        utf8 += i;
        utf8 += 0x20;

        str += QChar::ReplacementCharacter;
        str += QChar(0x0020);
    }
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.2.4") << utf8 << str << -1;

    // 3.2.5
    utf8.clear();
    str.clear();
    for (uint i = 0xfc; i <= 0xfd; ++i) {
        utf8 += i;
        utf8 += 0x20;

        str += QChar::ReplacementCharacter;
        str += QChar(0x0020);
    }
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.2.5") << utf8 << str << -1;

    // 3.3.1
    utf8.clear();
    utf8 += char(0xc0);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.1") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.1-1") << utf8 << str << -1;

    // 3.3.2
    utf8.clear();
    utf8 += char(0xe0);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.2") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.2-1") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xe0);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.2-2") << utf8 << str << -1;
    utf8 += 0x30;
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.2-3") << utf8 << str << -1;

    // 3.3.3
    utf8.clear();
    utf8 += char(0xf0);
    utf8 += char(0x80);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.3") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.3-1") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xf0);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.3-2") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.3-3") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xf0);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.3-4") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.3-5") << utf8 << str << -1;

    // 3.3.4
    utf8.clear();
    utf8 += char(0xf8);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.4") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.4-1") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xf8);
    utf8 += char(0x80);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.4-2") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.4-3") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xf8);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.4-4") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.4-5") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xf8);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.4-6") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.4-7") << utf8 << str << -1;

    // 3.3.5
    utf8.clear();
    utf8 += char(0xfc);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.5") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.5-1") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xfc);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.5-2") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.5-3") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xfc);
    utf8 += char(0x80);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.5-4") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.5-5") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xfc);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.5-6") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.5-7") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xfc);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.5-8") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.5-9") << utf8 << str << -1;

    // 3.3.6
    utf8.clear();
    utf8 += char(0xdf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.6") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.6-1") << utf8 << str << -1;

    // 3.3.7
    utf8.clear();
    utf8 += char(0xef);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.7") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.7-1") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xef);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.7-2") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.7-3") << utf8 << str << -1;

    // 3.3.8
    utf8.clear();
    utf8 += char(0xf7);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.8") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.8-1") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xf7);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.8-2") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.8-3") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xf7);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.8-4") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.8-5") << utf8 << str << -1;

    // 3.3.9
    utf8.clear();
    utf8 += char(0xfb);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.9") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.9-1") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xfb);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.9-2") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.9-3") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xfb);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.9-4") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.9-5") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xfb);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.9-6") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.9-7") << utf8 << str << -1;

    // 3.3.10
    utf8.clear();
    utf8 += char(0xfd);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.10") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.10-1") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xfd);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.10-2") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.10-3") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xfd);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.10-4") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.10-5") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xfd);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.10-6") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.10-7") << utf8 << str << -1;

    utf8.clear();
    utf8 += char(0xfd);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.10-8") << utf8 << str << -1;
    utf8 += char(0x30);
    str += 0x30;
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.3.10-9") << utf8 << str << -1;

    // 3.4
    utf8.clear();
    utf8 += char(0xc0);
    utf8 += char(0xe0);
    utf8 += char(0x80);
    utf8 += char(0xf0);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0xf8);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0xfc);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0xdf);
    utf8 += char(0xef);
    utf8 += char(0xbf);
    utf8 += char(0xf7);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xfb);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xfd);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.4") << utf8 << str << -1;

    // 3.5.1
    utf8.clear();
    utf8 += char(0xfe);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.5.1") << utf8 << str << -1;

    // 3.5.2
    utf8.clear();
    utf8 += char(0xff);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.5.1") << utf8 << str << -1;

    // 3.5.2
    utf8.clear();
    utf8 += char(0xfe);
    utf8 += char(0xfe);
    utf8 += char(0xff);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 3.5.1") << utf8 << str << -1;

    // 4.1.1
    utf8.clear();
    utf8 += char(0xc0);
    utf8 += char(0xaf);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 4.1.1") << utf8 << str << -1;

    // 4.1.2
    utf8.clear();
    utf8 += char(0xe0);
    utf8 += char(0x80);
    utf8 += char(0xaf);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 4.1.2") << utf8 << str << -1;

    // 4.1.3
    utf8.clear();
    utf8 += char(0xf0);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0xaf);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 4.1.3") << utf8 << str << -1;

    // 4.1.4
    utf8.clear();
    utf8 += char(0xf8);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0xaf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 4.1.4") << utf8 << str << -1;

    // 4.1.5
    utf8.clear();
    utf8 += char(0xfc);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0xaf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 4.1.5") << utf8 << str << -1;

    // 4.2.1
    utf8.clear();
    utf8 += char(0xc1);
    utf8 += char(0xbf);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 4.2.1") << utf8 << str << -1;

    // 4.2.2
    utf8.clear();
    utf8 += char(0xe0);
    utf8 += char(0x9f);
    utf8 += char(0xbf);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 4.2.2") << utf8 << str << -1;

    // 4.2.3
    utf8.clear();
    utf8 += char(0xf0);
    utf8 += char(0x8f);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 4.2.3") << utf8 << str << -1;

    // 4.2.4
    utf8.clear();
    utf8 += char(0xf8);
    utf8 += char(0x87);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 4.2.4") << utf8 << str << -1;

    // 4.2.5
    utf8.clear();
    utf8 += char(0xfc);
    utf8 += char(0x83);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 4.2.5") << utf8 << str << -1;

    // 4.3.1
    utf8.clear();
    utf8 += char(0xc0);
    utf8 += char(0x80);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 4.3.1") << utf8 << str << -1;

    // 4.3.2
    utf8.clear();
    utf8 += char(0xe0);
    utf8 += char(0x80);
    utf8 += char(0x80);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 4.3.2") << utf8 << str << -1;

    // 4.3.3
    utf8.clear();
    utf8 += char(0xf0);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 4.3.3") << utf8 << str << -1;

    // 4.3.4
    utf8.clear();
    utf8 += char(0xf8);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 4.3.4") << utf8 << str << -1;

    // 4.3.5
    utf8.clear();
    utf8 += char(0xfc);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    utf8 += char(0x80);
    str = fromInvalidUtf8Sequence(utf8);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 4.3.5") << utf8 << str << -1;

    // 5.1.1
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0xa0);
    utf8 += char(0x80);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.1.1") << utf8 << str << -1;

    // 5.1.2
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0xad);
    utf8 += char(0xbf);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.1.2") << utf8 << str << -1;

    // 5.1.3
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0xae);
    utf8 += char(0x80);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.1.3") << utf8 << str << -1;

    // 5.1.4
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0xaf);
    utf8 += char(0xbf);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.1.4") << utf8 << str << -1;

    // 5.1.5
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0xb0);
    utf8 += char(0x80);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.1.5") << utf8 << str << -1;

    // 5.1.6
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0xbe);
    utf8 += char(0x80);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.1.6") << utf8 << str << -1;

    // 5.1.7
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.1.7") << utf8 << str << -1;

    // 5.2.1
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0xa0);
    utf8 += char(0x80);
    utf8 += char(0xed);
    utf8 += char(0xb0);
    utf8 += char(0x80);
    str.clear();
    str += QChar(QChar::ReplacementCharacter);
    str += QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.2.1") << utf8 << str << -1;

    // 5.2.2
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0xa0);
    utf8 += char(0x80);
    utf8 += char(0xed);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str.clear();
    str += QChar(QChar::ReplacementCharacter);
    str += QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.2.2") << utf8 << str << -1;

    // 5.2.3
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0xad);
    utf8 += char(0xbf);
    utf8 += char(0xed);
    utf8 += char(0xb0);
    utf8 += char(0x80);
    str.clear();
    str += QChar(QChar::ReplacementCharacter);
    str += QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.2.3") << utf8 << str << -1;

    // 5.2.4
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0xad);
    utf8 += char(0xbf);
    utf8 += char(0xed);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str.clear();
    str += QChar(QChar::ReplacementCharacter);
    str += QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.2.4") << utf8 << str << -1;

    // 5.2.5
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0xae);
    utf8 += char(0x80);
    utf8 += char(0xed);
    utf8 += char(0xb0);
    utf8 += char(0x80);
    str.clear();
    str += QChar(QChar::ReplacementCharacter);
    str += QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.2.5") << utf8 << str << -1;

    // 5.2.6
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0xae);
    utf8 += char(0x80);
    utf8 += char(0xed);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str.clear();
    str += QChar(QChar::ReplacementCharacter);
    str += QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.2.6") << utf8 << str << -1;

    // 5.2.7
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0xaf);
    utf8 += char(0xbf);
    utf8 += char(0xed);
    utf8 += char(0xb0);
    utf8 += char(0x80);
    str.clear();
    str += QChar(QChar::ReplacementCharacter);
    str += QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.2.7") << utf8 << str << -1;

    // 5.2.8
    utf8.clear();
    utf8 += char(0xed);
    utf8 += char(0xaf);
    utf8 += char(0xbf);
    utf8 += char(0xed);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str.clear();
    str += QChar(QChar::ReplacementCharacter);
    str += QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.2.8") << utf8 << str << -1;

    // 5.3.1
    utf8.clear();
    utf8 += char(0xef);
    utf8 += char(0xbf);
    utf8 += char(0xbe);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.3.1") << utf8 << str << -1;

    // 5.3.2
    utf8.clear();
    utf8 += char(0xef);
    utf8 += char(0xbf);
    utf8 += char(0xbf);
    str = QChar(QChar::ReplacementCharacter);
    QTest::newRow("http://www.w3.org/2001/06/utf-8-wrong/UTF-8-test.html 5.3.2") << utf8 << str << -1;
}

void tst_QTextCodec::utf8Codec()
{
    QTextCodec *codec = QTextCodec::codecForMib(106); // UTF-8
    QVERIFY(codec != 0);

    QFETCH(QByteArray, utf8);
    QFETCH(QString, res);
    QFETCH(int, len);

    QString str = codec->toUnicode(utf8.isNull() ? 0 : utf8.constData(),
                                   len < 0 ? qstrlen(utf8.constData()) : len);
    QCOMPARE(str, res);

    str = QString::fromUtf8(utf8.isNull() ? 0 : utf8.constData(), len);
    QCOMPARE(str, res);
}

void tst_QTextCodec::utf8bom_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QString>("result");

    QTest::newRow("nobom")
        << QByteArray("\302\240", 2)
        << QString("\240");

    {
        static const ushort data[] = { 0x201d };
        QTest::newRow("nobom 2")
            << QByteArray("\342\200\235", 3)
            << QString::fromUtf16(data, sizeof(data)/sizeof(short));
    }

    {
        static const ushort data[] = { 0xf000 };
        QTest::newRow("bom1")
            << QByteArray("\357\200\200", 3)
            << QString::fromUtf16(data, sizeof(data)/sizeof(short));
    }

    {
        static const ushort data[] = { 0xfec0 };
        QTest::newRow("bom2")
            << QByteArray("\357\273\200", 3)
            << QString::fromUtf16(data, sizeof(data)/sizeof(short));
    }

    {
        QTest::newRow("normal-bom")
            << QByteArray("\357\273\277a", 4)
            << QString("a");
    }

    {
        static const ushort data[] = { 0x61, 0xfeff, 0x62 };
        QTest::newRow("middle-bom")
            << QByteArray("a\357\273\277b", 5)
            << QString::fromUtf16(data, sizeof(data)/sizeof(short));
    }
}

void tst_QTextCodec::utf8bom()
{
    QFETCH(QByteArray, data);
    QFETCH(QString, result);

    QTextCodec *const codec = QTextCodec::codecForMib(106); // UTF-8
    Q_ASSERT(codec);

    QCOMPARE(codec->toUnicode(data.constData(), data.length(), 0), result);

    QTextCodec::ConverterState state;
    QCOMPARE(codec->toUnicode(data.constData(), data.length(), &state), result);
}

void tst_QTextCodec::utfHeaders_data()
{
    QTest::addColumn<QByteArray>("codecName");
    QTest::addColumn<int>("flags");
    QTest::addColumn<QByteArray>("encoded");
    QTest::addColumn<QString>("unicode");
    QTest::addColumn<bool>("toUnicode");

    QTest::newRow("utf8 bom")
        << QByteArray("UTF-8")
        << 0
        << QByteArray("\xef\xbb\xbfhello")
        << QString::fromLatin1("hello")
        << true;
    QTest::newRow("utf8 nobom")
        << QByteArray("UTF-8")
        << 0
        << QByteArray("hello")
        << QString::fromLatin1("hello")
        << true;
    QTest::newRow("utf8 bom ignore header")
        << QByteArray("UTF-8")
        << (int)QTextCodec::IgnoreHeader
        << QByteArray("\xef\xbb\xbfhello")
        << (QString(QChar(0xfeff)) + QString::fromLatin1("hello"))
        << true;
    QTest::newRow("utf8 nobom ignore header")
        << QByteArray("UTF-8")
        << (int)QTextCodec::IgnoreHeader
        << QByteArray("hello")
        << QString::fromLatin1("hello")
        << true;

    QTest::newRow("utf16 bom be")
        << QByteArray("UTF-16")
        << 0
        << QByteArray("\xfe\xff\0h\0e\0l", 8)
        << QString::fromLatin1("hel")
        << true;
    QTest::newRow("utf16 bom le")
        << QByteArray("UTF-16")
        << 0
        << QByteArray("\xff\xfeh\0e\0l\0", 8)
        << QString::fromLatin1("hel")
        << true;
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        QTest::newRow("utf16 nobom")
            << QByteArray("UTF-16")
            << 0
            << QByteArray("\0h\0e\0l", 6)
            << QString::fromLatin1("hel")
            << true;
        QTest::newRow("utf16 bom be ignore header")
            << QByteArray("UTF-16")
            << (int)QTextCodec::IgnoreHeader
            << QByteArray("\xfe\xff\0h\0e\0l", 8)
            << (QString(QChar(0xfeff)) + QString::fromLatin1("hel"))
            << true;
    } else {
        QTest::newRow("utf16 nobom")
            << QByteArray("UTF-16")
            << 0
            << QByteArray("h\0e\0l\0", 6)
            << QString::fromLatin1("hel")
            << true;
        QTest::newRow("utf16 bom le ignore header")
            << QByteArray("UTF-16")
            << (int)QTextCodec::IgnoreHeader
            << QByteArray("\xff\xfeh\0e\0l\0", 8)
            << (QString(QChar(0xfeff)) + QString::fromLatin1("hel"))
            << true;
    }

    QTest::newRow("utf16-be bom be")
        << QByteArray("UTF-16BE")
        << 0
        << QByteArray("\xfe\xff\0h\0e\0l", 8)
        << QString::fromLatin1("hel")
        << true;
    QTest::newRow("utf16-be nobom")
        << QByteArray("UTF-16BE")
        << 0
        << QByteArray("\0h\0e\0l", 6)
        << QString::fromLatin1("hel")
        << true;
    QTest::newRow("utf16-be bom be ignore header")
        << QByteArray("UTF-16BE")
        << (int)QTextCodec::IgnoreHeader
        << QByteArray("\xfe\xff\0h\0e\0l", 8)
        << (QString(QChar(0xfeff)) + QString::fromLatin1("hel"))
        << true;

    QTest::newRow("utf16-le bom le")
        << QByteArray("UTF-16LE")
        << 0
        << QByteArray("\xff\xfeh\0e\0l\0", 8)
        << QString::fromLatin1("hel")
        << true;
    QTest::newRow("utf16-le nobom")
        << QByteArray("UTF-16LE")
        << 0
        << QByteArray("h\0e\0l\0", 6)
        << QString::fromLatin1("hel")
        << true;
    QTest::newRow("utf16-le bom le ignore header")
        << QByteArray("UTF-16LE")
        << (int)QTextCodec::IgnoreHeader
        << QByteArray("\xff\xfeh\0e\0l\0", 8)
        << (QString(QChar(0xfeff)) + QString::fromLatin1("hel"))
        << true;


    QTest::newRow("utf32 bom be")
        << QByteArray("UTF-32")
        << 0
        << QByteArray("\0\0\xfe\xff\0\0\0h\0\0\0e\0\0\0l", 16)
        << QString::fromLatin1("hel")
        << true;
    QTest::newRow("utf32 bom le")
        << QByteArray("UTF-32")
        << 0
        << QByteArray("\xff\xfe\0\0h\0\0\0e\0\0\0l\0\0\0", 16)
        << QString::fromLatin1("hel")
        << true;
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        QTest::newRow("utf32 nobom")
            << QByteArray("UTF-32")
            << 0
            << QByteArray("\0\0\0h\0\0\0e\0\0\0l", 12)
            << QString::fromLatin1("hel")
            << true;
        QTest::newRow("utf32 bom be ignore header")
            << QByteArray("UTF-32")
            << (int)QTextCodec::IgnoreHeader
            << QByteArray("\0\0\xfe\xff\0\0\0h\0\0\0e\0\0\0l", 16)
            << (QString(QChar(0xfeff)) + QString::fromLatin1("hel"))
            << true;
    } else {
        QTest::newRow("utf32 nobom")
            << QByteArray("UTF-32")
            << 0
            << QByteArray("h\0\0\0e\0\0\0l\0\0\0", 12)
            << QString::fromLatin1("hel")
            << true;
        QTest::newRow("utf32 bom le ignore header")
            << QByteArray("UTF-32")
            << (int)QTextCodec::IgnoreHeader
            << QByteArray("\xff\xfe\0\0h\0\0\0e\0\0\0l\0\0\0", 16)
            << (QString(QChar(0xfeff)) + QString::fromLatin1("hel"))
            << true;
    }


    QTest::newRow("utf32-be bom be")
        << QByteArray("UTF-32BE")
        << 0
        << QByteArray("\0\0\xfe\xff\0\0\0h\0\0\0e\0\0\0l", 16)
        << QString::fromLatin1("hel")
        << true;
    QTest::newRow("utf32-be nobom")
        << QByteArray("UTF-32BE")
        << 0
        << QByteArray("\0\0\0h\0\0\0e\0\0\0l", 12)
        << QString::fromLatin1("hel")
        << true;
    QTest::newRow("utf32-be bom be ignore header")
        << QByteArray("UTF-32BE")
        << (int)QTextCodec::IgnoreHeader
        << QByteArray("\0\0\xfe\xff\0\0\0h\0\0\0e\0\0\0l", 16)
        << (QString(QChar(0xfeff)) + QString::fromLatin1("hel"))
        << true;


    QTest::newRow("utf32-le bom le")
        << QByteArray("UTF-32LE")
        << 0
        << QByteArray("\xff\xfe\0\0h\0\0\0e\0\0\0l\0\0\0", 16)
        << QString::fromLatin1("hel")
        << true;
    QTest::newRow("utf32-le nobom")
        << QByteArray("UTF-32LE")
        << 0
        << QByteArray("h\0\0\0e\0\0\0l\0\0\0", 12)
        << QString::fromLatin1("hel")
        << true;
    QTest::newRow("utf32-le bom le ignore header")
        << QByteArray("UTF-32LE")
        << (int)QTextCodec::IgnoreHeader
        << QByteArray("\xff\xfe\0\0h\0\0\0e\0\0\0l\0\0\0", 16)
        << (QString(QChar(0xfeff)) + QString::fromLatin1("hel"))
        << true;
}

void tst_QTextCodec::utfHeaders()
{
    QFETCH(QByteArray, codecName);
    QTextCodec *codec = QTextCodec::codecForName(codecName);
    QVERIFY(codec != 0);

    QFETCH(int, flags);
    QTextCodec::ConversionFlags cFlags = QTextCodec::ConversionFlags(flags);
    QTextCodec::ConverterState state(cFlags);

    QFETCH(QByteArray, encoded);
    QFETCH(QString, unicode);

    QFETCH(bool, toUnicode);

    QLatin1String ignoreReverseTestOn = (QSysInfo::ByteOrder == QSysInfo::BigEndian) ? QLatin1String(" le") : QLatin1String(" be");
    QString rowName(QTest::currentDataTag());

    for (int i = 0; i < encoded.length(); ++i)
        qDebug() << hex << "    " << (uint)(uchar)encoded.at(i);
    if (toUnicode) {
        QString result = codec->toUnicode(encoded.constData(), encoded.length(), &state);
        for (int i = 0; i < result.length(); ++i)
            qDebug() << hex << "    " << (uint)result.at(i).unicode();
        QCOMPARE(result.length(), unicode.length());
        QCOMPARE(result, unicode);

        if (!rowName.endsWith("nobom") && !rowName.contains(ignoreReverseTestOn)) {
            QTextCodec::ConverterState state2(cFlags);
            QByteArray reencoded = codec->fromUnicode(unicode.unicode(), unicode.length(), &state2);
            QCOMPARE(reencoded, encoded);
        }
    } else {
        QByteArray result = codec->fromUnicode(unicode.unicode(), unicode.length(), &state);
        QCOMPARE(result, encoded);
    }
}

void tst_QTextCodec::codecForHtml()
{
    QByteArray html("<html><head></head><body>blah</body></html>");

    QCOMPARE(QTextCodec::codecForHtml(html)->mibEnum(), 4); // latin 1

    QCOMPARE(QTextCodec::codecForHtml(html, QTextCodec::codecForMib(106))->mibEnum(), 106); // UTF-8

    html = "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=ISO-8859-15\" /></head></html>";
    QCOMPARE(QTextCodec::codecForHtml(html, QTextCodec::codecForMib(106))->mibEnum(), 111); // latin 15

    html = "<html><head><meta content=\"text/html; charset=ISO-8859-15\" http-equiv=\"content-type\" /></head></html>";
    QCOMPARE(QTextCodec::codecForHtml(html, QTextCodec::codecForMib(106))->mibEnum(), 111); // latin 15

    html = "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=invalid-foo\" /></head></html>";
    QCOMPARE(QTextCodec::codecForHtml(html, QTextCodec::codecForMib(106))->mibEnum(), 106); // UTF-8
    QCOMPARE(QTextCodec::codecForHtml(html)->mibEnum(), 4); // latin 1
}

void tst_QTextCodec::codecForUtfText_data()
{
    QTest::addColumn<QByteArray>("encoded");
    QTest::addColumn<bool>("detected");
    QTest::addColumn<int>("mib");


    QTest::newRow("utf8 bom")
        << QByteArray("\xef\xbb\xbfhello")
        << true
        << 106;
    QTest::newRow("utf8 nobom")
        << QByteArray("hello")
        << false
        << 0;

    QTest::newRow("utf16 bom be")
        << QByteArray("\xfe\xff\0h\0e\0l", 8)
        << true
        << 1013;
    QTest::newRow("utf16 bom le")
        << QByteArray("\xff\xfeh\0e\0l\0", 8)
        << true
        << 1014;
    QTest::newRow("utf16 nobom")
        << QByteArray("\0h\0e\0l", 6)
        << false
        << 0;

    QTest::newRow("utf32 bom be")
        << QByteArray("\0\0\xfe\xff\0\0\0h\0\0\0e\0\0\0l", 16)
        << true
        << 1018;
    QTest::newRow("utf32 bom le")
        << QByteArray("\xff\xfe\0\0h\0\0\0e\0\0\0l\0\0\0", 16)
        << true
        << 1019;
    QTest::newRow("utf32 nobom")
        << QByteArray("\0\0\0h\0\0\0e\0\0\0l", 12)
        << false
        << 0;
}

void tst_QTextCodec::codecForUtfText()
{
    QFETCH(QByteArray, encoded);
    QFETCH(bool, detected);
    QFETCH(int, mib);

    QTextCodec *codec = QTextCodec::codecForUtfText(encoded, 0);
    if (detected)
        QCOMPARE(codec->mibEnum(), mib);
    else
        QVERIFY(codec == 0);
}

#ifdef Q_OS_UNIX
void tst_QTextCodec::toLocal8Bit()
{
#ifdef QT_NO_PROCESS
    QSKIP("This test requires QProcess", SkipAll);
#else
    QProcess process;
    process.start("echo/echo");
    QString string(QChar(0x410));
    process.write((const char*)string.utf16(), string.length()*2);

    process.closeWriteChannel();
    process.waitForFinished();
    QCOMPARE(process.exitStatus(), QProcess::NormalExit);
    QCOMPARE(process.exitCode(), 0);
#endif
}
#endif

QTEST_MAIN(tst_QTextCodec)
#include "tst_qtextcodec.moc"
