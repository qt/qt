/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qscriptclass_p.h"
#include "qscriptcontext.h"
#include "qscriptcontext_p.h"
#include "qscriptcontextinfo.h"
#include "qscriptengine.h"
#include "qscriptengine_p.h"
#include "qscriptengineagent.h"
#include "qscriptengineagent_p.h"
#include "qscriptextensioninterface.h"
#include "qscriptfunction_p.h"
#include "qscriptstring.h"
#include "qscriptstring_p.h"
#include "qscriptvalue.h"
#include "qscriptvalue_p.h"
#include "qscriptsyntaxcheckresult_p.h"
#include "qscriptqobject_p.h"
#include "qscriptisolate_p.h"
#include "qscriptprogram_p.h"
#include "qscript_impl_p.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qvariant.h>
#include <QtCore/qdatetime.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qpluginloader.h>
#include <qthread.h>
#include <qmutex.h>
#include <qwaitcondition.h>

QT_BEGIN_NAMESPACE

static void QtVariantWeakCallback(v8::Persistent<v8::Value> val, void *arg)
{
    Q_UNUSED(val);
    QtVariantData *data = static_cast<QtVariantData*>(arg);
    val.Dispose();
    delete data;
}

// This callback implements QVariant.prototype.toString.
static v8::Handle<v8::Value> QtVariantToStringCallback(const v8::Arguments& args)
{
    // FIXME: Is the type check required here?
//    if (!thisValue.inherits(&QScriptObject::info))
//        return throwError(exec, JSC::TypeError, "This object is not a QVariant");
    const QVariant &v = QtVariantData::get(args.This())->value();
    if (!v.isValid())
        return v8::String::New("undefined");
    QString result = v.toString();
    if (result.isEmpty() && !v.canConvert(QVariant::String))
        result = QString::fromLatin1("QVariant(%0)").arg(QString::fromLatin1(v.typeName()));
    return QScriptConverter::toString(result);
}

// This callback implements QVariant.prototype.valueOf.
static v8::Handle<v8::Value> QtVariantValueOfCallback(const v8::Arguments& args)
{
    // FIXME: Is the type check required here?
    //    if (!thisValue.inherits(&QScriptObject::info))
    //        return throwError(exec, JSC::TypeError);
    const QVariant &v = QtVariantData::get(args.This())->value();
    switch (v.type()) {
    case QVariant::Invalid:
        return v8::Undefined();
    case QVariant::String:
        return QScriptConverter::toString(v.toString());
    case QVariant::Int:
    case QVariant::Double:
    case QVariant::UInt:
        return v8::Number::New(v.toDouble());
    case QVariant::Bool:
        return v8::Boolean::New(v.toBool());

//    case QVariant::Char:
//        return JSC::jsNumber(exec, v.toChar().unicode());

    default:
        ;
    }
    return args.This();
}

// Converts a JS Date to a QDateTime.
QDateTime QScriptEnginePrivate::qtDateTimeFromJS(v8::Handle<v8::Date> jsDate)
{
    return QDateTime::fromMSecsSinceEpoch(jsDate->NumberValue());
}

// Converts a QDateTime to a JS Date.
v8::Handle<v8::Value> QScriptEnginePrivate::qtDateTimeToJS(const QDateTime &dt)
{
    qsreal date;
    if (!dt.isValid())
        date = qSNaN();
    else
        date = dt.toMSecsSinceEpoch();
    return v8::Date::New(date);
}

// Converts a QStringList to JS.
// The result is a new Array object with length equal to the length
// of the QStringList, and the elements being the QStringList's
// elements converted to JS Strings.
v8::Handle<v8::Array> QScriptEnginePrivate::stringListToJS(const QStringList &lst)
{
    v8::Handle<v8::Array> result = v8::Array::New(lst.size());
    for (int i = 0; i < lst.size(); ++i)
        result->Set(i, QScriptConverter::toString(lst.at(i)));
    return result;
}

// Converts a JS Array object to a QStringList.
// The result is a QStringList with length equal to the length
// of the JS Array, and elements being the JS Array's elements
// converted to QStrings.
QStringList QScriptEnginePrivate::stringListFromJS(v8::Handle<v8::Array> jsArray)
{
    QStringList result;
    uint32_t length = jsArray->Length();
    for (uint32_t i = 0; i < length; ++i)
        result.append(QScriptConverter::toString(jsArray->Get(i)->ToString()));
    return result;
}

// Converts a QVariantList to JS.
// The result is a new Array object with length equal to the length
// of the QVariantList, and the elements being the QVariantList's
// elements converted to JS, recursively.
v8::Handle<v8::Array> QScriptEnginePrivate::variantListToJS(const QVariantList &lst)
{
    v8::Handle<v8::Array> result = v8::Array::New(lst.size());
    for (int i = 0; i < lst.size(); ++i)
        result->Set(i, variantToJS(lst.at(i)));
    return result;
}

// Converts a JS Array object to a QVariantList.
// The result is a QVariantList with length equal to the length
// of the JS Array, and elements being the JS Array's elements
// converted to QVariants, recursively.
QVariantList QScriptEnginePrivate::variantListFromJS(v8::Handle<v8::Array> jsArray)
{
    QVariantList result;
    int hash = jsArray->GetIdentityHash();
    if (visitedConversionObjects.contains(hash))
        return result; // Avoid recursion.
    v8::HandleScope handleScope;
    visitedConversionObjects.insert(hash);
    uint32_t length = jsArray->Length();
    for (uint32_t i = 0; i < length; ++i)
        result.append(variantFromJS(jsArray->Get(i)));
    visitedConversionObjects.remove(hash);
    return result;
}

// Converts a QVariantMap to JS.
// The result is a new Object object with property names being
// the keys of the QVariantMap, and values being the values of
// the QVariantMap converted to JS, recursively.
v8::Handle<v8::Object> QScriptEnginePrivate::variantMapToJS(const QVariantMap &vmap)
{
    v8::Handle<v8::Object> result = v8::Object::New();
    QVariantMap::const_iterator it;
    for (it = vmap.constBegin(); it != vmap.constEnd(); ++it)
        result->Set(QScriptConverter::toString(it.key()), variantToJS(it.value()));
    return result;
}

// Converts a JS Object to a QVariantMap.
// The result is a QVariantMap with keys being the property names
// of the object, and values being the values of the JS object's
// properties converted to QVariants, recursively.
QVariantMap QScriptEnginePrivate::variantMapFromJS(v8::Handle<v8::Object> jsObject)
{
    QVariantMap result;
    int hash = jsObject->GetIdentityHash();
    if (visitedConversionObjects.contains(hash))
        return result; // Avoid recursion.
    visitedConversionObjects.insert(hash);
    v8::HandleScope handleScope;
    // TODO: Only object's own property names. Include non-enumerable properties.
    v8::Handle<v8::Array> propertyNames = jsObject->GetPropertyNames();
    uint32_t length = propertyNames->Length();
    for (uint32_t i = 0; i < length; ++i) {
        v8::Handle<v8::Value> name = propertyNames->Get(i);
        result.insert(QScriptConverter::toString(name->ToString()), variantFromJS(jsObject->Get(name)));
    }
    visitedConversionObjects.remove(hash);
    return result;
}

// Converts the meta-type defined by the given type and data to JS.
// Returns the value if conversion succeeded, an empty handle otherwise.
v8::Handle<v8::Value> QScriptEnginePrivate::metaTypeToJS(int type, const void *data)
{
    Q_Q(QScriptEngine);
    Q_ASSERT(data != 0);
    v8::Handle<v8::Value> result;
    TypeInfos::TypeInfo info = m_typeInfos.value(type);
    if (info.marshal) {
        result = QScriptValuePrivate::get(info.marshal(q, data))->asV8Value(this);
    } else {
        // check if it's one of the types we know
        switch (QMetaType::Type(type)) {
        case QMetaType::Void:
            return v8::Undefined();
        case QMetaType::Bool:
            return v8::Boolean::New(*reinterpret_cast<const bool*>(data));
        case QMetaType::Int:
            return v8::Int32::New(*reinterpret_cast<const int*>(data));
        case QMetaType::UInt:
            return v8::Uint32::New(*reinterpret_cast<const uint*>(data));
        case QMetaType::LongLong:
            return v8::Number::New(qsreal(*reinterpret_cast<const qlonglong*>(data)));
        case QMetaType::ULongLong:
#if defined(Q_OS_WIN) && defined(_MSC_FULL_VER) && _MSC_FULL_VER <= 12008804
#pragma message("** NOTE: You need the Visual Studio Processor Pack to compile support for 64bit unsigned integers.")
            return v8::Number::New(qsreal((qlonglong)*reinterpret_cast<const qulonglong*>(data)));
#elif defined(Q_CC_MSVC) && !defined(Q_CC_MSVC_NET)
            return v8::Number::New(qsreal((qlonglong)*reinterpret_cast<const qulonglong*>(data)));
#else
            return v8::Number::New(qsreal(*reinterpret_cast<const qulonglong*>(data)));
#endif
        case QMetaType::Double:
            return v8::Number::New(qsreal(*reinterpret_cast<const double*>(data)));
        case QMetaType::QString:
            return QScriptConverter::toString(*reinterpret_cast<const QString*>(data));
        case QMetaType::Float:
            return v8::Number::New(*reinterpret_cast<const float*>(data));
        case QMetaType::Short:
            return v8::Int32::New(*reinterpret_cast<const short*>(data));
        case QMetaType::UShort:
            return v8::Uint32::New(*reinterpret_cast<const unsigned short*>(data));
        case QMetaType::Char:
            return v8::Int32::New(*reinterpret_cast<const char*>(data));
        case QMetaType::UChar:
            return v8::Uint32::New(*reinterpret_cast<const unsigned char*>(data));
        case QMetaType::QChar:
            return v8::Uint32::New((*reinterpret_cast<const QChar*>(data)).unicode());
        case QMetaType::QStringList:
            result = stringListToJS(*reinterpret_cast<const QStringList *>(data));
            break;
        case QMetaType::QVariantList:
            result = variantListToJS(*reinterpret_cast<const QVariantList *>(data));
            break;
        case QMetaType::QVariantMap:
            result = variantMapToJS(*reinterpret_cast<const QVariantMap *>(data));
            break;
        case QMetaType::QDateTime:
            result = qtDateTimeToJS(*reinterpret_cast<const QDateTime *>(data));
            break;
        case QMetaType::QDate:
            result = qtDateTimeToJS(QDateTime(*reinterpret_cast<const QDate *>(data)));
            break;
#ifndef QT_NO_REGEXP
        case QMetaType::QRegExp:
            result = QScriptConverter::toRegExp(*reinterpret_cast<const QRegExp *>(data));
            break;
#endif
        case QMetaType::QObjectStar:
        case QMetaType::QWidgetStar:
            result = newQObject(*reinterpret_cast<QObject* const *>(data));
            break;
        case QMetaType::QVariant:
            result = variantToJS(*reinterpret_cast<const QVariant*>(data));
            break;
        default:
            if (type == qMetaTypeId<QScriptValue>()) {
                return QScriptValuePrivate::get(*reinterpret_cast<const QScriptValue*>(data))->asV8Value(this);
            }
            // lazy registration of some common list types
            else if (type == qMetaTypeId<QObjectList>()) {
                qScriptRegisterSequenceMetaType<QObjectList>(q);
                return metaTypeToJS(type, data);
            }
            else if (type == qMetaTypeId<QList<int> >()) {
                qScriptRegisterSequenceMetaType<QList<int> >(q);
                return metaTypeToJS(type, data);
            } else {
                QByteArray typeName = QMetaType::typeName(type);
                if (typeName.endsWith('*') && !*reinterpret_cast<void* const *>(data)) {
                    return v8::Null();
                } else {
                    // Fall back to wrapping in a QVariant.
                    result = newVariant(QVariant(type, data));
                }
            }
        }
    }

    if (!result.IsEmpty() && result->IsObject() && !info.prototype.IsEmpty())
        v8::Object::Cast(*result)->SetPrototype(info.prototype);
#if 0
    if (result && result.isObject() && info && info->prototype
        && JSC::JSValue::strictEqual(exec, JSC::asObject(result)->prototype(), eng->originalGlobalObject()->objectPrototype())) {
        JSC::asObject(result)->setPrototype(info->prototype);
    }
#endif
    return result;
}

