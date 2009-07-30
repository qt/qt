/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "lupdate.h"

#include <translator.h>

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QStack>
#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>

#include <ctype.h>              // for isXXX()

QT_BEGIN_NAMESPACE

/* qmake ignore Q_OBJECT */

static const char MagicComment[] = "TRANSLATOR ";

#define STRINGIFY_INTERNAL(x) #x
#define STRINGIFY(x) STRINGIFY_INTERNAL(x)
#define STRING(s) static QString str##s(QLatin1String(STRINGIFY(s)))

//#define DIAGNOSE_RETRANSLATABILITY // FIXME: should make a runtime option of this

uint qHash(const QStringList &qsl)
{
    uint hash = 0;
    foreach (const QString &qs, qsl) {
        hash ^= qHash(qs) ^ 0xa09df22f;
        hash = (hash << 13) | (hash >> 19);
    }
    return hash;
}

struct Namespace {

    Namespace() :
            isClass(false),
            hasTrFunctions(false), needsTrFunctions(false), complained(false)
    {}

    QString name;
    QMap<QString, Namespace *> children;
    QMap<QString, QStringList> aliases;
    QSet<QStringList> usings;

    int fileId;

    bool isClass;

    bool hasTrFunctions;
    bool needsTrFunctions;
    bool complained; // ... that tr functions are missing.
};

typedef QList<Namespace *> NamespaceList;

struct ParseResults {

    ParseResults()
    {
        static int nextFileId;
        rootNamespace.fileId = nextFileId++;
        tor = 0;
    }
    bool detachNamespace(Namespace **that);
    Namespace *include(Namespace *that, const Namespace *other);
    void unite(const ParseResults *other);

    Namespace rootNamespace;
    Translator *tor;
    QSet<QString> allIncludes;
};

typedef QHash<QString, const ParseResults *> ParseResultHash;

class CppFiles {

public:
    static const ParseResults *getResults(const QString &cleanFile);
    static void setResults(const QString &cleanFile, const ParseResults *results);
    static bool isBlacklisted(const QString &cleanFile);
    static void setBlacklisted(const QString &cleanFile);

private:
    static ParseResultHash &parsedFiles();
    static QSet<QString> &blacklistedFiles();
};

class CppParser {

public:
    CppParser(ParseResults *results = 0);
    void setInput(const QString &in);
    void setInput(QTextStream &ts, const QString &fileName);
    void setTranslator(Translator *tor) { results->tor = tor; }
    void parse(const QString &initialContext, ConversionData &cd, QSet<QString> &inclusions);
    void parseInternal(ConversionData &cd, QSet<QString> &inclusions);
    const ParseResults *getResults() const { return results; }
    void deleteResults() { delete results; }

    struct SavedState {
        QStringList namespaces;
        QStack<int> namespaceDepths;
        QStringList functionContext;
        QString functionContextUnresolved;
        QString pendingContext;
    };

private:
    struct IfdefState {
        IfdefState() {}
        IfdefState(int _braceDepth, int _parenDepth) :
            braceDepth(_braceDepth),
            parenDepth(_parenDepth),
            elseLine(-1)
        {}

        SavedState state;
        int braceDepth, braceDepth1st;
        int parenDepth, parenDepth1st;
        int elseLine;
    };

    uint getChar();
    uint getToken();
    bool match(uint t);
    bool matchString(QString *s);
    bool matchEncoding(bool *utf8);
    bool matchInteger(qlonglong *number);
    bool matchStringOrNull(QString *s);
    bool matchExpression();

    QString transcode(const QString &str, bool utf8);
    void recordMessage(
        int line, const QString &context, const QString &text, const QString &comment,
        const QString &extracomment, const QString &msgid, const TranslatorMessage::ExtraData &extra,
        bool utf8, bool plural);

    void processInclude(const QString &file, ConversionData &cd,
                        QSet<QString> &inclusions);

    void saveState(SavedState *state);
    void loadState(const SavedState *state);

    static QString stringifyNamespace(const NamespaceList &namespaces);
    static QStringList stringListifyNamespace(const NamespaceList &namespaces);
    void modifyNamespace(NamespaceList *namespaces);
    NamespaceList resolveNamespaces(const QStringList &segments);
    bool qualifyOne(const NamespaceList &namespaces, int nsIdx, const QString &segment,
                    NamespaceList *resolved);
    bool fullyQualify(const NamespaceList &namespaces, const QStringList &segments,
                      bool isDeclaration,
                      NamespaceList *resolved, QStringList *unresolved);
    void enterNamespace(NamespaceList *namespaces, const QString &name);
    void truncateNamespaces(NamespaceList *namespaces, int lenght);

    enum {
        Tok_Eof, Tok_class, Tok_friend, Tok_namespace, Tok_using, Tok_return,
        Tok_tr = 10, Tok_trUtf8, Tok_translate, Tok_translateUtf8, Tok_trid,
        Tok_Q_OBJECT = 20, Tok_Q_DECLARE_TR_FUNCTIONS,
        Tok_Ident, Tok_Comment, Tok_String, Tok_Arrow, Tok_Colon, Tok_ColonColon,
        Tok_Equals,
        Tok_LeftBrace = 30, Tok_RightBrace, Tok_LeftParen, Tok_RightParen, Tok_Comma, Tok_Semicolon,
        Tok_Integer = 40,
        Tok_QuotedInclude = 50, Tok_AngledInclude,
        Tok_Other = 99
    };

    // Tokenizer state
    QString yyFileName;
    int yyCh;
    bool yyAtNewline;
    bool yyCodecIsUtf8;
    bool yyForceUtf8;
    QString yyIdent;
    QString yyComment;
    QString yyString;
    qlonglong yyInteger;
    QStack<IfdefState> yyIfdefStack;
    int yyBraceDepth;
    int yyParenDepth;
    int yyLineNo;
    int yyCurLineNo;
    int yyBraceLineNo;
    int yyParenLineNo;

    // the string to read from and current position in the string
    QTextCodec *yySourceCodec;
    bool yySourceIsUnicode;
    QString yyInStr;
    int yyInPos;

    // Parser state
    uint yyTok;

    NamespaceList namespaces;
    QStack<int> namespaceDepths;
    NamespaceList functionContext;
    QString functionContextUnresolved;
    QString prospectiveContext;
    QString pendingContext;
    ParseResults *results;
    bool directInclude;

    SavedState savedState;
    int yyMinBraceDepth;
    bool inDefine;
};

CppParser::CppParser(ParseResults *_results)
{
    if (_results) {
        results = _results;
        directInclude = true;
    } else {
        results = new ParseResults;
        directInclude = false;
    }
    yyInPos = 0;
    yyBraceDepth = 0;
    yyParenDepth = 0;
    yyCurLineNo = 1;
    yyBraceLineNo = 1;
    yyParenLineNo = 1;
    yyAtNewline = true;
    yyMinBraceDepth = 0;
    inDefine = false;
}

void CppParser::setInput(const QString &in)
{
    yyInStr = in;
    yyFileName = QString();
    yySourceCodec = 0;
    yySourceIsUnicode = true;
    yyForceUtf8 = true;
}

void CppParser::setInput(QTextStream &ts, const QString &fileName)
{
    yyInStr = ts.readAll();
    yyFileName = fileName;
    yySourceCodec = ts.codec();
    yySourceIsUnicode = yySourceCodec->name().startsWith("UTF-");
    yyForceUtf8 = false;
}

