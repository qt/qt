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

#include "qdeclarativev4compiler_p.h"
#include "qdeclarativev4compiler_p_p.h"
#include "qdeclarativev4program_p.h"
#include "qdeclarativev4ir_p.h"
#include "qdeclarativev4irbuilder_p.h"

#include <private/qdeclarativejsast_p.h>
#include <private/qdeclarativefastproperties_p.h>
#include <private/qdeclarativejsengine_p.h>
#include <private/qdeclarativeanchors_p_p.h> // For AnchorLine
#include <private/qsganchors_p_p.h> // For AnchorLine

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(bindingsDump, QML_BINDINGS_DUMP)
DEFINE_BOOL_CONFIG_OPTION(qmlDisableOptimizer, QML_DISABLE_OPTIMIZER)
DEFINE_BOOL_CONFIG_OPTION(qmlExperimental, QML_EXPERIMENTAL)
DEFINE_BOOL_CONFIG_OPTION(qmlVerboseCompiler, QML_VERBOSE_COMPILER)
DEFINE_BOOL_CONFIG_OPTION(qmlBindingsTestEnv, QML_BINDINGS_TEST)

Q_GLOBAL_STATIC(QDeclarativeFastProperties, fastProperties)

static bool qmlBindingsTest = false;

using namespace QDeclarativeJS;
QDeclarativeV4CompilerPrivate::QDeclarativeV4CompilerPrivate()
: _function(0) , _block(0) , _discarded(false)
{
}

//
// tracing
//
void QDeclarativeV4CompilerPrivate::trace(int line, int column)
{
    bytecode.clear();

    this->currentReg = _function->tempCount;

    foreach (IR::BasicBlock *bb, _function->basicBlocks) {
        if (! bb->isTerminated() && (bb->index + 1) < _function->basicBlocks.size())
            bb->JUMP(_function->basicBlocks.at(bb->index + 1));
    }

    QVector<IR::BasicBlock *> blocks;
    trace(&blocks);
    currentBlockMask = 0x00000001;


    for (int i = 0; i < blocks.size(); ++i) {
        IR::BasicBlock *block = blocks.at(i);
        IR::BasicBlock *next = i + 1 < blocks.size() ? blocks.at(i + 1) : 0;
        if (IR::Stmt *terminator = block->terminator()) {
            if (IR::CJump *cj = terminator->asCJump()) {
                if (cj->iffalse != next) {
                    block->i(new IR::Jump(cj->iffalse));
                }
            } else if (IR::Jump *j = terminator->asJump()) {
                if (j->target == next) {
                    delete block->statements.back();
                    block->statements.resize(block->statements.size() - 1);
                }
            }
        }

        block->offset = bytecode.size();

        if (bytecode.isEmpty()) {
            if (qmlBindingsTest || bindingsDump()) {
                Instr id;
                id.common.type = Instr::BindingId;
                id.id.column = column;
                id.id.line = line;
                gen(id);
            }

            if (qmlBindingsTest) {
                QString str = expression->expression.asScript();
                QByteArray strdata((const char *)str.constData(), str.length() * sizeof(QChar));
                int offset = data.count();
                data += strdata;

                Instr test;
                test.common.type = Instr::EnableV4Test;
                test.string_value.reg = 0;
                test.string_value.offset = offset;
                test.string_value.length = str.length();
                gen(test);
            }
        }

        bool usic = false;
        int patchesCount = patches.count();
        qSwap(usedSubscriptionIdsChanged, usic);

        int blockopIndex = bytecode.size();
        Instr blockop;
        blockop.block(currentBlockMask);
        gen(blockop);

        foreach (IR::Stmt *s, block->statements)
            s->accept(this);

        qSwap(usedSubscriptionIdsChanged, usic);

        if (usic) {
            if (currentBlockMask == 0x80000000) {
                discard();
                return;
            }
            currentBlockMask <<= 1;
        } else {
            const int adjust = bytecode.remove(blockopIndex);
            // Correct patches
            for (int ii = patchesCount; ii < patches.count(); ++ii) 
                patches[ii].offset -= adjust;
        }
    }

#ifdef DEBUG_IR_STRUCTURE
    IR::IRDump dump;
    for (int i = 0; i < blocks.size(); ++i) {
        dump.basicblock(blocks.at(i));
    }
#endif


    // back patching
    foreach (const Patch &patch, patches) {
        Instr &instr = bytecode[patch.offset];
        instr.branchop.offset = patch.block->offset - patch.offset - instr.size();
    }

    patches.clear();
}

