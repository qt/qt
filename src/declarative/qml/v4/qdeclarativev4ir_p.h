/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QDECLARATIVEV4IR_P_H
#define QDECLARATIVEV4IR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qdeclarativejsast_p.h>
#include <private/qdeclarativejsengine_p.h>
#include <private/qdeclarativeparser_p.h>
#include <private/qdeclarativeimport_p.h>
#include <private/qdeclarativeengine_p.h>
#include <private/qdeclarativev4compiler_p.h>

#include <QtCore/qvector.h>

// #define DEBUG_IR_STRUCTURE

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QTextStream;
class QDeclarativeType;

namespace QDeclarativeJS {

namespace IR {

struct BasicBlock;
struct Function;
struct Module;

struct Stmt;
struct Expr;

// expressions
struct Const;
struct String;
struct Name;
struct Temp;
struct Unop;
struct Binop;
struct Call;

// statements
struct Exp;
struct Move;
struct Jump;
struct CJump;
struct Ret;

enum AluOp {
    OpInvalid = 0,

    OpIfTrue,
    OpNot,
    OpUMinus,
    OpUPlus,
    OpCompl,

    OpBitAnd,
    OpBitOr,
    OpBitXor,

    OpAdd,
    OpSub,
    OpMul,
    OpDiv,
    OpMod,

    OpLShift,
    OpRShift,
    OpURShift,

    OpGt,
    OpLt,
    OpGe,
    OpLe,
    OpEqual,
    OpNotEqual,
    OpStrictEqual,
    OpStrictNotEqual,

    OpAnd,
    OpOr
};
AluOp binaryOperator(int op);

enum Type {
    InvalidType,
    UndefinedType,
    NullType,
    VoidType,
    StringType,
    UrlType,
    AnchorLineType,
    SGAnchorLineType,
    AttachType,
    ObjectType,

    FirstNumberType,
    BoolType = FirstNumberType,
    IntType,
    RealType,
    RealNaNType
};
Type maxType(IR::Type left, IR::Type right);

struct ExprVisitor {
    virtual ~ExprVisitor() {}
    virtual void visitConst(Const *) {}
    virtual void visitString(String *) {}
    virtual void visitName(Name *) {}
    virtual void visitTemp(Temp *) {}
    virtual void visitUnop(Unop *) {}
    virtual void visitBinop(Binop *) {}
    virtual void visitCall(Call *) {}
};

struct StmtVisitor {
    virtual ~StmtVisitor() {}
    virtual void visitExp(Exp *) {}
    virtual void visitMove(Move *) {}
    virtual void visitJump(Jump *) {}
    virtual void visitCJump(CJump *) {}
    virtual void visitRet(Ret *) {}
};

struct Expr {
    Type type;

    Expr(Type type): type(type) {}
    virtual ~Expr() {}
    virtual void accept(ExprVisitor *) = 0;
    virtual Const *asConst() { return 0; }
    virtual String *asString() { return 0; }
    virtual Name *asName() { return 0; }
    virtual Temp *asTemp() { return 0; }
    virtual Unop *asUnop() { return 0; }
    virtual Binop *asBinop() { return 0; }
    virtual Call *asCall() { return 0; }
    virtual void dump(QTextStream &out) = 0;
};

struct Const: Expr {
    double value;
    Const(Type type, double value): Expr(type), value(value) {}

    virtual void accept(ExprVisitor *v) { v->visitConst(this); }
    virtual Const *asConst() { return this; }

    virtual void dump(QTextStream &out);
};

struct String: Expr {
    QString value;
    String(const QString &value): Expr(StringType), value(value) {}

    virtual void accept(ExprVisitor *v) { v->visitString(this); }
    virtual String *asString() { return this; }

    virtual void dump(QTextStream &out);
    static QString escape(const QString &s);
};

enum BuiltinSymbol {
    NoBuiltinSymbol,
    MathSinBultinFunction,
    MathCosBultinFunction,
    MathRoundBultinFunction,
    MathFloorBultinFunction,

    MathPIBuiltinConstant
};

struct Name: Expr {
    enum Symbol {
        Unbound,
        IdObject,      // This is a load of a id object.  Storage will always be IdStorage
        AttachType,    // This is a load of an attached object 
        Object,        // XXX what is this for?
        Property,      // This is a load of a regular property
        Slot           // XXX what is this for?
    };

    enum Storage {
        MemberStorage, // This is a property of a previously fetched object
        IdStorage,     // This is a load of a id object.  Symbol will always be IdObject
        RootStorage,   // This is a property of the root object
        ScopeStorage   // This is a property of the scope object
    };

    Name *base;
    QString id;
    Symbol symbol;
    union {
        void *ptr;
        const QMetaObject *meta;
        const QDeclarativeType *declarativeType;
        const QDeclarativeParser::Object *idObject;
    };
    int index;
    Storage storage;
    BuiltinSymbol builtin;
    quint32 line;
    quint32 column;