// Converts a JS value to a meta-type.
// data must point to a place that can store a value of the given type.
// Returns true if conversion succeeded, false otherwise.
bool QScriptEnginePrivate::metaTypeFromJS(v8::Handle<v8::Value> value, int type, void *data)
{
    TypeInfos::TypeInfo info = m_typeInfos.value(type);
    if (info.demarshal) {
        info.demarshal(QScriptValuePrivate::get(new QScriptValuePrivate(this, value)), data);
        return true;
    }
    // check if it's one of the types we know
    switch (QMetaType::Type(type)) {
    case QMetaType::Bool:
        *reinterpret_cast<bool*>(data) = value->ToBoolean()->Value();
        return true;
    case QMetaType::Int:
        *reinterpret_cast<int*>(data) = value->ToInt32()->Value();
        return true;
    case QMetaType::UInt:
        *reinterpret_cast<uint*>(data) = value->ToUint32()->Value();
        return true;
    case QMetaType::LongLong:
        *reinterpret_cast<qlonglong*>(data) = qlonglong(value->ToInteger()->Value());
        return true;
    case QMetaType::ULongLong:
        *reinterpret_cast<qulonglong*>(data) = qulonglong(value->ToInteger()->Value());
        return true;
    case QMetaType::Double:
        *reinterpret_cast<double*>(data) = value->ToNumber()->Value();
        return true;
    case QMetaType::QString:
        if (value->IsUndefined() || value->IsNull())
            *reinterpret_cast<QString*>(data) = QString();
        else
            *reinterpret_cast<QString*>(data) = QScriptConverter::toString(value->ToString());
        return true;
    case QMetaType::Float:
        *reinterpret_cast<float*>(data) = value->ToNumber()->Value();
        return true;
    case QMetaType::Short:
        *reinterpret_cast<short*>(data) = short(value->ToInt32()->Value());
        return true;
    case QMetaType::UShort:
        *reinterpret_cast<unsigned short*>(data) = ushort(value->ToInt32()->Value()); // ### QScript::ToUInt16()
        return true;
    case QMetaType::Char:
        *reinterpret_cast<char*>(data) = char(value->ToInt32()->Value());
        return true;
    case QMetaType::UChar:
        *reinterpret_cast<unsigned char*>(data) = (unsigned char)(value->ToInt32()->Value());
        return true;
    case QMetaType::QChar:
        if (value->IsString()) {
            QString str = QScriptConverter::toString(v8::Handle<v8::String>::Cast(value));
            *reinterpret_cast<QChar*>(data) = str.isEmpty() ? QChar() : str.at(0);
        } else {
            *reinterpret_cast<QChar*>(data) = QChar(ushort(value->ToInt32()->Value())); // ### QScript::ToUInt16()
        }
        return true;
    case QMetaType::QDateTime:
        if (value->IsDate()) {
            *reinterpret_cast<QDateTime *>(data) = qtDateTimeFromJS(v8::Handle<v8::Date>::Cast(value));
            return true;
        } break;
    case QMetaType::QDate:
        if (value->IsDate()) {
            *reinterpret_cast<QDate *>(data) = qtDateTimeFromJS(v8::Handle<v8::Date>::Cast(value)).date();
            return true;
        } break;
#if !defined(QT_NO_REGEXP)
    case QMetaType::QRegExp:
        if (value->IsRegExp()) {
            *reinterpret_cast<QRegExp *>(data) = QScriptConverter::toRegExp(v8::Handle<v8::RegExp>::Cast(value));
            return true;
        } break;
#endif
    case QMetaType::QObjectStar:
        if (isQtObject(value) || value->IsNull()) {
            *reinterpret_cast<QObject* *>(data) = qtObjectFromJS(value);
            return true;
        } break;
    case QMetaType::QWidgetStar:
        if (isQtObject(value) || value->IsNull()) {
            QObject *qo = qtObjectFromJS(value);
            if (!qo || qo->isWidgetType()) {
                *reinterpret_cast<QWidget* *>(data) = reinterpret_cast<QWidget*>(qo);
                return true;
            }
        } break;
    case QMetaType::QStringList:
        if (value->IsArray()) {
            *reinterpret_cast<QStringList *>(data) = stringListFromJS(v8::Handle<v8::Array>::Cast(value));
            return true;
        } break;
    case QMetaType::QVariantList:
        if (value->IsArray()) {
            *reinterpret_cast<QVariantList *>(data) = variantListFromJS(v8::Handle<v8::Array>::Cast(value));
            return true;
        } break;
    case QMetaType::QVariantMap:
        if (value->IsObject()) {
            *reinterpret_cast<QVariantMap *>(data) = variantMapFromJS(v8::Handle<v8::Object>::Cast(value));
            return true;
        } break;
    case QMetaType::QVariant:
        *reinterpret_cast<QVariant*>(data) = variantFromJS(value);
        return true;
    default:
    ;
    }

#if 0
    if (isQtVariant(value)) {
        const QVariant &var = variantValue(value);
        // ### Enable once constructInPlace() is in qt master.
        if (var.userType() == type) {
            QMetaType::constructInPlace(type, data, var.constData());
            return true;
        }
        if (var.canConvert(QVariant::Type(type))) {
            QVariant vv = var;
            vv.convert(QVariant::Type(type));
            Q_ASSERT(vv.userType() == type);
            QMetaType::constructInPlace(type, data, vv.constData());
            return true;
        }

    }
#endif

    // Try to use magic.

    QByteArray name = QMetaType::typeName(type);
    if (convertToNativeQObject(value, name, reinterpret_cast<void* *>(data)))
        return true;
    if (isQtVariant(value) && name.endsWith('*')) {
        int valueType = QMetaType::type(name.left(name.size()-1));
        QVariant &var = variantValue(value);
        if (valueType == var.userType()) {
            // We have T t, T* is requested, so return &t.
            *reinterpret_cast<void* *>(data) = var.data();
            return true;
        } else {
            // Look in the prototype chain.
            v8::Handle<v8::Value> proto = value->ToObject()->GetPrototype();
            while (proto->IsObject()) {
                bool canCast = false;
                if (isQtVariant(proto)) {
                    canCast = (type == variantValue(proto).userType())
                              || (valueType && (valueType == variantValue(proto).userType()));
                }
                else if (isQtObject(proto)) {
                    QByteArray className = name.left(name.size()-1);
                    if (QObject *qobject = qtObjectFromJS(proto))
                        canCast = qobject->qt_metacast(className) != 0;
                }
                if (canCast) {
                    QByteArray varTypeName = QMetaType::typeName(var.userType());
                    if (varTypeName.endsWith('*'))
                        *reinterpret_cast<void* *>(data) = *reinterpret_cast<void* *>(var.data());
                    else
                        *reinterpret_cast<void* *>(data) = var.data();
                    return true;
                }
                proto = proto->ToObject()->GetPrototype();
            }
        }
    } else if (value->IsNull() && name.endsWith('*')) {
        *reinterpret_cast<void* *>(data) = 0;
        return true;
    } else if (type == qMetaTypeId<QScriptValue>()) {
        *reinterpret_cast<QScriptValue*>(data) = QScriptValuePrivate::get(new QScriptValuePrivate(this, value));
        return true;
    }
    // lazy registration of some common list types
    else if (type == qMetaTypeId<QObjectList>()) {
        qScriptRegisterSequenceMetaType<QObjectList>(q_func());
        return metaTypeFromJS(value, type, data);
    }
    else if (type == qMetaTypeId<QList<int> >()) {
        qScriptRegisterSequenceMetaType<QList<int> >(q_func());
        return metaTypeFromJS(value, type, data);
    }

    return false;
}

// Converts a QVariant to JS.
v8::Handle<v8::Value> QScriptEnginePrivate::variantToJS(const QVariant &value)
{
    return metaTypeToJS(value.userType(), value.constData());
}

// Converts a JS value to a QVariant.
// Null, Undefined -> QVariant() (invalid)
// Boolean -> QVariant(bool)
// Number -> QVariant(double)
// String -> QVariant(QString)
// Array -> QVariantList(...)
// Date -> QVariant(QDateTime)
// RegExp -> QVariant(QRegExp)
// [Any other object] -> QVariantMap(...)
QVariant QScriptEnginePrivate::variantFromJS(v8::Handle<v8::Value> value)
{
    Q_ASSERT(!value.IsEmpty());
    if (value->IsNull() || value->IsUndefined())
        return QVariant();
    if (value->IsBoolean())
        return value->ToBoolean()->Value();
    else if (value->IsInt32())
        return value->ToInt32()->Value();
    else if (value->IsNumber())
        return value->ToNumber()->Value();
    if (value->IsString())
        return QScriptConverter::toString(value->ToString());
    Q_ASSERT(value->IsObject());
    if (value->IsArray())
        return variantListFromJS(v8::Handle<v8::Array>::Cast(value));
    if (value->IsDate())
        return qtDateTimeFromJS(v8::Handle<v8::Date>::Cast(value));
#ifndef QT_NO_REGEXP
    if (value->IsRegExp())
        return QScriptConverter::toRegExp(v8::Handle<v8::RegExp>::Cast(value));
#endif
    if (isQtVariant(value))
        return variantValue(value);
    if (isQtObject(value))
        return qVariantFromValue(qtObjectFromJS(value));
    return variantMapFromJS(value->ToObject());
}

bool QScriptEnginePrivate::isQtObject(v8::Handle<v8::Value> value)
{
    return qobjectTemplate()->HasInstance(value);
}

QObject *QScriptEnginePrivate::qtObjectFromJS(v8::Handle<v8::Value> value)
{
    if (isQtObject(value)) {
        QScriptQObjectData *data = QScriptQObjectData::get(v8::Handle<v8::Object>::Cast(value));
        Q_ASSERT(data);
        Q_ASSERT(this == data->engine());
        QObject *result = data->cppObject();
        return result;
    }

    if (isQtVariant(value)) {
        QVariant var = variantFromJS(value);
        int type = var.userType();
        if ((type == QMetaType::QObjectStar) || (type == QMetaType::QWidgetStar))
            return *reinterpret_cast<QObject* const *>(var.constData());
    }

    return 0;
}

bool QScriptEnginePrivate::convertToNativeQObject(v8::Handle<v8::Value> value,
                                                  const QByteArray &targetType,
                                                  void **result)
{
    if (!targetType.endsWith('*'))
        return false;
    if (QObject *qobject = qtObjectFromJS(value)) {
        int start = targetType.startsWith("const ") ? 6 : 0;
        QByteArray className = targetType.mid(start, targetType.size()-start-1);
        if (void *instance = qobject->qt_metacast(className)) {
            *result = instance;
            return true;
        }
    }
    return false;
}

QVariant &QScriptEnginePrivate::variantValue(v8::Handle<v8::Value> value)
{
    Q_ASSERT(isQtVariant(value));
    return QtVariantData::get(v8::Handle<v8::Object>::Cast(value))->value();
}