void QDeclarativeV4CompilerPrivate::trace(QVector<IR::BasicBlock *> *blocks)
{
    QList<IR::BasicBlock *> todo = QList<IR::BasicBlock *>::fromVector(_function->basicBlocks);
    while (! todo.isEmpty()) {
        IR::BasicBlock *block = todo.takeFirst();

        while (! blocks->contains(block)) {
            blocks->append(block);

            if (IR::Stmt *terminator = block->terminator()) {
                if (IR::CJump *cj = terminator->asCJump())
                    block = cj->iffalse;
                else if (IR::Jump *j = terminator->asJump())
                    block = j->target;
            }
        }
    }
}

void QDeclarativeV4CompilerPrivate::traceExpression(IR::Expr *e, quint8 r)
{
    if (!e) {
        discard();
    } else {
        qSwap(currentReg, r);
        e->accept(this);
        qSwap(currentReg, r);
    }
}

//
// expressions
//
void QDeclarativeV4CompilerPrivate::visitConst(IR::Const *e)
{
    Instr i;
    switch (e->type) {
    case IR::BoolType:
        i.move_reg_bool(currentReg, e->value);
        gen(i);
        break;

    case IR::IntType:
        i.move_reg_int(currentReg, e->value);
        gen(i);
        break;

    case IR::RealType:
        i.move_reg_qreal(currentReg, e->value);
        gen(i);
        break;

    default:
        if (qmlVerboseCompiler())
            qWarning() << Q_FUNC_INFO << "unexpected type";
        discard();
    }
}

void QDeclarativeV4CompilerPrivate::visitString(IR::String *e)
{
    registerLiteralString(currentReg, e->value);
}

void QDeclarativeV4CompilerPrivate::visitName(IR::Name *e)
{
    if (e->base) {
        // fetch the object and store it in reg.
        traceExpression(e->base, currentReg);
    } else {
        _subscribeName.clear();
    }

    if (e->storage == IR::Name::RootStorage) {

        Instr instr;
        instr.load_root(currentReg);
        gen(instr);

        if (e->symbol == IR::Name::IdObject) {
            // The ID is a reference to the root object
            return;
        }

    } else if (e->storage == IR::Name::ScopeStorage) {

        Instr instr;
        instr.load_scope(currentReg);
        gen(instr);

        _subscribeName << contextName();

    } else if (e->storage == IR::Name::IdStorage) {

        Instr instr;
        instr.load_id(currentReg, e->index);
        gen(instr);

        _subscribeName << QLatin1String("$$$ID_") + e->id;

        if (blockNeedsSubscription(_subscribeName)) {
            Instr sub;
            sub.subscribeId(currentReg, subscriptionIndex(_subscribeName), instr.load.index);
            gen(sub);
        }

        return;
    } else {
        // No action needed
    }

    switch (e->symbol) {
    case IR::Name::Unbound: 
    case IR::Name::IdObject: 
    case IR::Name::Slot:
    case IR::Name::Object: {
        Q_ASSERT(!"Unreachable");
        discard();
    } break;

    case IR::Name::AttachType: {
        _subscribeName << e->id;

        Instr attached;
        attached.common.type = Instr::LoadAttached;
        attached.attached.output = currentReg;
        attached.attached.reg = currentReg;
        attached.attached.exceptionId = exceptionId(e->line, e->column);
        Q_ASSERT(e->declarativeType->attachedPropertiesId() != -1);
        attached.attached.id = e->declarativeType->attachedPropertiesId();
        gen(attached);
    } break;

    case IR::Name::Property: {
        _subscribeName << e->id;

        QMetaProperty prop = e->meta->property(e->index);
        int fastFetchIndex = fastProperties()->accessorIndexForProperty(e->meta, e->index);

        const int propTy = prop.userType();
        QDeclarativeRegisterType regType;

        switch (propTy) {
        case QMetaType::QReal:
            regType = QRealType;
            break;
        case QMetaType::Bool:
            regType = BoolType;
            break;
        case QMetaType::Int:
            regType = IntType;
            break;
        case QMetaType::QString:
            regType = QStringType;
            break;

        default:
            if (propTy == qMetaTypeId<QDeclarativeAnchorLine>()) {
                regType = PODValueType;
            } else if (propTy == qMetaTypeId<QSGAnchorLine>()) {
                regType = PODValueType;
            } else if (QDeclarativeMetaType::isQObject(propTy)) {
                regType = QObjectStarType;
            } else {
                if (qmlVerboseCompiler())
                    qWarning() << "Discard unsupported property type:" << QMetaType::typeName(propTy);
                discard(); // Unsupported type
                return;
            }

            break;
        } // switch

        Instr fetch;

        if (fastFetchIndex != -1) {
            fetch.common.type = Instr::FetchAndSubscribe;
            fetch.fetchAndSubscribe.reg = currentReg;
            fetch.fetchAndSubscribe.function = fastFetchIndex;
            fetch.fetchAndSubscribe.subscription = subscriptionIndex(_subscribeName);
            fetch.fetchAndSubscribe.exceptionId = exceptionId(e->line, e->column);
            fetch.fetchAndSubscribe.valueType = regType;
        } else {
            if (blockNeedsSubscription(_subscribeName) && prop.hasNotifySignal() && prop.notifySignalIndex() != -1) {
                Instr sub;
                sub.subscribe(currentReg, subscriptionIndex(_subscribeName), prop.notifySignalIndex());
                gen(sub);
            }

            fetch.common.type = Instr::Fetch;
            fetch.fetch.reg = currentReg;
            fetch.fetch.index = e->index;
            fetch.fetch.exceptionId = exceptionId(e->line, e->column);
            fetch.fetch.valueType = regType;
        }

        gen(fetch);

    } break;
    } // switch
}