    Name(Name *base, Type type, const QString &id, Symbol symbol, quint32 line, quint32 column);

    inline bool is(Symbol s) const { return s == symbol; }
    inline bool isNot(Symbol s) const { return s != symbol; }

    virtual void accept(ExprVisitor *v) { v->visitName(this); }
    virtual Name *asName() { return this; }

    virtual void dump(QTextStream &out);
};

struct Temp: Expr {
    int index;
    Temp(Type type, int index): Expr(type), index(index) {}

    virtual void accept(ExprVisitor *v) { v->visitTemp(this); }
    virtual Temp *asTemp() { return this; }

    virtual void dump(QTextStream &out);
};

struct Unop: Expr {
    AluOp op;
    Expr *expr;

    Unop(AluOp op, Expr *expr)
        : Expr(typeForOp(op, expr)), op(op), expr(expr) {}

    virtual void accept(ExprVisitor *v) { v->visitUnop(this); }
    virtual Unop *asUnop() { return this; }

    virtual void dump(QTextStream &out);

private:
    static Type typeForOp(AluOp op, Expr *expr);
};

struct Binop: Expr {
    AluOp op;
    Expr *left;
    Expr *right;
    Binop(AluOp op, Expr *left, Expr *right)
        : Expr(typeForOp(op, left, right)), op(op), left(left), right(right) {}

    virtual void accept(ExprVisitor *v) { v->visitBinop(this); }
    virtual Binop *asBinop() { return this; }

    virtual void dump(QTextStream &out);

private:
    static Type typeForOp(AluOp op, Expr *left, Expr *right);
};

struct Call: Expr {
    Expr *base;
    QVector<Expr *> args;

    Call(Expr *base, const QVector<Expr *> &args)
        : Expr(typeForFunction(base)), base(base), args(args) {}

    virtual void accept(ExprVisitor *v) { v->visitCall(this); }
    virtual Call *asCall() { return this; }

    virtual void dump(QTextStream &out);

private:
    static Type typeForFunction(Expr *base);
};

struct Stmt {
    enum Mode {
        HIR,
        MIR
    };

    virtual ~Stmt() {}
    virtual Stmt *asTerminator() { return 0; }

    virtual void accept(StmtVisitor *) = 0;
    virtual Exp *asExp() { return 0; }
    virtual Move *asMove() { return 0; }
    virtual Jump *asJump() { return 0; }
    virtual CJump *asCJump() { return 0; }
    virtual Ret *asRet() { return 0; }
    virtual void dump(QTextStream &out, Mode mode = HIR) = 0;
};

struct Exp: Stmt {
    Expr *expr;
    Exp(Expr *expr): expr(expr) {}

    virtual void accept(StmtVisitor *v) { v->visitExp(this); }
    virtual Exp *asExp() { return this; }

    virtual void dump(QTextStream &out, Mode);
};

struct Move: Stmt {
    Expr *target;
    Expr *source;
    bool isMoveForReturn;
    Move(Expr *target, Expr *source, bool isMoveForReturn): target(target), source(source), isMoveForReturn(isMoveForReturn) {}

    virtual void accept(StmtVisitor *v) { v->visitMove(this); }
    virtual Move *asMove() { return this; }

    virtual void dump(QTextStream &out, Mode);
};

struct Jump: Stmt {
    BasicBlock *target;
    Jump(BasicBlock *target): target(target) {}

    virtual Stmt *asTerminator() { return this; }

    virtual void accept(StmtVisitor *v) { v->visitJump(this); }
    virtual Jump *asJump() { return this; }

    virtual void dump(QTextStream &out, Mode mode);
};

struct CJump: Stmt {
    Expr *cond;
    BasicBlock *iftrue;
    BasicBlock *iffalse;
    CJump(Expr *cond, BasicBlock *iftrue, BasicBlock *iffalse)
        : cond(cond), iftrue(iftrue), iffalse(iffalse) {}

    virtual Stmt *asTerminator() { return this; }

    virtual void accept(StmtVisitor *v) { v->visitCJump(this); }
    virtual CJump *asCJump() { return this; }

    virtual void dump(QTextStream &out, Mode mode);
};

struct Ret: Stmt {
    Expr *expr;
    Type type;
    quint32 line;
    quint32 column;
    Ret(Expr *expr, Type type, quint32 line, quint32 column): expr(expr), type(type), line(line), column(column) {}

    virtual Stmt *asTerminator() { return this; }

    virtual void accept(StmtVisitor *v) { v->visitRet(this); }
    virtual Ret *asRet() { return this; }