/*!
  \internal
  Returns the template for the given meta-object, \a mo; creates a template if one
  doesn't already exist.
*/
v8::Handle<v8::FunctionTemplate> QScriptEnginePrivate::qtClassTemplate(const QMetaObject *mo, const QScriptEngine::QObjectWrapOptions &options)
{
    Q_ASSERT(mo != 0);
    QScriptEngine::QObjectWrapOptions mask =
        QScriptEngine::ExcludeSuperClassMethods |
        QScriptEngine::ExcludeSuperClassProperties |
        QScriptEngine::ExcludeSuperClassContents |
        QScriptEngine::SkipMethodsInEnumeration |
        QScriptEngine::ExcludeDeleteLater |
        QScriptEngine::ExcludeSlots;
    mask &= options;
    ClassTemplateHash::const_iterator it;
    QPair<const QMetaObject *, QScriptEngine::QObjectWrapOptions> key = qMakePair(mo, mask);
    it = m_qtClassTemplates.constFind(key);
    if (it != m_qtClassTemplates.constEnd())
        return *it;

    v8::Persistent<v8::FunctionTemplate> persistent = v8::Persistent<v8::FunctionTemplate>::New(createQtClassTemplate(this, mo, mask));
    m_qtClassTemplates.insert(key, persistent);
    return persistent;
}

// We need a custom version of the 'toString' for dealing with objects created from QScriptClasses
v8::Handle<v8::FunctionTemplate> QScriptEnginePrivate::scriptClassToStringTemplate()
{
    if (m_scriptClassToStringTemplate.IsEmpty())
        m_scriptClassToStringTemplate = v8::Persistent<v8::FunctionTemplate>::New(QScriptClassPrivate::createToStringTemplate());
    return m_scriptClassToStringTemplate;
}

// Creates a QVariant wrapper object.
v8::Handle<v8::Object> QScriptEnginePrivate::newVariant(const QVariant &value)
{
    v8::Handle<v8::ObjectTemplate> instanceTempl = variantTemplate()->InstanceTemplate();
    v8::Handle<v8::Object> instance = instanceTempl->NewInstance();

    TypeInfos::TypeInfo info = m_typeInfos.value(value.userType());
    if (!info.prototype.IsEmpty())
        instance->SetPrototype(info.prototype);

    Q_ASSERT(instance->InternalFieldCount() == 1);
    QtVariantData *data = new QtVariantData(this, value);
    instance->SetPointerInInternalField(0, data);

    v8::Persistent<v8::Object> persistent = v8::Persistent<v8::Object>::New(instance);
    persistent.MakeWeak(data, QtVariantWeakCallback);
    return persistent;
}

v8::Isolate *QScriptEnginePrivate::Isolates::createEnterIsolate(QScriptEnginePrivate *engine)
{
    v8::Isolate *isolate = v8::Isolate::New();
    isolate->Enter();
    {
        Isolates *i = isolates();
        QMutexLocker lock(&(i->m_protector));
        i->m_mapping.insert(isolate, engine);
    }
    // FIXME It doesn't capture the stack, so backtrace test is failing.
    v8::V8::SetCaptureStackTraceForUncaughtExceptions(/* capture */ true, /* frame_limit */ 50);
    return isolate;
}

QScriptEnginePrivate *QScriptEnginePrivate::Isolates::engine(v8::Isolate *isolate)
{
    Isolates *i = isolates();
    QMutexLocker lock(&(i->m_protector));
    return i->m_mapping.value(isolate, 0);
}

QScriptEnginePrivate::QScriptEnginePrivate(QScriptEngine::ContextOwnership ownership)
    : m_isolate(Isolates::createEnterIsolate(this))
    , m_v8Context(ownership == QScriptEngine::AdoptCurrentContext ?
            v8::Persistent<v8::Context>::New(v8::Context::GetCurrent()) : v8::Context::New())
    , m_originalGlobalObject(this, m_v8Context)
    , m_reportedAddtionalMemoryCost(-1)
    , m_currentQsContext(0)
    , m_state(Idle)
    , m_currentAgent(0)
    , m_processEventTimeoutThread(0)
    , m_processEventInterval(-1)
    , m_shouldAbort(false)
{
    {
        v8::HandleScope scope;
        updateGlobalObjectCache();
    }
    qMetaTypeId<QScriptValue>();
    qMetaTypeId<QList<int> >();
    qMetaTypeId<QObjectList>();

    Q_ASSERT(!m_v8Context.IsEmpty());
    m_baseQsContext.reset(new QScriptContextPrivate(this));
    m_isolate->Exit();
}

// Creates a template for QMetaObject wrapper objects.
v8::Handle<v8::FunctionTemplate> QScriptEnginePrivate::createMetaObjectTemplate()
{
    v8::Handle<v8::FunctionTemplate> funcTempl = v8::FunctionTemplate::New();
    funcTempl->SetClassName(v8::String::New("QMetaObject"));

    v8::Handle<v8::ObjectTemplate> instTempl = funcTempl->InstanceTemplate();
    instTempl->SetInternalFieldCount(1); // QtMetaObjectData*

    instTempl->SetCallAsFunctionHandler(QtMetaObjectCallback);
    instTempl->SetNamedPropertyHandler(QtMetaObjectPropertyGetter, 0, 0, 0, QtMetaObjectEnumerator);

    return funcTempl;
}

// Creates a template for QVariant wrapper objects.
// QVariant wrapper objects have a signle internal field that
// contains a pointer to a QtVariantData object.
// The QVariant prototype object has functions toString() and valueOf().
v8::Handle<v8::FunctionTemplate> QScriptEnginePrivate::createVariantTemplate()
{
    v8::Handle<v8::FunctionTemplate> funcTempl = v8::FunctionTemplate::New();
    funcTempl->SetClassName(v8::String::New("QVariant"));

    v8::Handle<v8::ObjectTemplate> instTempl = funcTempl->InstanceTemplate();
    instTempl->SetInternalFieldCount(1); // QtVariantData*

    v8::Handle<v8::ObjectTemplate> protoTempl = funcTempl->PrototypeTemplate();
    v8::Handle<v8::Signature> sig = v8::Signature::New(funcTempl);
    protoTempl->Set(v8::String::New("toString"), v8::FunctionTemplate::New(QtVariantToStringCallback, v8::Handle<v8::Value>(), sig));
    protoTempl->Set(v8::String::New("valueOf"), v8::FunctionTemplate::New(QtVariantValueOfCallback, v8::Handle<v8::Value>(), sig));

    return funcTempl;
}

/*!
  Returns the hidden field name used to implement QScriptValue::data().
 */
v8::Handle<v8::String> QScriptEnginePrivate::qtDataId()
{
    if (m_qtDataId.IsEmpty())
        m_qtDataId = v8::Persistent<v8::String>::New(v8::String::NewSymbol("qt_data"));
    return m_qtDataId;
}

/* Thread used by setProcessEventInterval */
class QScriptEnginePrivate::ProcessEventTimeoutThread : public QThread {
    static void callback(void *data) {
        QScriptEnginePrivate *engine = static_cast<QScriptEnginePrivate *>(data);
        //executed in the JS thread
        if (engine->isEvaluating()) {
            QScriptContextPrivate qScriptContext(engine);
            qApp->processEvents();
        }
    }
    void run() {
        QScriptIsolate api(engine);
        QMutexLocker locker(&mutex);
        while(waitTime >= 0) {
            if (!cond.wait(&mutex, waitTime))
                v8::V8::ExecuteUserCallback(callback, engine);
        }
    }
public:
    int processEventInterval;
    int waitTime;
    QWaitCondition cond;
    QMutex mutex;
    QScriptEnginePrivate *engine;
    void destroy() {
        resetTime(-1);
        wait();
        delete this;
    }
    void resetTime(int newTime) {
        QMutexLocker locker (&mutex);
        waitTime = newTime;
        cond.wakeOne();
    }
};

QScriptEnginePrivate::EvaluateScope::EvaluateScope(QScriptEnginePrivate *engine)
    : engine(engine), wasEvaluating(engine->m_state == Evaluating)
{
    if (!wasEvaluating) {
        engine->m_shouldAbort = false;
        engine->m_state = Evaluating;
        if (engine->m_processEventTimeoutThread)
            engine->m_processEventTimeoutThread->resetTime(engine->m_processEventInterval);
    }
}
QScriptEnginePrivate::EvaluateScope::~EvaluateScope()
{
    if (!wasEvaluating) {
        if (engine->m_processEventTimeoutThread)
            engine->m_processEventTimeoutThread->resetTime(INT_MAX);
        engine->m_state = Idle;
    }
}

QScriptEnginePrivate::~QScriptEnginePrivate()
{
    m_isolate->Enter();

    if (m_processEventTimeoutThread)
        m_processEventTimeoutThread->destroy();

    Q_ASSERT_X(!m_currentAgent && m_agents.isEmpty(), Q_FUNC_INFO, "Destruction of QScriptEnginePrivate is not safe if an agent is active");
    invalidateAllScripts();
    invalidateAllValues();
    invalidateAllString();

    // FIXME Do we really need to dispose all persistent handlers before context destruction?
    m_variantTemplate.Dispose();
    m_metaObjectTemplate.Dispose();
    m_abortResult.Dispose();

    ClassTemplateHash::iterator i = m_qtClassTemplates.begin();
    for (; i != m_qtClassTemplates.end(); ++i) {
        (*i).Dispose();
    }
    m_qobjectBaseTemplate.Dispose();

    m_typeInfos.clear();
    clearExceptions();
    m_currentGlobalObject.Dispose();
    m_originalGlobalObject.destroy();

    m_v8Context->Exit(); // Exit the context that was entered in QScriptOriginalGlobalObject ctor.
    m_v8Context.Dispose();

    m_isolate->Exit();
    m_isolate->Dispose();
    m_state = Destroyed;

    deallocateAdditionalResources();
}

QScriptContextPrivate *QScriptEnginePrivate::pushContext()
{
    if (m_currentAgent)
        m_currentAgent->pushContext();
    return new QScriptContextPrivate(this, v8::Context::NewFunctionContext());
}

void QScriptEnginePrivate::popContext()
{
    QScriptContextPrivate *ctx = currentContext();
    if (!ctx->isPushedContext()) {
        qWarning("QScriptEngine::popContext() doesn't match with pushContext()");
    } else {
        delete ctx;
    }
    if (m_currentAgent)
        m_currentAgent->popContext();
}

QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::evaluate(v8::Handle<v8::Script> script, v8::TryCatch& tryCatch)
{
    v8::HandleScope handleScope;
    EvaluateScope evaluateScope(this);

    clearExceptions();
    if (script.IsEmpty()) {
        v8::Handle<v8::Value> exception = tryCatch.Exception();
        if (exception.IsEmpty()) {
            // This is possible on syntax errors like { a:12, b:21 } <- missing "(", ")" around expression.
            return InvalidValue();
        }
        setException(exception, tryCatch.Message());
        return new QScriptValuePrivate(this, exception);
    }
    v8::Handle<v8::Value> result;
    if (m_baseQsContext.data() == m_currentQsContext) {
        result = script->Run();
    } else {
        v8::Handle<v8::Object> thisObj = m_currentQsContext->thisObject();
        Q_ASSERT(!thisObj.IsEmpty());

        // Lazily initialize the 'arguments' property in JS context
        if (m_currentQsContext->isNativeFunction())
            m_currentQsContext->initializeArgumentsProperty();

        result = script->Run(thisObj);
    }
    if (m_shouldAbort) {
        v8::Local<v8::Value> abortResult = v8::Local<v8::Value>::New(m_abortResult);
        m_abortResult.Dispose();
        m_shouldAbort = false;
        if (abortResult.IsEmpty())
            return InvalidValue();
        return new QScriptValuePrivate(this, abortResult);
    }
    if (result.IsEmpty()) {
        v8::Handle<v8::Value> exception = tryCatch.Exception();
        // TODO: figure out why v8 doesn't always produce an exception value
        //Q_ASSERT(!exception.IsEmpty());
        if (exception.IsEmpty())
            exception = v8::Exception::Error(v8::String::New("missing exception value"));
        setException(exception, tryCatch.Message());
        return new QScriptValuePrivate(this, exception);
    }
    return new QScriptValuePrivate(this, result);
}

