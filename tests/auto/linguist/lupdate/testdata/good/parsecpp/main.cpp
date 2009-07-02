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