/*
  The first part of this source file is the C++ tokenizer.  We skip
  most of C++; the only tokens that interest us are defined here.
  Thus, the code fragment

      int main()
      {
          printf("Hello, world!\n");
          return 0;
      }

  is broken down into the following tokens (Tok_ omitted):

      Ident Ident LeftParen RightParen
      LeftBrace
          Ident LeftParen String RightParen Semicolon
          return Semicolon
      RightBrace.

  The 0 doesn't produce any token.
*/

uint CppParser::getChar()
{
    forever {
        if (yyInPos >= yyInStr.size())
            return EOF;
        uint c = yyInStr[yyInPos++].unicode();
        if (c == '\\' && yyInPos < yyInStr.size()) {
            if (yyInStr[yyInPos].unicode() == '\n') {
                ++yyCurLineNo;
                ++yyInPos;
                continue;
            }
            if (yyInStr[yyInPos].unicode() == '\r') {
                ++yyCurLineNo;
                ++yyInPos;
                if (yyInPos < yyInStr.size() && yyInStr[yyInPos].unicode() == '\n')
                    ++yyInPos;
                continue;
            }
        }
        if (c == '\r') {
            if (yyInPos < yyInStr.size() && yyInStr[yyInPos].unicode() == '\n')
                ++yyInPos;
            c = '\n';
            ++yyCurLineNo;
            yyAtNewline = true;
        } else if (c == '\n') {
            ++yyCurLineNo;
            yyAtNewline = true;
        } else if (c != ' ' && c != '\t' && c != '#') {
            yyAtNewline = false;
        }
        return c;
    }
}