    virtual void dump(QTextStream &out, Mode);
};

struct Function {
    Module *module;
    const NameId *name;
    int tempCount;
    QVector<BasicBlock *> basicBlocks;
    QVector<Expr *> temps;

    template <typename BB> inline BB i(BB i) { basicBlocks.append(i); return i; }
    template <typename E> inline E e(E e) { temps.append(e); return e; }

    Function(Module *module, const NameId *name = 0): module(module), name(name), tempCount(0) {}
    ~Function();

    BasicBlock *newBasicBlock();

    virtual void dump(QTextStream &out);
};

struct BasicBlock {
    Function *function;
    int index;
    QVector<Stmt *> statements;
    int offset;

    BasicBlock(Function *function, int index): function(function), index(index), offset(-1) {}
    ~BasicBlock() { qDeleteAll(statements); }

    template <typename Instr> inline Instr i(Instr i) { statements.append(i); return i; }

    inline bool isEmpty() const {
        return statements.isEmpty();
    }

    inline Stmt *terminator() const {
        if (! statements.isEmpty() && statements.last()->asTerminator() != 0)
            return statements.last();
        return 0;
    }

    inline bool isTerminated() const {
        if (terminator() != 0)
            return true;
        return false;
    }

    Temp *TEMP(Type type, int index);
    Temp *TEMP(Type type);

    Expr *CONST(double value);
    Expr *CONST(Type type, double value);
    Expr *STRING(const QString &value);

    Name *NAME(const QString &id, quint32 line, quint32 column);
    Name *NAME(Name *base, const QString &id, quint32 line, quint32 column);
    Name *SYMBOL(Type type, const QString &id, const QMetaObject *meta, int index, Name::Storage storage, quint32 line, quint32 column);
    Name *SYMBOL(Name *base, Type type, const QString &id, const QMetaObject *meta, int index, quint32 line, quint32 column);
    Name *SYMBOL(Name *base, Type type, const QString &id, const QMetaObject *meta, int index, Name::Storage storage, quint32 line, quint32 column);
    Name *ID_OBJECT(const QString &id, const QDeclarativeParser::Object *object, quint32 line, quint32 column);
    Name *ATTACH_TYPE(const QString &id, const QDeclarativeType *attachType, Name::Storage storage, quint32 line, quint32 column);

    Expr *UNOP(AluOp op, Expr *expr);
    Expr *BINOP(AluOp op, Expr *left, Expr *right);
    Expr *CALL(Expr *base, const QVector<Expr *> &args);

    Stmt *EXP(Expr *expr);
    Stmt *MOVE(Expr *target, Expr *source, bool = false);

    Stmt *JUMP(BasicBlock *target);
    Stmt *CJUMP(Expr *cond, BasicBlock *iftrue, BasicBlock *iffalse);
    Stmt *RET(Expr *expr, Type type, quint32 line, quint32 column);

    virtual void dump(QTextStream &out);
};

struct Module {
    QVector<Function *> functions;

    Module() {}
    ~Module() { qDeleteAll(functions); }

    template <typename BB> inline BB i(BB i) { functions.append(i); return i; }

    Function *newFunction(const NameId *name = 0) { return i(new Function(this, name)); }

    virtual void dump(QTextStream &out);
};

#ifdef DEBUG_IR_STRUCTURE
struct IRDump : public ExprVisitor,
                public StmtVisitor 
{
public:
    IRDump();

    void expression(QDeclarativeJS::IR::Expr *);
    void basicblock(QDeclarativeJS::IR::BasicBlock *);
    void statement(QDeclarativeJS::IR::Stmt *);
    void function(QDeclarativeJS::IR::Function *);
protected:

    const char *indent();

    //
    // expressions
    //
    virtual void visitConst(QDeclarativeJS::IR::Const *e);
    virtual void visitString(QDeclarativeJS::IR::String *e);
    virtual void visitName(QDeclarativeJS::IR::Name *e);
    virtual void visitTemp(QDeclarativeJS::IR::Temp *e);
    virtual void visitUnop(QDeclarativeJS::IR::Unop *e);
    virtual void visitBinop(QDeclarativeJS::IR::Binop *e);
    virtual void visitCall(QDeclarativeJS::IR::Call *e);

    //
    // statements
    //
    virtual void visitExp(QDeclarativeJS::IR::Exp *s);
    virtual void visitMove(QDeclarativeJS::IR::Move *s);
    virtual void visitJump(QDeclarativeJS::IR::Jump *s);
    virtual void visitCJump(QDeclarativeJS::IR::CJump *s);
    virtual void visitRet(QDeclarativeJS::IR::Ret *s);

private:
    int indentSize;
    QByteArray indentData;
    void inc();
    void dec();
};
#endif

} // end of namespace IR

} // end of namespace QDeclarativeJS

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDECLARATIVEV4IR_P_H