void QDeclarativeV4CompilerPrivate::visitTemp(IR::Temp *e)
{
    if (currentReg != e->index) {
        Instr i;
        i.move_reg_reg(currentReg, e->index);
        gen(i);
    }
}

void QDeclarativeV4CompilerPrivate::visitUnop(IR::Unop *e)
{
    Instr i;

    quint8 src = currentReg;
    
    if (IR::Temp *temp = e->expr->asTemp()) {
        src = temp->index;
    } else {
        traceExpression(e->expr, src);
    }

    switch (e->op) {
    case IR::OpInvalid:
        Q_ASSERT(!"unreachable");
        break;

    case IR::OpIfTrue:
        if (src != currentReg) {
            i.move_reg_reg(currentReg, src);
            gen(i);
        } else {
            // nothing to do
        }
        break;

    case IR::OpNot:
        i.unary_not(currentReg, src);
        gen(i);
        break;

    case IR::OpUMinus:
        if (e->expr->type == IR::RealType) {
            i.uminus_real(currentReg, src);
            gen(i);
        } else if (e->expr->type == IR::IntType) {
            convertToReal(e->expr, currentReg);
            i.uminus_real(currentReg, src);
            gen(i);
        } else {
            discard();
        }
        break;

    case IR::OpUPlus:
        if (e->expr->type == IR::RealType) {
            i.uplus_real(currentReg, src);
            gen(i);
        } else if (e->expr->type == IR::IntType) {
            convertToReal(e->expr, currentReg);
            i.uplus_real(currentReg, src);
            gen(i);
        } else {
            discard();
        }
        break;

    case IR::OpCompl:
        i.ucompl_real(currentReg, src);
        gen(i);
        discard(); // ???
        break;

    case IR::OpBitAnd:
    case IR::OpBitOr:
    case IR::OpBitXor:
    case IR::OpAdd:
    case IR::OpSub:
    case IR::OpMul:
    case IR::OpDiv:
    case IR::OpMod:
    case IR::OpLShift:
    case IR::OpRShift:
    case IR::OpURShift:
    case IR::OpGt:
    case IR::OpLt:
    case IR::OpGe:
    case IR::OpLe:
    case IR::OpEqual:
    case IR::OpNotEqual:
    case IR::OpStrictEqual:
    case IR::OpStrictNotEqual:
    case IR::OpAnd:
    case IR::OpOr:
        Q_ASSERT(!"unreachable");
        break;
    } // switch
}

void QDeclarativeV4CompilerPrivate::convertToReal(IR::Expr *expr, int reg)
{
    if (expr->type == IR::RealType)
        return;

    Instr conv;
    conv.unaryop.output = reg;
    conv.unaryop.src = reg;

    switch (expr->type) {
    case IR::BoolType:
        conv.common.type = Instr::ConvertBoolToReal;
        gen(conv);
        break;

    case IR::IntType:
        conv.common.type = Instr::ConvertIntToReal;
        gen(conv);
        break;

    case IR::RealType:
        // nothing to do
        return;

    default:
        discard();
        break;
    } // switch
}

void QDeclarativeV4CompilerPrivate::convertToInt(IR::Expr *expr, int reg)
{
    if (expr->type == IR::IntType)
        return;

    Instr conv;
    conv.unaryop.output = reg;
    conv.unaryop.src = reg;

    switch (expr->type) {
    case IR::BoolType:
        conv.common.type = Instr::ConvertBoolToInt;
        gen(conv);
        break;

    case IR::IntType:
        // nothing to do
        return;

    case IR::RealType:
        conv.common.type = Instr::ConvertRealToInt;
        gen(conv);
        break;

    default:
        discard();
        break;
    } // switch
}