uint CppParser::getToken()
{
  restart:
    yyIdent.clear();
    yyComment.clear();
    yyString.clear();

    while (yyCh != EOF) {
        yyLineNo = yyCurLineNo;

        if (yyCh == '#' && yyAtNewline) {
            /*
              Early versions of lupdate complained about
              unbalanced braces in the following code:

                  #ifdef ALPHA
                      while (beta) {
                  #else
                      while (gamma) {
                  #endif
                          delta;
                      }

              The code contains, indeed, two opening braces for
              one closing brace; yet there's no reason to panic.

              The solution is to remember yyBraceDepth as it was
              when #if, #ifdef or #ifndef was met, and to set
              yyBraceDepth to that value when meeting #elif or
              #else.
            */
            do {
                yyCh = getChar();
            } while (isspace(yyCh) && yyCh != '\n');

            switch (yyCh) {
            case 'd': // define
                // Skip over the name of the define to avoid it being interpreted as c++ code
                do { // Rest of "define"
                    yyCh = getChar();
                    if (yyCh == EOF)
                        return Tok_Eof;
                    if (yyCh == '\n')
                        goto restart;
                } while (!isspace(yyCh));
                do { // Space beween "define" and macro name
                    yyCh = getChar();
                    if (yyCh == EOF)
                        return Tok_Eof;
                    if (yyCh == '\n')
                        goto restart;
                } while (isspace(yyCh));
                do { // Macro name
                    if (yyCh == '(') {
                        // Argument list. Follows the name without a space, and no
                        // paren nesting is possible.
                        do {
                            yyCh = getChar();
                            if (yyCh == EOF)
                                return Tok_Eof;
                            if (yyCh == '\n')
                                goto restart;
                        } while (yyCh != ')');
                        break;
                    }
                    yyCh = getChar();
                    if (yyCh == EOF)
                        return Tok_Eof;
                    if (yyCh == '\n')
                        goto restart;
                } while (!isspace(yyCh));
                do { // Shortcut the immediate newline case if no comments follow.
                    yyCh = getChar();
                    if (yyCh == EOF)
                        return Tok_Eof;
                    if (yyCh == '\n')
                        goto restart;
                } while (isspace(yyCh));

                saveState(&savedState);
                yyMinBraceDepth = yyBraceDepth;
                inDefine = true;
                goto restart;
            case 'i':
                yyCh = getChar();
                if (yyCh == 'f') {
                    // if, ifdef, ifndef
                    yyIfdefStack.push(IfdefState(yyBraceDepth, yyParenDepth));
                    yyCh = getChar();
                } else if (yyCh == 'n') {
                    // include
                    do {
                        yyCh = getChar();
                    } while (yyCh != EOF && !isspace(yyCh));
                    do {
                        yyCh = getChar();
                    } while (isspace(yyCh));
                    int tChar;
                    if (yyCh == '"')
                        tChar = '"';
                    else if (yyCh == '<')
                        tChar = '>';
                    else
                        break;
                    forever {
                        yyCh = getChar();
                        if (yyCh == EOF || yyCh == '\n')
                            break;
                        if (yyCh == tChar) {
                            yyCh = getChar();
                            break;
                        }
                        yyString += yyCh;
                    }
                    return (tChar == '"') ? Tok_QuotedInclude : Tok_AngledInclude;
                }
                break;
            case 'e':
                yyCh = getChar();
                if (yyCh == 'l') {
                    // elif, else
                    if (!yyIfdefStack.isEmpty()) {
                        IfdefState &is = yyIfdefStack.top();
                        if (is.elseLine != -1) {
                            if (yyBraceDepth != is.braceDepth1st || yyParenDepth != is.parenDepth1st)
                                qWarning("%s:%d: Parenthesis/brace mismatch between "
                                         "#if and #else branches; using #if branch\n",
                                         qPrintable(yyFileName), is.elseLine);
                        } else {
                            is.braceDepth1st = yyBraceDepth;
                            is.parenDepth1st = yyParenDepth;
                            saveState(&is.state);
                        }
                        is.elseLine = yyLineNo;
                        yyBraceDepth = is.braceDepth;
                        yyParenDepth = is.parenDepth;
                    }
                    yyCh = getChar();
                } else if (yyCh == 'n') {
                    // endif
                    if (!yyIfdefStack.isEmpty()) {
                        IfdefState is = yyIfdefStack.pop();
                        if (is.elseLine != -1) {
                            if (yyBraceDepth != is.braceDepth1st || yyParenDepth != is.parenDepth1st)
                                qWarning("%s:%d: Parenthesis/brace mismatch between "
                                         "#if and #else branches; using #if branch\n",
                                         qPrintable(yyFileName), is.elseLine);
                            yyBraceDepth = is.braceDepth1st;
                            yyParenDepth = is.parenDepth1st;
                            loadState(&is.state);
                        }
                    }
                    yyCh = getChar();
                }
                break;
            }
            // Optimization: skip over rest of preprocessor directive
            do {
                if (yyCh == '/') {
                    yyCh = getChar();
                    if (yyCh == '/') {
                        do {
                            yyCh = getChar();
                        } while (yyCh != EOF && yyCh != '\n');
                        break;
                    } else if (yyCh == '*') {
                        bool metAster = false;

                        forever {
                            yyCh = getChar();
                            if (yyCh == EOF) {
                                qWarning("%s:%d: Unterminated C++ comment\n",
                                         qPrintable(yyFileName), yyLineNo);
                                break;
                            }

                            if (yyCh == '*') {
                                metAster = true;
                            } else if (metAster && yyCh == '/') {
                                yyCh = getChar();
                                break;
                            } else {
                                metAster = false;
                            }
                        }
                    }
                } else {
                    yyCh = getChar();
                }
            } while (yyCh != '\n' && yyCh != EOF);
            yyCh = getChar();
        } else if (isalpha(yyCh) || yyCh == '_') {
            do {
                yyIdent += yyCh;
                yyCh = getChar();
            } while (isalnum(yyCh) || yyCh == '_');

            //qDebug() << "IDENT: " << yyIdent;

            switch (yyIdent.at(0).unicode()) {
            case 'Q':
                if (yyIdent == QLatin1String("Q_OBJECT"))
                    return Tok_Q_OBJECT;
                if (yyIdent == QLatin1String("Q_DECLARE_TR_FUNCTIONS"))
                    return Tok_Q_DECLARE_TR_FUNCTIONS;
                if (yyIdent == QLatin1String("QT_TR_NOOP"))
                    return Tok_tr;
                if (yyIdent == QLatin1String("QT_TRID_NOOP"))
                    return Tok_trid;
                if (yyIdent == QLatin1String("QT_TRANSLATE_NOOP"))
                    return Tok_translate;
                if (yyIdent == QLatin1String("QT_TRANSLATE_NOOP3"))
                    return Tok_translate;
                if (yyIdent == QLatin1String("QT_TR_NOOP_UTF8"))
                    return Tok_trUtf8;
                if (yyIdent == QLatin1String("QT_TRANSLATE_NOOP_UTF8"))
                    return Tok_translateUtf8;
                if (yyIdent == QLatin1String("QT_TRANSLATE_NOOP3_UTF8"))
                    return Tok_translateUtf8;
                break;
            case 'T':
                // TR() for when all else fails
                if (yyIdent.compare(QLatin1String("TR"), Qt::CaseInsensitive) == 0) {
                    return Tok_tr;
                }
                break;
            case 'c':
                if (yyIdent == QLatin1String("class"))
                    return Tok_class;
                break;
            case 'f':
                /*
                  QTranslator::findMessage() has the same parameters as
                  QApplication::translate().
                */
                if (yyIdent == QLatin1String("findMessage"))
                    return Tok_translate;
                if (yyIdent == QLatin1String("friend"))
                    return Tok_friend;
                break;
            case 'n':
                if (yyIdent == QLatin1String("namespace"))
                    return Tok_namespace;
                break;
            case 'q':
                if (yyIdent == QLatin1String("qtTrId"))
                    return Tok_trid;
                break;
            case 'r':
                if (yyIdent == QLatin1String("return"))
                    return Tok_return;
                break;
            case 's':
                if (yyIdent == QLatin1String("struct"))
                    return Tok_class;
                break;
            case 't':
                if (yyIdent == QLatin1String("tr")) {
                    return Tok_tr;
                }
                if (yyIdent == QLatin1String("trUtf8")) {
                    return Tok_trUtf8;
                }
                if (yyIdent == QLatin1String("translate")) {
                    return Tok_translate;
                }
                break;
            case 'u':
                if (yyIdent == QLatin1String("using"))
                    return Tok_using;
                break;
            }
            return Tok_Ident;
        } else {
            switch (yyCh) {
            case '\n':
                if (inDefine) {
                    loadState(&savedState);
                    prospectiveContext.clear();
                    yyBraceDepth = yyMinBraceDepth;
                    yyMinBraceDepth = 0;
                    inDefine = false;
                }
                yyCh = getChar();
                break;
            case '/':
                yyCh = getChar();
                if (yyCh == '/') {
                    do {
                        yyCh = getChar();
                        if (yyCh == EOF)
                            break;
                        yyComment.append(yyCh);
                    } while (yyCh != '\n');
                } else if (yyCh == '*') {
                    bool metAster = false;

                    forever {
                        yyCh = getChar();
                        if (yyCh == EOF) {
                            qWarning("%s:%d: Unterminated C++ comment\n",
                                     qPrintable(yyFileName), yyLineNo);
                            return Tok_Comment;
                        }
                        yyComment.append(yyCh);

                        if (yyCh == '*')
                            metAster = true;
                        else if (metAster && yyCh == '/')
                            break;
                        else
                            metAster = false;
                    }
                    yyCh = getChar();
                    yyComment.chop(2);
                }
                return Tok_Comment;
            case '"':
                yyCh = getChar();
                while (yyCh != EOF && yyCh != '\n' && yyCh != '"') {
                    if (yyCh == '\\') {
                        yyCh = getChar();
                        if (yyCh == EOF || yyCh == '\n')
                            break;
                        yyString.append(QLatin1Char('\\'));
                    }
                    yyString.append(yyCh);
                    yyCh = getChar();
                }

                if (yyCh != '"')
                    qWarning("%s:%d: Unterminated C++ string\n",
                              qPrintable(yyFileName), yyLineNo);
                else
                    yyCh = getChar();
                return Tok_String;
            case '-':
                yyCh = getChar();
                if (yyCh == '>') {
                    yyCh = getChar();
                    return Tok_Arrow;
                }
                break;
            case ':':
                yyCh = getChar();
                if (yyCh == ':') {
                    yyCh = getChar();
                    return Tok_ColonColon;
                }
                return Tok_Colon;
            // Incomplete: '<' might be part of '<=' or of template syntax.
            // The main intent of not completely ignoring it is to break
            // parsing of things like   std::cout << QObject::tr()  as
            // context std::cout::QObject (see Task 161106)
            case '=':
                yyCh = getChar();
                return Tok_Equals;
            case '>':
            case '<':
                yyCh = getChar();
                return Tok_Other;
            case '\'':
                yyCh = getChar();
                if (yyCh == '\\')
                    yyCh = getChar();

                forever {
                    if (yyCh == EOF || yyCh == '\n') {
                        qWarning("%s:%d: Unterminated C++ character\n",
                                  qPrintable(yyFileName), yyLineNo);
                        break;
                    }
                    yyCh = getChar();
                    if (yyCh == '\'') {
                        yyCh = getChar();
                        break;
                    }
                }
                break;
            case '{':
                if (yyBraceDepth == 0)
                    yyBraceLineNo = yyCurLineNo;
                yyBraceDepth++;
                yyCh = getChar();
                return Tok_LeftBrace;
            case '}':
                if (yyBraceDepth == yyMinBraceDepth) {
                    if (!inDefine)
                        qWarning("%s:%d: Excess closing brace in C++ code"
                                  " (or abuse of the C++ preprocessor)\n",
                                  qPrintable(yyFileName), yyCurLineNo);
                    // Avoid things getting messed up even more
                    yyCh = getChar();
                    return Tok_Semicolon;
                }
                yyBraceDepth--;
                yyCh = getChar();
                return Tok_RightBrace;
            case '(':
                if (yyParenDepth == 0)
                    yyParenLineNo = yyCurLineNo;
                yyParenDepth++;
                yyCh = getChar();
                return Tok_LeftParen;
            case ')':
                if (yyParenDepth == 0)
                    qWarning("%s:%d: Excess closing parenthesis in C++ code"
                             " (or abuse of the C++ preprocessor)\n",
                             qPrintable(yyFileName), yyCurLineNo);
                else
                    yyParenDepth--;
                yyCh = getChar();
                return Tok_RightParen;
            case ',':
                yyCh = getChar();
                return Tok_Comma;
            case ';':
                yyCh = getChar();
                return Tok_Semicolon;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                {
                    QByteArray ba;
                    ba += yyCh;
                    yyCh = getChar();
                    bool hex = yyCh == 'x';
                    if (hex) {
                        ba += yyCh;
                        yyCh = getChar();
                    }
                    while (hex ? isxdigit(yyCh) : isdigit(yyCh)) {
                        ba += yyCh;
                        yyCh = getChar();
                    }
                    bool ok;
                    yyInteger = ba.toLongLong(&ok);
                    if (ok)
                        return Tok_Integer;
                    break;
                }
            default:
                yyCh = getChar();
                break;
            }
        }
    }
    return Tok_Eof;
}

