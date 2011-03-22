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

#ifndef QDECLARATIVEV4INSTRUCTION_P_H
#define QDECLARATIVEV4INSTRUCTION_P_H

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

#include <QtCore/qglobal.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qvector.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#define FOR_EACH_QML_INSTR(F) \
    F(Noop, common) \
    F(BindingId, id) \
    F(Subscribe, subscribeop) \
    F(SubscribeId, subscribeop) \
    F(FetchAndSubscribe, fetchAndSubscribe) \
    F(LoadId, load) \
    F(LoadScope, load) \
    F(LoadRoot, load) \
    F(LoadAttached, attached) \
    F(UnaryNot, unaryop) \
    F(UnaryMinusReal, unaryop) \
    F(UnaryMinusInt, unaryop) \
    F(UnaryPlusReal, unaryop) \
    F(UnaryPlusInt, unaryop) \
    F(ConvertBoolToInt, unaryop) \
    F(ConvertBoolToReal, unaryop) \
    F(ConvertBoolToString, unaryop) \
    F(ConvertIntToBool, unaryop) \
    F(ConvertIntToReal, unaryop) \
    F(ConvertIntToString, unaryop) \
    F(ConvertRealToBool, unaryop) \
    F(ConvertRealToInt, unaryop) \
    F(ConvertRealToString, unaryop) \
    F(ConvertStringToBool, unaryop) \
    F(ConvertStringToInt, unaryop) \
    F(ConvertStringToReal, unaryop) \
    F(MathSinReal, unaryop) \
    F(MathCosReal, unaryop) \
    F(MathRoundReal, unaryop) \
    F(MathFloorReal, unaryop) \
    F(MathPIReal, unaryop) \
    F(Real, real_value) \
    F(Int, int_value) \
    F(Bool, bool_value) \
    F(String, string_value) \
    F(EnableV4Test, string_value) \
    F(TestV4Store, storetest) \
    F(BitAndInt, binaryop) \
    F(BitOrInt, binaryop) \
    F(BitXorInt, binaryop) \
    F(AddReal, binaryop) \
    F(AddString, binaryop) \
    F(SubReal, binaryop) \
    F(MulReal, binaryop) \
    F(DivReal, binaryop) \
    F(ModReal, binaryop) \
    F(LShiftInt, binaryop) \
    F(RShiftInt, binaryop) \
    F(URShiftInt, binaryop) \
    F(GtReal, binaryop) \
    F(LtReal, binaryop) \
    F(GeReal, binaryop) \
    F(LeReal, binaryop) \
    F(EqualReal, binaryop) \
    F(NotEqualReal, binaryop) \
    F(StrictEqualReal, binaryop) \
    F(StrictNotEqualReal, binaryop) \
    F(GtString, binaryop) \
    F(LtString, binaryop) \
    F(GeString, binaryop) \
    F(LeString, binaryop) \
    F(EqualString, binaryop) \
    F(NotEqualString, binaryop) \
    F(StrictEqualString, binaryop) \
    F(StrictNotEqualString, binaryop) \
    F(NewString, construct) \
    F(NewUrl, construct) \
    F(CleanupRegister, cleanup) \
    F(Copy, copy) \
    F(Fetch, fetch) \
    F(Store, store) \
    F(Jump, jump) \
    F(BranchTrue, branchop) \
    F(BranchFalse, branchop) \
    F(Branch, branchop) \
    F(Block, blockop) \
    /* Speculative property resolution */ \
    F(InitString, initstring)

#if defined(Q_CC_GNU) && (!defined(Q_CC_INTEL) || __INTEL_COMPILER >= 1200)
#  define QML_THREADED_INTERPRETER
#endif

#ifdef Q_ALIGNOF
#  define QML_INSTR_ALIGN_MASK (Q_ALIGNOF(Instr) - 1)
#else
#  define QML_INSTR_ALIGN_MASK (sizeof(void *) - 1)
#endif

#define QML_INSTR_ENUM(I, FMT) I,
#define QML_INSTR_ADDR(I, FMT) &&op_##I,
#define QML_INSTR_SIZE(I, FMT) ((sizeof(Instr::instr_##FMT) + QML_INSTR_ALIGN_MASK) & ~QML_INSTR_ALIGN_MASK)

#ifdef QML_THREADED_INTERPRETER
#  define QML_BEGIN_INSTR(I,FMT) op_##I:
#  define QML_END_INSTR(I,FMT) code += QML_INSTR_SIZE(I, FMT); instr = (const Instr *) code; goto *instr->common.code;
#  define QML_INSTR_HEADER void *code;
#else
#  define QML_BEGIN_INSTR(I,FMT) case Instr::I:
#  define QML_END_INSTR(I,FMT) code += QML_INSTR_SIZE(I, FMT); instr = (const Instr *) code; break;
#  define QML_INSTR_HEADER
#endif

namespace QDeclarativeJS {

union Instr {
    int size() const;
    void dump(int = -1) const;
    void noop();
    void load_root(quint8 reg);
    void load_scope(quint8 reg);
    void load_id(quint8 reg, quint32 idIndex);
    void subscribe(qint8 reg, quint16 offset, quint32 index);
    void subscribeId(qint8 reg, quint16 offset, quint32 index);
    void move_reg_bool(quint8 reg, bool value);
    void move_reg_int(quint8 reg, int value);
    void move_reg_qreal(quint8 reg, qreal value);
    void move_reg_reg(quint8 reg, quint8 src);