void QDeclarativeV4CompilerPrivate::convertToBool(IR::Expr *expr, int reg)
{
    if (expr->type == IR::BoolType)
        return;

    Instr conv;
    conv.unaryop.output = reg;
    conv.unaryop.src = reg;

    switch (expr->type) {
    case IR::BoolType:
        // nothing to do
        break;

    case IR::IntType:
        conv.common.type = Instr::ConvertIntToBool;
        gen(conv);
        break;

    case IR::RealType:
        conv.common.type = Instr::ConvertRealToBool;
        gen(conv);
        return;

    case IR::StringType:
        conv.common.type = Instr::ConvertStringToBool;
        gen(conv);
        return;

    default:
        discard();
        break;
    } // switch
}

quint8 QDeclarativeV4CompilerPrivate::instructionOpcode(IR::Binop *e)
{
    switch (e->op) {
    case IR::OpInvalid:
        return Instr::Noop;

    case IR::OpIfTrue:
    case IR::OpNot:
    case IR::OpUMinus:
    case IR::OpUPlus:
    case IR::OpCompl:
        return Instr::Noop;

    case IR::OpBitAnd:
        return Instr::BitAndInt;

    case IR::OpBitOr:
        return Instr::BitOrInt;

    case IR::OpBitXor:
        return Instr::BitXorInt;

    case IR::OpAdd:
        if (e->type == IR::StringType)
            return Instr::AddString;
        return Instr::AddReal;

    case IR::OpSub:
        return Instr::SubReal;

    case IR::OpMul:
        return Instr::MulReal;

    case IR::OpDiv:
        return Instr::DivReal;

    case IR::OpMod:
        return Instr::ModReal;

    case IR::OpLShift:
        return Instr::LShiftInt;

    case IR::OpRShift:
        return Instr::RShiftInt;

    case IR::OpURShift:
        return Instr::URShiftInt;

    case IR::OpGt:
        if (e->left->type == IR::StringType)
            return Instr::GtString;
        return Instr::GtReal;

    case IR::OpLt:
        if (e->left->type == IR::StringType)
            return Instr::LtString;
        return Instr::LtReal;

    case IR::OpGe:
        if (e->left->type == IR::StringType)
            return Instr::GeString;
        return Instr::GeReal;

    case IR::OpLe:
        if (e->left->type == IR::StringType)
            return Instr::LeString;
        return Instr::LeReal;

    case IR::OpEqual:
        if (e->left->type == IR::StringType)
            return Instr::EqualString;
        return Instr::EqualReal;

    case IR::OpNotEqual:
        if (e->left->type == IR::StringType)
            return Instr::NotEqualString;
        return Instr::NotEqualReal;

    case IR::OpStrictEqual:
        if (e->left->type == IR::StringType)
            return Instr::StrictEqualString;
        return Instr::StrictEqualReal;

    case IR::OpStrictNotEqual:
        if (e->left->type == IR::StringType)
            return Instr::StrictNotEqualString;
        return Instr::StrictNotEqualReal;

    case IR::OpAnd:
    case IR::OpOr:
        return Instr::Noop;

    } // switch

    return Instr::Noop;
}

void QDeclarativeV4CompilerPrivate::visitBinop(IR::Binop *e)
{
    int left = currentReg;
    int right = currentReg + 1; 

    if (e->left->asTemp() && e->type != IR::StringType)  // Not sure if the e->type != String test is needed
        left = e->left->asTemp()->index;
    else
        traceExpression(e->left, left);

    if (IR::Temp *t = e->right->asTemp())
        right = t->index;
    else
        traceExpression(e->right, right);

    if (e->left->type != e->right->type) {
        if (qmlVerboseCompiler())
            qWarning().nospace() << "invalid operands to binary operator " << IR::binaryOperator(e->op)
                                 << "(`" << IR::binaryOperator(e->left->type)
                                 << "' and `"
                                 << IR::binaryOperator(e->right->type)
                                 << "'";
        discard();
        return;
    }

    switch (e->op) {
    case IR::OpInvalid:
        discard();
        break;

    // unary
    case IR::OpIfTrue:
    case IR::OpNot:
    case IR::OpUMinus:
    case IR::OpUPlus:
    case IR::OpCompl:
        discard();
        break;

    case IR::OpBitAnd:
    case IR::OpBitOr:
    case IR::OpBitXor:
    case IR::OpLShift:
    case IR::OpRShift:
    case IR::OpURShift:
        convertToInt(e->left, left);
        convertToInt(e->right, right);
        break;

    case IR::OpAdd:
        if (e->type != IR::StringType) {
            convertToReal(e->left, left);
            convertToReal(e->right, right);
        }
        break;

    case IR::OpSub:
    case IR::OpMul:
    case IR::OpDiv:
    case IR::OpMod:
        convertToReal(e->left, left);
        convertToReal(e->right, right);
        break;

    case IR::OpGt:
    case IR::OpLt:
    case IR::OpGe:
    case IR::OpLe:
    case IR::OpEqual:
    case IR::OpNotEqual:
    case IR::OpStrictEqual:
    case IR::OpStrictNotEqual:
        if (e->left->type != IR::StringType) {
            convertToReal(e->left, left);
            convertToReal(e->right, right);
        }
        break;

    case IR::OpAnd:
    case IR::OpOr:
        discard(); // ### unreachable
        break;
    } // switch

    const quint8 opcode = instructionOpcode(e);
    if (opcode != Instr::Noop) {
        Instr instr;
        instr.common.type = opcode;
        instr.binaryop.output = currentReg;
        instr.binaryop.left = left;
        instr.binaryop.right = right;
        gen(instr);
    }
}

