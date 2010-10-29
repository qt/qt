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

#ifndef TST_QSCRIPTVALUE_H
#define TST_QSCRIPTVALUE_H

#include <QtCore/qobject.h>
#include <QtCore/qnumeric.h>
#include <QtScript/qscriptclass.h>
#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptvalue.h>
#include <QtTest/QtTest>

Q_DECLARE_METATYPE(QVariant)
Q_DECLARE_METATYPE(QScriptValue)

class tst_QScriptValue : public QObject
{
    Q_OBJECT

public:
    tst_QScriptValue();
    virtual ~tst_QScriptValue();

private slots:
    void toObject();

    void ctor_invalid();
    void ctor_undefinedWithEngine();
    void ctor_undefined();
    void ctor_nullWithEngine();
    void ctor_null();
    void ctor_boolWithEngine();
    void ctor_bool();
    void ctor_intWithEngine();
    void ctor_int();
    void ctor_uintWithEngine();
    void ctor_uint();
    void ctor_floatWithEngine();
    void ctor_float();
    void ctor_stringWithEngine();
    void ctor_string();
    void ctor_copyAndAssignWithEngine();
    void ctor_copyAndAssign();
    void ctor_nullEngine();

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
    void getSetScriptClass_nonObjects_data();
    void getSetScriptClass_nonObjects();
    void getSetScriptClass_JSObjectFromCpp();
    void getSetScriptClass_JSObjectFromJS();
    void getSetScriptClass_QVariant();
    void getSetScriptClass_QObject();
    void call();
    void construct();
    void construct_constructorThrowsPrimitive();
    void castToPointer();
    void prettyPrinter_data();
    void prettyPrinter();
    void engineDeleted();
    void valueOfWithClosure();
    void objectId();
    void nestedObjectToVariant_data();
    void nestedObjectToVariant();

private:
    QScriptEngine *engine;
};

#endif
