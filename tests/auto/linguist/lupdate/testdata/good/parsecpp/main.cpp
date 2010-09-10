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

// IMPORTANT!!!! If you want to add testdata to this file,
// always add it to the end in order to not change the linenumbers of translations!!!
int main(char **argv, int argc)
{
    Size size = QSize(1,1);
}

QString qt_detectRTLLanguage()
{
     return QApplication::tr("QT_LAYOUT_DIRECTION",
                         "Translate this string to the string 'LTR' in left-to-right"
                         " languages or to 'RTL' in right-to-left languages (such as Hebrew"
                         " and Arabic) to get proper widget layout.") == QLatin1String("RTL");
}


class Dialog2 : public QDialog
{
    Q_OBJECT
    void func();
    void func3();
    int getCount() const { return 2; }

};

void Dialog2::func()
{
    int n = getCount();
    tr("%n files", "plural form", n);
    tr("%n cars", 0, n);
    tr("&Find %n cars", 0, n);
    tr("Search in %n items?", 0, n);
    tr("%1. Search in %n items?", 0, n);
    tr("Age: %1");
    tr("There are %n house(s)", "Plurals and function call", getCount());




    QCoreApplication::translate("Plurals, QCoreApplication", "%n house(s)", "Plurals and identifier", QCoreApplication::UnicodeUTF8, n);
    QCoreApplication::translate("Plurals, QCoreApplication", "%n car(s)", "Plurals and literal number", QCoreApplication::UnicodeUTF8, 1);
    QCoreApplication::translate("Plurals, QCoreApplication", "%n horse(s)", "Plurals and function call", QCoreApplication::UnicodeUTF8, getCount());








    QTranslator trans;
    trans.translate("QTranslator", "Simple");
    trans.translate("QTranslator", "Simple", 0);
    trans.translate("QTranslator", "Simple with comment", "with comment");
    trans.translate("QTranslator", "Plural without comment", 0, 1);
    trans.translate("QTranslator", "Plural with comment", "comment 1", n);
    trans.translate("QTranslator", "Plural with comment", "comment 2", getCount());












}




/* This is actually a test of how many alternative ways a struct/class can be found in a source file.
 * Due to the simple parser in lupdate, it will actually not treat the remaining lines in the define
 * as a macro, which is a case the 'Tok_Class' parser block might not consider, and it might loop infinite
 * if it just tries to fetch the next token until it gets a '{' or a ';'. Another pitfall is that the
 * context of tr("func3") might not be parsed, it won't resume normal evaluation until the '{' after the function
 * signature.
 *
 */
typedef struct S_
{
int a;
} S, *SPtr;
class ForwardDecl;


#define FT_DEFINE_SERVICE( name )            \
  typedef struct FT_Service_ ## name ## Rec_ \
    FT_Service_ ## name ## Rec ;             \
  typedef struct FT_Service_ ## name ## Rec_ \
    const * FT_Service_ ## name ;            \
  struct FT_Service_ ## name ## Rec_


/* removing this comment will break this test */

void Dialog2::func3()
{
    tr("func3");
}




namespace Gui { class BaseClass  {}; }


class TestClass : QObject {
    Q_OBJECT


    inline QString inlineFunc1() {
        return tr("inline function", "TestClass");
    }

    QString inlineFunc2() {
        return tr("inline function 2", "TestClass");
    }

    static inline QString staticInlineFunc() {
        return tr("static inline function", "TestClass");
    }

    class NoQObject : public Gui::BaseClass {
        public:
        inline QString hello() { return QString("hello"); }

    };

};


class Testing : QObject {
    Q_OBJECT

    inline QString f1() {
        //: this is an extra comment for the translator
        return tr("extra-commented string");
        return tr("not extra-commented string");
        /*: another extra-comment */
        return tr("another extra-commented string");
        /*: blah! */
        return QApplication::translate("scope", "works in translate, too", "blabb", 0);
    }

};

//: extra comment for NOOP
//: which spans multiple lines
QT_TRANSLATE_NOOP("scope", "string") // 4.4 says the line of this is at the next statement
//: extra comment for NOOP3
QT_TRANSLATE_NOOP3_UTF8("scope", "string", "comment") // 4.4 doesn't see this

QT_TRANSLATE_NOOP("scope", "string " // this is an interleaved comment
                  "continuation on next line")


class TestingTake17 : QObject {
    Q_OBJECT

    int function(void)
    {
        //: random comment
        //= this_is_an_id
        //~ loc-layout_id fooish_bar
        //~ po-ignore_me totally foo-barred  nonsense
        tr("something cool");

        tr("less cool");

        //= another_id
        tr("even more cool");
    }
};




//: again an extra comment, this time for id-based NOOP
//% "This is supposed\tto be quoted \" newline\n"
//% "backslashed \\ stuff."
QT_TRID_NOOP("this_a_id")

//~ some thing
//% "This needs to be here. Really."
QString test = qtTrId("this_another_id", n);



class YetAnotherTest : QObject {
    Q_OBJECT

    int function(void)
    {
        //
        //:
        //=
        //~
        //#
        //=============
        //~~~~~~~~~~~~~
        //:::::::::::::
        tr("nothing");
    }
};



//: This is a message without a source string
QString test = qtTrId("yet_another_id");



// QTBUG-9276: context in static initializers
class Bogus : QObject {
    Q_OBJECT

    static const char * const s_strings[];
};

const char * const Bogus::s_strings[] = {
    QT_TR_NOOP("this should be in Bogus")
};

const char * const Bogus::s_strings[SIZE] = {
    QT_TR_NOOP("this should be in Bogus")
};

void bogosity()
{
    // no spaces here. test collateral damage from ignoring equal sign
    Class::member=QObject::tr("just QObject");
}



namespace Internal {

class Message : public QObject
{
    Q_OBJECT
public:
    Message(QObject *parent = 0);
};

} // The temporary closing of the namespace triggers the problem

namespace Internal {

static inline QString message1()
{
    return Message::tr("message1"); // Had no namespace
}

static inline QString message2()
{
    return Message::tr("message2"); // Already had namespace
}

}



// QTBUG-11426: operator overloads
class LotsaFun : public QObject
{
    Q_OBJECT
public:
    int operator<<(int left, int right);
};

int LotsaFun::operator<<(int left, int right)
{
    tr("this is inside operator<<");
    return left << right;
}



// QTBUG-12683: define in re-opened namespace
namespace NameSchpace {

class YetMoreFun : public QObject
{
    Q_OBJECT
public:
    void funStuff();
};

}

namespace NameSchpace {

#define somevar 1

void YetMoreFun::funStuff()
{
    tr("funStuff!");
}

}