void QDeclarativeV4CompilerPrivate::visitCall(IR::Call *call)
{
    if (IR::Name *name = call->base->asName()) {
        if (call->args.size() == 1 && call->args.at(0)->type == IR::RealType) {
            traceExpression(call->args.at(0), currentReg);

            Instr instr;
            instr.common.type = Instr::Noop;

            switch (name->builtin) {
            case IR::NoBuiltinSymbol:
                break;

            case IR::MathSinBultinFunction:
                instr.math_sin_real(currentReg);
                break;

            case IR::MathCosBultinFunction:
                instr.math_cos_real(currentReg);
                break;

            case IR::MathRoundBultinFunction:
                instr.math_round_real(currentReg);
                break;

            case IR::MathFloorBultinFunction:
                instr.math_floor_real(currentReg);
                break;

            case IR::MathPIBuiltinConstant:
                break;
            } // switch

            if (instr.common.type != Instr::Noop) {
                gen(instr);
                return;
            }
        }
    }

    if (qmlVerboseCompiler())
        qWarning() << "TODO:" << Q_FUNC_INFO << __LINE__;
    discard();
}


//
// statements
//
void QDeclarativeV4CompilerPrivate::visitExp(IR::Exp *s)
{
    traceExpression(s->expr, currentReg);
}

void QDeclarativeV4CompilerPrivate::visitMove(IR::Move *s)
{
    IR::Temp *target = s->target->asTemp();
    Q_ASSERT(target != 0);

    quint8 dest = target->index;

    if (target->type != s->source->type) {
        quint8 src = dest;

        if (IR::Temp *t = s->source->asTemp()) 
            src = t->index;
        else
            traceExpression(s->source, dest);

        Instr conv;
        conv.common.type = Instr::Noop;
        if (target->type == IR::BoolType) {
            switch (s->source->type) {
            case IR::IntType: conv.common.type = Instr::ConvertIntToBool; break;
            case IR::RealType: conv.common.type = Instr::ConvertRealToBool; break;
            case IR::StringType: conv.common.type = Instr::ConvertStringToBool; break;
            default: break;
            } // switch
        } else if (target->type == IR::IntType) {
            switch (s->source->type) {
            case IR::BoolType: conv.common.type = Instr::ConvertBoolToInt; break;
            case IR::RealType: {
                if (s->isMoveForReturn)
                    conv.common.type = Instr::MathRoundReal;
                else
                    conv.common.type = Instr::ConvertRealToInt; 
                break;
            }
            case IR::StringType: conv.common.type = Instr::ConvertStringToInt; break;
            default: break;
            } // switch
        } else if (target->type == IR::RealType) {
            switch (s->source->type) {
            case IR::BoolType: conv.common.type = Instr::ConvertBoolToReal; break;
            case IR::IntType: conv.common.type = Instr::ConvertIntToReal; break;
            case IR::StringType: conv.common.type = Instr::ConvertStringToReal; break;
            default: break;
            } // switch
        } else if (target->type == IR::StringType) {
            switch (s->source->type) {
            case IR::BoolType: conv.common.type = Instr::ConvertBoolToString; break;
            case IR::IntType: conv.common.type = Instr::ConvertIntToString; break;
            case IR::RealType: conv.common.type = Instr::ConvertRealToString; break;
            default: break;
            } // switch
        }
        if (conv.common.type != Instr::Noop) {
            conv.unaryop.output = dest;
            conv.unaryop.src = src;
            gen(conv);
        } else {
            discard();
        }
    } else {
        traceExpression(s->source, dest);
    }
}

void QDeclarativeV4CompilerPrivate::visitJump(IR::Jump *s)
{
    patches.append(Patch(s->target, bytecode.size()));

    Instr i;
    i.branch(0); // ### backpatch
    gen(i);
}

