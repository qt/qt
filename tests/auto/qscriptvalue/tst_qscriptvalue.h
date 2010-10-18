/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TST_QSCRIPTVALUE_H
#define TST_QSCRIPTVALUE_H

#include <QtCore/qobject.h>
#include <QtCore/qnumeric.h>
#include <QtScript/qscriptclass.h>
#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptvalue.h>
#include <QtTest/QtTest>

#define DEFINE_TEST_VALUE(expr) m_values.insert(QString::fromLatin1(#expr), expr)

Q_DECLARE_METATYPE(QVariant)
Q_DECLARE_METATYPE(QScriptValue)

class tst_QScriptValue : public QObject
{
    Q_OBJECT

public:
    tst_QScriptValue();
    virtual ~tst_QScriptValue();

private slots:
    // Generated test functions
    void isArray_data();
    void isArray();

    void isBool_data();
    void isBool();

    void isBoolean_data();
    void isBoolean();

    void isDate_data();
    void isDate();

    void isError_data();
    void isError();

    void isFunction_data();
    void isFunction();

    void isNull_data();
    void isNull();

    void isNumber_data();
    void isNumber();

    void isObject_data();
    void isObject();

    void isQMetaObject_data();
    void isQMetaObject();

    void isQObject_data();
    void isQObject();

    void isRegExp_data();
    void isRegExp();

    void isString_data();
    void isString();

    void isUndefined_data();
    void isUndefined();

    void isValid_data();
    void isValid();

    void isVariant_data();
    void isVariant();

    void toBool_data();
    void toBool();

    void toBoolean_data();
    void toBoolean();

//    void toDateTime_data();
//    void toDateTime();

    void toInt32_data();
    void toInt32();

    void toInteger_data();
    void toInteger();

    void toNumber_data();
    void toNumber();

//    void toQMetaObject_data();
//    void toQMetaObject();

//    void toQObject_data();
//    void toQObject();

//    void toRegExp_data();
//    void toRegExp();

    void toString_data();
    void toString();

    void toUInt16_data();
    void toUInt16();

    void toUInt32_data();
    void toUInt32();

//    void toVariant_data();
//    void toVariant();

    void equals_data();
    void equals();

    void strictlyEquals_data();
    void strictlyEquals();

    void lessThan_data();
    void lessThan();

    void instanceOf_data();
    void instanceOf();

    void assignAndCopyConstruct_data();
    void assignAndCopyConstruct();

    void qscriptvalue_castQString_data();
    void qscriptvalue_castQString();

    void qscriptvalue_castqsreal_data();
    void qscriptvalue_castqsreal();

    void qscriptvalue_castbool_data();
    void qscriptvalue_castbool();

    void qscriptvalue_castqint32_data();
    void qscriptvalue_castqint32();

    void qscriptvalue_castquint32_data();
    void qscriptvalue_castquint32();

    void qscriptvalue_castquint16_data();
    void qscriptvalue_castquint16();

    // Non-generated test functions

    void toObject();
    void ctor();

    void toString_old();
    void toNumber_old();
    void toBoolean_old();
    void toBool_old();
    void toInteger_old();
    void toInt32_old();
    void toUInt32_old();
    void toUInt16_old();
    void toVariant_old();
    void toQObject_old();
    void toDateTime_old();
    void toRegExp_old();
    void instanceOf_old();
    void isArray_old();
    void isDate_old();
    void isError_old();
    void isRegExp_old();

    void lessThan_old();
    void equals_old();
    void strictlyEquals_old();

    void getSetPrototype();
    void getSetScope();
    void getSetProperty();
    void arrayElementGetterSetter();
    void getSetData();
    void getSetScriptClass();
    void call();
    void construct();
    void castToPointer();
    void prettyPrinter_data();
    void prettyPrinter();
    void engineDeleted();
    void valueOfWithClosure();
    void objectId();
    void nestedObjectToVariant_data();
    void nestedObjectToVariant();

private:
    typedef void (tst_QScriptValue::*InitDataFunction)();
    typedef void (tst_QScriptValue::*DefineDataFunction)(const char *);
    void dataHelper(InitDataFunction init, DefineDataFunction define);
    QTestData &newRow(const char *tag);

    typedef void (tst_QScriptValue::*TestFunction)(const char *, const QScriptValue &);
    void testHelper(TestFunction fun);

    // Generated functions

    void initScriptValues();

    void isArray_initData();
    void isArray_makeData(const char *expr);
    void isArray_test(const char *expr, const QScriptValue &value);

    void isBool_initData();
    void isBool_makeData(const char *expr);
    void isBool_test(const char *expr, const QScriptValue &value);

    void isBoolean_initData();
    void isBoolean_makeData(const char *expr);
    void isBoolean_test(const char *expr, const QScriptValue &value);

    void isDate_initData();
    void isDate_makeData(const char *expr);
    void isDate_test(const char *expr, const QScriptValue &value);

    void isError_initData();
    void isError_makeData(const char *expr);
    void isError_test(const char *expr, const QScriptValue &value);

    void isFunction_initData();
    void isFunction_makeData(const char *expr);
    void isFunction_test(const char *expr, const QScriptValue &value);

    void isNull_initData();
    void isNull_makeData(const char *expr);
    void isNull_test(const char *expr, const QScriptValue &value);