QScriptValue QScriptEngine::evaluate(const QScriptProgram& program)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(d->evaluate(QScriptProgramPrivate::get(program)));
}

v8::Handle<v8::Value> QScriptEnginePrivate::newQObject(QObject *object,
                                   QScriptEngine::ValueOwnership own,
                                   const QScriptEngine::QObjectWrapOptions &opt)
{
    if (!object)
        return makeJSValue(QScriptValue::NullValue);
    v8::HandleScope handleScope;
    v8::Handle<v8::FunctionTemplate> templ = this->qtClassTemplate(object->metaObject(), opt);
    Q_ASSERT(!templ.IsEmpty());
    v8::Handle<v8::ObjectTemplate> instanceTempl = templ->InstanceTemplate();
    Q_ASSERT(!instanceTempl.IsEmpty());
    v8::Handle<v8::Object> instance = instanceTempl->NewInstance();
    Q_ASSERT(instance->InternalFieldCount() == 1);

    /* FIXME according to valgrind this can leak tst_QScriptValue::getSetData_objects test */
    QScriptQObjectData *data = new QScriptQObjectData(this, object, own, opt);
    instance->SetPointerInInternalField(0, data);

    // Add accessors for current dynamic properties.
    {
        QList<QByteArray> dpNames = object->dynamicPropertyNames();
        for (int i = 0; i < dpNames.size(); ++i) {
            QByteArray name = dpNames.at(i);
            instance->SetAccessor(v8::String::New(name),
                                  QtDynamicPropertyGetter,
                                  QtDynamicPropertySetter);
        }
    }

    for (const QMetaObject* cls = object->metaObject(); cls; cls = cls->superClass()) {
        QByteArray className = cls->className();
        className.append("*"); // We are searching for a pointer.
        v8::Handle<v8::Object> prototype = defaultPrototype(className.data());
        if (!prototype.IsEmpty()) {
            instance->SetPrototype(prototype);
            break;
        }
    }

    v8::Persistent<v8::Object> persistent = v8::Persistent<v8::Object>::New(instance);
    persistent.MakeWeak(data, QScriptV8ObjectWrapperHelper::weakCallback<QScriptQObjectData>);
    return handleScope.Close(instance);
}

QScriptValue QScriptEnginePrivate::scriptValueFromInternal(v8::Handle<v8::Value> value) const 
{
    if (value.IsEmpty())
        return QScriptValuePrivate::get(InvalidValue());
    return QScriptValuePrivate::get(new QScriptValuePrivate(const_cast<QScriptEnginePrivate *>(this), value));
}

/*!
    Constructs a QScriptEngine object.

    The globalObject() is initialized to have properties as described in
    \l{ECMA-262}, Section 15.1.
*/
QScriptEngine::QScriptEngine()
    : QObject(*new QScriptEnginePrivate)
{
}

/*!
    \internal
*/
QScriptEngine::QScriptEngine(QScriptEngine::ContextOwnership ownership)
    : QObject(*new QScriptEnginePrivate(ownership))
{
}

/*!
    Constructs a QScriptEngine object with the given \a parent.

    The globalObject() is initialized to have properties as described in
    \l{ECMA-262}, Section 15.1.
*/

QScriptEngine::QScriptEngine(QObject *parent)
    : QObject(*new QScriptEnginePrivate, parent)
{
}

/*!
    Destroys this QScriptEngine.
*/
QScriptEngine::~QScriptEngine()
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d);
    // We need to delete all Agents here as they have virtual destructor which can use public engine
    // pointer.
    d->invalidateAllAgents();
}

/*!
  Checks the syntax of the given \a program. Returns a
  QScriptSyntaxCheckResult object that contains the result of the check.
*/
QScriptSyntaxCheckResult QScriptEngine::checkSyntax(const QString &program)
{
    return QScriptSyntaxCheckResultPrivate::get(new QScriptSyntaxCheckResultPrivate(program));
}

/*!
  \obsolete

  Returns true if \a program can be evaluated; i.e. the code is
  sufficient to determine whether it appears to be a syntactically
  correct program, or contains a syntax error.

  This function returns false if \a program is incomplete; i.e. the
  input is syntactically correct up to the point where the input is
  terminated.

  Note that this function only does a static check of \a program;
  e.g. it does not check whether references to variables are
  valid, and so on.

  A typical usage of canEvaluate() is to implement an interactive
  interpreter for QtScript. The user is repeatedly queried for
  individual lines of code; the lines are concatened internally, and
  only when canEvaluate() returns true for the resulting program is it
  passed to evaluate().

  The following are some examples to illustrate the behavior of
  canEvaluate(). (Note that all example inputs are assumed to have an
  explicit newline as their last character, since otherwise the
  QtScript parser would automatically insert a semi-colon character at
  the end of the input, and this could cause canEvaluate() to produce
  different results.)

  Given the input
  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 14
  canEvaluate() will return true, since the program appears to be complete.

  Given the input
  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 15
  canEvaluate() will return false, since the if-statement is not complete,
  but is syntactically correct so far.

  Given the input
  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 16
  canEvaluate() will return true, but evaluate() will throw a
  SyntaxError given the same input.

  Given the input
  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 17
  canEvaluate() will return true, even though the code is clearly not
  syntactically valid QtScript code. evaluate() will throw a
  SyntaxError when this code is evaluated.

  Given the input
  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 18
  canEvaluate() will return true, but evaluate() will throw a
  ReferenceError if \c{foo} is not defined in the script
  environment.

  \sa evaluate(), checkSyntax()
*/
bool QScriptEngine::canEvaluate(const QString &program) const
{
    return QScriptSyntaxCheckResultPrivate(program).state() != QScriptSyntaxCheckResult::Intermediate;
}

/*!
    Returns true if the last script evaluation resulted in an uncaught
    exception; otherwise returns false.

    The exception state is cleared when evaluate() is called.

    \sa uncaughtException(), uncaughtExceptionLineNumber(),
      uncaughtExceptionBacktrace()
*/
bool QScriptEngine::hasUncaughtException() const
{
    Q_D(const QScriptEngine);
    QScriptIsolate api(d);
    return d->hasUncaughtException();
}

/*!
    Returns the current uncaught exception, or an invalid QScriptValue
    if there is no uncaught exception.

    The exception value is typically an \c{Error} object; in that case,
    you can call toString() on the return value to obtain an error
    message.

    \sa hasUncaughtException(), uncaughtExceptionLineNumber(),
      uncaughtExceptionBacktrace()
*/
QScriptValue QScriptEngine::uncaughtException() const
{
    Q_D(const QScriptEngine);
    QScriptIsolate api(d);
    return d->scriptValueFromInternal(d->uncaughtException());
}

v8::Handle<v8::Value> QScriptEnginePrivate::uncaughtException() const
{
    if (!hasUncaughtException())
        return v8::Handle<v8::Value>();
    return m_exception;
}

/*!
    Clears any uncaught exceptions in this engine.

    \sa hasUncaughtException()
*/
void QScriptEngine::clearExceptions()
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d);
    d->clearExceptions();
}

/*!
    Returns the line number where the last uncaught exception occurred.

    Line numbers are 1-based, unless a different base was specified as
    the second argument to evaluate().

    \sa hasUncaughtException(), uncaughtExceptionBacktrace()
*/
int QScriptEngine::uncaughtExceptionLineNumber() const
{
    Q_D(const QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return d->uncaughtExceptionLineNumber();
}

/*!
    Returns a human-readable backtrace of the last uncaught exception.

    Each line is of the form \c{<function-name>(<arguments>)@<file-name>:<line-number>}.

    \sa uncaughtException()
*/
QStringList QScriptEngine::uncaughtExceptionBacktrace() const
{
    Q_D(const QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    return d->uncaughtExceptionBacktrace();
}


/*!
    Runs the garbage collector.

    The garbage collector will attempt to reclaim memory by locating and disposing of objects that are
    no longer reachable in the script environment.

    Normally you don't need to call this function; the garbage collector will automatically be invoked
    when the QScriptEngine decides that it's wise to do so (i.e. when a certain number of new objects
    have been created). However, you can call this function to explicitly request that garbage
    collection should be performed as soon as possible.

    \sa reportAdditionalMemoryCost()
*/
void QScriptEngine::collectGarbage()
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d);
    d->collectGarbage();
}

/*!
  \since 4.7

  Reports an additional memory cost of the given \a size, measured in
  bytes, to the garbage collector.

  This function can be called to indicate that a JavaScript object has
  memory associated with it that isn't managed by Qt Script itself.
  Reporting the additional cost makes it more likely that the garbage
  collector will be triggered.

  Note that if the additional memory is shared with objects outside
  the scripting environment, the cost should not be reported, since
  collecting the JavaScript object would not cause the memory to be
  freed anyway.

  Negative \a size values are ignored, i.e. this function can't be
  used to report that the additional memory has been deallocated.

  \sa collectGarbage()
*/
void QScriptEngine::reportAdditionalMemoryCost(int cost)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d);
    d->reportAdditionalMemoryCost(cost);
}

void QScriptEnginePrivate::GCEpilogueCallback(v8::GCType type, v8::GCCallbackFlags flags)
{
    Q_UNUSED(type);
    Q_UNUSED(flags);
    QScriptEnginePrivate *engine = Isolates::engine(v8::Isolate::GetCurrent());
    if (engine->m_reportedAddtionalMemoryCost) {
        v8::V8::AdjustAmountOfExternalAllocatedMemory(-engine->m_reportedAddtionalMemoryCost);
        engine->m_reportedAddtionalMemoryCost = 0;
    }
}

/*!
    Evaluates \a program, using \a lineNumber as the base line number,
    and returns the result of the evaluation.

    The script code will be evaluated in the current context.

    The evaluation of \a program can cause an exception in the
    engine; in this case the return value will be the exception
    that was thrown (typically an \c{Error} object). You can call
    hasUncaughtException() to determine if an exception occurred in
    the last call to evaluate().

    \a lineNumber is used to specify a starting line number for \a
    program; line number information reported by the engine that pertain
    to this evaluation (e.g. uncaughtExceptionLineNumber()) will be
    based on this argument. For example, if \a program consists of two
    lines of code, and the statement on the second line causes a script
    exception, uncaughtExceptionLineNumber() would return the given \a
    lineNumber plus one. When no starting line number is specified, line
    numbers will be 1-based.

    \a fileName is used for error reporting. For example in error objects
    the file name is accessible through the "fileName" property if it's
    provided with this function.
*/
QScriptValue QScriptEngine::evaluate(const QString& program, const QString& fileName, int lineNumber)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(d->evaluate(program, fileName, lineNumber));
}

/*!
  \since 4.4

  Returns true if this engine is currently evaluating a script,
  otherwise returns false.

  \sa evaluate(), abortEvaluation()
*/
bool QScriptEngine::isEvaluating() const
{
    Q_D(const QScriptEngine);
    return d->isEvaluating();
}

/*!
  \since 4.4

  Aborts any script evaluation currently taking place in this engine.
  The given \a result is passed back as the result of the evaluation
  (i.e. it is returned from the call to evaluate() being aborted).

  If the engine isn't evaluating a script (i.e. isEvaluating() returns
  false), this function does nothing.

  Call this function if you need to abort a running script for some
  reason, e.g.  when you have detected that the script has been
  running for several seconds without completing.

  \sa evaluate(), isEvaluating(), setProcessEventsInterval()
*/
void QScriptEngine::abortEvaluation(const QScriptValue &result)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d);
    v8::HandleScope handleScope;
    d->abortEvaluation(QScriptValuePrivate::get(result)->asV8Value(d));
}