/*
  The second part of this source file are namespace/class related
  utilities for the third part.
*/

void CppParser::saveState(SavedState *state)
{
    state->namespaces = stringListifyNamespace(namespaces);
    state->namespaceDepths = namespaceDepths;
    state->functionContext = stringListifyNamespace(functionContext);
    state->functionContextUnresolved = functionContextUnresolved;
    state->pendingContext = pendingContext;
}

void CppParser::loadState(const SavedState *state)
{
    namespaces = resolveNamespaces(state->namespaces);
    namespaceDepths = state->namespaceDepths;
    functionContext = resolveNamespaces(state->functionContext);
    functionContextUnresolved = state->functionContextUnresolved;
    pendingContext = state->pendingContext;
}

bool ParseResults::detachNamespace(Namespace **that)
{
    if ((*that)->fileId != rootNamespace.fileId) {
        Namespace *newThat = new Namespace;
        *newThat = **that;
        newThat->fileId = rootNamespace.fileId;
        *that = newThat;
        return true;
    }
    return false;
}

Namespace *ParseResults::include(Namespace *that, const Namespace *other)
{
    Namespace *origThat = that;
    foreach (Namespace *otherSub, other->children) {
        if (Namespace *thisSub = that->children.value(otherSub->name)) {
            // Don't make these cause a detach - it's best
            // (though not necessary) if they are shared
            thisSub->isClass |= otherSub->isClass;
            thisSub->hasTrFunctions |= otherSub->hasTrFunctions;
            thisSub->needsTrFunctions |= otherSub->needsTrFunctions;
            thisSub->complained |= otherSub->complained;

            if (Namespace *newSub = include(thisSub, otherSub)) {
                thisSub = newSub;
                detachNamespace(&that);
                that->children[thisSub->name] = thisSub;
            }
        } else {
            detachNamespace(&that);
            that->children[otherSub->name] = otherSub;
        }
    }
    if ((that->aliases != other->aliases && !other->aliases.isEmpty())
        || (that->usings != other->usings && !other->usings.isEmpty())) {
        detachNamespace(&that);
        that->aliases.unite(other->aliases);
        that->usings.unite(other->usings);
    }
    return (that != origThat) ? that : 0;
}

void ParseResults::unite(const ParseResults *other)
{
    allIncludes.unite(other->allIncludes);
    include(&rootNamespace, &other->rootNamespace);
}

void CppParser::modifyNamespace(NamespaceList *namespaces)
{
    Namespace *pns = 0;
    int i = namespaces->count();
    forever {
        --i;
        Namespace *ns = namespaces->at(i);
        bool detached = results->detachNamespace(&ns);
        if (pns)
            ns->children[pns->name] = pns;
        if (!detached) // Known to be true for root namespace
            return;
        pns = ns;
        namespaces->replace(i, ns);
    }
}

QString CppParser::stringifyNamespace(const NamespaceList &namespaces)
{
    QString ret;
    for (int i = 1; i < namespaces.count(); ++i) {
        if (i > 1)
            ret += QLatin1String("::");
        ret += namespaces.at(i)->name;
    }
    return ret;
}

QStringList CppParser::stringListifyNamespace(const NamespaceList &namespaces)
{
    QStringList ret;
    for (int i = 1; i < namespaces.count(); ++i)
        ret << namespaces.at(i)->name;
    return ret;
}

// This function is called only with known-existing namespaces
NamespaceList CppParser::resolveNamespaces(const QStringList &segments)
{
    NamespaceList ret;
    Namespace *ns = &results->rootNamespace;
    ret << ns;
    foreach (const QString &seg, segments) {
        ns = ns->children.value(seg);
        ret << ns;
    }
    return ret;
}

bool CppParser::qualifyOne(const NamespaceList &namespaces, int nsIdx, const QString &segment,
                           NamespaceList *resolved)
{
    const Namespace *ns = namespaces.at(nsIdx);
    QMap<QString, Namespace *>::ConstIterator cnsi = ns->children.constFind(segment);
    if (cnsi != ns->children.constEnd()) {
        *resolved = namespaces.mid(0, nsIdx + 1);
        *resolved << *cnsi;
        return true;
    }
    QMap<QString, QStringList>::ConstIterator nsai = ns->aliases.constFind(segment);
    if (nsai != ns->aliases.constEnd()) {
        *resolved = resolveNamespaces(*nsai);
        return true;
    }
    foreach (const QStringList &use, ns->usings) {
        NamespaceList usedNs = resolveNamespaces(use);
        if (qualifyOne(usedNs, usedNs.count() - 1, segment, resolved))
            return true;
    }
    return false;
}

bool CppParser::fullyQualify(const NamespaceList &namespaces, const QStringList &segments,
                             bool isDeclaration,
                             NamespaceList *resolved, QStringList *unresolved)
{
    int nsIdx;
    int initSegIdx;

    if (segments.first().isEmpty()) {
        // fully qualified
        if (segments.count() == 1) {
            resolved->clear();
            *resolved << &results->rootNamespace;
            return true;
        }
        initSegIdx = 1;
        nsIdx = 0;
    } else {
        initSegIdx = 0;
        nsIdx = namespaces.count() - 1;
    }

    do {
        if (qualifyOne(namespaces, nsIdx, segments[initSegIdx], resolved)) {
            int segIdx = initSegIdx;
            while (++segIdx < segments.count()) {
                if (!qualifyOne(*resolved, resolved->count() - 1, segments[segIdx], resolved)) {
                    if (unresolved)
                        *unresolved = segments.mid(segIdx);
                    return false;
                }
            }
            return true;
        }
    } while (!isDeclaration && --nsIdx >= 0);
    resolved->clear();
    *resolved << &results->rootNamespace;
    if (unresolved)
        *unresolved = segments.mid(initSegIdx);
    return false;
}

void CppParser::enterNamespace(NamespaceList *namespaces, const QString &name)
{
    Namespace *ns = namespaces->last()->children.value(name);
    if (!ns) {
        ns = new Namespace;
        ns->fileId = results->rootNamespace.fileId;
        ns->name = name;
        modifyNamespace(namespaces);
        namespaces->last()->children[name] = ns;
    }
    *namespaces << ns;
}

void CppParser::truncateNamespaces(NamespaceList *namespaces, int length)
{
    if (namespaces->count() > length)
        namespaces->erase(namespaces->begin() + length, namespaces->end());
}

/*
  Functions for processing include files.
*/

ParseResultHash &CppFiles::parsedFiles()
{
    static ParseResultHash parsed;

    return parsed;
}

QSet<QString> &CppFiles::blacklistedFiles()
{
    static QSet<QString> blacklisted;

    return blacklisted;
}

const ParseResults *CppFiles::getResults(const QString &cleanFile)
{
    ParseResultHash::ConstIterator it = parsedFiles().find(cleanFile);
    if (it == parsedFiles().constEnd())
        return 0;
    return *it;
}

void CppFiles::setResults(const QString &cleanFile, const ParseResults *results)
{
    parsedFiles().insert(cleanFile, results);
}

bool CppFiles::isBlacklisted(const QString &cleanFile)
{
    return blacklistedFiles().contains(cleanFile);
}