    void isNumber_initData();
    void isNumber_makeData(const char *expr);
    void isNumber_test(const char *expr, const QScriptValue &value);

    void isObject_initData();
    void isObject_makeData(const char *expr);
    void isObject_test(const char *expr, const QScriptValue &value);

    void isQMetaObject_initData();
    void isQMetaObject_makeData(const char *expr);
    void isQMetaObject_test(const char *expr, const QScriptValue &value);

    void isQObject_initData();
    void isQObject_makeData(const char *expr);
    void isQObject_test(const char *expr, const QScriptValue &value);

    void isRegExp_initData();
    void isRegExp_makeData(const char *expr);
    void isRegExp_test(const char *expr, const QScriptValue &value);

    void isString_initData();
    void isString_makeData(const char *expr);
    void isString_test(const char *expr, const QScriptValue &value);

    void isUndefined_initData();
    void isUndefined_makeData(const char *expr);
    void isUndefined_test(const char *expr, const QScriptValue &value);

    void isValid_initData();
    void isValid_makeData(const char *expr);
    void isValid_test(const char *expr, const QScriptValue &value);

    void isVariant_initData();
    void isVariant_makeData(const char *expr);
    void isVariant_test(const char *expr, const QScriptValue &value);

    void toBool_initData();
    void toBool_makeData(const char *);
    void toBool_test(const char *, const QScriptValue &value);

    void toBoolean_initData();
    void toBoolean_makeData(const char *);
    void toBoolean_test(const char *, const QScriptValue &value);

    void toDateTime_initData();
    void toDateTime_makeData(const char *);
    void toDateTime_test(const char *, const QScriptValue &value);

    void toInt32_initData();
    void toInt32_makeData(const char *);
    void toInt32_test(const char *, const QScriptValue &value);

    void toInteger_initData();
    void toInteger_makeData(const char *);
    void toInteger_test(const char *, const QScriptValue &value);

    void toNumber_initData();
    void toNumber_makeData(const char *);
    void toNumber_test(const char *, const QScriptValue &value);

    void toQMetaObject_initData();
    void toQMetaObject_makeData(const char *);
    void toQMetaObject_test(const char *, const QScriptValue &value);

    void toQObject_initData();
    void toQObject_makeData(const char *);
    void toQObject_test(const char *, const QScriptValue &value);

    void toRegExp_initData();
    void toRegExp_makeData(const char *);
    void toRegExp_test(const char *, const QScriptValue &value);

    void toString_initData();
    void toString_makeData(const char *);
    void toString_test(const char *, const QScriptValue &value);

    void toUInt16_initData();
    void toUInt16_makeData(const char *);
    void toUInt16_test(const char *, const QScriptValue &value);

    void toUInt32_initData();
    void toUInt32_makeData(const char *);
    void toUInt32_test(const char *, const QScriptValue &value);

    void toVariant_initData();
    void toVariant_makeData(const char *);
    void toVariant_test(const char *, const QScriptValue &value);

    void equals_initData();
    void equals_makeData(const char *);
    void equals_test(const char *, const QScriptValue &value);

    void strictlyEquals_initData();
    void strictlyEquals_makeData(const char *);
    void strictlyEquals_test(const char *, const QScriptValue &value);

    void lessThan_initData();
    void lessThan_makeData(const char *);
    void lessThan_test(const char *, const QScriptValue &value);

    void instanceOf_initData();
    void instanceOf_makeData(const char *);
    void instanceOf_test(const char *, const QScriptValue &value);

    void assignAndCopyConstruct_initData();
    void assignAndCopyConstruct_makeData(const char *);
    void assignAndCopyConstruct_test(const char *, const QScriptValue &value);

    void qscriptvalue_castQString_initData();
    void qscriptvalue_castQString_makeData(const char *);
    void qscriptvalue_castQString_test(const char *, const QScriptValue &value);

    void qscriptvalue_castqsreal_initData();
    void qscriptvalue_castqsreal_makeData(const char *);
    void qscriptvalue_castqsreal_test(const char *, const QScriptValue &value);

    void qscriptvalue_castbool_initData();
    void qscriptvalue_castbool_makeData(const char *);
    void qscriptvalue_castbool_test(const char *, const QScriptValue &value);

    void qscriptvalue_castqint32_initData();
    void qscriptvalue_castqint32_makeData(const char *);
    void qscriptvalue_castqint32_test(const char *, const QScriptValue &value);

    void qscriptvalue_castquint32_initData();
    void qscriptvalue_castquint32_makeData(const char *);
    void qscriptvalue_castquint32_test(const char *, const QScriptValue &value);

    void qscriptvalue_castquint16_initData();
    void qscriptvalue_castquint16_makeData(const char *);
    void qscriptvalue_castquint16_test(const char *, const QScriptValue &value);

private:
    QScriptEngine *engine;
    QHash<QString, QScriptValue> m_values;
    QString m_currentExpression;
};

#define DEFINE_TEST_FUNCTION(name) \
void tst_QScriptValue::name##_data() { dataHelper(&tst_QScriptValue::name##_initData, &tst_QScriptValue::name##_makeData); } \
void tst_QScriptValue::name() { testHelper(&tst_QScriptValue::name##_test); }

#endif