QScriptValue QScriptEngine::nullValue()
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(new QScriptValuePrivate(d, v8::Null()));
}

QScriptValue QScriptEngine::undefinedValue()
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(new QScriptValuePrivate(d, v8::Undefined()));
}

QScriptValue QScriptEngine::newObject()
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(d->newObject());
}

QScriptValue QScriptEngine::newArray(uint length)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(d->newArray(length));
}

/*!
  Creates a QtScript object that wraps the given QObject \a
  object, using the given \a ownership. The given \a options control
  various aspects of the interaction with the resulting script object.

  Signals and slots, properties and children of \a object are
  available as properties of the created QScriptValue. For more
  information, see the \l{QtScript} documentation.

  If \a object is a null pointer, this function returns nullValue().

  If a default prototype has been registered for the \a object's class
  (or its superclass, recursively), the prototype of the new script
  object will be set to be that default prototype.

  If the given \a object is deleted outside of QtScript's control, any
  attempt to access the deleted QObject's members through the QtScript
  wrapper object (either by script code or C++) will result in a
  script exception.

  \sa QScriptValue::toQObject(), reportAdditionalMemoryCost()
*/
QScriptValue QScriptEngine::newQObject(QObject *object, ValueOwnership ownership,
                                       const QObjectWrapOptions &options)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return d->scriptValueFromInternal(d->newQObject(object, ownership, options));
}

/*!
  \since 4.4
  \overload

  Initializes the given \a scriptObject to hold the given \a qtObject,
  and returns the \a scriptObject.

  This function enables you to "promote" a plain Qt Script object
  (created by the newObject() function) to a QObject proxy, or to
  replace the QObject contained inside an object previously created by
  the newQObject() function.

  The prototype() of the \a scriptObject will remain unchanged.

  If \a scriptObject is not an object, this function behaves like the
  normal newQObject(), i.e. it creates a new script object and returns
  it.

  This function is useful when you want to provide a script
  constructor for a QObject-based class. If your constructor is
  invoked in a \c{new} expression
  (QScriptContext::isCalledAsConstructor() returns true), you can pass
  QScriptContext::thisObject() (the default constructed script object)
  to this function to initialize the new object.

  \sa reportAdditionalMemoryCost()
*/
QScriptValue QScriptEngine::newQObject(const QScriptValue &scriptObject, QObject *qtObject,
                                       ValueOwnership ownership, const QObjectWrapOptions &options)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(d->newQObject(QScriptValuePrivate::get(scriptObject), qtObject, ownership, options));
}

/*!
    Creates a QScriptValue that wraps a native (C++) function. \a fun
    must be a C++ function with signature QScriptEngine::FunctionSignature.
    \a length is the number of arguments that \a fun expects; this becomes
    the \c{length} property of the created QScriptValue.

    Note that \a length only gives an indication of the number of
    arguments that the function expects; an actual invocation of a
    function can include any number of arguments. You can check the
    \l{QScriptContext::argumentCount()}{argumentCount()} of the
    QScriptContext associated with the invocation to determine the
    actual number of arguments passed.

    A \c{prototype} property is automatically created for the resulting
    function object, to provide for the possibility that the function
    will be used as a constructor.

    By combining newFunction() and the property flags
    QScriptValue::PropertyGetter and QScriptValue::PropertySetter, you
    can create script object properties that behave like normal
    properties in script code, but are in fact accessed through
    functions (analogous to how properties work in \l{Qt's Property
    System}). Example:

    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 11

    When the property \c{foo} of the script object is subsequently
    accessed in script code, \c{getSetFoo()} will be invoked to handle
    the access.  In this particular case, we chose to store the "real"
    value of \c{foo} as a property of the accessor function itself; you
    are of course free to do whatever you like in this function.

    In the above example, a single native function was used to handle
    both reads and writes to the property; the argument count is used to
    determine if we are handling a read or write. You can also use two
    separate functions; just specify the relevant flag
    (QScriptValue::PropertyGetter or QScriptValue::PropertySetter) when
    setting the property, e.g.:

    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 12

    \sa QScriptValue::call()
*/
QScriptValue QScriptEngine::newFunction(QScriptEngine::FunctionSignature fun, int length)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(d->newFunction(fun, 0, length));
}

/*!
    Creates a constructor function from \a fun, with the given \a length.
    The \c{prototype} property of the resulting function is set to be the
    given \a prototype. The \c{constructor} property of \a prototype is
    set to be the resulting function.

    When a function is called as a constructor (e.g. \c{new Foo()}), the
    `this' object associated with the function call is the new object
    that the function is expected to initialize; the prototype of this
    default constructed object will be the function's public
    \c{prototype} property. If you always want the function to behave as
    a constructor (e.g. \c{Foo()} should also create a new object), or
    if you need to create your own object rather than using the default
    `this' object, you should make sure that the prototype of your
    object is set correctly; either by setting it manually, or, when
    wrapping a custom type, by having registered the defaultPrototype()
    of that type. Example:

    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 9

    To wrap a custom type and provide a constructor for it, you'd typically
    do something like this:

    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 10
*/
QScriptValue QScriptEngine::newFunction(QScriptEngine::FunctionSignature fun, const QScriptValue &prototype, int length)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(d->newFunction(fun, QScriptValuePrivate::get(prototype), length));
}

/*!
    \internal
    \since 4.4
*/
QScriptValue QScriptEngine::newFunction(QScriptEngine::FunctionWithArgSignature fun, void *arg)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(d->newFunction(fun, arg));
}

/*!
  Creates a QtScript object holding the given variant \a value.

  If a default prototype has been registered with the meta type id of
  \a value, then the prototype of the created object will be that
  prototype; otherwise, the prototype will be the Object prototype
  object.

  \sa setDefaultPrototype(), QScriptValue::toVariant(), reportAdditionalMemoryCost()
*/
QScriptValue QScriptEngine::newVariant(const QVariant &value)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return d->scriptValueFromInternal(d->newVariant(value));
}

/*!
  \since 4.4
  \overload

  Initializes the given Qt Script \a object to hold the given variant
  \a value, and returns the \a object.

  This function enables you to "promote" a plain Qt Script object
  (created by the newObject() function) to a variant, or to replace
  the variant contained inside an object previously created by the
  newVariant() function.

  The prototype() of the \a object will remain unchanged.

  If \a object is not an object, this function behaves like the normal
  newVariant(), i.e. it creates a new script object and returns it.

  This function is useful when you want to provide a script
  constructor for a C++ type. If your constructor is invoked in a
  \c{new} expression (QScriptContext::isCalledAsConstructor() returns
  true), you can pass QScriptContext::thisObject() (the default
  constructed script object) to this function to initialize the new
  object.

  \sa reportAdditionalMemoryCost()
*/
QScriptValue QScriptEngine::newVariant(const QScriptValue &object,
                                       const QVariant &value)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(d->newVariant(QScriptValuePrivate::get(object), value));
}


QScriptValue QScriptEngine::globalObject() const
{
    Q_D(const QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return d->scriptValueFromInternal(d->globalObject());
}

void QScriptEnginePrivate::setGlobalObject(QScriptValuePrivate* newGlobalObjectValue)
{
    if (!newGlobalObjectValue->isObject())
        return;

    v8::Handle<v8::Value> securityToken = m_v8Context->GetSecurityToken();
    m_v8Context->Exit();
    m_v8Context.Dispose();
    m_v8Context = v8::Context::New();
    m_v8Context->Enter();
    m_v8Context->SetSecurityToken(securityToken);
    updateGlobalObjectCache();
    v8::Handle<v8::Object> global = globalObject();
    global->SetPrototype(*newGlobalObjectValue);
    newGlobalObjectValue->reinitialize(this, global);
}

/*!
  \since 4.5

  Sets this engine's Global Object to be the given \a object.
  If \a object is not a valid script object, this function does
  nothing.

  When setting a custom global object, you may want to use
  QScriptValueIterator to copy the properties of the standard Global
  Object; alternatively, you can set the internal prototype of your
  custom object to be the original Global Object.
*/
void QScriptEngine::setGlobalObject(const QScriptValue &object)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    d->setGlobalObject(QScriptValuePrivate::get(object));
}

/*!
  Returns the default prototype associated with the given \a metaTypeId,
  or an invalid QScriptValue if no default prototype has been set.

  \sa setDefaultPrototype()
*/
QScriptValue QScriptEngine::defaultPrototype(int metaTypeId) const
{
    Q_D(const QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    return QScriptValuePrivate::get(const_cast<QScriptEnginePrivate *>(d)->defaultPrototype(metaTypeId));
}

/*!
  Sets the default prototype of the C++ type identified by the given
  \a metaTypeId to \a prototype.

  The default prototype provides a script interface for values of
  type \a metaTypeId when a value of that type is accessed from script
  code.  Whenever the script engine (implicitly or explicitly) creates
  a QScriptValue from a value of type \a metaTypeId, the default
  prototype will be set as the QScriptValue's prototype.

  The \a prototype object itself may be constructed using one of two
  principal techniques; the simplest is to subclass QScriptable, which
  enables you to define the scripting API of the type through QObject
  properties and slots.  Another possibility is to create a script
  object by calling newObject(), and populate the object with the
  desired properties (e.g. native functions wrapped with
  newFunction()).

  \sa defaultPrototype(), qScriptRegisterMetaType(), QScriptable, {Default Prototypes Example}
*/
void QScriptEngine::setDefaultPrototype(int metaTypeId, const QScriptValue &prototype)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    d->setDefaultPrototype(metaTypeId, QScriptValuePrivate::get(prototype));
}

void QScriptEnginePrivate::setDefaultPrototype(int metaTypeId, const QScriptValuePrivate *prototype)
{
    TypeInfos::TypeInfo info = m_typeInfos.value(metaTypeId);
    if (prototype->isObject())
        m_typeInfos.registerCustomType(metaTypeId, info.marshal, info.demarshal, *prototype);
    if (!prototype->isValid() || prototype->isNull()) {
        m_typeInfos.registerCustomType(metaTypeId, info.marshal, info.demarshal);
    }
}

QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::defaultPrototype(int metaTypeId)
{
    TypeInfos::TypeInfo info = m_typeInfos.value(metaTypeId);
    if (info.prototype.IsEmpty())
        return InvalidValue();
    return new QScriptValuePrivate(this, info.prototype);
}

v8::Handle<v8::Object> QScriptEnginePrivate::defaultPrototype(const char* metaTypeName)
{
    int metaTypeId = QMetaType::type(metaTypeName);
    TypeInfos::TypeInfo info = m_typeInfos.value(metaTypeId);
    return info.prototype;
}

QScriptString QScriptEngine::toStringHandle(const QString& str)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptStringPrivate::get(new QScriptStringPrivate(d, QScriptConverter::toString(str)));
}

QScriptValue QScriptEngine::toObject(const QScriptValue& value)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(QScriptValuePrivate::get(value)->toObject(d));
}

QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::newObject()
{
    return new QScriptValuePrivate(this, v8::Object::New());
}

QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::newObject(QScriptClassPrivate* scriptclass, QScriptValuePrivate* data)
{
    QScriptPassPointer<QScriptValuePrivate> object =
        new QScriptValuePrivate(this, QScriptClassObject::newInstance(scriptclass, v8::Handle<v8::Object>(), this));
    object->setData(data);
    return object;
}

QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::newArray(uint length)
{
    if (int(length) < 0) {
        // FIXME v8 have a limitation that max size of an array is 512 MB (it is defined in object.h FixedArray::kMaxSize)
        // It is wrong as ECMA standard says something different (15.4). It have to be reported, for now try to not crash.
        Q_UNIMPLEMENTED();
        length = 12345;
    }

    v8::Persistent<v8::Array> array(v8::Persistent<v8::Array>::New(v8::Array::New(length)));
    // FIXME: This is a workaround for http://code.google.com/p/v8/issues/detail?id=1256
    array->Set(v8::String::New("length"), v8::Number::New(length));
    return new QScriptValuePrivate(this, array);
}

QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::newFunction(QScriptEngine::FunctionSignature fun, QScriptValuePrivate *prototype, int length)
{
    Q_UNUSED(length);

    // FIXME Valgrind said that we are leaking here!
    QScriptNativeFunctionData *data = new QScriptNativeFunctionData(this, fun);
    v8::Local<v8::Value> dataJS = v8::External::New(reinterpret_cast<void *>(data));

    // ### We need to create a FunctionTemplate and use the GetFunction() until we come up
    // with a way of creating instances of a Template that are Functions (for IsFunction()),
    // then we could share the templates and hold the 'dataJS' in an internal field.
    v8::Local<v8::FunctionTemplate> funTempl = v8::FunctionTemplate::New(QtNativeFunctionCallback<QScriptNativeFunctionData>, dataJS);
    v8::Persistent<v8::Function> function = v8::Persistent<v8::Function>::New(funTempl->GetFunction());

    // ### Note that I couldn't make this callback to be called, so for some reason we
    // are leaking this.
    function.MakeWeak(reinterpret_cast<void *>(data), QtNativeFunctionCleanup<QScriptNativeFunctionData>);

    QScriptPassPointer<QScriptValuePrivate> result(new QScriptValuePrivate(this, function));

    if (prototype) {
        result->setProperty(v8::String::New("prototype"), prototype, v8::PropertyAttribute(v8::DontDelete | v8::DontEnum));
        prototype->setProperty(v8::String::New("constructor"), result.data(), v8::DontEnum);
    }

    return result;
}

QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::newFunction(QScriptEngine::FunctionWithArgSignature fun, void *arg)
{
    // See other newFunction() for commentary. They should have similar implementations.
    // FIXME valgrind said that we are leaking here!
    QScriptNativeFunctionWithArgData *data = new QScriptNativeFunctionWithArgData(this, fun, arg);
    v8::Local<v8::Value> dataJS(v8::External::New(reinterpret_cast<void *>(data)));

    v8::Local<v8::FunctionTemplate> funTempl = v8::FunctionTemplate::New(QtNativeFunctionCallback<QScriptNativeFunctionWithArgData>, dataJS);
    v8::Persistent<v8::Function> function = v8::Persistent<v8::Function>::New(funTempl->GetFunction());

    function.MakeWeak(reinterpret_cast<void *>(data), QtNativeFunctionCleanup<QScriptNativeFunctionWithArgData>);

    return new QScriptValuePrivate(this, function);
}

QScriptValue QScriptEngine::newObject(QScriptClass *scriptclass, const QScriptValue &data)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(d->newObject(QScriptClassPrivate::get(scriptclass), QScriptValuePrivate::get(data)));
}

/*!
  Creates a QtScript object of class Date from the given \a value.

  \sa QScriptValue::toDateTime()
*/
QScriptValue QScriptEngine::newDate(const QDateTime &dt)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return d->scriptValueFromInternal(v8::Handle<v8::Value>(d->qtDateTimeToJS(dt)));
}

/*!
  Creates a QtScript object of class Date with the given
  \a value (the number of milliseconds since 01 January 1970,
  UTC).
*/
QScriptValue QScriptEngine::newDate(double date)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return d->scriptValueFromInternal(v8::Handle<v8::Value>(v8::Date::New(date)));
}

/*!
  Creates a QtScript object of class RegExp with the given
  \a regexp.

  \sa QScriptValue::toRegExp()
*/
QScriptValue QScriptEngine::newRegExp(const QRegExp &regexp)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(d->newRegExp(regexp));
}

/*!
  Creates a QtScript object of class RegExp with the given
  \a pattern and \a flags.

  The legal flags are 'g' (global), 'i' (ignore case), and 'm'
  (multiline).
*/
QScriptValue QScriptEngine::newRegExp(const QString &pattern, const QString &flags)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return QScriptValuePrivate::get(d->newRegExp(pattern, flags));
}

QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::newRegExp(const QString &pattern, const QString &flags)
{
    int f = v8::RegExp::kNone;

    QString::const_iterator i = flags.constBegin();
    for (; i != flags.constEnd(); ++i) {
        switch (i->unicode()) {
        case 'i':
            f |= v8::RegExp::kIgnoreCase;
            break;
        case 'm':
            f |= v8::RegExp::kMultiline;
            break;
        case 'g':
            f |= v8::RegExp::kGlobal;
            break;
        default:
            {
                // ignore a Syntax Error.
            }
        }
    }

    v8::Handle<v8::RegExp> regexp = v8::RegExp::New(QScriptConverter::toString(pattern), static_cast<v8::RegExp::Flags>(f));
    return new QScriptValuePrivate(this, regexp);
}

QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::newRegExp(const QRegExp &regexp)
{
    return new QScriptValuePrivate(this, QScriptConverter::toRegExp(regexp));
}

/*!
 * Creates a QtScript object that represents a QObject class, using the
 * the given \a metaObject and constructor \a ctor.
 *
 * Enums of \a metaObject (declared with Q_ENUMS) are available as
 * properties of the created QScriptValue. When the class is called as
 * a function, \a ctor will be called to create a new instance of the
 * class.
 *
 * Example:
 *
 * \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 27
 *
 * \sa newQObject(), scriptValueFromQMetaObject()
 */
QScriptValue QScriptEngine::newQMetaObject(const QMetaObject *metaObject, const QScriptValue &ctor)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return d->scriptValueFromInternal(d->newQMetaObject(metaObject, ctor));
}

QScriptValue QScriptEngine::newActivationObject()
{
    Q_UNIMPLEMENTED();
    return QScriptValue();
}

/*!
  \internal

  Returns the object with the given \a id, or an invalid
  QScriptValue if there is no object with that id.

  \note This will crash or return wrong value if the garbage collector has been run.

  \sa QScriptValue::objectId()
*/
QScriptValue QScriptEngine::objectById(qint64 id) const
{
    Q_D(const QScriptEngine);
    if(id == -1)
        return QScriptValue();
    quintptr ptr = id;
    quintptr *ptrptr = &ptr;
    QScriptIsolate api(d);
    v8::HandleScope handleScope;
    return const_cast<QScriptEnginePrivate *>(d)->scriptValueFromInternal(v8::Handle<v8::Value>(*reinterpret_cast<v8::Value **>(&ptrptr)));
}

/*!
 *  \internal
 * used by QScriptEngine::toScriptValue
 */
QScriptValue QScriptEngine::create(int type, const void *ptr)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return d->scriptValueFromInternal(d->metaTypeToJS(type, ptr));
}

/*!
    \internal
    Called from inline code prior to Qt 4.5.
*/
bool QScriptEngine::convert(const QScriptValue &value, int type, void *ptr)
{
    return convertV2(value, type, ptr);
}

/*!
    \internal
    \since 4.5
    convert \a value to \a type, store the result in \a ptr
*/
bool QScriptEngine::convertV2(const QScriptValue &value, int type, void *ptr)
{
    QScriptValuePrivate *vp = QScriptValuePrivate::get(value);
    QScriptEnginePrivate *engine = vp->engine();
    if (engine) {
        QScriptIsolate api(engine, QScriptIsolate::NotNullEngine);
        v8::HandleScope handleScope;
        return engine->metaTypeFromJS(*vp, type, ptr);
    } else {
        switch (type) {
            case QMetaType::Bool:
                *reinterpret_cast<bool*>(ptr) = vp->toBool();
                return true;
            case QMetaType::Int:
                *reinterpret_cast<int*>(ptr) = vp->toInt32();
                return true;
            case QMetaType::UInt:
                *reinterpret_cast<uint*>(ptr) = vp->toUInt32();
                return true;
            case QMetaType::LongLong:
                *reinterpret_cast<qlonglong*>(ptr) = vp->toInteger();
                return true;
            case QMetaType::ULongLong:
                *reinterpret_cast<qulonglong*>(ptr) = vp->toInteger();
                return true;
            case QMetaType::Double:
                *reinterpret_cast<double*>(ptr) = vp->toNumber();
                return true;
            case QMetaType::QString:
                *reinterpret_cast<QString*>(ptr) = vp->toString();
                return true;
            case QMetaType::Float:
                *reinterpret_cast<float*>(ptr) = vp->toNumber();
                return true;
            case QMetaType::Short:
                *reinterpret_cast<short*>(ptr) = vp->toInt32();
                return true;
            case QMetaType::UShort:
                *reinterpret_cast<unsigned short*>(ptr) = vp->toUInt16();
                return true;
            case QMetaType::Char:
                *reinterpret_cast<char*>(ptr) = vp->toInt32();
                return true;
            case QMetaType::UChar:
                *reinterpret_cast<unsigned char*>(ptr) = vp->toUInt16();
                return true;
            case QMetaType::QChar:
                *reinterpret_cast<QChar*>(ptr) = vp->toUInt16();
                return true;
            default:
                return false;
        }
    }
}

void QScriptEngine::registerCustomType(int type, MarshalFunction mf, DemarshalFunction df,
                                       const QScriptValue &prototype)
{
    Q_D(QScriptEngine);
    d->registerCustomType(type, mf, df, QScriptValuePrivate::get(prototype));
}

void QScriptEnginePrivate::registerCustomType(int type, QScriptEngine::MarshalFunction mf, QScriptEngine::DemarshalFunction df, const QScriptValuePrivate *prototype)
{
    if (prototype->isObject())
        m_typeInfos.registerCustomType(type, mf, df, *prototype);
    else
        m_typeInfos.registerCustomType(type, mf, df);
}

QScriptContext *QScriptEngine::currentContext() const
{
    Q_D(const QScriptEngine);
    return d->currentContext();
}

QScriptContext *QScriptEngine::pushContext()
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    return d->pushContext();
}

void QScriptEngine::popContext()
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    d->popContext();
}

void QScriptEngine::installTranslatorFunctions(const QScriptValue &object)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    d->installTranslatorFunctions(QScriptValuePrivate::get(object));
}

v8::Handle<v8::Value> QtTranslateFunctionQsTranslate(const v8::Arguments& arguments)
{
    if (arguments.Length() < 2) {
        return v8::ThrowException(v8::Exception::Error(v8::String::New("qsTranslate() requires at least two arguments")));
    }
    if (!arguments[0]->IsString()) {
        return v8::ThrowException(v8::Exception::Error(v8::String::New("qsTranslate(): first argument (context) must be a string")));
    }
    if (!arguments[1]->IsString()) {
        return v8::ThrowException(v8::Exception::Error(v8::String::New("qsTranslate(): second argument (text) must be a string")));
    }
    if ((arguments.Length() > 2) && !arguments[2]->IsString()) {
        return v8::ThrowException(v8::Exception::Error(v8::String::New("qsTranslate(): third argument (comment) must be a string")));
    }
    if ((arguments.Length() > 3) && !arguments[3]->IsString()) {
        return v8::ThrowException(v8::Exception::Error(v8::String::New("qsTranslate(): fourth argument (encoding) must be a string")));
    }
    if ((arguments.Length() > 4) && !arguments[4]->IsNumber()) {
        return v8::ThrowException(v8::Exception::Error(v8::String::New("qsTranslate(): fifth argument (n) must be a number")));
    }

    QString context(QScriptConverter::toString(arguments[0]->ToString()));
    QString text(QScriptConverter::toString(arguments[1]->ToString()));
    QString comment;
    if (arguments.Length() > 2)
        comment = QScriptConverter::toString(arguments[2]->ToString());
    QCoreApplication::Encoding encoding = QCoreApplication::UnicodeUTF8;
    if (arguments.Length() > 3) {
        QString encStr(QScriptConverter::toString(arguments[3]->ToString()));
        if (encStr == QLatin1String("CodecForTr"))
            encoding = QCoreApplication::CodecForTr;
        else if (encStr == QLatin1String("UnicodeUTF8"))
            encoding = QCoreApplication::UnicodeUTF8;
        else {
            QString errorStr =  QString::fromLatin1("qsTranslate(): invalid encoding '%0'").arg(encStr);
            return v8::ThrowException(v8::Exception::Error(QScriptConverter::toString(errorStr)));
        }
    }
    int n = -1;
    if (arguments.Length() > 4)
        n = arguments[4]->Int32Value();
    QString result = QCoreApplication::translate(context.toUtf8().constData(),
                                         text.toUtf8().constData(),
                                         comment.toUtf8().constData(),
                                         encoding, n);
    return QScriptConverter::toString(result);
}