void QDeclarativeV4CompilerPrivate::visitCJump(IR::CJump *s)
{
    traceExpression(s->cond, currentReg);

    patches.append(Patch(s->iftrue, bytecode.size()));

    Instr i;
    i.branch_true(currentReg, 0); // ### backpatch
    gen(i);
}

void QDeclarativeV4CompilerPrivate::visitRet(IR::Ret *s)
{
    Q_ASSERT(s->expr != 0);

    int storeReg = currentReg;

    if (IR::Temp *temp = s->expr->asTemp()) {
        storeReg = temp->index;
    } else {
        traceExpression(s->expr, storeReg);
    }

    if (qmlBindingsTest) {
        Instr test;
        test.common.type = Instr::TestV4Store;
        test.storetest.reg = storeReg;
        switch (s->type) {
        case IR::StringType:
            test.storetest.regType = QMetaType::QString;
            break;
        case IR::UrlType:
            test.storetest.regType = QMetaType::QUrl;
            break;
        case IR::AnchorLineType:
            test.storetest.regType = qMetaTypeId<QDeclarativeAnchorLine>();
            break;
        case IR::SGAnchorLineType:
            test.storetest.regType = qMetaTypeId<QSGAnchorLine>();
            break;
        case IR::ObjectType:
            test.storetest.regType = QMetaType::QObjectStar;
            break;
        case IR::BoolType:
            test.storetest.regType = QMetaType::Bool;
            break;
        case IR::IntType:
            test.storetest.regType = QMetaType::Int;
            break;
        case IR::RealType:
            test.storetest.regType = QMetaType::QReal;
            break;
        default:
            discard();
            return;
        }
        gen(test);
    }

    Instr store;
    store.common.type = Instr::Store;
    store.store.output = 0;
    store.store.index = expression->property->index;
    store.store.reg = storeReg;
    store.store.exceptionId = exceptionId(s->line, s->column);
    gen(store);
}

void QDeclarativeV4Compiler::dump(const QByteArray &programData)
{
    const QDeclarativeV4Program *program = (const QDeclarativeV4Program *)programData.constData();

    qWarning() << "Program.bindings:" << program->bindings;
    qWarning() << "Program.dataLength:" << program->dataLength;
    qWarning() << "Program.subscriptions:" << program->subscriptions;
    qWarning() << "Program.indentifiers:" << program->identifiers;

    const int programSize = program->instructionCount;
    const char *start = program->instructions();
    const char *code = start;
    const char *end = code + programSize;
    while (code < end) {
        Instr *instr = (Instr *) code;
        instr->dump(code - start);
        code += instr->size();
    }
}

QDeclarativeFastProperties *QDeclarativeV4Compiler::fastPropertyAccessor()
{
    return fastProperties();
}

/*!
Clear the state associated with attempting to compile a specific binding.
This does not clear the global "committed binding" states.
*/
void QDeclarativeV4CompilerPrivate::resetInstanceState()
{
    registerCleanups.clear();
    data = committed.data;
    exceptions = committed.exceptions;
    usedSubscriptionIds.clear();
    subscriptionIds = committed.subscriptionIds;
    registeredStrings = committed.registeredStrings;
    bytecode.clear();
    patches.clear();
    currentReg = 0;
}

/*!
Mark the last compile as successful, and add it to the "committed data"
section.

Returns the index for the committed binding.
*/
int QDeclarativeV4CompilerPrivate::commitCompile()
{
    int rv = committed.count();
    committed.offsets << committed.bytecode.count();
    committed.dependencies << usedSubscriptionIds;
    committed.bytecode += bytecode.code();
    committed.data = data;
    committed.exceptions = exceptions;
    committed.subscriptionIds = subscriptionIds;
    committed.registeredStrings = registeredStrings;
    return rv;
}

bool QDeclarativeV4CompilerPrivate::compile(QDeclarativeJS::AST::Node *node)
{
    resetInstanceState();

    if (expression->property->type == -1)
        return false;

    AST::SourceLocation location;
    if (AST::ExpressionNode *astExpression = node->expressionCast()) {
        location = astExpression->firstSourceLocation();
    } else if (AST::Statement *astStatement = node->statementCast()) {
        if (AST::Block *block = AST::cast<AST::Block *>(astStatement))
            location = block->lbraceToken;
        else if (AST::IfStatement *ifStmt = AST::cast<AST::IfStatement *>(astStatement))
            location = ifStmt->ifToken;
        else
            return false;
    } else {
        return false;
    }

    IR::Module module;
    IR::Function *function = 0;

    QDeclarativeV4IRBuilder irBuilder(expression, engine);
    if (!(function = irBuilder(&module, node)))
        return false;

    bool discarded = false;
    qSwap(_discarded, discarded);
    qSwap(_function, function);
    trace(location.startLine, location.startColumn);
    qSwap(_function, function);
    qSwap(_discarded, discarded);

    if (qmlVerboseCompiler()) {
        QTextStream qerr(stderr, QIODevice::WriteOnly);
        if (discarded)
            qerr << "======== TODO ====== " << endl;
        else 
            qerr << "==================== " << endl;
        qerr << "\tline: " << location.startLine
             << "\tcolumn: " << location.startColumn
             << endl;
        foreach (IR::BasicBlock *bb, function->basicBlocks)
            bb->dump(qerr);
        qerr << endl;
    }

    if (discarded || subscriptionIds.count() > 0xFFFF || registeredStrings.count() > 0xFFFF)
        return false;

    return true;
}