void CppFiles::setBlacklisted(const QString &cleanFile)
{
    blacklistedFiles().insert(cleanFile);
}

void CppParser::processInclude(const QString &file, ConversionData &cd,
                               QSet<QString> &inclusions)
{
    QString cleanFile = QDir::cleanPath(file);

    if (inclusions.contains(cleanFile)) {
        qWarning("%s:%d: circular inclusion of %s\n",
                 qPrintable(yyFileName), yyLineNo, qPrintable(cleanFile));
        return;
    }

    // If the #include is in any kind of namespace, has been blacklisted previously,
    // or is not a header file (stdc++ extensionless or *.h*), then really include
    // it. Otherwise it is safe to process it stand-alone and re-use the parsed
    // namespace data for inclusion into other files.
    bool isIndirect = false;
    if (namespaces.count() == 1 && functionContext.count() == 1
        && functionContextUnresolved.isEmpty() && pendingContext.isEmpty()
        && !CppFiles::isBlacklisted(cleanFile)) {
        QString fileExt = QFileInfo(cleanFile).suffix();
        if (fileExt.isEmpty() || fileExt.startsWith(QLatin1Char('h'), Qt::CaseInsensitive)) {

            if (results->allIncludes.contains(cleanFile))
                return;
            results->allIncludes.insert(cleanFile);

            if (const ParseResults *res = CppFiles::getResults(cleanFile)) {
                results->unite(res);
                return;
            }

            isIndirect = true;
        }
    }

    QFile f(cleanFile);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning("%s:%d: Cannot open %s: %s\n",
                 qPrintable(yyFileName), yyLineNo,
                 qPrintable(cleanFile), qPrintable(f.errorString()));
        return;
    }

    QTextStream ts(&f);
    ts.setCodec(yySourceCodec);
    ts.setAutoDetectUnicode(true);

    inclusions.insert(cleanFile);
    if (isIndirect) {
        CppParser parser;
        foreach (const QString &projectRoot, cd.m_projectRoots)
            if (cleanFile.startsWith(projectRoot)) {
                parser.setTranslator(new Translator);
                break;
            }
        parser.setInput(ts, cleanFile);
        parser.parse(cd.m_defaultContext, cd, inclusions);
        CppFiles::setResults(cleanFile, parser.getResults());
        results->unite(parser.results);
    } else {
        CppParser parser(results);
        parser.namespaces = namespaces;
        parser.functionContext = functionContext;
        parser.functionContextUnresolved = functionContextUnresolved;
        parser.pendingContext = pendingContext;
        parser.setInput(ts, cleanFile);
        parser.parseInternal(cd, inclusions);
        // Don't wreak havoc if not enough braces were found.
        truncateNamespaces(&parser.namespaces, namespaces.count());
        truncateNamespaces(&parser.functionContext, functionContext.count());
        // Copy them back - the pointers might have changed.
        namespaces = parser.namespaces;
        functionContext = parser.functionContext;
        // Avoid that messages obtained by direct scanning are used
        CppFiles::setBlacklisted(cleanFile);
    }
    inclusions.remove(cleanFile);
}

/*
  The third part of this source file is the parser. It accomplishes
  a very easy task: It finds all strings inside a tr() or translate()
  call, and possibly finds out the context of the call. It supports
  three cases: (1) the context is specified, as in
  FunnyDialog::tr("Hello") or translate("FunnyDialog", "Hello");
  (2) the call appears within an inlined function; (3) the call
  appears within a function defined outside the class definition.
*/

bool CppParser::match(uint t)
{
    bool matches = (yyTok == t);
    if (matches)
        yyTok = getToken();
    return matches;
}

bool CppParser::matchString(QString *s)
{
    bool matches = (yyTok == Tok_String);
    s->clear();
    while (yyTok == Tok_String) {
        *s += yyString;
        do {
            yyTok = getToken();
        } while (yyTok == Tok_Comment);
    }
    return matches;
}

bool CppParser::matchEncoding(bool *utf8)
{
    STRING(QApplication);
    STRING(QCoreApplication);
    STRING(UnicodeUTF8);
    STRING(DefaultCodec);
    STRING(CodecForTr);

    if (yyTok != Tok_Ident)
        return false;
    if (yyIdent == strQApplication || yyIdent == strQCoreApplication) {
        yyTok = getToken();
        if (yyTok == Tok_ColonColon)
            yyTok = getToken();
    }
    if (yyIdent == strUnicodeUTF8) {
        *utf8 = true;
        yyTok = getToken();
        return true;
    }
    if (yyIdent == strDefaultCodec || yyIdent == strCodecForTr) {
        *utf8 = false;
        yyTok = getToken();
    return true;
    }
    return false;
}

bool CppParser::matchInteger(qlonglong *number)
{
    bool matches = (yyTok == Tok_Integer);
    if (matches) {
        yyTok = getToken();
        *number = yyInteger;
    }
    return matches;
}

bool CppParser::matchStringOrNull(QString *s)
{
    bool matches = matchString(s);
    qlonglong num = 0;
    if (!matches)
        matches = matchInteger(&num);
    return matches && num == 0;
}

/*
 * match any expression that can return a number, which can be
 * 1. Literal number (e.g. '11')
 * 2. simple identifier (e.g. 'm_count')
 * 3. simple function call (e.g. 'size()' )
 * 4. function call on an object (e.g. 'list.size()')
 * 5. function call on an object (e.g. 'list->size()')
 *
 * Other cases:
 * size(2,4)
 * list().size()
 * list(a,b).size(2,4)
 * etc...
 */
bool CppParser::matchExpression()
{
    if (match(Tok_Integer))
        return true;

    int parenlevel = 0;
    while (match(Tok_Ident) || parenlevel > 0) {
        if (yyTok == Tok_RightParen) {
            if (parenlevel == 0) break;
            --parenlevel;
            yyTok = getToken();
        } else if (yyTok == Tok_LeftParen) {
            yyTok = getToken();
            if (yyTok == Tok_RightParen) {
                yyTok = getToken();
            } else {
                ++parenlevel;
            }
        } else if (yyTok == Tok_Ident) {
            continue;
        } else if (yyTok == Tok_Arrow) {
            yyTok = getToken();
        } else if (parenlevel == 0) {
            return false;
        }
    }
    return true;
}

QString CppParser::transcode(const QString &str, bool utf8)
{
    static const char tab[] = "abfnrtv";
    static const char backTab[] = "\a\b\f\n\r\t\v";
    const QString in = (!utf8 || yySourceIsUnicode)
        ? str : QString::fromUtf8(yySourceCodec->fromUnicode(str).data());
    QString out;

    out.reserve(in.length());
    for (int i = 0; i < in.length();) {
        ushort c = in[i++].unicode();
        if (c == '\\') {
            if (i >= in.length())
                break;
            c = in[i++].unicode();

            if (c == '\n')
                continue;

            if (c == 'x') {
                QByteArray hex;
                while (i < in.length() && isxdigit((c = in[i].unicode()))) {
                    hex += c;
                    i++;
                }
                out += hex.toUInt(0, 16);
            } else if (c >= '0' && c < '8') {
                QByteArray oct;
                int n = 0;
                oct += c;
                while (n < 2 && i < in.length() && (c = in[i].unicode()) >= '0' && c < '8') {
                    i++;
                    n++;
                    oct += c;
                }
                out += oct.toUInt(0, 8);
            } else {
                const char *p = strchr(tab, c);
                out += QChar(QLatin1Char(!p ? c : backTab[p - tab]));
            }
        } else {
            out += c;
        }
    }
    return out;
}