v8::Handle<v8::Value> QtTranslateFunctionQsTranslateNoOp(const v8::Arguments& arguments)
{
    if (arguments.Length() < 2)
        return v8::Undefined();
    return arguments[1];
}

v8::Handle<v8::Value> QtTranslateFunctionQsTr(const v8::Arguments& arguments)
{
    if (arguments.Length() < 1) {
        return v8::ThrowException(v8::Exception::Error(v8::String::New("qsTr() requires at least one argument")));
    }
    if (!arguments[0]->IsString()) {
        return v8::ThrowException(v8::Exception::Error(v8::String::New("qsTr(): first argument (text) must be a string")));
    }
    if ((arguments.Length() > 1) && !arguments[1]->IsString()) {
        return v8::ThrowException(v8::Exception::Error(v8::String::New("qsTr(): second argument (comment) must be a string")));
    }
    if ((arguments.Length() > 2) && !arguments[2]->IsNumber()) {
        return v8::ThrowException(v8::Exception::Error(v8::String::New("qsTr(): third argument (n) must be a number")));
    }

    QString context;
    // This engine should be always valid, because this function can be called if and only if the engine is still alive.
    QScriptEnginePrivate *engine = static_cast<QScriptEnginePrivate*>(v8::Handle<v8::Object>::Cast(arguments.Data())->GetPointerFromInternalField(0));
    QScriptContextPrivate qScriptContext(engine, &arguments);
    QScriptContext *ctx = qScriptContext.parentContext();
    while (ctx) {
        QString fn = QScriptContextInfo(ctx).fileName();
        if (!fn.isEmpty()) {
            context = QFileInfo(fn).baseName();
            break;
        }
        ctx = ctx->parentContext();
    }
    QString text(QScriptConverter::toString(arguments[0]->ToString()));
    QString comment;
    if (arguments.Length() > 1)
        comment = QScriptConverter::toString(arguments[1]->ToString());
    int n = -1;
    if (arguments.Length() > 2)
        n = arguments[2]->Int32Value();

    QString result = QCoreApplication::translate(context.toUtf8().constData(),
                                         text.toUtf8().constData(),
                                         comment.toUtf8().constData(),
                                         QCoreApplication::UnicodeUTF8, n);
    return QScriptConverter::toString(result);
}

v8::Handle<v8::Value> QtTranslateFunctionQsTrNoOp(const v8::Arguments& arguments)
{
    if (arguments.Length() < 1)
        return v8::Undefined();
    return arguments[0];
}

v8::Handle<v8::Value> QtTranslateFunctionQsTrId(const v8::Arguments& arguments)
{
    if (arguments.Length() < 1) {
        return v8::ThrowException(v8::Exception::Error(v8::String::New("qsTrId() requires at least one argument")));
    }
    if (!arguments[0]->IsString()) {
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New("qsTrId(): first argument (id) must be a string")));
    }
    if ((arguments.Length() > 1) && !arguments[1]->IsNumber()) {
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New("qsTrId(): second argument (n) must be a number")));
    }
    v8::Handle<v8::String> id = arguments[0]->ToString();
    int n = -1;
    if (arguments.Length() > 1)
        n = arguments[1]->Int32Value();
    return QScriptConverter::toString(qtTrId(QScriptConverter::toString(id).toUtf8().constData(), n));
}

v8::Handle<v8::Value> QtTranslateFunctionQsTrIdNoOp(const v8::Arguments& arguments)
{
    if (arguments.Length() < 1)
        return v8::Undefined();
    return arguments[0];
}

v8::Handle<v8::Value> QtTranslateFunctionStringArg(const v8::Arguments& arguments)
{
    QString value(QScriptConverter::toString(arguments.This()->ToString()));
    v8::Handle<v8::Value> arg;
    if (arguments.Length() != 0)
        arg = arguments[0];
    else
        arg = v8::Undefined();
    QString result;
    if (arg->IsString())
        result = value.arg(QScriptConverter::toString(arg->ToString()));
    else if (arg->IsNumber())
        result = value.arg(arg->NumberValue());
    return QScriptConverter::toString(result);
}

void QScriptEnginePrivate::installTranslatorFunctions(QScriptValuePrivate* object)
{
    if (object->isObject())
        installTranslatorFunctions(*object);
    else
        installTranslatorFunctions(m_v8Context->Global());

    // FIXME That is strange operation, I believe that Qt5 should change it. Why we are installing
    // arg funciton on String prototype even if it could be not accessible? why we are support
    // String.prototype.arg even if it doesn't exist after setGlobalObject call?
    m_originalGlobalObject.installArgFunctionOnOrgStringPrototype(v8::FunctionTemplate::New(QtTranslateFunctionStringArg)->GetFunction());
}

void QScriptEnginePrivate::installTranslatorFunctions(v8::Handle<v8::Value> value)
{
    Q_ASSERT(value->IsObject());
    v8::Handle<v8::Object> object = v8::Handle<v8::Object>::Cast(value);

    v8::Handle<v8::ObjectTemplate> dataTemplate = v8::ObjectTemplate::New();
    dataTemplate->SetInternalFieldCount(1);
    v8::Handle<v8::Object> data = dataTemplate->NewInstance();
    data->SetPointerInInternalField(0, this);
    object->Set(v8::String::New("qsTranslate"), v8::FunctionTemplate::New(QtTranslateFunctionQsTranslate)->GetFunction());
    object->Set(v8::String::New("QT_TRANSLATE_NOOP"), v8::FunctionTemplate::New(QtTranslateFunctionQsTranslateNoOp)->GetFunction());
    object->Set(v8::String::New("qsTr"), v8::FunctionTemplate::New(QtTranslateFunctionQsTr, data)->GetFunction());
    object->Set(v8::String::New("QT_TR_NOOP"), v8::FunctionTemplate::New(QtTranslateFunctionQsTrNoOp)->GetFunction());
    object->Set(v8::String::New("qsTrId"), v8::FunctionTemplate::New(QtTranslateFunctionQsTrId)->GetFunction());
    object->Set(v8::String::New("QT_TRID_NOOP"), v8::FunctionTemplate::New(QtTranslateFunctionQsTrIdNoOp)->GetFunction());
}

#if !defined(QT_NO_QOBJECT) && !defined(QT_NO_LIBRARY)
static QScriptValue QtSetupPackage(QScriptContext *ctx, QScriptEngine *eng)
{
    QString path = ctx->argument(0).toString();
    QStringList components = path.split(QLatin1Char('.'));
    QScriptValue o = eng->globalObject();
    for (int i = 0; i < components.count(); ++i) {
        QString name = components.at(i);
        QScriptValue oo = o.property(name);
        if (!oo.isValid()) {
            oo = eng->newObject();
            o.setProperty(name, oo);
        }
        o = oo;
    }
    return o;
}
#endif

QScriptValue QScriptEngine::importExtension(const QString &extension)
{
#if defined(QT_NO_QOBJECT) || defined(QT_NO_LIBRARY) || defined(QT_NO_SETTINGS)
    Q_UNUSED(extension);
#else
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    if (d->importedExtensions.contains(extension))
        return undefinedValue(); // already imported

    QScriptContext *context = currentContext();
    QCoreApplication *app = QCoreApplication::instance();
    if (!app)
        return context->throwError(QLatin1String("No application object"));

    QObjectList staticPlugins = QPluginLoader::staticInstances();
    QStringList libraryPaths = app->libraryPaths();
    QString dot = QLatin1String(".");
    QStringList pathComponents = extension.split(dot);
    QString initDotJs = QLatin1String("__init__.js");

    QString ext;
    for (int i = 0; i < pathComponents.count(); ++i) {
        if (!ext.isEmpty())
            ext.append(dot);
        ext.append(pathComponents.at(i));
        if (d->importedExtensions.contains(ext))
            continue; // already imported

        if (d->extensionsBeingImported.contains(ext)) {
            return context->throwError(QString::fromLatin1("recursive import of %0")
                                       .arg(extension));
        }
        d->extensionsBeingImported.insert(ext);

        QScriptExtensionInterface *iface = 0;
        QString initjsContents;
        QString initjsFileName;

        // look for the extension in static plugins
        for (int j = 0; j < staticPlugins.size(); ++j) {
            iface = qobject_cast<QScriptExtensionInterface*>(staticPlugins.at(j));
            if (!iface)
                continue;
            if (iface->keys().contains(ext))
                break; // use this one
            else
                iface = 0; // keep looking
        }

        {
            // look for __init__.js resource
            QString path = QString::fromLatin1(":/qtscriptextension");
            for (int j = 0; j <= i; ++j) {
                path.append(QLatin1Char('/'));
                path.append(pathComponents.at(j));
            }
            path.append(QLatin1Char('/'));
            path.append(initDotJs);
            QFile file(path);
            if (file.open(QIODevice::ReadOnly)) {
                QTextStream ts(&file);
                initjsContents = ts.readAll();
                initjsFileName = path;
                file.close();
            }
        }

        if (!iface && initjsContents.isEmpty()) {
            // look for the extension in library paths
            for (int j = 0; j < libraryPaths.count(); ++j) {
                QString libPath = libraryPaths.at(j) + QDir::separator() + QLatin1String("script");
                QDir dir(libPath);
                if (!dir.exists(dot))
                    continue;

                // look for C++ plugin
                QFileInfoList files = dir.entryInfoList(QDir::Files);
                for (int k = 0; k < files.count(); ++k) {
                    QFileInfo entry = files.at(k);
                    QString filePath = entry.canonicalFilePath();
                    QPluginLoader loader(filePath);
                    iface = qobject_cast<QScriptExtensionInterface*>(loader.instance());
                    if (iface) {
                        if (iface->keys().contains(ext))
                            break; // use this one
                        else
                            iface = 0; // keep looking
                    }
                }

                // look for __init__.js in the corresponding dir
                QDir dirdir(libPath);
                bool dirExists = dirdir.exists();
                for (int k = 0; dirExists && (k <= i); ++k)
                    dirExists = dirdir.cd(pathComponents.at(k));
                if (dirExists && dirdir.exists(initDotJs)) {
                    QFile file(dirdir.canonicalPath()
                               + QDir::separator() + initDotJs);
                    if (file.open(QIODevice::ReadOnly)) {
                        QTextStream ts(&file);
                        initjsContents = ts.readAll();
                        initjsFileName = file.fileName();
                        file.close();
                    }
                }

                if (iface || !initjsContents.isEmpty())
                    break;
            }
        }

        if (!iface && initjsContents.isEmpty()) {
            d->extensionsBeingImported.remove(ext);
            return context->throwError(
                QString::fromLatin1("Unable to import %0: no such extension")
                .arg(extension));
        }

        // initialize the extension in a new context
        QScriptContext *ctx = pushContext();
        ctx->setThisObject(globalObject());
        ctx->activationObject().setProperty(QLatin1String("__extension__"), ext,
                                            QScriptValue::ReadOnly | QScriptValue::Undeletable);
        ctx->activationObject().setProperty(QLatin1String("__setupPackage__"),
                                            newFunction(QtSetupPackage));
        ctx->activationObject().setProperty(QLatin1String("__postInit__"), QScriptValue(QScriptValue::UndefinedValue));

        // the script is evaluated first
        if (!initjsContents.isEmpty()) {
            QScriptValue ret = evaluate(initjsContents, initjsFileName);
            if (hasUncaughtException()) {
                popContext();
                d->extensionsBeingImported.remove(ext);
                return ret;
            }
        }

        // next, the C++ plugin is called
        if (iface) {
            iface->initialize(ext, this);
            if (hasUncaughtException()) {
                QScriptValue ret = uncaughtException(); // ctx_p->returnValue();
                popContext();
                d->extensionsBeingImported.remove(ext);
                return ret;
            }
        }

        // if the __postInit__ function has been set, we call it
        QScriptValue postInit = ctx->activationObject().property(QLatin1String("__postInit__"));
        if (postInit.isFunction()) {
            postInit.call(globalObject());
            if (hasUncaughtException()) {
                QScriptValue ret = uncaughtException(); // ctx_p->returnValue();
                popContext();
                d->extensionsBeingImported.remove(ext);
                return ret;
            }
        }

        popContext();

        d->importedExtensions.insert(ext);
        d->extensionsBeingImported.remove(ext);
    } // for (i)
#endif // QT_NO_QOBJECT
    return undefinedValue();
}