// Returns a reg
int QDeclarativeV4CompilerPrivate::registerLiteralString(quint8 reg, const QString &str)
{
    // ### string cleanup

    QByteArray strdata((const char *)str.constData(), str.length() * sizeof(QChar));
    int offset = data.count();
    data += strdata;

    Instr string;
    string.common.type = Instr::String;
    string.string_value.reg = reg;
    string.string_value.offset = offset;
    string.string_value.length = str.length();
    gen(string);

    return reg;
}

// Returns an identifier offset
int QDeclarativeV4CompilerPrivate::registerString(const QString &string)
{
    Q_ASSERT(!string.isEmpty());

    QHash<QString, QPair<int, int> >::ConstIterator iter = registeredStrings.find(string);

    if (iter == registeredStrings.end()) {
        quint32 len = string.length();
        QByteArray lendata((const char *)&len, sizeof(quint32));
        QByteArray strdata((const char *)string.constData(), string.length() * sizeof(QChar));
        strdata.prepend(lendata);
        int rv = data.count();
        data += strdata;

        iter = registeredStrings.insert(string, qMakePair(registeredStrings.count(), rv));
    } 

    Instr reg;
    reg.common.type = Instr::InitString;
    reg.initstring.offset = iter->first;
    reg.initstring.dataIdx = iter->second;
    gen(reg);
    return reg.initstring.offset;
}

/*!
Returns true if the current expression has not already subscribed to \a sub in currentBlockMask.
*/
bool QDeclarativeV4CompilerPrivate::blockNeedsSubscription(const QStringList &sub)
{
    QString str = sub.join(QLatin1String("."));

    QHash<QString, int>::ConstIterator iter = subscriptionIds.find(str);
    if (iter == subscriptionIds.end())
        return true;

    QHash<int, quint32>::ConstIterator uiter = usedSubscriptionIds.find(*iter);
    if (uiter == usedSubscriptionIds.end())
        return true;
    else
        return !(*uiter & currentBlockMask);
}

int QDeclarativeV4CompilerPrivate::subscriptionIndex(const QStringList &sub)
{
    QString str = sub.join(QLatin1String("."));
    QHash<QString, int>::ConstIterator iter = subscriptionIds.find(str);
    if (iter == subscriptionIds.end()) 
        iter = subscriptionIds.insert(str, subscriptionIds.count());
    if (!(usedSubscriptionIds[*iter] & currentBlockMask)) {
        usedSubscriptionIds[*iter] |= currentBlockMask;
        usedSubscriptionIdsChanged = true;
    }
    return *iter;
}

quint32 QDeclarativeV4CompilerPrivate::subscriptionBlockMask(const QStringList &sub)
{
    QString str = sub.join(QLatin1String("."));

    QHash<QString, int>::ConstIterator iter = subscriptionIds.find(str);
    Q_ASSERT(iter != subscriptionIds.end());

    QHash<int, quint32>::ConstIterator uiter = usedSubscriptionIds.find(*iter);
    Q_ASSERT(uiter != usedSubscriptionIds.end());

    return *uiter;
}

quint8 QDeclarativeV4CompilerPrivate::exceptionId(quint32 line, quint32 column)
{
    quint8 rv = 0xFF;
    if (exceptions.count() < 0xFF) {
        rv = (quint8)exceptions.count();
        quint64 e = line;
        e <<= 32;
        e |= column;
        exceptions.append(e);
    }
    return rv;
}

quint8 QDeclarativeV4CompilerPrivate::exceptionId(QDeclarativeJS::AST::ExpressionNode *n)
{
    quint8 rv = 0xFF;
    if (n && exceptions.count() < 0xFF) {
        QDeclarativeJS::AST::SourceLocation l = n->firstSourceLocation();
        rv = exceptionId(l.startLine, l.startColumn);
    }
    return rv;
}

QDeclarativeV4Compiler::QDeclarativeV4Compiler()
: d(new QDeclarativeV4CompilerPrivate)
{
    qmlBindingsTest |= qmlBindingsTestEnv();
}

