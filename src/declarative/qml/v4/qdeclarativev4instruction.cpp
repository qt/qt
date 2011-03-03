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

#include "qdeclarativev4instruction_p.h"
#include "qdeclarativev4bindings_p.h"

#include <QtCore/qdebug.h>
#include <private/qdeclarativeglobal_p.h>

// Define this to do a test dump of all the instructions at startup.  This is 
// helpful to test that each instruction's Instr::dump() case uses the correct
// number of tabs etc and otherwise looks correct.
// #define DEBUG_INSTR_DUMP

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(qmlVerboseCompiler, QML_VERBOSE_COMPILER)

namespace QDeclarativeJS {

#ifdef DEBUG_INSTR_DUMP
static struct DumpInstrAtStartup {
    DumpInstrAtStartup() {
#define DUMP_INSTR_AT_STARTUP(Type, FMT) { Instr i; i.common.type = Instr::Type; i.dump(0); }
        FOR_EACH_QML_INSTR(DUMP_INSTR_AT_STARTUP);
    }
} dump_instr_at_startup;
#endif

int Instr::size() const
{
#define QML_RETURN_INSTR_SIZE(I, FMT) case I: return QML_INSTR_SIZE(I, FMT);
    switch (common.type) {
    FOR_EACH_QML_INSTR(QML_RETURN_INSTR_SIZE)
    }
#undef QML_RETURN_INSTR_SIZE
    return 0;
}

void Instr::dump(int address) const
{
    QByteArray leading;
    if (address != -1) {
        leading = QByteArray::number(address);
        leading.prepend(QByteArray(8 - leading.count(), ' '));
        leading.append("\t");
    }

#define INSTR_DUMP qWarning().nospace() << leading.constData() 

    switch (common.type) {
    case Instr::Noop:
        INSTR_DUMP << "\t" << "Noop";
        break;
    case Instr::BindingId:
        INSTR_DUMP << id.line << ":" << id.column << ":";
        break;
    case Instr::Subscribe:
        INSTR_DUMP << "\t" << "Subscribe" << "\t\t" << "Object_Reg(" << subscribeop.reg << ") Notify_Signal(" << subscribeop.index << ") -> Subscribe_Slot(" << subscribeop.offset << ")";
        break;
    case Instr::SubscribeId:
        INSTR_DUMP << "\t" << "SubscribeId" << "\t\t" << "Id_Offset(" << subscribeop.index << ") -> Subscribe_Slot(" << subscribeop.offset << ")";
        break;
    case Instr::FetchAndSubscribe:
        INSTR_DUMP << "\t" << "FetchAndSubscribe" << "\t" << "Object_Reg(" << fetchAndSubscribe.reg << ") Fast_Accessor(" << fetchAndSubscribe.function << ") -> Output_Reg(" << fetchAndSubscribe.reg << ") Subscription_Slot(" <<  fetchAndSubscribe.subscription << ")";
        break;
    case Instr::LoadId:
        INSTR_DUMP << "\t" << "LoadId" << "\t\t\t" << "Id_Offset(" << load.index << ") -> Output_Reg(" << load.reg << ")";
        break;
    case Instr::LoadScope:
        INSTR_DUMP << "\t" << "LoadScope" << "\t\t" << "-> Output_Reg(" << load.reg << ")";
        break;
    case Instr::LoadRoot:
        INSTR_DUMP << "\t" << "LoadRoot" << "\t\t" << "-> Output_Reg(" << load.reg << ")";
        break;
    case Instr::LoadAttached:
        INSTR_DUMP << "\t" << "LoadAttached" << "\t\t" << "Object_Reg(" << attached.reg << ") Attached_Index(" << attached.id << ") -> Output_Reg(" << attached.output << ")";
        break;
    case Instr::UnaryNot:
        INSTR_DUMP << "\t" << "UnaryNot" << "\t\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::UnaryMinusReal:
        INSTR_DUMP << "\t" << "UnaryMinusReal" << "\t\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::UnaryMinusInt:
        INSTR_DUMP << "\t" << "UnaryMinusInt" << "\t\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::UnaryPlusReal:
        INSTR_DUMP << "\t" << "UnaryPlusReal" << "\t\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::UnaryPlusInt:
        INSTR_DUMP << "\t" << "UnaryPlusInt" << "\t\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::ConvertBoolToInt:
        INSTR_DUMP << "\t" << "ConvertBoolToInt" << "\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::ConvertBoolToReal:
        INSTR_DUMP << "\t" << "ConvertBoolToReal" << "\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::ConvertBoolToString:
        INSTR_DUMP << "\t" << "ConvertBoolToString" << "\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::ConvertIntToBool:
        INSTR_DUMP << "\t" << "ConvertIntToBool" << "\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::ConvertIntToReal:
        INSTR_DUMP << "\t" << "ConvertIntToReal" << "\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::ConvertIntToString:
        INSTR_DUMP << "\t" << "ConvertIntToString" << "\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::ConvertRealToBool:
        INSTR_DUMP << "\t" << "ConvertRealToBool" << "\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::ConvertRealToInt:
        INSTR_DUMP << "\t" << "ConvertRealToInt" << "\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::ConvertRealToString:
        INSTR_DUMP << "\t" << "ConvertRealToString" << "\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::ConvertStringToBool:
        INSTR_DUMP << "\t" << "ConvertStringToBool" << "\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::ConvertStringToInt:
        INSTR_DUMP << "\t" << "ConvertStringToInt" << "\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::ConvertStringToReal:
        INSTR_DUMP << "\t" << "ConvertStringToReal" << "\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::MathSinReal:
        INSTR_DUMP << "\t" << "MathSinReal" << "\t\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::MathCosReal:
        INSTR_DUMP << "\t" << "MathCosReal" << "\t\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::MathRoundReal:
        INSTR_DUMP << "\t" << "MathRoundReal" << "\t\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::MathFloorReal:
        INSTR_DUMP << "\t" << "MathFloorReal" << "\t\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::MathPIReal:
        INSTR_DUMP << "\t" << "MathPIReal" << "\t\t" << "Input_Reg(" << unaryop.src << ") -> Output_Reg(" << unaryop.output << ")";
        break;
    case Instr::Real:
        INSTR_DUMP << "\t" << "Real" << "\t\t\t" << "Constant(" << real_value.value << ") -> Output_Reg(" << real_value.reg << ")";
        break;
    case Instr::Int:
        INSTR_DUMP << "\t" << "Int" << "\t\t\t" << "Constant(" << int_value.value << ") -> Output_Reg(" << int_value.reg << ")";
        break;
    case Instr::Bool:
        INSTR_DUMP << "\t" << "Bool" << "\t\t\t" << "Constant(" << bool_value.value << ") -> Output_Reg(" << bool_value.reg << ")";
        break;
    case Instr::String:
        INSTR_DUMP << "\t" << "String" << "\t\t\t" << "String_DataIndex(" << string_value.offset << ") String_Length(" << string_value.length << ") -> Output_Register(" << string_value.reg << ")";
        break;
    case Instr::EnableV4Test:
        INSTR_DUMP << "\t" << "EnableV4Test" << "\t\t" << "String_DataIndex(" << string_value.offset << ") String_Length(" << string_value.length << ")";
        break;
    case Instr::TestV4Store:
        INSTR_DUMP << "\t" << "TestV4Store" << "\t\t" << "Input_Reg(" << storetest.reg << ") Reg_Type(" << storetest.regType << ")";
        break;
    case Instr::BitAndInt:
        INSTR_DUMP << "\t" << "BitAndInt" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::BitOrInt:
        INSTR_DUMP << "\t" << "BitOrInt" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::BitXorInt:
        INSTR_DUMP << "\t" << "BitXorInt" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::AddReal:
        INSTR_DUMP << "\t" << "AddReal" << "\t\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::AddString:
        INSTR_DUMP << "\t" << "AddString" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::SubReal:
        INSTR_DUMP << "\t" << "SubReal" << "\t\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::MulReal:
        INSTR_DUMP << "\t" << "MulReal" << "\t\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::DivReal:
        INSTR_DUMP << "\t" << "DivReal" << "\t\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::ModReal:
        INSTR_DUMP << "\t" << "ModReal" << "\t\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::LShiftInt:
        INSTR_DUMP << "\t" << "LShiftInt" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::RShiftInt:
        INSTR_DUMP << "\t" << "RShiftInt" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::URShiftInt:
        INSTR_DUMP << "\t" << "URShiftInt" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::GtReal:
        INSTR_DUMP << "\t" << "GtReal" << "\t\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::LtReal:
        INSTR_DUMP << "\t" << "LtReal" << "\t\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::GeReal:
        INSTR_DUMP << "\t" << "GeReal" << "\t\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::LeReal:
        INSTR_DUMP << "\t" << "LeReal" << "\t\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::EqualReal:
        INSTR_DUMP << "\t" << "EqualReal" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::NotEqualReal:
        INSTR_DUMP << "\t" << "NotEqualReal" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::StrictEqualReal:
        INSTR_DUMP << "\t" << "StrictEqualReal" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::StrictNotEqualReal:
        INSTR_DUMP << "\t" << "StrictNotEqualReal" << "\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::GtString:
        INSTR_DUMP << "\t" << "GtString" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::LtString:
        INSTR_DUMP << "\t" << "LtString" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::GeString:
        INSTR_DUMP << "\t" << "GeString" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::LeString:
        INSTR_DUMP << "\t" << "LeString" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::EqualString:
        INSTR_DUMP << "\t" << "EqualString" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::NotEqualString:
        INSTR_DUMP << "\t" << "NotEqualString" << "\t\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::StrictEqualString:
        INSTR_DUMP << "\t" << "StrictEqualString" << "\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::StrictNotEqualString:
        INSTR_DUMP << "\t" << "StrictNotEqualString" << "\t" << "Input_Reg(" << binaryop.left << ") Input_Reg(" << binaryop.right << ") -> Output_Reg(" << binaryop.output << ")";
        break;
    case Instr::NewString:
        INSTR_DUMP << "\t" << "NewString" << "\t\t" << "Register(" << construct.reg << ")";
        break;
    case Instr::NewUrl:
        INSTR_DUMP << "\t" << "NewUrl" << "\t\t\t" << "Register(" << construct.reg << ")";
        break;
    case Instr::CleanupRegister:
        INSTR_DUMP << "\t" << "CleanupRegister" << "\t\t" << "Register(" << cleanup.reg << ")";
        break;
    case Instr::Fetch:
        INSTR_DUMP << "\t" << "Fetch" << "\t\t\t" << "Object_Reg(" << fetch.reg << ") Property_Index(" << fetch.index << ") -> Output_Reg(" << fetch.reg << ")";
        break;
    case Instr::Store:
        INSTR_DUMP << "\t" << "Store" << "\t\t\t" << "Input_Reg(" << store.reg << ") -> Object_Reg(" << store.output << ") Property_Index(" << store.index << ")";
        break;
    case Instr::Copy:
        INSTR_DUMP << "\t" << "Copy" << "\t\t\t" << "Input_Reg(" << copy.src << ") -> Output_Reg(" << copy.reg << ")";
        break;
    case Instr::Jump:
        if (jump.reg != -1) {
            INSTR_DUMP << "\t" << "Jump" << "\t\t\t" << "Address(" << (address + size() + jump.count) << ") [if false == Input_Reg(" << jump.reg << ")]";
        } else {
            INSTR_DUMP << "\t" << "Jump" << "\t\t\t" << "Address(" << (address + size() + jump.count) << ")";
        }
        break;
    case Instr::BranchFalse:
        INSTR_DUMP << "\t" << "BranchFalse" << "\t\t" << "Address(" << (address + size() + branchop.offset) << ") [if false == Input_Reg(" << branchop.reg << ")]";
        break;
    case Instr::BranchTrue:
        INSTR_DUMP << "\t" << "BranchTrue" << "\t\t" << "Address(" << (address + size() + branchop.offset) << ") [if true == Input_Reg(" << branchop.reg << ")]";
        break;
    case Instr::Branch:
        INSTR_DUMP << "\t" << "Branch" << "\t\t\t" << "Address(" << (address + size() + branchop.offset) << ")";
        break;
    case Instr::InitString:
        INSTR_DUMP << "\t" << "InitString" << "\t\t" << "String_DataIndex(" << initstring.dataIdx << ") -> String_Slot(" << initstring.offset << ")";
        break;
    case Instr::Block:
        INSTR_DUMP << "\t" << "Block" << "\t\t\t" << "Mask(" << QByteArray::number(blockop.block, 16).constData()  << ")";
        break;
    default:
        INSTR_DUMP << "\t" << "Unknown";
        break;
    }
}

void Instr::noop()
{
    common.type = Noop;
}

void Instr::load_root(quint8 reg)
{
    common.type = LoadRoot;
    load.reg = reg;
    load.index = 0;
}

void Instr::load_scope(quint8 reg)
{
    common.type = LoadScope;
    load.reg = reg;
    load.index = 0;
}

void Instr::load_id(quint8 reg, quint32 idIndex)
{
    common.type = LoadId;
    load.reg = reg;
    load.index = idIndex;
}

void Instr::subscribe(qint8 reg, quint16 subscribeSlot, quint32 notifyIndex)
{
    common.type = Instr::Subscribe;
    subscribeop.reg = reg;
    subscribeop.offset = subscribeSlot;
    subscribeop.index = notifyIndex; 
}

void Instr::subscribeId(qint8 reg, quint16 subscribeSlot, quint32 idIndex)
{
    common.type = Instr::SubscribeId;
    subscribeop.reg = reg;
    subscribeop.offset = subscribeSlot;
    subscribeop.index = idIndex;
}

void Instr::move_reg_bool(quint8 reg, bool value)
{
    common.type = Bool;
    bool_value.reg = reg;
    bool_value.value = value;
}

void Instr::move_reg_int(quint8 reg, int value)
{
    common.type = Int;
    int_value.reg = reg;
    int_value.value = value;
}

void Instr::move_reg_qreal(quint8 reg, qreal value)
{
    common.type = Real;
    real_value.reg = reg;
    real_value.value = value;
}

void Instr::move_reg_reg(quint8 reg, quint8 src)
{
    common.type = Copy;
    copy.reg = reg;
    copy.src = src;
}

void Instr::unary_not(quint8 dest, quint8 src)
{
    common.type = UnaryNot;
    unaryop.src = src;
    unaryop.output = dest;
}

void Instr::uminus_real(quint8 dest, quint8 src)
{
    common.type = UnaryMinusReal;
    unaryop.src = src;
    unaryop.output = dest;
}

void Instr::uminus_int(quint8 dest, quint8 src)
{
    common.type = UnaryMinusInt;
    unaryop.src = src;
    unaryop.output = dest;
}

void Instr::uplus_real(quint8 dest, quint8 src)
{
    common.type = UnaryPlusReal;
    unaryop.src = src;
    unaryop.output = dest;
}

void Instr::uplus_int(quint8 dest, quint8 src)
{
    common.type = UnaryPlusInt;
    unaryop.src = src;
    unaryop.output = dest;
}

void Instr::ucompl_real(quint8 dest, quint8 src)
{
    Q_UNUSED(dest);
    Q_UNUSED(src);
    if (qmlVerboseCompiler())
        qWarning() << "TODO" << Q_FUNC_INFO;
}

void Instr::ucompl_int(quint8 dest, quint8 src)
{
    Q_UNUSED(dest);
    Q_UNUSED(src);
    if (qmlVerboseCompiler())
        qWarning() << "TODO" << Q_FUNC_INFO;
}

void Instr::math_sin_real(quint8 reg)
{
    common.type = MathSinReal;
    unaryop.src = reg;
    unaryop.output = reg;
}

void Instr::math_cos_real(quint8 reg)
{
    common.type = MathCosReal;
    unaryop.src = reg;
    unaryop.output = reg;
}

void Instr::math_round_real(quint8 reg)
{
    common.type = MathRoundReal;
    unaryop.src = reg;
    unaryop.output = reg;
}

void Instr::math_floor_real(quint8 reg)
{
    common.type = MathFloorReal;
    unaryop.src = reg;
    unaryop.output = reg;
}

void Instr::math_pi_real(quint8 reg)
{
    common.type = MathPIReal;
    unaryop.src = reg;
    unaryop.output = reg;
}

void Instr::branch_true(quint8 reg, qint16 offset)
{
    common.type = BranchTrue;
    branchop.reg = reg;
    branchop.offset = offset;
}

void Instr::branch_false(quint8 reg, qint16 offset)
{
    common.type = BranchFalse;
    branchop.reg = reg;
    branchop.offset = offset;
}

void Instr::branch(qint16 offset)
{
    common.type = Branch;
    branchop.offset = offset;
}

void Instr::block(quint32 mask)
{
    common.type = Block;
    blockop.block = mask;
}

Bytecode::Bytecode()
{
#ifdef QML_THREADED_INTERPRETER
    decodeInstr = QDeclarativeV4Bindings::getDecodeInstrTable();
#endif
}

void Bytecode::append(const Instr &instr)
{
    const char *it;
#ifdef QML_THREADED_INTERPRETER
    Instr i = instr;
    i.common.code = decodeInstr[i.common.type];
    it = (const char *) &i;
#else
    it = (const char *) &instr;
#endif
    d.append(it, instr.size());
}

void Bytecode::append(const QVector<Instr> &instrs)
{
    foreach (const Instr &i, instrs)
        append(i);
}

int Bytecode::remove(int offset)
{
    const Instr *instr = (const Instr *) (d.begin() + offset);
    const int instrSize = instr->size();
    d.remove(offset, instrSize);
    return instrSize;
}

const Instr &Bytecode::operator[](int offset) const
{
    return *((const Instr *) (d.begin() + offset));
}

Instr &Bytecode::operator[](int offset)
{
    return *((Instr *) (d.begin() + offset));
}

}

QT_END_NAMESPACE