QStringList QScriptEngine::availableExtensions() const
{
#if defined(QT_NO_QOBJECT) || defined(QT_NO_LIBRARY) || defined(QT_NO_SETTINGS)
    return QStringList();
#else
    QCoreApplication *app = QCoreApplication::instance();
    if (!app)
        return QStringList();

    QSet<QString> result;

    QObjectList staticPlugins = QPluginLoader::staticInstances();
    for (int i = 0; i < staticPlugins.size(); ++i) {
        QScriptExtensionInterface *iface;
        iface = qobject_cast<QScriptExtensionInterface*>(staticPlugins.at(i));
        if (iface) {
            QStringList keys = iface->keys();
            for (int j = 0; j < keys.count(); ++j)
                result << keys.at(j);
        }
    }

    QStringList libraryPaths = app->libraryPaths();
    for (int i = 0; i < libraryPaths.count(); ++i) {
        QString libPath = libraryPaths.at(i) + QDir::separator() + QLatin1String("script");
        QDir dir(libPath);
        if (!dir.exists())
            continue;

        // look for C++ plugins
        QFileInfoList files = dir.entryInfoList(QDir::Files);
        for (int j = 0; j < files.count(); ++j) {
            QFileInfo entry = files.at(j);
            QString filePath = entry.canonicalFilePath();
            QPluginLoader loader(filePath);
            QScriptExtensionInterface *iface;
            iface = qobject_cast<QScriptExtensionInterface*>(loader.instance());
            if (iface) {
                QStringList keys = iface->keys();
                for (int k = 0; k < keys.count(); ++k)
                    result << keys.at(k);
            }
            // ### Because of compatibility with plugins that do not support
            // being unloaded, we let QPluginLoader go out of scope without
            // calling unload(), and this will "leak" the plugin.
        }

        // look for scripts
        QString initDotJs = QLatin1String("__init__.js");
        QList<QFileInfo> stack;
        stack << dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        while (!stack.isEmpty()) {
            QFileInfo entry = stack.takeLast();
            QDir dd(entry.canonicalFilePath());
            if (dd.exists(initDotJs)) {
                QString rpath = dir.relativeFilePath(dd.canonicalPath());
                QStringList components = rpath.split(QLatin1Char('/'));
                result << components.join(QLatin1String("."));
                stack << dd.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
            }
        }
    }

    QStringList lst = result.toList();
    qSort(lst);
    return lst;
#endif
}

QStringList QScriptEngine::importedExtensions() const
{
    Q_D(const QScriptEngine);
    QStringList lst = d->importedExtensions.toList();
    qSort(lst);
    return lst;
}

void QScriptEngine::setProcessEventsInterval(int interval)
{
    Q_D(QScriptEngine);
    d->m_processEventInterval = interval;
    if (d->m_processEventTimeoutThread) {
        if (interval < 0) {
            d->m_processEventTimeoutThread->destroy();
            d->m_processEventTimeoutThread = 0;
        } else {
            if(d->isEvaluating())
                d->m_processEventTimeoutThread->resetTime(interval);
        }
    } else if (interval >= 0) {
        d->m_processEventTimeoutThread = new QScriptEnginePrivate::ProcessEventTimeoutThread;
        d->m_processEventTimeoutThread->waitTime = d->isEvaluating() ? interval : INT_MAX;
        d->m_processEventTimeoutThread->engine = d;
        d->m_processEventTimeoutThread->start();
    }
}

int QScriptEngine::processEventsInterval() const
{
    Q_D(const QScriptEngine);
    return d->m_processEventInterval;
}

void QScriptEngine::setAgent(QScriptEngineAgent *agent)
{
    Q_D(QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    agent ? d->setAgent(QScriptEngineAgentPrivate::get(agent)) : d->setAgent(0);
}

QScriptEngineAgent *QScriptEngine::agent() const
{
    Q_D(const QScriptEngine);
    QScriptIsolate api(d, QScriptIsolate::NotNullEngine);
    QScriptEngineAgentPrivate *agent = d->agent();
    return agent ? QScriptEngineAgentPrivate::get(agent) : 0;
}

inline v8::Persistent<v8::Object> QScriptEnginePrivate::v8ObjectForConnectedObject(const QObject *o) const
{
    return m_connectedObjects.value(o);
}

inline void QScriptEnginePrivate::addV8ObjectForConnectedObject(const QObject *o, v8::Persistent<v8::Object> v8Object)
{
    m_connectedObjects.insert(o, v8Object);
}

void QScriptEnginePrivate::_q_removeConnectedObject(QObject *o)
{
    m_connectedObjects.take(o).Dispose();
}

bool qScriptConnect(QObject *sender, const char *signal,
                    const QScriptValue &receiver,
                    const QScriptValue &function)
{
    if (!sender || !signal)
        return false;
    if (!function.isFunction()) {
        qWarning("qScriptConnect(): 'function' is not a function");
        return false;
    }
    if (receiver.isObject() && (receiver.engine() != function.engine())) {
        qWarning("qScriptConnect(): 'receiver' and 'function' don't share the same engine");
        return false;
    }

    QScriptEnginePrivate *engine = QScriptEnginePrivate::get(function.engine());
    v8::Handle<v8::Object> v8Sender = engine->v8ObjectForConnectedObject(sender);
    if (v8Sender.IsEmpty()) {
        v8Sender = v8::Handle<v8::Object>::Cast(engine->newQObject(sender));
        engine->addV8ObjectForConnectedObject(sender, v8::Persistent<v8::Object>::New(v8Sender));
        QObject::connect(sender, SIGNAL(destroyed(QObject*)),
                         function.engine(), SLOT(_q_removeConnectedObject(QObject*)));
    }

    QString signalName(QString::fromLatin1(signal));
    signalName.remove(0, 1);

    v8::Handle<v8::Object> signalData =  v8Sender->Get(QScriptConverter::toString(signalName))->ToObject();
    if (signalData.IsEmpty() || signalData->IsError() || signalData->IsUndefined()) {
        qWarning("qScriptConnect(): signal '%s' is undefined", qPrintable(signalName));
        return false;
    }
    v8::Handle<v8::Object> v8Receiver;
    if (receiver.isObject())
        v8Receiver = v8::Handle<v8::Object>(*QScriptValuePrivate::get(receiver));
    return !QScriptSignalData::get(signalData)->connect(v8Receiver, v8::Handle<v8::Object>(*QScriptValuePrivate::get(function)))->IsError();
}

bool qScriptDisconnect(QObject *sender, const char *signal,
                       const QScriptValue &receiver,
                       const QScriptValue &function)
{
    if (!sender || !signal)
        return false;
    if (!function.isFunction()) {
        qWarning("qScriptDisconnect(): 'function' is not a function");
        return false;
    }
    if (receiver.isObject() && (receiver.engine() != function.engine())) {
        qWarning("qScriptDisconnect(): 'receiver' and 'function' don't share the same engine");
        return false;
    }

    QScriptEnginePrivate *engine = QScriptEnginePrivate::get(function.engine());
    v8::Handle<v8::Object> v8Sender = engine->v8ObjectForConnectedObject(sender);
    if (v8Sender.IsEmpty()) {
        qWarning("qScriptDisconnect(): 'sender' and ('receiver','function') were not connected with qScriptConnect()");
        return false;
    }

    QString signalName(QString::fromLatin1(signal));
    signalName.remove(0, 1);

    v8::Handle<v8::Object> signalData =  v8Sender->Get(QScriptConverter::toString(signalName))->ToObject();
    if (signalData.IsEmpty() || signalData->IsError() || signalData->IsUndefined()) {
        qWarning("qScriptDisconnect(): signal '%s' is undefined", qPrintable(signalName));
        return false;
    }
    return !QScriptSignalData::get(signalData)->disconnect(v8::Handle<v8::Function>::Cast(v8::Handle<v8::Value>(*QScriptValuePrivate::get(function))))->IsError();
}

#ifdef QT_BUILD_INTERNAL
Q_AUTOTEST_EXPORT bool qt_script_isJITEnabled()
{
    // In V8 there is only JIT.
    return true;
}
#endif

void QScriptEnginePrivate::emitSignalHandlerException()
{
    Q_Q(QScriptEngine);
    emit q->signalHandlerException(scriptValueFromInternal(uncaughtException()));
}

/*!
  \internal
  This method was created only because it couldn't be inlined in getOwnProperty.
  It shouldn't be used in other places.
  \note that it assume that object has a QScriptClassObject instance associated.
  */
v8::Local<v8::Value> QScriptEnginePrivate::getOwnPropertyFromScriptClassInstance(v8::Handle<v8::Object> object, v8::Handle<v8::Value> propertyName) const
{
#ifndef QT_NO_DEBUG
    Q_ASSERT(object->InternalFieldCount() == 1);
    QScriptValuePrivate *ptr = new QScriptValuePrivate(const_cast<QScriptEnginePrivate*>(this), object);
    Q_ASSERT(QScriptClassObject::safeGet(ptr));
    delete ptr;
#endif
    QScriptClassObject *data = QScriptClassObject::get(object);
    return m_originalGlobalObject.getOwnProperty(data->original(), propertyName);
}

/*!
  \internal
  This method was created only because it couldn't be inlined in getPropertyFlags.
  It shouldn't be used in other places.
  \note that it assume that object has a QScriptClassObject instance associated.
  */
QScriptValue::PropertyFlags QScriptEnginePrivate::getPropertyFlagsFromScriptClassInstance(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property, const QScriptValue::ResolveFlags& mode)
{
#ifndef QT_NO_DEBUG
    Q_ASSERT(object->InternalFieldCount() == 1);
    QScriptValuePrivate *ptr = new QScriptValuePrivate(const_cast<QScriptEnginePrivate*>(this), object);
    Q_ASSERT(QScriptClassObject::safeGet(ptr));
    delete ptr;
#endif
    QScriptClassObject *data = QScriptClassObject::get(object);
    return m_originalGlobalObject.getPropertyFlags(data->original(), property, mode);
}

QT_END_NAMESPACE

#include "moc_qscriptengine.cpp"