    void unary_not(quint8 dest, quint8 src);
    void uminus_real(quint8 dest, quint8 src);
    void uminus_int(quint8 dest, quint8 src);
    void uplus_real(quint8 dest, quint8 src);
    void uplus_int(quint8 dest, quint8 src);
    void ucompl_real(quint8 dest, quint8 src);
    void ucompl_int(quint8 dest, quint8 src);

    void math_sin_real(quint8 reg);
    void math_cos_real(quint8 reg);
    void math_round_real(quint8 reg);
    void math_floor_real(quint8 reg);
    void math_pi_real(quint8 reg);
    void branch_true(quint8 reg, qint16 offset);
    void branch_false(quint8 reg, qint16 offset);
    void branch(qint16 offset);
    void block(quint32 mask);

    enum {
        FOR_EACH_QML_INSTR(QML_INSTR_ENUM)
    };

    struct instr_common {
        QML_INSTR_HEADER
        quint8 type;
    };

    struct instr_id {
        QML_INSTR_HEADER
        quint8 type;
        quint16 column;
        quint32 line;
    };

    struct instr_init {
        QML_INSTR_HEADER
        quint8 type;
        quint16 subscriptions;
        quint16 identifiers;
    };

    struct instr_subscribeop {
        QML_INSTR_HEADER
        quint8 type;
        qint8 reg;
        quint16 offset;
        quint32 index;
    };

    struct instr_load {
        QML_INSTR_HEADER
        quint8 type;
        qint8 reg;
        quint32 index;
    };

    struct instr_attached {
        QML_INSTR_HEADER
        quint8 type;
        qint8 output;
        qint8 reg;
        quint8 exceptionId;
        quint32 id;
    };

    struct instr_store {
        QML_INSTR_HEADER
        quint8 type;
        qint8 output;
        qint8 reg;
        quint8 exceptionId;
        quint32 index;
    };

    struct instr_storetest {
        QML_INSTR_HEADER
        quint8 type;
        qint8 reg;
        qint32 regType;
    };

    struct instr_fetchAndSubscribe {
        QML_INSTR_HEADER
        quint8 type;
        qint8 reg;
        quint8 exceptionId;
        quint8 valueType;
        quint16 subscription;
        quint16 function;
    };

    struct instr_fetch{
        QML_INSTR_HEADER
        quint8 type;
        qint8 reg;
        quint8 exceptionId;
        quint8 valueType;
        quint32 index;
    };

    struct instr_copy {
        QML_INSTR_HEADER
        quint8 type;
        qint8 reg;
        qint8 src;
    };

    struct instr_construct {
        QML_INSTR_HEADER
        quint8 type;
        qint8 reg;
    };

    struct instr_real_value {
        QML_INSTR_HEADER
        quint8 type;
        qint8 reg;
        qreal value; // XXX Makes the instruction 12 bytes
    };

    struct instr_int_value {
        QML_INSTR_HEADER
        quint8 type;
        qint8 reg;
        int value;
    };

    struct instr_bool_value {
        QML_INSTR_HEADER
        quint8 type;
        qint8 reg;
        bool value;
    };

    struct instr_string_value {
        QML_INSTR_HEADER
        quint8 type;
        qint8 reg;
        quint16 length;
        quint32 offset;
    };

    struct instr_binaryop {
        QML_INSTR_HEADER
        quint8 type;
        qint8 output;
        qint8 left;
        qint8 right;
    };

    struct instr_unaryop {
        QML_INSTR_HEADER
        quint8 type;
        qint8 output;
        qint8 src;
    };

    struct instr_jump {
        QML_INSTR_HEADER
        quint8 type;
        qint8 reg;
        quint32 count;
    };

    struct instr_find {
        QML_INSTR_HEADER
        quint8 type;
        qint8 reg;
        qint8 src;
        quint8 exceptionId;
        quint16 name;
        quint16 subscribeIndex;
    };

    struct instr_cleanup {
        QML_INSTR_HEADER
        quint8 type;
        qint8 reg;
    };

    struct instr_initstring {
        QML_INSTR_HEADER
        quint8 type;
        quint16 offset;
        quint32 dataIdx;
    };

    struct instr_branchop {
        QML_INSTR_HEADER
        quint8 type;
        quint8 reg;
        qint16 offset;
    };

    struct instr_blockop {
        QML_INSTR_HEADER
        quint8 type;
        quint32 block;
    };

    instr_common common;
    instr_id id;
    instr_init init;
    instr_subscribeop subscribeop;
    instr_load load;
    instr_attached attached;
    instr_store store;
    instr_storetest storetest;
    instr_fetchAndSubscribe fetchAndSubscribe;
    instr_fetch fetch;
    instr_copy copy;
    instr_construct construct;
    instr_real_value real_value;
    instr_int_value int_value;
    instr_bool_value bool_value;
    instr_string_value string_value;
    instr_binaryop binaryop;
    instr_unaryop unaryop;
    instr_jump jump;
    instr_find find;
    instr_cleanup cleanup;
    instr_initstring initstring;
    instr_branchop branchop;
    instr_blockop blockop;
};

class Bytecode
{
    Q_DISABLE_COPY(Bytecode)

public:
    Bytecode();

    QByteArray code() const { return d; }
    const char *constData() const { return d.constData(); }
    int size() const { return d.size(); }
    int count() const { return d.count(); }
    void clear() { d.clear(); }
    bool isEmpty() const { return d.isEmpty(); }
    void append(const Instr &instr);
    void append(const QVector<Instr> &instrs);
    int remove(int index);

    const Instr &operator[](int offset) const;
    Instr &operator[](int offset);

private:
    QByteArray d;
#ifdef QML_THREADED_INTERPRETER
    void **decodeInstr;
#endif
};

}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDECLARATIVEV4INSTRUCTION_P_H