QDeclarativeV4Compiler::~QDeclarativeV4Compiler()
{
    delete d; d = 0;
}

/* 
Returns true if any bindings were compiled.
*/
bool QDeclarativeV4Compiler::isValid() const
{
    return !d->committed.bytecode.isEmpty();
}

/* 
-1 on failure, otherwise the binding index to use.
*/
int QDeclarativeV4Compiler::compile(const Expression &expression, QDeclarativeEnginePrivate *engine)
{
    if (!expression.expression.asAST()) return false;

    if (!qmlExperimental() && expression.property->isValueTypeSubProperty)
        return -1;

    if (qmlDisableOptimizer())
        return -1;

    d->expression = &expression;
    d->engine = engine;

    if (d->compile(expression.expression.asAST())) {
        return d->commitCompile();
    } else {
        return -1;
    }
}

QByteArray QDeclarativeV4CompilerPrivate::buildSignalTable() const
{
    QHash<int, QList<QPair<int, quint32> > > table;

    for (int ii = 0; ii < committed.count(); ++ii) {
        const QHash<int, quint32> &deps = committed.dependencies.at(ii);
        for (QHash<int, quint32>::ConstIterator iter = deps.begin(); iter != deps.end(); ++iter) 
            table[iter.key()].append(qMakePair(ii, iter.value()));
    }

    QVector<quint32> header;
    QVector<quint32> data;
    for (int ii = 0; ii < committed.subscriptionIds.count(); ++ii) {
        header.append(committed.subscriptionIds.count() + data.count());
        const QList<QPair<int, quint32> > &bindings = table[ii];
        data.append(bindings.count());
        for (int jj = 0; jj < bindings.count(); ++jj) {
            data.append(bindings.at(jj).first);
            data.append(bindings.at(jj).second);
        }
    }
    header << data;

    return QByteArray((const char *)header.constData(), header.count() * sizeof(quint32));
}

QByteArray QDeclarativeV4CompilerPrivate::buildExceptionData() const
{
    QByteArray rv;
    rv.resize(committed.exceptions.count() * sizeof(quint64));
    ::memcpy(rv.data(), committed.exceptions.constData(), rv.size());
    return rv;
}

/* 
Returns the compiled program.
*/
QByteArray QDeclarativeV4Compiler::program() const
{
    QByteArray programData;

    if (isValid()) {
        QDeclarativeV4Program prog;
        prog.bindings = d->committed.count();

        Bytecode bc;
        Instr jump;
        jump.common.type = Instr::Jump;
        jump.jump.reg = -1;

        for (int ii = 0; ii < d->committed.count(); ++ii) {
            jump.jump.count = d->committed.count() - ii - 1;
            jump.jump.count*= jump.size();
            jump.jump.count+= d->committed.offsets.at(ii);
            bc.append(jump);
        }


        QByteArray bytecode = bc.code();
        bytecode += d->committed.bytecode;

        QByteArray data = d->committed.data;
        while (data.count() % 4) data.append('\0');
        prog.signalTableOffset = data.count();
        data += d->buildSignalTable();
        while (data.count() % 4) data.append('\0');
        prog.exceptionDataOffset = data.count();
        data += d->buildExceptionData();

        prog.dataLength = 4 * ((data.size() + 3) / 4);
        prog.subscriptions = d->committed.subscriptionIds.count();
        prog.identifiers = d->committed.registeredStrings.count();
        prog.instructionCount = bytecode.count();
        int size = sizeof(QDeclarativeV4Program) + bytecode.count();
        size += prog.dataLength;

        programData.resize(size);
        memcpy(programData.data(), &prog, sizeof(QDeclarativeV4Program));
        if (prog.dataLength)
            memcpy((char *)((QDeclarativeV4Program *)programData.data())->data(), data.constData(), 
                   data.size());
        memcpy((char *)((QDeclarativeV4Program *)programData.data())->instructions(), bytecode.constData(),
               bytecode.count());
    } 

    if (bindingsDump()) {
        qWarning().nospace() << "Subscription slots:";

        for (QHash<QString, int>::ConstIterator iter = d->committed.subscriptionIds.begin();
                iter != d->committed.subscriptionIds.end();
                ++iter) {
            qWarning().nospace() << "    " << iter.value() << "\t-> " << iter.key();
        }


        QDeclarativeV4Compiler::dump(programData);
    }

    return programData;
}

void QDeclarativeV4Compiler::enableBindingsTest(bool e)
{
    if (e)
        qmlBindingsTest = true;
    else 
        qmlBindingsTest = qmlBindingsTestEnv();
}

QT_END_NAMESPACE