void CppParser::recordMessage(
    int line, const QString &context, const QString &text, const QString &comment,
    const QString &extracomment, const QString &msgid, const TranslatorMessage::ExtraData &extra,
    bool utf8, bool plural)
{
    TranslatorMessage msg(
        transcode(context, utf8), transcode(text, utf8), transcode(comment, utf8), QString(),
        yyFileName, line, QStringList(),
        TranslatorMessage::Unfinished, plural);
    msg.setExtraComment(transcode(extracomment.simplified(), utf8));
    msg.setId(msgid);
    msg.setExtras(extra);
    if ((utf8 || yyForceUtf8) && !yyCodecIsUtf8 && msg.needs8Bit())
        msg.setUtf8(true);
    results->tor->append(msg);
}

void CppParser::parse(const QString &initialContext, ConversionData &cd,
                      QSet<QString> &inclusions)
{
    if (results->tor)
        yyCodecIsUtf8 = (results->tor->codecName() == "UTF-8");

    namespaces << &results->rootNamespace;
    functionContext = namespaces;
    functionContextUnresolved = initialContext;

    parseInternal(cd, inclusions);
}

void CppParser::parseInternal(ConversionData &cd, QSet<QString> &inclusions)
{
    static QString strColons(QLatin1String("::"));

    QString context;
    QString text;
    QString comment;
    QString extracomment;
    QString msgid;
    QString sourcetext;
    TranslatorMessage::ExtraData extra;
    QString prefix;
#ifdef DIAGNOSE_RETRANSLATABILITY
    QString functionName;
#endif
    int line;
    bool utf8;
    bool yyTokColonSeen = false; // Start of c'tor's initializer list

    yyCh = getChar();
    yyTok = getToken();
    while (yyTok != Tok_Eof) {
        //qDebug() << "TOKEN: " << yyTok;
        switch (yyTok) {
        case Tok_QuotedInclude: {
            text = QDir(QFileInfo(yyFileName).absolutePath()).absoluteFilePath(yyString);
            if (QFileInfo(text).isFile()) {
                processInclude(text, cd, inclusions);
                yyTok = getToken();
                break;
            }
        }
        /* fall through */
        case Tok_AngledInclude: {
            QStringList cSources = cd.m_allCSources.values(yyString);
            if (!cSources.isEmpty()) {
                foreach (const QString &cSource, cSources)
                    processInclude(cSource, cd, inclusions);
                goto incOk;
            }
            foreach (const QString &incPath, cd.m_includePath) {
                text = QDir(incPath).absoluteFilePath(yyString);
                if (QFileInfo(text).isFile()) {
                    processInclude(text, cd, inclusions);
                    goto incOk;
                }
            }
          incOk:
            yyTok = getToken();
            break;
        }
        case Tok_friend:
            yyTok = getToken();
            // Ensure that these don't end up being interpreted as forward declarations
            // (they are forwards, but with different namespacing).
            if (yyTok == Tok_class)
                yyTok = getToken();
            break;
        case Tok_class:
            yyTokColonSeen = false;
            /*
              Partial support for inlined functions.
            */
            yyTok = getToken();
            if (yyBraceDepth == namespaceDepths.count() && yyParenDepth == 0) {
                QStringList fct;
                do {
                    /*
                      This code should execute only once, but we play
                      safe with impure definitions such as
                      'class Q_EXPORT QMessageBox', in which case
                      'QMessageBox' is the class name, not 'Q_EXPORT'.
                    */
                    fct = QStringList(yyIdent);
                    yyTok = getToken();
                } while (yyTok == Tok_Ident);
                while (yyTok == Tok_ColonColon) {
                    yyTok = getToken();
                    if (yyTok != Tok_Ident)
                        break; // Oops ...
                    fct += yyIdent;
                    yyTok = getToken();
                }
                if (fct.count() > 1) {
                    // Forward-declared class definitions can be namespaced
                    NamespaceList nsl;
                    if (!fullyQualify(namespaces, fct, true, &nsl, 0)) {
                        qWarning("%s:%d: Ignoring definition of undeclared qualified class\n",
                                 qPrintable(yyFileName), yyLineNo);
                        break;
                    }
                    namespaceDepths.push(namespaces.count());
                    namespaces = nsl;
                } else {
                    namespaceDepths.push(namespaces.count());
                    enterNamespace(&namespaces, fct.first());
                }
                namespaces.last()->isClass = true;

                while (yyTok == Tok_Comment)
                    yyTok = getToken();
                if (yyTok == Tok_Colon) {
                    // Skip any token until '{' since lupdate might do things wrong if it finds
                    // a '::' token here.
                    do {
                        yyTok = getToken();
                    } while (yyTok != Tok_LeftBrace && yyTok != Tok_Eof);
                } else {
                    if (yyTok != Tok_LeftBrace) {
                        // Obviously a forward decl
                        truncateNamespaces(&namespaces, namespaceDepths.pop());
                        break;
                    }
                }

                functionContext = namespaces;
                functionContextUnresolved.clear(); // Pointless
                prospectiveContext.clear();
                pendingContext.clear();
            }
            break;
        case Tok_namespace:
            yyTokColonSeen = false;
            yyTok = getToken();
            if (yyTok == Tok_Ident) {
                QString ns = yyIdent;
                yyTok = getToken();
                if (yyTok == Tok_LeftBrace) {
                    namespaceDepths.push(namespaces.count());
                    enterNamespace(&namespaces, ns);
                    yyTok = getToken();
                } else if (yyTok == Tok_Equals) {
                    // e.g. namespace Is = OuterSpace::InnerSpace;
                    QStringList fullName;
                    yyTok = getToken();
                    if (yyTok == Tok_ColonColon)
                        fullName.append(QString());
                    while (yyTok == Tok_ColonColon || yyTok == Tok_Ident) {
                        if (yyTok == Tok_Ident)
                            fullName.append(yyIdent);
                        yyTok = getToken();
                    }
                    if (fullName.isEmpty())
                        break;
                    NamespaceList nsl;
                    if (fullyQualify(namespaces, fullName, false, &nsl, 0)) {
                        modifyNamespace(&namespaces);
                        namespaces.last()->aliases.insert(ns, stringListifyNamespace(nsl));
                    }
                }
            } else if (yyTok == Tok_LeftBrace) {
                // Anonymous namespace
                namespaceDepths.push(namespaces.count());
                yyTok = getToken();
            }
            break;
        case Tok_using:
            yyTok = getToken();
            if (yyTok == Tok_namespace) {
                QStringList fullName;
                yyTok = getToken();
                if (yyTok == Tok_ColonColon)
                    fullName.append(QString());
                while (yyTok == Tok_ColonColon || yyTok == Tok_Ident) {
                    if (yyTok == Tok_Ident)
                        fullName.append(yyIdent);
                    yyTok = getToken();
                }
                NamespaceList nsl;
                QStringList unresolved;
                if (fullyQualify(namespaces, fullName, false, &nsl, &unresolved)) {
                    modifyNamespace(&namespaces);
                    namespaces.last()->usings.insert(stringListifyNamespace(nsl));
                }
            } else {
                QStringList fullName;
                if (yyTok == Tok_ColonColon)
                    fullName.append(QString());
                while (yyTok == Tok_ColonColon || yyTok == Tok_Ident) {
                    if (yyTok == Tok_Ident)
                        fullName.append(yyIdent);
                    yyTok = getToken();
                }
                if (fullName.isEmpty())
                    break;
                NamespaceList nsl;
                if (fullyQualify(namespaces, fullName, false, &nsl, 0)) {
                    modifyNamespace(&namespaces);
                    namespaces.last()->aliases.insert(nsl.last()->name, stringListifyNamespace(nsl));
                }
            }
            break;
        case Tok_tr:
        case Tok_trUtf8:
            if (!results->tor)
                goto case_default;
            if (!sourcetext.isEmpty())
                qWarning("%s:%d: //%% cannot be used with tr() / QT_TR_NOOP(). Ignoring\n",
                         qPrintable(yyFileName), yyLineNo);
            utf8 = (yyTok == Tok_trUtf8);
            line = yyLineNo;
            yyTok = getToken();
            if (match(Tok_LeftParen) && matchString(&text) && !text.isEmpty()) {
                comment.clear();
                bool plural = false;

                if (match(Tok_RightParen)) {
                    // no comment
                } else if (match(Tok_Comma) && matchStringOrNull(&comment)) {   //comment
                    if (match(Tok_RightParen)) {
                        // ok,
                    } else if (match(Tok_Comma)) {
                        plural = true;
                    }
                }
                if (!pendingContext.isEmpty()) {
                    QStringList unresolved;
                    if (!fullyQualify(namespaces, pendingContext.split(strColons), true,
                                      &functionContext, &unresolved)) {
                        functionContextUnresolved = unresolved.join(strColons);
                        qWarning("%s:%d: Qualifying with unknown namespace/class %s::%s\n",
                                 qPrintable(yyFileName), yyLineNo,
                                 qPrintable(stringifyNamespace(functionContext)),
                                 qPrintable(unresolved.first()));
                    }
                    pendingContext.clear();
                }
                if (prefix.isEmpty()) {
                    if (functionContextUnresolved.isEmpty()) {
                        int idx = functionContext.length();
                        if (idx < 2) {
                            qWarning("%s:%d: tr() cannot be called without context\n",
                                     qPrintable(yyFileName), yyLineNo);
                            break;
                        }
                        while (!functionContext.at(idx - 1)->hasTrFunctions) {
                            if (idx == 1 || !functionContext.at(idx - 2)->isClass) {
                                idx = functionContext.length();
                                if (!functionContext.last()->complained) {
                                    qWarning("%s:%d: Class '%s' lacks Q_OBJECT macro\n",
                                             qPrintable(yyFileName), yyLineNo,
                                             qPrintable(stringifyNamespace(functionContext)));
                                    functionContext.last()->complained = true;
                                }
                                break;
                            }
                            --idx;
                        }
                        context.clear();
                        for (int i = 1;;) {
                            context += functionContext.at(i)->name;
                            if (++i == idx)
                                break;
                            context += strColons;
                        }
                    } else {
                        context = (stringListifyNamespace(functionContext)
                                   << functionContextUnresolved).join(strColons);
                    }
                } else {
#ifdef DIAGNOSE_RETRANSLATABILITY
                    int last = prefix.lastIndexOf(strColons);
                    QString className = prefix.mid(last == -1 ? 0 : last + 2);
                    if (!className.isEmpty() && className == functionName) {
                        qWarning("%s::%d: It is not recommended to call tr() from within a constructor '%s::%s' ",
                                  qPrintable(yyFileName), yyLineNo,
                                  className.constData(), functionName.constData());
                    }
#endif
                    prefix.chop(2);
                    NamespaceList nsl;
                    QStringList unresolved;
                    if (fullyQualify(functionContext, prefix.split(strColons), false, &nsl, &unresolved)) {
                        if (!nsl.last()->hasTrFunctions && !nsl.last()->complained) {
                            qWarning("%s:%d: Class '%s' lacks Q_OBJECT macro\n",
                                     qPrintable(yyFileName), yyLineNo,
                                     qPrintable(stringifyNamespace(nsl)));
                            nsl.last()->complained = true;
                        }
                        context = stringifyNamespace(nsl);
                    } else {
                        context = (stringListifyNamespace(nsl) + unresolved).join(strColons);
                    }
                    prefix.clear();
                }

                recordMessage(line, context, text, comment, extracomment, msgid, extra, utf8, plural);
            }
            extracomment.clear();
            msgid.clear();
            extra.clear();
            break;
        case Tok_translateUtf8:
        case Tok_translate:
            if (!results->tor)
                goto case_default;
            if (!sourcetext.isEmpty())
                qWarning("%s:%d: //%% cannot be used with translate() / QT_TRANSLATE_NOOP(). Ignoring\n",
                         qPrintable(yyFileName), yyLineNo);
            utf8 = (yyTok == Tok_translateUtf8);
            line = yyLineNo;
            yyTok = getToken();
            if (match(Tok_LeftParen)
                && matchString(&context)
                && match(Tok_Comma)
                && matchString(&text) && !text.isEmpty())
            {
                comment.clear();
                bool plural = false;
                if (!match(Tok_RightParen)) {
                    // look for comment
                    if (match(Tok_Comma) && matchStringOrNull(&comment)) {
                        if (!match(Tok_RightParen)) {
                            // look for encoding
                            if (match(Tok_Comma)) {
                                if (matchEncoding(&utf8)) {
                                    if (!match(Tok_RightParen)) {
                                        // look for the plural quantifier,
                                        // this can be a number, an identifier or
                                        // a function call,
                                        // so for simplicity we mark it as plural if
                                        // we know we have a comma instead of an
                                        // right parentheses.
                                        plural = match(Tok_Comma);
                                    }
                                } else {
                                    // This can be a QTranslator::translate("context",
                                    // "source", "comment", n) plural translation
                                    if (matchExpression() && match(Tok_RightParen)) {
                                        plural = true;
                                    } else {
                                        break;
                                    }
                                }
                            } else {
                                break;
                            }
                        }
                    } else {
                        break;
                    }
                }
                recordMessage(line, context, text, comment, extracomment, msgid, extra, utf8, plural);
            }
            extracomment.clear();
            msgid.clear();
            extra.clear();
            break;
        case Tok_trid:
            if (!results->tor)
                goto case_default;
            if (sourcetext.isEmpty()) {
                yyTok = getToken();
            } else {
                if (!msgid.isEmpty())
                    qWarning("%s:%d: //= cannot be used with qtTrId() / QT_TRID_NOOP(). Ignoring\n",
                             qPrintable(yyFileName), yyLineNo);
                //utf8 = false; // Maybe use //%% or something like that
                line = yyLineNo;
                yyTok = getToken();
                if (match(Tok_LeftParen) && matchString(&msgid) && !msgid.isEmpty()) {
                    bool plural = match(Tok_Comma);
                    recordMessage(line, QString(), sourcetext, QString(), extracomment,
                                  msgid, extra, false, plural);
                }
                sourcetext.clear();
            }
            extracomment.clear();
            msgid.clear();
            extra.clear();
            break;
        case Tok_Q_DECLARE_TR_FUNCTIONS:
        case Tok_Q_OBJECT:
            namespaces.last()->hasTrFunctions = true;
            yyTok = getToken();
            break;
        case Tok_Ident:
            prefix += yyIdent;
            yyTok = getToken();
            if (yyTok != Tok_ColonColon) {
                prefix.clear();
                if (yyTok == Tok_Ident && !yyParenDepth)
                    prospectiveContext.clear();
            }
            break;
        case Tok_Comment:
            if (!results->tor)
                goto case_default;
            if (yyComment.startsWith(QLatin1Char(':'))) {
                yyComment.remove(0, 1);
                extracomment.append(yyComment);
            } else if (yyComment.startsWith(QLatin1Char('='))) {
                yyComment.remove(0, 1);
                msgid = yyComment.simplified();
            } else if (yyComment.startsWith(QLatin1Char('~'))) {
                yyComment.remove(0, 1);
                yyComment = yyComment.trimmed();
                int k = yyComment.indexOf(QLatin1Char(' '));
                if (k > -1)
                    extra.insert(yyComment.left(k), yyComment.mid(k + 1).trimmed());
            } else if (yyComment.startsWith(QLatin1Char('%'))) {
                int p = 1, c;
                forever {
                    if (p >= yyComment.length())
                        break;
                    c = yyComment.unicode()[p++].unicode();
                    if (isspace(c))
                        continue;
                    if (c != '"') {
                        qWarning("%s:%d: Unexpected character in meta string\n",
                                 qPrintable(yyFileName), yyLineNo);
                        break;
                    }
                    forever {
                        if (p >= yyComment.length()) {
                          whoops:
                            qWarning("%s:%d: Unterminated meta string\n",
                                     qPrintable(yyFileName), yyLineNo);
                            break;
                        }
                        c = yyComment.unicode()[p++].unicode();
                        if (c == '"')
                            break;
                        if (c == '\\') {
                            if (p >= yyComment.length())
                                goto whoops;
                            c = yyComment.unicode()[p++].unicode();
                            if (c == '\n')
                                goto whoops;
                            sourcetext.append(QLatin1Char('\\'));
                        }
                        sourcetext.append(c);
                    }
                }
            } else {
                comment = yyComment.simplified();
                if (comment.startsWith(QLatin1String(MagicComment))) {
                    comment.remove(0, sizeof(MagicComment) - 1);
                    int k = comment.indexOf(QLatin1Char(' '));
                    if (k == -1) {
                        context = comment;
                    } else {
                        context = comment.left(k);
                        comment.remove(0, k + 1);
                        recordMessage(yyLineNo, context, QString(), comment, extracomment,
                                      QString(), TranslatorMessage::ExtraData(), false, false);
                        extracomment.clear();
                        results->tor->setExtras(extra);
                        extra.clear();
                    }
                }
            }
            yyTok = getToken();
            break;
        case Tok_Arrow:
            yyTok = getToken();
            if (yyTok == Tok_tr || yyTok == Tok_trUtf8)
                qWarning("%s:%d: Cannot invoke tr() like this\n",
                          qPrintable(yyFileName), yyLineNo);
            break;
        case Tok_ColonColon:
            if (yyBraceDepth == namespaceDepths.count() && yyParenDepth == 0 && !yyTokColonSeen)
                prospectiveContext = prefix;
            prefix += strColons;
            yyTok = getToken();
#ifdef DIAGNOSE_RETRANSLATABILITY
            if (yyTok == Tok_Ident && yyBraceDepth == namespaceDepths.count() && yyParenDepth == 0)
                functionName = yyIdent;
#endif
            break;
        case Tok_RightBrace:
            if (yyBraceDepth + 1 == namespaceDepths.count()) {
                // class or namespace
                Namespace *ns = namespaces.last();
                if (ns->needsTrFunctions && !ns->hasTrFunctions && !ns->complained) {
                    qWarning("%s:%d: Class '%s' lacks Q_OBJECT macro\n",
                             qPrintable(yyFileName), yyLineNo,
                             qPrintable(stringifyNamespace(namespaces)));
                    ns->complained = true;
                }
                truncateNamespaces(&namespaces, namespaceDepths.pop());
            }
            if (yyBraceDepth == namespaceDepths.count()) {
                // function, class or namespace
                if (!yyBraceDepth && !directInclude) {
                    truncateNamespaces(&functionContext, 1);
                    functionContextUnresolved = cd.m_defaultContext;
                } else {
                    functionContext = namespaces;
                    functionContextUnresolved.clear();
                }
                pendingContext.clear();
            }
            // fallthrough
        case Tok_Semicolon:
            prospectiveContext.clear();
            prefix.clear();
            extracomment.clear();
            msgid.clear();
            extra.clear();
            yyTokColonSeen = false;
            yyTok = getToken();
            break;
        case Tok_Colon:
            if (!prospectiveContext.isEmpty()
                && yyBraceDepth == namespaceDepths.count() && yyParenDepth == 0)
                pendingContext = prospectiveContext;
            yyTokColonSeen = true;
            yyTok = getToken();
            break;
        case Tok_LeftBrace:
            if (!prospectiveContext.isEmpty()
                && yyBraceDepth == namespaceDepths.count() + 1 && yyParenDepth == 0)
                pendingContext = prospectiveContext;
            // fallthrough
        case Tok_LeftParen:
        case Tok_RightParen:
            yyTokColonSeen = false;
            yyTok = getToken();
            break;
        default:
            if (!yyParenDepth)
                prospectiveContext.clear();
        case_default:
            yyTok = getToken();
            break;
        }
    }

    if (yyBraceDepth != 0)
        qWarning("%s:%d: Unbalanced opening brace in C++ code"
                  " (or abuse of the C++ preprocessor)\n",
                  qPrintable(yyFileName), yyBraceLineNo);
    else if (yyParenDepth != 0)
        qWarning("%s:%d: Unbalanced opening parenthesis in C++ code"
                 " (or abuse of the C++ preprocessor)\n",
                 qPrintable(yyFileName), yyParenLineNo);
}

