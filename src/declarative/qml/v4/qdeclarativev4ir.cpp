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

#include "qdeclarativev4ir_p.h"

#include <QtCore/qtextstream.h>
#include <QtCore/qdebug.h>
#include <math.h>

QT_BEGIN_NAMESPACE

namespace QDeclarativeJS {
namespace IR {

inline const char *typeName(Type t)
{
    switch (t) {
    case InvalidType: return "invalid";
    case UndefinedType: return "undefined";
    case NullType: return "null";
    case VoidType: return "void";
    case StringType: return "string";
    case UrlType: return "url";
    case AnchorLineType: return "AnchorLine";
    case SGAnchorLineType: return "SGAnchorLine";
    case AttachType: return "AttachType";
    case ObjectType: return "object";
    case BoolType: return "bool";
    case IntType: return "int";
    case RealType: return "qreal";
    case RealNaNType: return "NaN";
    default: return "invalid";
    }
}

IR::Type maxType(IR::Type left, IR::Type right)
{
    if (left == right)
        return left;
    else if (left >= IR::FirstNumberType && right >= IR::FirstNumberType)
        return qMax(left, right);
    else if ((left >= IR::FirstNumberType && right == IR::StringType) ||
             (right >= IR::FirstNumberType && left == IR::StringType))
        return IR::StringType;
    else
        return IR::InvalidType;
}


const char *opname(AluOp op)
{
    switch (op) {
    case OpInvalid: return "?";

    case OpIfTrue: return "(bool)";
    case OpNot: return "!";
    case OpUMinus: return "-";
    case OpUPlus: return "+";
    case OpCompl: return "~";

    case OpBitAnd: return "&";
    case OpBitOr: return "|";
    case OpBitXor: return "^";

    case OpAdd: return "+";
    case OpSub: return "-";
    case OpMul: return "*";
    case OpDiv: return "/";
    case OpMod: return "%";

    case OpLShift: return "<<";
    case OpRShift: return ">>";
    case OpURShift: return ">>>";

    case OpGt: return ">";
    case OpLt: return "<";
    case OpGe: return ">=";
    case OpLe: return "<=";
    case OpEqual: return "==";
    case OpNotEqual: return "!=";
    case OpStrictEqual: return "===";
    case OpStrictNotEqual: return "!==";

    case OpAnd: return "&&";
    case OpOr: return "||";

    default: return "?";

    } // switch
}

AluOp binaryOperator(int op)
{
    switch (static_cast<QSOperator::Op>(op)) {
    case QSOperator::Add: return OpAdd;
    case QSOperator::And: return OpAnd;
    case QSOperator::BitAnd: return OpBitAnd;
    case QSOperator::BitOr: return OpBitOr;
    case QSOperator::BitXor: return OpBitXor;
    case QSOperator::Div: return OpDiv;
    case QSOperator::Equal: return OpEqual;
    case QSOperator::Ge: return OpGe;
    case QSOperator::Gt: return OpGt;
    case QSOperator::Le: return OpLe;
    case QSOperator::LShift: return OpLShift;
    case QSOperator::Lt: return OpLt;
    case QSOperator::Mod: return OpMod;
    case QSOperator::Mul: return OpMul;
    case QSOperator::NotEqual: return OpNotEqual;
    case QSOperator::Or: return OpOr;
    case QSOperator::RShift: return OpRShift;
    case QSOperator::StrictEqual: return OpStrictEqual;
    case QSOperator::StrictNotEqual: return OpStrictNotEqual;
    case QSOperator::Sub: return OpSub;
    case QSOperator::URShift: return OpURShift;
    default: return OpInvalid;
    }
}

void Const::dump(QTextStream &out)
{
    out << value;
}

void String::dump(QTextStream &out)
{
    out << '"' << escape(value) << '"';
}

QString String::escape(const QString &s)
{
    QString r;
    foreach (const QChar &ch, s) {
        if (ch == QLatin1Char('\n'))
            r += QLatin1String("\\n");
        else if (ch == QLatin1Char('\r'))
            r += QLatin1String("\\r");
        else if (ch == QLatin1Char('\\'))
            r += QLatin1String("\\\\");
        else if (ch == QLatin1Char('"'))
            r += QLatin1String("\\\"");
        else if (ch == QLatin1Char('\''))
            r += QLatin1String("\\'");
        else
            r += ch;
    }
    return r;
}

Name::Name(Name *base, Type type, const QString &id, Symbol symbol, quint32 line, quint32 column)
: Expr(type)
  , base(base)
  , id(id)
  , symbol(symbol)
  , ptr(0)
  , index(-1)
  , storage(MemberStorage)
  , builtin(NoBuiltinSymbol)
  , line(line)
  , column(column)
{
    if (id.length() == 8 && id == QLatin1String("Math.sin")) {
        builtin = MathSinBultinFunction;
    } else if (id.length() == 8 && id == QLatin1String("Math.cos")) {
        builtin = MathCosBultinFunction;
    } else if (id.length() == 10 && id == QLatin1String("Math.round")) {
        builtin = MathRoundBultinFunction;
    } else if (id.length() == 10 && id == QLatin1String("Math.floor)")) {
        builtin = MathFloorBultinFunction;
    } else if (id.length() == 7 && id == QLatin1String("Math.PI")) {
        builtin = MathPIBuiltinConstant;
        type = RealType;
    }
}

void Name::dump(QTextStream &out)
{
    if (base) {
        base->dump(out);
        out << '.';
    }

    out << id;
}

void Temp::dump(QTextStream &out)
{
    out << 't' << index;
}

void Unop::dump(QTextStream &out)
{
    out << opname(op);
    expr->dump(out);
}

Type Unop::typeForOp(AluOp op, Expr *expr)
{
    switch (op) {
    case OpIfTrue: return BoolType;
    case OpNot: return BoolType;

    case OpUMinus:
    case OpUPlus:
    case OpCompl:
        return maxType(expr->type, RealType);

    default:
        break;
    }

    return InvalidType;
}

void Binop::dump(QTextStream &out)
{
    left->dump(out);
    out << ' ' << opname(op) << ' ';
    right->dump(out);
}

Type Binop::typeForOp(AluOp op, Expr *left, Expr *right)
{
    if (! (left && right))
        return InvalidType;

    switch (op) {
    case OpInvalid:
        return InvalidType;

    // unary operators
    case OpIfTrue:
    case OpNot:
    case OpUMinus:
    case OpUPlus:
    case OpCompl:
        return InvalidType;

    // bit fields
    case OpBitAnd:
    case OpBitOr:
    case OpBitXor:
        return IntType;

    case OpAdd:
        if (left->type == StringType)
            return StringType;
        return RealType;

    case OpSub:
    case OpMul:
    case OpDiv:
    case OpMod:
        return RealType;

    case OpLShift:
    case OpRShift:
    case OpURShift:
        return IntType;

    case OpAnd:
    case OpOr:
        return BoolType;

    case OpGt:
    case OpLt:
    case OpGe:
    case OpLe:
    case OpEqual:
    case OpNotEqual:
    case OpStrictEqual:
    case OpStrictNotEqual:
        return BoolType;
    } // switch

    return InvalidType;
}

void Call::dump(QTextStream &out)
{
    base->dump(out);
    out << '(';
    for (int i = 0; i < args.size(); ++i) {
        if (i)
            out << ", ";
        args.at(i)->dump(out);
    }
    out << ')';
}

Type Call::typeForFunction(Expr *base)
{
    if (! base)
        return InvalidType;

    if (Name *name = base->asName()) {
        switch (name->builtin) {
        case MathSinBultinFunction:
        case MathCosBultinFunction:
            return RealType;

        case MathRoundBultinFunction:
        case MathFloorBultinFunction:
            return IntType;

        case NoBuiltinSymbol:
        case MathPIBuiltinConstant:
            break;
        }
    } // switch

    return InvalidType;
}

void Exp::dump(QTextStream &out, Mode)
{
    out << "(void) ";
    expr->dump(out);
    out << ';';
}

void Move::dump(QTextStream &out, Mode)
{
    target->dump(out);
    out << " = ";
    if (source->type != target->type)
        out << typeName(source->type) << "_to_" << typeName(target->type) << '(';
    source->dump(out);
    if (source->type != target->type)
        out << ')';
    out << ';';
}

void Jump::dump(QTextStream &out, Mode mode)
{
    Q_UNUSED(mode);
    out << "goto " << 'L' << target << ';';
}

void CJump::dump(QTextStream &out, Mode mode)
{
    Q_UNUSED(mode);
    out << "if (";
    cond->dump(out);
    out << ") goto " << 'L' << iftrue << "; else goto " << 'L' << iffalse << ';';
}

void Ret::dump(QTextStream &out, Mode)
{
    out << "return";
    if (expr) {
        out << ' ';
        expr->dump(out);
    }
    out << ';';
}

Function::~Function()
{
    qDeleteAll(basicBlocks);
    qDeleteAll(temps);
}

BasicBlock *Function::newBasicBlock()
{
    const int index = basicBlocks.size();
    return i(new BasicBlock(this, index));
}

void Function::dump(QTextStream &out)
{
    QString fname;
    if (name)
        fname = name->asString();
    else
        fname = QLatin1String("$anonymous");
    out << "function " << fname << "() {" << endl;
    foreach (BasicBlock *bb, basicBlocks) {
        bb->dump(out);
    }
    out << '}' << endl;
}

Temp *BasicBlock::TEMP(Type type, int index) 
{ 
    return function->e(new Temp(type, index)); 
}

Temp *BasicBlock::TEMP(Type type) 
{ 
    return TEMP(type, function->tempCount++); 
}

Expr *BasicBlock::CONST(double value) 
{ 
    return CONST(IR::RealType, value); 
}

Expr *BasicBlock::CONST(Type type, double value) 
{ 
    return function->e(new Const(type, value)); 
}

Expr *BasicBlock::STRING(const QString &value) 
{ 
    return function->e(new String(value)); 
}

Name *BasicBlock::NAME(const QString &id, quint32 line, quint32 column)
{ 
    return NAME(0, id, line, column);
}

Name *BasicBlock::NAME(Name *base, const QString &id, quint32 line, quint32 column)
{ 
    return function->e(new Name(base, InvalidType, id, Name::Unbound, line, column));
}

Name *BasicBlock::SYMBOL(Type type, const QString &id, const QMetaObject *meta, int index, Name::Storage storage,
                         quint32 line, quint32 column)
{
    Name *name = SYMBOL(/*base = */ 0, type, id, meta, index, line, column);
    name->storage = storage;
    return name;
}

Name *BasicBlock::SYMBOL(Name *base, Type type, const QString &id, const QMetaObject *meta, int index, Name::Storage storage,
                         quint32 line, quint32 column)
{
    Name *name = new Name(base, type, id, Name::Property, line, column);
    name->meta = meta;
    name->index = index;
    name->storage = storage;
    return function->e(name);
}

Name *BasicBlock::SYMBOL(Name *base, Type type, const QString &id, const QMetaObject *meta, int index,
                         quint32 line, quint32 column)
{
    Name *name = new Name(base, type, id, Name::Property, line, column);
    name->meta = meta;
    name->index = index;
    return function->e(name);
}

Name *BasicBlock::ID_OBJECT(const QString &id, const QDeclarativeParser::Object *object, quint32 line, quint32 column)
{
    Name *name = new Name(/*base = */ 0, IR::ObjectType, id, Name::IdObject, line, column);
    name->idObject = object;
    name->index = object->idIndex;
    name->storage = Name::IdStorage;
    return function->e(name);
}

Name *BasicBlock::ATTACH_TYPE(const QString &id, const QDeclarativeType *attachType, Name::Storage storage,
                              quint32 line, quint32 column)
{ 
    Name *name = new Name(/*base = */ 0, IR::AttachType, id, Name::AttachType, line, column);
    name->declarativeType = attachType;
    name->storage = storage;
    return function->e(name);
}


Expr *BasicBlock::UNOP(AluOp op, Expr *expr) 
{ 
    return function->e(new Unop(op, expr)); 
}

Expr *BasicBlock::BINOP(AluOp op, Expr *left, Expr *right)
{
    if (left && right) {
        if (Const *c1 = left->asConst()) {
            if (Const *c2 = right->asConst()) {
                switch (op) {
                case OpAdd: return CONST(c1->value + c2->value);
                case OpAnd: return CONST(c1->value ? c2->value : 0);
                case OpBitAnd: return CONST(int(c1->value) & int(c2->value));
                case OpBitOr: return CONST(int(c1->value) | int(c2->value));
                case OpBitXor: return CONST(int(c1->value) ^ int(c2->value));
                case OpDiv: return CONST(c1->value / c2->value);
                case OpEqual: return CONST(c1->value == c2->value);
                case OpGe: return CONST(c1->value >= c2->value);
                case OpGt: return CONST(c1->value > c2->value);
                case OpLe: return CONST(c1->value <= c2->value);
                case OpLShift: return CONST(int(c1->value) << int(c2->value));
                case OpLt: return CONST(c1->value < c2->value);
                case OpMod: return CONST(::fmod(c1->value, c2->value));
                case OpMul: return CONST(c1->value * c2->value);
                case OpNotEqual: return CONST(c1->value != c2->value);
                case OpOr: return CONST(c1->value ? c1->value : c2->value);
                case OpRShift: return CONST(int(c1->value) >> int(c2->value));
                case OpStrictEqual: return CONST(c1->value == c2->value);
                case OpStrictNotEqual: return CONST(c1->value != c2->value);
                case OpSub: return CONST(c1->value - c2->value);
                case OpURShift: return CONST(unsigned(c1->value) >> int(c2->value));

                case OpIfTrue: // unary ops
                case OpNot:
                case OpUMinus:
                case OpUPlus:
                case OpCompl:
                case OpInvalid:
                    break;
                }
            }
        }
    }

    return function->e(new Binop(op, left, right));
}

Expr *BasicBlock::CALL(Expr *base, const QVector<Expr *> &args) 
{ 
    return function->e(new Call(base, args));
}

Stmt *BasicBlock::EXP(Expr *expr) 
{ 
    return i(new Exp(expr));
}

Stmt *BasicBlock::MOVE(Expr *target, Expr *source, bool isMoveForReturn) 
{ 
    return i(new Move(target, source, isMoveForReturn));
}

Stmt *BasicBlock::JUMP(BasicBlock *target) 
{
    if (isTerminated())
        return 0;
    else
        return i(new Jump(target));
}

Stmt *BasicBlock::CJUMP(Expr *cond, BasicBlock *iftrue, BasicBlock *iffalse) 
{
    if (isTerminated())
        return 0;
    return i(new CJump(cond, iftrue, iffalse));
}

Stmt *BasicBlock::RET(Expr *expr, Type type, quint32 line, quint32 column)
{
    if (isTerminated())
        return 0;
    else
        return i(new Ret(expr, type, line, column));
}

void BasicBlock::dump(QTextStream &out)
{
    out << 'L' << this << ':' << endl;
    foreach (Stmt *s, statements) {
        out << '\t';
        s->dump(out);
        out << endl;
    }
}

void Module::dump(QTextStream &out)
{
    foreach (Function *fun, functions) {
        fun->dump(out);
        out << endl;
    }
}

#ifdef DEBUG_IR_STRUCTURE

static const char *symbolname(Name::Symbol s) 
{
    switch (s) {
    case Name::Unbound:
        return "Unbound";
    case Name::IdObject:
        return "IdObject";
    case Name::AttachType:
        return "AttachType";
    case Name::Object:
        return "Object";
    case Name::Property:
        return "Property";
    case Name::Slot:
        return "Slot";
    default:
        Q_ASSERT(!"Unreachable");
        return "Unknown"; 
    }
}

static const char *storagename(Name::Storage s)
{
    switch (s) {
    case Name::MemberStorage:
        return "MemberStorage";
    case Name::IdStorage:
        return "IdStorage";
    case Name::RootStorage:
        return "RootStorage";
    case Name::ScopeStorage:
        return "ScopeStorage";
    default:
        Q_ASSERT(!"Unreachable");
        return "UnknownStorage";
    }
}

IRDump::IRDump()
: indentSize(0)
{
}

void IRDump::inc()
{
    indentSize++;
    indentData = QByteArray(indentSize * 4, ' ');
}

void IRDump::dec()
{
    indentSize--;
    indentData = QByteArray(indentSize * 4, ' ');
}

void IRDump::dec();

void IRDump::expression(QDeclarativeJS::IR::Expr *e)
{
    inc();

    e->accept(this);

    dec();
}

void IRDump::basicblock(QDeclarativeJS::IR::BasicBlock *b)
{
    inc();

    qWarning().nospace() << indent() << "BasicBlock " << b << " {";
    for (int ii = 0; ii < b->statements.count(); ++ii) {
        statement(b->statements.at(ii));
        if (ii != (b->statements.count() - 1))
            qWarning();
    }
    qWarning().nospace() << indent() << "}";

    dec();
}

void IRDump::statement(QDeclarativeJS::IR::Stmt *s)
{
    inc();

    s->accept(this);

    dec();
}

void IRDump::function(QDeclarativeJS::IR::Function *f)
{
    inc();

    qWarning().nospace() << indent() << "Function {";
    for (int ii = 0; ii < f->basicBlocks.count(); ++ii) {
        basicblock(f->basicBlocks.at(ii));
    }
    qWarning().nospace() << indent() << "}";

    dec();
}

const char *IRDump::indent()
{
    return indentData.constData();
}

void IRDump::visitConst(QDeclarativeJS::IR::Const *e)
{
    qWarning().nospace() << indent() << "Const:Expr { type: " << typeName(e->type) << ", value: " << e->value << "}";
}

void IRDump::visitString(QDeclarativeJS::IR::String *e)
{
    qWarning().nospace() << indent() << "String:Expr { type: " << typeName(e->type) << ", value: " << e->value << "}";
}

static void namedumprecur(QDeclarativeJS::IR::Name *e, const char *indent) 
{
    if (e->base) namedumprecur(e->base, indent);
    qWarning().nospace() << indent << "    { type: " << typeName(e->type) << ", symbol: " << symbolname(e->symbol) << ", storage: " << storagename(e->storage) << ", id: " << e->id << "}";
}

void IRDump::visitName(QDeclarativeJS::IR::Name *e)
{
    qWarning().nospace() << indent() << "Name:Expr {";
    namedumprecur(e, indent());
    qWarning().nospace() << indent() << "}";
}

void IRDump::visitTemp(QDeclarativeJS::IR::Temp *e)
{
    qWarning().nospace() << indent() << "Temp:Expr { type: " << typeName(e->type) << ", index: " << e->index << " }";
}

void IRDump::visitUnop(QDeclarativeJS::IR::Unop *e)
{
    qWarning().nospace() << indent() << "Unop:Expr { ";
    qWarning().nospace() << indent() << "    type: " << typeName(e->type) << ", op: " << opname(e->op);
    qWarning().nospace() << indent() << "    expr: {";
    expression(e->expr);
    qWarning().nospace() << indent() << "    }";
    qWarning().nospace() << indent() << "}";
}

void IRDump::visitBinop(QDeclarativeJS::IR::Binop *e)
{
    qWarning().nospace() << indent() << "Binop:Expr { ";
    qWarning().nospace() << indent() << "    type: " << typeName(e->type) << ", op: " << opname(e->op);
    qWarning().nospace() << indent() << "    left: {";
    inc();
    expression(e->left);
    dec();
    qWarning().nospace() << indent() << "    },";
    qWarning().nospace() << indent() << "    right: {";
    inc();
    expression(e->right);
    dec();
    qWarning().nospace() << indent() << "    }";
    qWarning().nospace() << indent() << "}";
}

void IRDump::visitCall(QDeclarativeJS::IR::Call *e)
{
    Q_UNUSED(e);
    qWarning().nospace() << indent() << "Exp::Call { }";
}

void IRDump::visitExp(QDeclarativeJS::IR::Exp *s)
{
    qWarning().nospace() << indent() << "Exp:Stmt {";
    expression(s->expr);
    qWarning().nospace() << indent() << "}";
}

void IRDump::visitMove(QDeclarativeJS::IR::Move *s)
{
    qWarning().nospace() << indent() << "Move:Stmt {";
    qWarning().nospace() << indent() << "    isMoveForReturn: " << s->isMoveForReturn;
    qWarning().nospace() << indent() << "    target: {";
    inc();
    expression(s->target);
    dec();
    qWarning().nospace() << indent() << "    },";
    qWarning().nospace() << indent() << "    source: {";
    inc();
    expression(s->source);
    dec();
    qWarning().nospace() << indent() << "    }";
    qWarning().nospace() << indent() << "}";
}

void IRDump::visitJump(QDeclarativeJS::IR::Jump *s)
{
    qWarning().nospace() << indent() << "Jump:Stmt { BasicBlock(" << s->target << ") }";
}

void IRDump::visitCJump(QDeclarativeJS::IR::CJump *s)
{
    qWarning().nospace() << indent() << "CJump:Stmt {";
    qWarning().nospace() << indent() << "    cond: {";
    inc();
    expression(s->cond);
    dec();
    qWarning().nospace() << indent() << "    }";
    qWarning().nospace() << indent() << "    iftrue: BasicBlock(" << s->iftrue << ")";
    qWarning().nospace() << indent() << "    iffalse: BasicBlock(" << s->iffalse << ")";
    qWarning().nospace() << indent() << "}";
}

void IRDump::visitRet(QDeclarativeJS::IR::Ret *s)
{
    qWarning().nospace() << indent() << "Ret:Stmt {";
    qWarning().nospace() << indent() << "    type: " << typeName(s->type);
    expression(s->expr);
    qWarning().nospace() << indent() << "}";
}
#endif

} // end of namespace IR
} // end of namespace QDeclarativeJS

QT_END_NAMESPACE