/*
  Fetches tr() calls in C++ code in UI files (inside "<function>"
  tag). This mechanism is obsolete.
*/
void fetchtrInlinedCpp(const QString &in, Translator &translator, const QString &context)
{
    CppParser parser;
    parser.setInput(in);
    ConversionData cd;
    QSet<QString> inclusions;
    parser.setTranslator(&translator);
    parser.parse(context, cd, inclusions);
    parser.deleteResults();
}

void loadCPP(Translator &translator, const QStringList &filenames, ConversionData &cd)
{
    QByteArray codecName = cd.m_codecForSource.isEmpty()
                            ? translator.codecName() : cd.m_codecForSource;
    QTextCodec *codec = QTextCodec::codecForName(codecName);

    foreach (const QString &filename, filenames) {
        if (CppFiles::getResults(filename) || CppFiles::isBlacklisted(filename))
            continue;

        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            cd.appendError(QString::fromLatin1("Cannot open %1: %2")
                .arg(filename, file.errorString()));
            continue;
        }

        CppParser parser;
        QTextStream ts(&file);
        ts.setCodec(codec);
        ts.setAutoDetectUnicode(true);
        if (ts.codec()->name() == "UTF-16")
            translator.setCodecName("System");
        parser.setInput(ts, filename);
        Translator *tor = new Translator;
        tor->setCodecName(translator.codecName());
        parser.setTranslator(tor);
        QSet<QString> inclusions;
        parser.parse(cd.m_defaultContext, cd, inclusions);
        CppFiles::setResults(filename, parser.getResults());
    }

    foreach (const QString filename, filenames)
        if (!CppFiles::isBlacklisted(filename))
            if (Translator *tor = CppFiles::getResults(filename)->tor)
                foreach (const TranslatorMessage &msg, tor->messages())
                    translator.extend(msg);
}

QT_END_NAMESPACE
