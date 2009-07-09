/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "qscriptqobject_p.h"

#ifndef QT_NO_SCRIPT

#include <QtCore/qmetaobject.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qdebug.h>
#include <QtScript/qscriptable.h>
#include "../api/qscriptengine_p.h"
#include "../api/qscriptable_p.h"
#include "../api/qscriptcontext_p.h"

#include "Error.h"
#include "PrototypeFunction.h"
#include "PropertyNameArray.h"
#include "JSFunction.h"
#include "JSString.h"
#include "JSValue.h"

QT_BEGIN_NAMESPACE

namespace JSC
{
ASSERT_CLASS_FITS_IN_CELL(QScript::QObjectWrapperObject);
ASSERT_CLASS_FITS_IN_CELL(QScript::QObjectPrototype);
ASSERT_CLASS_FITS_IN_CELL(QScript::QMetaObjectWrapperObject);
ASSERT_CLASS_FITS_IN_CELL(QScript::QMetaObjectPrototype);
ASSERT_CLASS_FITS_IN_CELL(QScript::QtFunction);
ASSERT_CLASS_FITS_IN_CELL(QScript::QtPropertyFunction);
}

namespace QScript
{

struct QObjectConnection
{
    int slotIndex;
    JSC::JSValue receiver;
    JSC::JSValue slot;
    JSC::JSValue senderWrapper;

    QObjectConnection(int i, JSC::JSValue r, JSC::JSValue s,
                      JSC::JSValue sw)
        : slotIndex(i), receiver(r), slot(s), senderWrapper(sw) {}
    QObjectConnection() : slotIndex(-1) {}

    bool hasTarget(JSC::JSValue r, JSC::JSValue s) const
    {
        if ((r && r.isObject()) != (receiver && receiver.isObject()))
            return false;
        if (((r && r.isObject()) && (receiver && receiver.isObject()))
            && (r != receiver)) {
            return false;
        }
        return (s == slot);
    }

    void mark()
    {
        if (senderWrapper && !senderWrapper.marked()) {
            // see if the sender should be marked or not
            Q_ASSERT(JSC::asObject(senderWrapper)->inherits(&QObjectWrapperObject::info));
            QObjectWrapperObject *inst = static_cast<QObjectWrapperObject*>(JSC::asObject(senderWrapper));
            if ((inst->ownership() == QScriptEngine::ScriptOwnership)
                || ((inst->ownership() == QScriptEngine::AutoOwnership)
                    && inst->value() && !inst->value()->parent())) {
                senderWrapper = JSC::JSValue();
            } else {
                senderWrapper.mark();
            }
        }
        if (receiver)
            receiver.mark();
        if (slot)
            slot.mark();
    }
};

class QObjectNotifyCaller : public QObject
{
public:
    void callConnectNotify(const char *signal)
        { connectNotify(signal); }
    void callDisconnectNotify(const char *signal)
        { disconnectNotify(signal); }
};

class QObjectConnectionManager: public QObject
{
public:
    QObjectConnectionManager(QScriptEnginePrivate *engine);
    ~QObjectConnectionManager();

    bool addSignalHandler(QObject *sender, int signalIndex,
                          JSC::JSValue receiver,
                          JSC::JSValue slot,
                          JSC::JSValue senderWrapper = 0);
    bool removeSignalHandler(QObject *sender, int signalIndex,
                             JSC::JSValue receiver,
                             JSC::JSValue slot);

    static const QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const;
    virtual void *qt_metacast(const char *);
    virtual int qt_metacall(QMetaObject::Call, int, void **argv);

    void execute(int slotIndex, void **argv);

    void mark();

private:
    QScriptEnginePrivate *engine;
    int slotCounter;
    QVector<QVector<QObjectConnection> > connections;
};

static bool hasMethodAccess(const QMetaMethod &method, int index, const QScriptEngine::QObjectWrapOptions &opt)
{
    return (method.access() != QMetaMethod::Private)
        && ((index != 2) || !(opt & QScriptEngine::ExcludeDeleteLater));
}

static bool isEnumerableMetaProperty(const QMetaProperty &prop,
                                     const QMetaObject *mo, int index)
{
    return prop.isScriptable() && prop.isValid()
        // the following lookup is to ensure that we have the
        // "most derived" occurrence of the property with this name
        && (mo->indexOfProperty(prop.name()) == index);
}

static inline QByteArray methodName(const QMetaMethod &method)
{
    QByteArray signature = method.signature();
    return signature.left(signature.indexOf('('));
}

static QVariant variantFromValue(QScriptEnginePrivate *eng,
                                 int targetType, const QScriptValue &value)
{
    QVariant v(targetType, (void *)0);
    Q_ASSERT(eng);
    if (QScriptEnginePrivate::convert(value, targetType, v.data(), eng))
        return v;
    if (uint(targetType) == QVariant::LastType)
        return value.toVariant();
    if (value.isVariant()) {
        v = value.toVariant();
        if (v.canConvert(QVariant::Type(targetType))) {
            v.convert(QVariant::Type(targetType));
            return v;
        }
        QByteArray typeName = v.typeName();
        if (typeName.endsWith('*')
            && (QMetaType::type(typeName.left(typeName.size()-1)) == targetType)) {
            return QVariant(targetType, *reinterpret_cast<void* *>(v.data()));
        }
    }

    return QVariant();
}

static const bool GeneratePropertyFunctions = true;

static unsigned flagsForMetaProperty(const QMetaProperty &prop)
{
    return (JSC::DontDelete
            | (!prop.isWritable() ? unsigned(JSC::ReadOnly) : unsigned(0))
            | (GeneratePropertyFunctions
               ? unsigned(JSC::Getter | JSC::Setter)
                  : unsigned(0))
            | QObjectMemberAttribute);
}

static int indexOfMetaEnum(const QMetaObject *meta, const QByteArray &str)
{
    QByteArray scope;
    QByteArray name;
    int scopeIdx = str.lastIndexOf("::");
    if (scopeIdx != -1) {
        scope = str.left(scopeIdx);
        name = str.mid(scopeIdx + 2);
    } else {
        name = str;
    }
    for (int i = meta->enumeratorCount() - 1; i >= 0; --i) {
        QMetaEnum m = meta->enumerator(i);
        if ((m.name() == name) && (scope.isEmpty() || (m.scope() == scope)))
            return i;
    }
    return -1;
}

static inline QScriptable *scriptableFromQObject(QObject *qobj)
{
    void *ptr = qobj->qt_metacast("QScriptable");
    return reinterpret_cast<QScriptable*>(ptr);
}

JSC::UString qtStringToJSCUString(const QString &str);
QString qtStringFromJSCUString(const JSC::UString &str);

QtFunction::QtFunction(JSC::JSValue object, int initialIndex, bool maybeOverloaded,
                       JSC::JSGlobalData *data, WTF::PassRefPtr<JSC::Structure> sid,
                       const JSC::Identifier &ident)
    : JSC::InternalFunction(data, sid, ident),
      data(new Data(object, initialIndex, maybeOverloaded))
{
}

QtFunction::~QtFunction()
{
    delete data;
}

JSC::CallType QtFunction::getCallData(JSC::CallData &callData)
{
    callData.native.function = call;
    return JSC::CallTypeHost;
}

void QtFunction::mark()
{
    data->object.mark();
}

QObjectWrapperObject *QtFunction::wrapperObject() const
{
    Q_ASSERT(JSC::asObject(data->object)->inherits(&QObjectWrapperObject::info));
    return static_cast<QObjectWrapperObject*>(JSC::asObject(data->object));
}

QObject *QtFunction::qobject() const
{
    return wrapperObject()->value();
}

const QMetaObject *QtFunction::metaObject() const
{
    QObject *qobj = qobject();
    if (!qobj)
        return 0;
    return qobj->metaObject();
}

int QtFunction::initialIndex() const
{
    return data->initialIndex;
}

bool QtFunction::maybeOverloaded() const
{
    return data->maybeOverloaded;
}

int QtFunction::mostGeneralMethod(QMetaMethod *out) const
{
    const QMetaObject *meta = metaObject();
    if (!meta)
        return -1;
    int index = initialIndex();
    QMetaMethod method = meta->method(index);
    if (maybeOverloaded() && (method.attributes() & QMetaMethod::Cloned)) {
        // find the most general method
        do {
            method = meta->method(--index);
        } while (method.attributes() & QMetaMethod::Cloned);
    }
    if (out)
        *out = method;
    return index;
}

QList<int> QScript::QtFunction::overloadedIndexes() const
{
    if (!maybeOverloaded())
        return QList<int>();
    QList<int> result;
    QString name = functionName();
    const QMetaObject *meta = metaObject();
    for (int index = mostGeneralMethod() - 1; index >= 0; --index) {
        QString otherName = QString::fromLatin1(methodName(meta->method(index)));
        if (otherName == name)
            result.append(index);
    }
    return result;
}

QString QtFunction::functionName() const
{
    const QMetaObject *meta = metaObject();
    if (!meta)
        return QString();
    QMetaMethod method = meta->method(initialIndex());
    return QLatin1String(methodName(method));
}

class QScriptMetaType
{
public:
    enum Kind {
        Invalid,
        Variant,
        MetaType,
        Unresolved,
        MetaEnum
    };

    inline QScriptMetaType()
        : m_kind(Invalid) { }

    inline Kind kind() const
    { return m_kind; }

    int typeId() const;

    inline bool isValid() const
    { return (m_kind != Invalid); }

    inline bool isVariant() const
    { return (m_kind == Variant); }

    inline bool isMetaType() const
    { return (m_kind == MetaType); }

    inline bool isUnresolved() const
    { return (m_kind == Unresolved); }

    inline bool isMetaEnum() const
    { return (m_kind == MetaEnum); }

    QByteArray name() const;

    inline int enumeratorIndex() const
    { Q_ASSERT(isMetaEnum()); return m_typeId; }

    inline bool operator==(const QScriptMetaType &other) const
    {
        return (m_kind == other.m_kind) && (m_typeId == other.m_typeId);
    }

    static inline QScriptMetaType variant()
    { return QScriptMetaType(Variant); }

    static inline QScriptMetaType metaType(int typeId, const QByteArray &name)
    { return QScriptMetaType(MetaType, typeId, name); }

    static inline QScriptMetaType metaEnum(int enumIndex, const QByteArray &name)
    { return QScriptMetaType(MetaEnum, enumIndex, name); }

    static inline QScriptMetaType unresolved(const QByteArray &name)
    { return QScriptMetaType(Unresolved, /*typeId=*/0, name); }

private:
    inline QScriptMetaType(Kind kind, int typeId = 0, const QByteArray &name = QByteArray())
        : m_kind(kind), m_typeId(typeId), m_name(name) { }

    Kind m_kind;
    int m_typeId;
    QByteArray m_name;
};

int QScriptMetaType::typeId() const
{
    if (isVariant())
        return QMetaType::type("QVariant");
    return isMetaEnum() ? 2/*int*/ : m_typeId;
}

QByteArray QScriptMetaType::name() const
{
    if (!m_name.isEmpty())
        return m_name;
    else if (m_kind == Variant)
        return "QVariant";
    return QMetaType::typeName(typeId());
}

class QScriptMetaMethod
{
public:
    inline QScriptMetaMethod()
        { }
    inline QScriptMetaMethod(const QByteArray &name, const QVector<QScriptMetaType> &types)
        : m_name(name), m_types(types), m_firstUnresolvedIndex(-1)
    {
        QVector<QScriptMetaType>::const_iterator it;
        for (it = m_types.constBegin(); it != m_types.constEnd(); ++it) {
            if ((*it).kind() == QScriptMetaType::Unresolved) {
                m_firstUnresolvedIndex = it - m_types.constBegin();
                break;
            }
        }
    }
    inline bool isValid() const
    { return !m_types.isEmpty(); }

    QByteArray name() const
    { return m_name; }

    inline QScriptMetaType returnType() const
    { return m_types.at(0); }

    inline int argumentCount() const
    { return m_types.count() - 1; }

    inline QScriptMetaType argumentType(int arg) const
    { return m_types.at(arg + 1); }

    inline bool fullyResolved() const
    { return m_firstUnresolvedIndex == -1; }

    inline bool hasUnresolvedReturnType() const
    { return (m_firstUnresolvedIndex == 0); }

    inline int firstUnresolvedIndex() const
    { return m_firstUnresolvedIndex; }

    inline int count() const
    { return m_types.count(); }

    inline QScriptMetaType type(int index) const
    { return m_types.at(index); }

    inline QVector<QScriptMetaType> types() const
    { return m_types; }

private:
    QByteArray m_name;
    QVector<QScriptMetaType> m_types;
    int m_firstUnresolvedIndex;
};

struct QScriptMetaArguments
{
    int matchDistance;
    int index;
    QScriptMetaMethod method;
    QVarLengthArray<QVariant, 9> args;

    inline QScriptMetaArguments(int dist, int idx, const QScriptMetaMethod &mtd,
                                const QVarLengthArray<QVariant, 9> &as)
        : matchDistance(dist), index(idx), method(mtd), args(as) { }
    inline QScriptMetaArguments()
        : index(-1) { }

    inline bool isValid() const
    { return (index != -1); }
};

JSC::JSValue QtFunction::execute(JSC::ExecState *exec, JSC::JSValue thisValue,
                                 const JSC::ArgList &scriptArgs)
{
    Q_ASSERT(data->object.isObject(&QObjectWrapperObject::info));
    QObjectWrapperObject *wrapper = static_cast<QObjectWrapperObject*>(JSC::asObject(data->object));
    QScriptEnginePrivate *engine = static_cast<QScript::GlobalObject*>(exec->lexicalGlobalObject())->engine;
    QObject *qobj = wrapper->value();
    Q_ASSERT_X(qobj != 0, "QtFunction::call", "handle the case when QObject has been deleted");

    const QMetaObject *meta = qobj->metaObject();
    QObject *thisQObject;
    if (thisValue.isObject(&QObjectWrapperObject::info))
        thisQObject = static_cast<QObjectWrapperObject*>(JSC::asObject(thisValue))->value();
    else
        thisQObject = qobj; // ### TypeError

    if (!meta->cast(thisQObject)) {
        // invoking a function in the prototype
        thisQObject = qobj;
    }

    QByteArray funName;
    QScriptMetaMethod chosenMethod;
    int chosenIndex = -1;
    QVarLengthArray<QVariant, 9> args;
    QVector<QScriptMetaArguments> candidates;
    QVector<QScriptMetaArguments> unresolved;
    QVector<int> tooFewArgs;
    QVector<int> conversionFailed;
    int index;
    for (index = data->initialIndex; index >= 0; --index) {
        QMetaMethod method = meta->method(index);

        if (index == data->initialIndex)
            funName = methodName(method);
        else {
            if (methodName(method) != funName)
                continue;
        }

        QVector<QScriptMetaType> types;
        // resolve return type
        QByteArray returnTypeName = method.typeName();
        int rtype = QMetaType::type(returnTypeName);
        if ((rtype == 0) && !returnTypeName.isEmpty()) {
            if (returnTypeName == "QVariant") {
                types.append(QScriptMetaType::variant());
            } else {
                int enumIndex = indexOfMetaEnum(meta, returnTypeName);
                if (enumIndex != -1)
                    types.append(QScriptMetaType::metaEnum(enumIndex, returnTypeName));
                else
                    types.append(QScriptMetaType::unresolved(returnTypeName));
            }
        } else {
/*            if (callType == QMetaMethod::Constructor)
                types.append(QScriptMetaType::metaType(QMetaType::QObjectStar, "QObject*"));
                else*/ if (returnTypeName == "QVariant")
                types.append(QScriptMetaType::variant());
            else
                types.append(QScriptMetaType::metaType(rtype, returnTypeName));
        }

        // resolve argument types
        QList<QByteArray> parameterTypeNames = method.parameterTypes();
        for (int i = 0; i < parameterTypeNames.count(); ++i) {
            QByteArray argTypeName = parameterTypeNames.at(i);
            int atype = QMetaType::type(argTypeName);
            if (atype == 0) {
                if (argTypeName == "QVariant") {
                    types.append(QScriptMetaType::variant());
                } else {
                    int enumIndex = indexOfMetaEnum(meta, argTypeName);
                    if (enumIndex != -1)
                        types.append(QScriptMetaType::metaEnum(enumIndex, argTypeName));
                    else
                        types.append(QScriptMetaType::unresolved(argTypeName));
                }
            } else {
                if (argTypeName == "QVariant")
                    types.append(QScriptMetaType::variant());
                else
                    types.append(QScriptMetaType::metaType(atype, argTypeName));
            }
        }

        QScriptMetaMethod mtd = QScriptMetaMethod(methodName(method), types);

        if (int(scriptArgs.size()) < mtd.argumentCount()) {
            tooFewArgs.append(index);
            continue;
        }

        if (!mtd.fullyResolved()) {
            // remember it so we can give an error message later, if necessary
            unresolved.append(QScriptMetaArguments(/*matchDistance=*/INT_MAX, index,
                                                   mtd, QVarLengthArray<QVariant, 9>()));
            if (mtd.hasUnresolvedReturnType())
                continue;
        }

        if (args.count() != mtd.count())
            args.resize(mtd.count());

        QScriptMetaType retType = mtd.returnType();
        args[0] = QVariant(retType.typeId(), (void *)0); // the result

        // try to convert arguments
        bool converted = true;
        int matchDistance = 0;
        for (int i = 0; converted && i < mtd.argumentCount(); ++i) {
            QScriptValue actual = engine->scriptValueFromJSCValue(scriptArgs.at(i));
            QScriptMetaType argType = mtd.argumentType(i);
            int tid = -1;
            QVariant v;
            if (argType.isUnresolved()) {
                v = QVariant(QMetaType::QObjectStar, (void *)0);
                converted = engine->convertToNativeQObject(
                    actual, argType.name(), reinterpret_cast<void* *>(v.data()));
            } else if (argType.isVariant()) {
                if (actual.isVariant()) {
                    v = actual.toVariant();
                } else {
                    v = actual.toVariant();
                    converted = v.isValid() || actual.isUndefined() || actual.isNull();
                }
            } else {
                tid = argType.typeId();
                v = QVariant(tid, (void *)0);
                converted = QScriptEnginePrivate::convert(actual, tid, v.data(), engine);
                if (exec->hadException())
                    return exec->exception();
            }

            if (!converted) {
                if (actual.isVariant()) {
                    if (tid == -1)
                        tid = argType.typeId();
                    QVariant vv = actual.toVariant();
                    if (vv.canConvert(QVariant::Type(tid))) {
                        v = vv;
                        converted = v.convert(QVariant::Type(tid));
                        if (converted && (vv.userType() != tid))
                            matchDistance += 10;
                    } else {
                        QByteArray vvTypeName = vv.typeName();
                        if (vvTypeName.endsWith('*')
                            && (vvTypeName.left(vvTypeName.size()-1) == argType.name())) {
                            v = QVariant(tid, *reinterpret_cast<void* *>(vv.data()));
                            converted = true;
                            matchDistance += 10;
                        }
                    }
                } else if (actual.isNumber() || actual.isString()) {
                    // see if it's an enum value
                    QMetaEnum m;
                    if (argType.isMetaEnum()) {
                        m = meta->enumerator(argType.enumeratorIndex());
                    } else {
                        int mi = indexOfMetaEnum(meta, argType.name());
                        if (mi != -1)
                            m = meta->enumerator(mi);
                    }
                    if (m.isValid()) {
                        if (actual.isNumber()) {
                            int ival = actual.toInt32();
                            if (m.valueToKey(ival) != 0) {
                                qVariantSetValue(v, ival);
                                converted = true;
                                matchDistance += 10;
                            }
                        } else {
                            QString sval = actual.toString();
                            int ival = m.keyToValue(sval.toLatin1());
                            if (ival != -1) {
                                qVariantSetValue(v, ival);
                                converted = true;
                                matchDistance += 10;
                            }
                        }
                    }
                }
            } else {
                // determine how well the conversion matched
                if (actual.isNumber()) {
                    switch (tid) {
                    case QMetaType::Double:
                        // perfect
                        break;
                    case QMetaType::Float:
                        matchDistance += 1;
                        break;
                    case QMetaType::LongLong:
                    case QMetaType::ULongLong:
                        matchDistance += 2;
                        break;
                    case QMetaType::Long:
                    case QMetaType::ULong:
                        matchDistance += 3;
                        break;
                    case QMetaType::Int:
                    case QMetaType::UInt:
                        matchDistance += 4;
                        break;
                    case QMetaType::Short:
                    case QMetaType::UShort:
                        matchDistance += 5;
                        break;
                    case QMetaType::Char:
                    case QMetaType::UChar:
                        matchDistance += 6;
                        break;
                    default:
                        matchDistance += 10;
                        break;
                    }
                } else if (actual.isString()) {
                    switch (tid) {
                    case QMetaType::QString:
                        // perfect
                        break;
                    default:
                        matchDistance += 10;
                        break;
                    }
                } else if (actual.isBoolean()) {
                    switch (tid) {
                    case QMetaType::Bool:
                        // perfect
                        break;
                    default:
                        matchDistance += 10;
                        break;
                    }
                } else if (actual.isDate()) {
                    switch (tid) {
                    case QMetaType::QDateTime:
                        // perfect
                        break;
                    case QMetaType::QDate:
                        matchDistance += 1;
                        break;
                    case QMetaType::QTime:
                        matchDistance += 2;
                        break;
                    default:
                        matchDistance += 10;
                        break;
                    }
                } else if (actual.isRegExp()) {
                    switch (tid) {
                    case QMetaType::QRegExp:
                        // perfect
                        break;
                    default:
                        matchDistance += 10;
                        break;
                    }
                } else if (actual.isVariant()) {
                    if (argType.isVariant()
                        || (actual.toVariant().userType() == tid)) {
                        // perfect
                    } else {
                        matchDistance += 10;
                    }
                } else if (actual.isArray()) {
                    switch (tid) {
                    case QMetaType::QStringList:
                    case QMetaType::QVariantList:
                        matchDistance += 5;
                        break;
                    default:
                        matchDistance += 10;
                        break;
                    }
                } else if (actual.isQObject()) {
                    switch (tid) {
                    case QMetaType::QObjectStar:
                    case QMetaType::QWidgetStar:
                        // perfect
                        break;
                    default:
                        matchDistance += 10;
                        break;
                    }
                } else if (actual.isNull()) {
                    switch (tid) {
                    case QMetaType::VoidStar:
                    case QMetaType::QObjectStar:
                    case QMetaType::QWidgetStar:
                        // perfect
                        break;
                    default:
                        if (!argType.name().endsWith('*'))
                            matchDistance += 10;
                        break;
                    }
                } else {
                    matchDistance += 10;
                }
            }

            if (converted)
                args[i+1] = v;
        }

        if (converted) {
            if ((scriptArgs.size() == (size_t)mtd.argumentCount())
                && (matchDistance == 0)) {
                // perfect match, use this one
                chosenMethod = mtd;
                chosenIndex = index;
                break;
            } else {
                bool redundant = false;
                if (/*(callType != QMetaMethod::Constructor)
                      &&*/ (index < meta->methodOffset())) {
                    // it is possible that a virtual method is redeclared in a subclass,
                    // in which case we want to ignore the superclass declaration
                    for (int i = 0; i < candidates.size(); ++i) {
                        const QScriptMetaArguments &other = candidates.at(i);
                        if (mtd.types() == other.method.types()) {
                            redundant = true;
                            break;
                        }
                    }
                }
                if (!redundant) {
                    QScriptMetaArguments metaArgs(matchDistance, index, mtd, args);
                    if (candidates.isEmpty()) {
                        candidates.append(metaArgs);
                    } else {
                        const QScriptMetaArguments &otherArgs = candidates.at(0);
                        if ((args.count() > otherArgs.args.count())
                            || ((args.count() == otherArgs.args.count())
                                && (matchDistance <= otherArgs.matchDistance))) {
                            candidates.prepend(metaArgs);
                        } else {
                            candidates.append(metaArgs);
                        }
                    }
                }
            }
        } else if (mtd.fullyResolved()) {
            conversionFailed.append(index);
        }

        if (!data->maybeOverloaded)
            break;
    }

    JSC::JSValue result;
    if ((chosenIndex == -1) && candidates.isEmpty()) {
//        context->calleeMetaIndex = initialIndex;
//#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
//        engine->notifyFunctionEntry(context);
//#endif
        if (!conversionFailed.isEmpty()) {
            QString message = QString::fromLatin1("incompatible type of argument(s) in call to %0(); candidates were\n")
                              .arg(QLatin1String(funName));
            for (int i = 0; i < conversionFailed.size(); ++i) {
                if (i > 0)
                    message += QLatin1String("\n");
                QMetaMethod mtd = meta->method(conversionFailed.at(i));
                message += QString::fromLatin1("    %0").arg(QString::fromLatin1(mtd.signature()));
            }
            result = JSC::throwError(exec, JSC::TypeError, qtStringToJSCUString(message));
        } else if (!unresolved.isEmpty()) {
            QScriptMetaArguments argsInstance = unresolved.first();
            int unresolvedIndex = argsInstance.method.firstUnresolvedIndex();
            Q_ASSERT(unresolvedIndex != -1);
            QScriptMetaType unresolvedType = argsInstance.method.type(unresolvedIndex);
            QString unresolvedTypeName = QString::fromLatin1(unresolvedType.name());
            QString message = QString::fromLatin1("cannot call %0(): ")
                              .arg(QString::fromLatin1(funName));
            if (unresolvedIndex > 0) {
                message.append(QString::fromLatin1("argument %0 has unknown type `%1'").
                               arg(unresolvedIndex).arg(unresolvedTypeName));
            } else {
                message.append(QString::fromLatin1("unknown return type `%0'")
                               .arg(unresolvedTypeName));
            }
            message.append(QString::fromLatin1(" (register the type with qScriptRegisterMetaType())"));
            result = JSC::throwError(exec, JSC::TypeError, qtStringToJSCUString(message));
        } else {
            QString message = QString::fromLatin1("too few arguments in call to %0(); candidates are\n")
                              .arg(QLatin1String(funName));
            for (int i = 0; i < tooFewArgs.size(); ++i) {
                if (i > 0)
                    message += QLatin1String("\n");
                QMetaMethod mtd = meta->method(tooFewArgs.at(i));
                message += QString::fromLatin1("    %0").arg(QString::fromLatin1(mtd.signature()));
            }
            result = JSC::throwError(exec, JSC::SyntaxError, qtStringToJSCUString(message));
        }
    } else {
        if (chosenIndex == -1) {
            QScriptMetaArguments metaArgs = candidates.at(0);
            if ((candidates.size() > 1)
                && (metaArgs.args.count() == candidates.at(1).args.count())
                && (metaArgs.matchDistance == candidates.at(1).matchDistance)) {
                // ambiguous call
                QString message = QString::fromLatin1("ambiguous call of overloaded function %0(); candidates were\n")
                                  .arg(QLatin1String(funName));
                for (int i = 0; i < candidates.size(); ++i) {
                    if (i > 0)
                        message += QLatin1String("\n");
                    QMetaMethod mtd = meta->method(candidates.at(i).index);
                    message += QString::fromLatin1("    %0").arg(QString::fromLatin1(mtd.signature()));
                }
                result = JSC::throwError(exec, JSC::TypeError, qtStringToJSCUString(message));
            } else {
                chosenMethod = metaArgs.method;
                chosenIndex = metaArgs.index;
                args = metaArgs.args;
            }
        }

        if (chosenIndex != -1) {
            // call it
//            context->calleeMetaIndex = chosenIndex;

            QVarLengthArray<void*, 9> array(args.count());
            void **params = array.data();
            for (int i = 0; i < args.count(); ++i) {
                const QVariant &v = args[i];
                switch (chosenMethod.type(i).kind()) {
                case QScriptMetaType::Variant:
                    params[i] = const_cast<QVariant*>(&v);
                    break;
                case QScriptMetaType::MetaType:
                case QScriptMetaType::MetaEnum:
                case QScriptMetaType::Unresolved:
                    params[i] = const_cast<void*>(v.constData());
                    break;
                default:
                    Q_ASSERT(0);
                }
            }

            QScriptable *scriptable = 0;
            if (thisQObject)
                scriptable = scriptableFromQObject(thisQObject);
            QScriptEngine *oldEngine = 0;
            if (scriptable) {
                oldEngine = QScriptablePrivate::get(scriptable)->engine;
                QScriptablePrivate::get(scriptable)->engine = QScriptEnginePrivate::get(engine);
            }

// ### fixme
//#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
//            engine->notifyFunctionEntry(context);
//#endif

/*            if (callType == QMetaMethod::Constructor) {
                Q_ASSERT(meta != 0);
                meta->static_metacall(QMetaObject::CreateInstance, chosenIndex, params);
                } else*/ {
                Q_ASSERT(thisQObject != 0);
                thisQObject->qt_metacall(QMetaObject::InvokeMetaMethod, chosenIndex, params);
            }

            if (scriptable)
                QScriptablePrivate::get(scriptable)->engine = oldEngine;

            if (exec->hadException()) {
                result = exec->exception() ; // propagate
            } else {
                QScriptMetaType retType = chosenMethod.returnType();
                if (retType.isVariant()) {
                    result = engine->jscValueFromVariant(*(QVariant *)params[0]);
                } else if (retType.typeId() != 0) {
                    result = engine->scriptValueToJSCValue(engine->create(retType.typeId(), params[0]));
                    if (!result) {
                        QScriptValue sv = QScriptEnginePrivate::get(engine)->newVariant(QVariant(retType.typeId(), params[0]));
                        result = engine->scriptValueToJSCValue(sv);
                    }
                } else {
                    result = JSC::jsUndefined();
                }
            }
        }
    }

    return result;
}

const JSC::ClassInfo QtFunction::info = { "QtFunction", &InternalFunction::info, 0, 0 };

JSC::JSValue JSC_HOST_CALL QtFunction::call(JSC::ExecState *exec, JSC::JSObject *callee,
                                            JSC::JSValue thisValue, const JSC::ArgList &args)
{
    if (!callee->isObject(&QtFunction::info))
        return throwError(exec, JSC::TypeError, "callee is not a QtFunction object");
    QtFunction *qfun =  static_cast<QtFunction*>(callee);
    QScriptEnginePrivate *eng_p = static_cast<QScript::GlobalObject*>(exec->lexicalGlobalObject())->engine;
    JSC::ExecState *previousFrame = eng_p->currentFrame;
    eng_p->currentFrame = exec;
    JSC::JSValue result = qfun->execute(exec, thisValue, args);
    eng_p->currentFrame = previousFrame;
    return result;
}

const JSC::ClassInfo QtPropertyFunction::info = { "QtPropertyFunction", &InternalFunction::info, 0, 0 };

QtPropertyFunction::QtPropertyFunction(const QMetaObject *meta, int index,
                                       JSC::JSGlobalData *data,
                                       WTF::PassRefPtr<JSC::Structure> sid,
                                       const JSC::Identifier &ident)
    : JSC::InternalFunction(data, sid, ident),
      data(new Data(meta, index))
{
}

QtPropertyFunction::~QtPropertyFunction()
{
    delete data;
}

JSC::CallType QtPropertyFunction::getCallData(JSC::CallData &callData)
{
    callData.native.function = call;
    return JSC::CallTypeHost;
}

JSC::JSValue JSC_HOST_CALL QtPropertyFunction::call(
    JSC::ExecState *exec, JSC::JSObject *callee,
    JSC::JSValue thisValue, const JSC::ArgList &args)
{
    if (!callee->isObject(&QtPropertyFunction::info))
        return throwError(exec, JSC::TypeError, "callee is not a QtPropertyFunction object");
    QtPropertyFunction *qfun =  static_cast<QtPropertyFunction*>(callee);
    QScriptEnginePrivate *eng_p = static_cast<QScript::GlobalObject*>(exec->lexicalGlobalObject())->engine;
    JSC::ExecState *previousFrame = eng_p->currentFrame;
    eng_p->currentFrame = exec;
    JSC::JSValue result = qfun->execute(exec, thisValue, args);
    eng_p->currentFrame = previousFrame;
    return result;
}

JSC::JSValue QtPropertyFunction::execute(JSC::ExecState *exec,
                                         JSC::JSValue thisValue,
                                         const JSC::ArgList &args)
{
    JSC::JSValue result = JSC::jsUndefined();

    // ### don't go via QScriptValue
    QScriptEnginePrivate *engine = static_cast<QScript::GlobalObject*>(exec->lexicalGlobalObject())->engine;
    QScriptValue object = engine->scriptValueFromJSCValue(thisValue);
    QObject *qobject = object.toQObject();
    while ((!qobject || (qobject->metaObject() != data->meta))
           && object.prototype().isObject()) {
        object = object.prototype();
        qobject = object.toQObject();
    }
    Q_ASSERT(qobject);

    QMetaProperty prop = data->meta->property(data->index);
    Q_ASSERT(prop.isScriptable());
    if (args.size() == 0) {
        // get
        if (prop.isValid()) {
            QScriptable *scriptable = scriptableFromQObject(qobject);
            QScriptEngine *oldEngine = 0;
            if (scriptable) {
                oldEngine = QScriptablePrivate::get(scriptable)->engine;
                QScriptablePrivate::get(scriptable)->engine = QScriptEnginePrivate::get(engine);
            }

            QVariant v = prop.read(qobject);

            if (scriptable)
                QScriptablePrivate::get(scriptable)->engine = oldEngine;

            result = engine->jscValueFromVariant(v);
        }
    } else {
        // set
        JSC::JSValue arg = args.at(0);
        QVariant v;
        if (prop.isEnumType() && arg.isString()
            && !engine->hasDemarshalFunction(prop.userType())) {
            // give QMetaProperty::write() a chance to convert from
            // string to enum value
            v = qtStringFromJSCUString(arg.toString(exec));
        } else {
            // ### don't go via QScriptValue
            QScriptValue tmp = engine->scriptValueFromJSCValue(arg);
            v = variantFromValue(engine, prop.userType(), tmp);
        }

        QScriptable *scriptable = scriptableFromQObject(qobject);
        QScriptEngine *oldEngine = 0;
        if (scriptable) {
            oldEngine = QScriptablePrivate::get(scriptable)->engine;
            QScriptablePrivate::get(scriptable)->engine = QScriptEnginePrivate::get(engine);
        }

        prop.write(qobject, v);

        if (scriptable)
            QScriptablePrivate::get(scriptable)->engine = oldEngine;

        result = arg;
    }
    return result;
}

const QMetaObject *QtPropertyFunction::metaObject() const
{
    return data->meta;
}

int QtPropertyFunction::propertyIndex() const
{
    return data->index;
}

const JSC::ClassInfo QObjectWrapperObject::info = { "QObject", 0, 0, 0 };

QObjectWrapperObject::QObjectWrapperObject(
    QObject *object, QScriptEngine::ValueOwnership ownership,
    const QScriptEngine::QObjectWrapOptions &options,
    WTF::PassRefPtr<JSC::Structure> sid)
    : JSC::JSObject(sid), data(new Data(object, ownership, options))
{
}

QObjectWrapperObject::~QObjectWrapperObject()
{
    switch (data->ownership) {
    case QScriptEngine::QtOwnership:
        break;
    case QScriptEngine::ScriptOwnership:
        if (data->value)
            delete data->value; // ### fixme
//            eng->disposeQObject(value);
        break;
    case QScriptEngine::AutoOwnership:
        if (data->value && !data->value->parent())
            delete data->value; // ### fixme
//            eng->disposeQObject(value);
        break;
    }
    delete data;
}

bool QObjectWrapperObject::getOwnPropertySlot(JSC::ExecState *exec,
                                              const JSC::Identifier &propertyName,
                                              JSC::PropertySlot &slot)
{
    QByteArray name = qtStringFromJSCUString(propertyName.ustring()).toLatin1();
    QObject *qobject = data->value;
    if (!qobject) {
        QString message = QString::fromLatin1("cannot access member `%0' of deleted QObject")
                          .arg(QString::fromLatin1(name));
        JSC::throwError(exec, JSC::GeneralError, qtStringToJSCUString(message));
        return false;
    }

    const QMetaObject *meta = qobject->metaObject();
    {
        QHash<QByteArray, JSC::JSValue>::const_iterator it = data->cachedMembers.constFind(name);
        if (it != data->cachedMembers.constEnd()) {
            if (GeneratePropertyFunctions && (meta->indexOfProperty(name) != -1))
                slot.setGetterSlot(JSC::asObject(it.value()));
            else
                slot.setValue(it.value());
            return true;
        }
    }

    const QScriptEngine::QObjectWrapOptions &opt = data->options;
    QScriptEnginePrivate *eng = static_cast<QScript::GlobalObject*>(exec->lexicalGlobalObject())->engine;
    int index = -1;
    if (name.contains('(')) {
        QByteArray normalized = QMetaObject::normalizedSignature(name);
        if (-1 != (index = meta->indexOfMethod(normalized))) {
            QMetaMethod method = meta->method(index);
            if (hasMethodAccess(method, index, opt)) {
                if (!(opt & QScriptEngine::ExcludeSuperClassMethods)
                    || (index >= meta->methodOffset())) {
                    QtFunction *fun = new (exec)QtFunction(
                        this, index, /*maybeOverloaded=*/false,
                        &exec->globalData(), exec->lexicalGlobalObject()->functionStructure(),
                        propertyName);
                    slot.setValue(fun);
                    data->cachedMembers.insert(name, fun);
                    return true;
                }
            }
        }
    }

    index = meta->indexOfProperty(name);
    if (index != -1) {
        QMetaProperty prop = meta->property(index);
        if (prop.isScriptable()) {
            if (!(opt & QScriptEngine::ExcludeSuperClassProperties)
                || (index >= meta->propertyOffset())) {
                if (GeneratePropertyFunctions) {
                    QtPropertyFunction *fun = new (exec)QtPropertyFunction(
                        meta, index, &exec->globalData(),
                        exec->lexicalGlobalObject()->functionStructure(),
                        propertyName);
                    data->cachedMembers.insert(name, fun);
                    slot.setGetterSlot(fun);
                } else {
                    JSC::JSValue val;
                    if (!prop.isValid())
                        val = JSC::jsUndefined();
                    else
                        val = eng->jscValueFromVariant(prop.read(qobject));
                    slot.setValue(val);
                }
                return true;
            }
        }
    }

    index = qobject->dynamicPropertyNames().indexOf(name);
    if (index != -1) {
        JSC::JSValue val = eng->jscValueFromVariant(qobject->property(name));
        slot.setValue(val);
        return true;
    }

    const int offset = (opt & QScriptEngine::ExcludeSuperClassMethods)
                       ? meta->methodOffset() : 0;
    for (index = meta->methodCount() - 1; index >= offset; --index) {
        QMetaMethod method = meta->method(index);
        if (hasMethodAccess(method, index, opt)
            && (methodName(method) == name)) {
            QtFunction *fun = new (exec)QtFunction(
                this, index, /*maybeOverloaded=*/true,
                &exec->globalData(), exec->lexicalGlobalObject()->functionStructure(),
                propertyName);
            slot.setValue(fun);
            data->cachedMembers.insert(name, fun);
            return true;
        }
    }

    if (!(opt & QScriptEngine::ExcludeChildObjects)) {
        QList<QObject*> children = qobject->children();
        for (index = 0; index < children.count(); ++index) {
            QObject *child = children.at(index);
            if (child->objectName() == qtStringFromJSCUString(propertyName.ustring())) {
                QScriptEngine::QObjectWrapOptions opt = QScriptEngine::PreferExistingWrapperObject;
                QScriptValue tmp = QScriptEnginePrivate::get(eng)->newQObject(child, QScriptEngine::QtOwnership, opt);
                slot.setValue(eng->scriptValueToJSCValue(tmp));
                return true;
            }
        }
    }

    return JSC::JSObject::getOwnPropertySlot(exec, propertyName, slot);
}

void QObjectWrapperObject::put(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                               JSC::JSValue value, JSC::PutPropertySlot &slot)
{
    QByteArray name = qtStringFromJSCUString(propertyName.ustring()).toLatin1();
    QObject *qobject = data->value;
    if (!qobject) {
        QString message = QString::fromLatin1("cannot access member `%0' of deleted QObject")
                          .arg(QString::fromLatin1(name));
        JSC::throwError(exec, JSC::GeneralError, qtStringToJSCUString(message));
        return;
    }

    const QScriptEngine::QObjectWrapOptions &opt = data->options;
    const QMetaObject *meta = qobject->metaObject();
    QScriptEnginePrivate *eng = static_cast<QScript::GlobalObject*>(exec->lexicalGlobalObject())->engine;
    int index = -1;
    if (name.contains('(')) {
        QByteArray normalized = QMetaObject::normalizedSignature(name);
        if (-1 != (index = meta->indexOfMethod(normalized))) {
            QMetaMethod method = meta->method(index);
            if (hasMethodAccess(method, index, opt)) {
                if (!(opt & QScriptEngine::ExcludeSuperClassMethods)
                    || (index >= meta->methodOffset())) {
                    data->cachedMembers.insert(name, value);
                    return;
                }
            }
        }
    }

    index = meta->indexOfProperty(name);
    if (index != -1) {
        QMetaProperty prop = meta->property(index);
        if (prop.isScriptable()) {
            if (!(opt & QScriptEngine::ExcludeSuperClassProperties)
                || (index >= meta->propertyOffset())) {
                if (GeneratePropertyFunctions) {
                    // ### ideally JSC would do this for us already, i.e. find out
                    // that the property is a setter and call the setter.
                    // Maybe QtPropertyFunction needs to inherit JSC::GetterSetter.
                    JSC::JSValue fun;
                    QHash<QByteArray, JSC::JSValue>::const_iterator it;
                    it = data->cachedMembers.constFind(name);
                    if (it != data->cachedMembers.constEnd()) {
                        fun = it.value();
                    } else {
                        fun = new (exec)QtPropertyFunction(
                            meta, index, &exec->globalData(),
                            exec->lexicalGlobalObject()->functionStructure(),
                            propertyName);
                        data->cachedMembers.insert(name, fun);
                    }
                    JSC::CallData callData;
                    JSC::CallType callType = fun.getCallData(callData);
                    JSC::JSValue argv[1] = { value };
                    JSC::ArgList args(argv, 1);
                    (void)JSC::call(exec, fun, callType, callData, this, args);
                } else {
                    QVariant v;
                    if (prop.isEnumType() && value.isString()
                        && !eng->hasDemarshalFunction(prop.userType())) {
                        // give QMetaProperty::write() a chance to convert from
                        // string to enum value
                        v = qtStringFromJSCUString(value.toString(exec));
                    } else {
                        v = eng->jscValueToVariant(value, prop.userType());
                    }
                    (void)prop.write(qobject, v);
                }
                return;
            }
        }
    }

    index = qobject->dynamicPropertyNames().indexOf(name);
    if (index != -1) {
        QVariant v = eng->scriptValueFromJSCValue(value).toVariant();
        (void)qobject->setProperty(name, v);
        return;
    }

    const int offset = (opt & QScriptEngine::ExcludeSuperClassMethods)
                       ? meta->methodOffset() : 0;
    for (index = meta->methodCount() - 1; index >= offset; --index) {
        QMetaMethod method = meta->method(index);
        if (hasMethodAccess(method, index, opt)
            && (methodName(method) == name)) {
            data->cachedMembers.insert(name, value);
            return;
        }
    }

    JSC::JSObject::put(exec, propertyName, value, slot);
}

bool QObjectWrapperObject::deleteProperty(JSC::ExecState *exec,
                                          const JSC::Identifier& propertyName)
{
    QByteArray name = qtStringFromJSCUString(propertyName.ustring()).toLatin1();
    QObject *qobject = data->value;
    if (!qobject) {
        QString message = QString::fromLatin1("cannot access member `%0' of deleted QObject")
                          .arg(QString::fromLatin1(name));
        JSC::throwError(exec, JSC::GeneralError, qtStringToJSCUString(message));
        return false;
    }

    const QMetaObject *meta = qobject->metaObject();
    {
        QHash<QByteArray, JSC::JSValue>::iterator it = data->cachedMembers.find(name);
        if (it != data->cachedMembers.end()) {
            if (GeneratePropertyFunctions && (meta->indexOfProperty(name) != -1))
                return false;
            data->cachedMembers.erase(it);
            return true;
        }
    }

    const QScriptEngine::QObjectWrapOptions &opt = data->options;
    int index = meta->indexOfProperty(name);
    if (index != -1) {
        QMetaProperty prop = meta->property(index);
        if (prop.isScriptable() &&
            (!(opt & QScriptEngine::ExcludeSuperClassProperties)
             || (index >= meta->propertyOffset()))) {
                return false;
        }
    }

    index = qobject->dynamicPropertyNames().indexOf(name);
    if (index != -1) {
        (void)qobject->setProperty(name, QVariant());
        return true;
    }

    return JSC::JSObject::deleteProperty(exec, propertyName);
}

bool QObjectWrapperObject::getPropertyAttributes(JSC::ExecState *exec,
                                                 const JSC::Identifier &propertyName,
                                                 unsigned &attributes) const
{
    // ### try to avoid duplicating logic from getOwnPropertySlot()
    QByteArray name = qtStringFromJSCUString(propertyName.ustring()).toLatin1();
    QObject *qobject = data->value;
    if (!qobject)
        return false;

    const QScriptEngine::QObjectWrapOptions &opt = data->options;
    const QMetaObject *meta = qobject->metaObject();
    int index = -1;
    if (name.contains('(')) {
        QByteArray normalized = QMetaObject::normalizedSignature(name);
        if (-1 != (index = meta->indexOfMethod(normalized))) {
            QMetaMethod method = meta->method(index);
            if (hasMethodAccess(method, index, opt)) {
                if (!(opt & QScriptEngine::ExcludeSuperClassMethods)
                    || (index >= meta->methodOffset())) {
                    attributes = QObjectMemberAttribute;
                    if (opt & QScriptEngine::SkipMethodsInEnumeration)
                        attributes |= JSC::DontEnum;
                    return true;
                }
            }
        }
    }

    index = meta->indexOfProperty(name);
    if (index != -1) {
        QMetaProperty prop = meta->property(index);
        if (prop.isScriptable()) {
            if (!(opt & QScriptEngine::ExcludeSuperClassProperties)
                || (index >= meta->propertyOffset())) {
                attributes = flagsForMetaProperty(prop);
                return true;
            }
        }
    }

    index = qobject->dynamicPropertyNames().indexOf(name);
    if (index != -1) {
        attributes = QObjectMemberAttribute;
        return true;
    }

    const int offset = (opt & QScriptEngine::ExcludeSuperClassMethods)
                       ? meta->methodOffset() : 0;
    for (index = meta->methodCount() - 1; index >= offset; --index) {
        QMetaMethod method = meta->method(index);
        if (hasMethodAccess(method, index, opt)
            && (methodName(method) == name)) {
            attributes = QObjectMemberAttribute;
            if (opt & QScriptEngine::SkipMethodsInEnumeration)
                attributes |= JSC::DontEnum;
            return true;
        }
    }

    if (!(opt & QScriptEngine::ExcludeChildObjects)) {
        QList<QObject*> children = qobject->children();
        for (index = 0; index < children.count(); ++index) {
            QObject *child = children.at(index);
            if (child->objectName() == qtStringFromJSCUString(propertyName.ustring())) {
                attributes = JSC::ReadOnly | JSC::DontDelete | JSC::DontEnum;
                return true;
            }
        }
    }

    return JSC::JSObject::getPropertyAttributes(exec, propertyName, attributes);
}

JSC::JSValue QObjectWrapperObject::lookupGetter(JSC::ExecState* exec, const JSC::Identifier& propertyName)
{
    Q_ASSERT_X(false, Q_FUNC_INFO, "implement me");
    return JSC::JSObject::lookupGetter(exec, propertyName);
}

JSC::JSValue QObjectWrapperObject::lookupSetter(JSC::ExecState* exec, const JSC::Identifier& propertyName)
{
    Q_ASSERT_X(false, Q_FUNC_INFO, "implement me");
    return JSC::JSObject::lookupSetter(exec, propertyName);
}

void QObjectWrapperObject::getPropertyNames(JSC::ExecState *exec, JSC::PropertyNameArray &propertyNames)
{
    QObject *qobject = data->value;
    if (!qobject) {
        QString message = QString::fromLatin1("cannot get property names of deleted QObject");
        JSC::throwError(exec, JSC::GeneralError, qtStringToJSCUString(message));
        return;
    }

    const QScriptEngine::QObjectWrapOptions &opt = data->options;
    const QMetaObject *meta = qobject->metaObject();
    {
        int i = (opt & QScriptEngine::ExcludeSuperClassProperties)
                    ? meta->propertyOffset() : 0;
        for ( ; i < meta->propertyCount(); ++i) {
            QMetaProperty prop = meta->property(i);
            if (isEnumerableMetaProperty(prop, meta, i)) {
                QString name = QString::fromLatin1(prop.name());
                propertyNames.add(JSC::Identifier(exec, qtStringToJSCUString(name)));
            }
        }
    }

    {
        QList<QByteArray> dpNames = qobject->dynamicPropertyNames();
        for (int i = 0; i < dpNames.size(); ++i) {
            QString name = QString::fromLatin1(dpNames.at(i));
            propertyNames.add(JSC::Identifier(exec, qtStringToJSCUString(name)));
        }
    }

    {
        int i = (opt & QScriptEngine::ExcludeSuperClassMethods)
                    ? meta->methodOffset() : 0;
        for ( ; i < meta->methodCount(); ++i) {
            QMetaMethod method = meta->method(i);
            if (hasMethodAccess(method, i, opt)) {
                QMetaMethod method = meta->method(i);
                QString name = QString::fromLatin1(methodName(method));
                propertyNames.add(JSC::Identifier(exec, qtStringToJSCUString(name)));
                QString sig = QString::fromLatin1(method.signature());
                propertyNames.add(JSC::Identifier(exec, qtStringToJSCUString(sig)));
            }
        }
    }

    JSC::JSObject::getPropertyNames(exec, propertyNames);
}

void QObjectWrapperObject::mark()
{
    QHash<QByteArray, JSC::JSValue>::const_iterator it;
    for (it = data->cachedMembers.constBegin(); it != data->cachedMembers.constEnd(); ++it)
        JSC::asObject(it.value())->mark();

    JSC::JSObject::mark();
}

static JSC::JSValue JSC_HOST_CALL qobjectProtoFuncFindChild(JSC::ExecState *exec, JSC::JSObject*,
                                                            JSC::JSValue thisValue, const JSC::ArgList &args)
{
    if (!thisValue.isObject(&QObjectWrapperObject::info))
        return throwError(exec, JSC::TypeError, "this object is not a QObject");
    QObject *obj = static_cast<QObjectWrapperObject*>(JSC::asObject(thisValue))->value();
    QString name;
    if (args.size() != 0)
        name = QScript::qtStringFromJSCUString(args.at(0).toString(exec));
    QObject *child = qFindChild<QObject*>(obj, name);
    QScriptEngine::QObjectWrapOptions opt = QScriptEngine::PreferExistingWrapperObject;
    QScriptEnginePrivate *engine = static_cast<QScript::GlobalObject*>(exec->lexicalGlobalObject())->engine;
    return engine->newQObject(child, QScriptEngine::QtOwnership, opt);
}

static JSC::JSValue JSC_HOST_CALL qobjectProtoFuncFindChildren(JSC::ExecState *exec, JSC::JSObject*,
                                                               JSC::JSValue thisValue, const JSC::ArgList &args)
{
    if (!thisValue.isObject(&QObjectWrapperObject::info))
        return throwError(exec, JSC::TypeError, "this object is not a QObject");
    return throwError(exec, JSC::GeneralError, "QObject.prototype.findChildren not implemented");
}

static JSC::JSValue JSC_HOST_CALL qobjectProtoFuncToString(JSC::ExecState *exec, JSC::JSObject*,
                                                           JSC::JSValue thisValue, const JSC::ArgList&)
{
    if (!thisValue.isObject(&QObjectWrapperObject::info))
        return throwError(exec, JSC::TypeError, "this object is not a QObject");
    QObject *obj = static_cast<QObjectWrapperObject*>(JSC::asObject(thisValue))->value();
    const QMetaObject *meta = obj ? obj->metaObject() : &QObject::staticMetaObject;
    QString name = obj ? obj->objectName() : QString::fromUtf8("unnamed");
    QString str = QString::fromUtf8("%0(name = \"%1\")")
                  .arg(QLatin1String(meta->className())).arg(name);
    return JSC::jsString(exec, qtStringToJSCUString(str));
}

QObjectPrototype::QObjectPrototype(JSC::ExecState* exec, WTF::PassRefPtr<JSC::Structure> structure,
                                   JSC::Structure* prototypeFunctionStructure)
    : QObjectWrapperObject(new QObject(), QScriptEngine::AutoOwnership, /*options=*/0, structure)
{
    putDirectFunction(exec, new (exec) JSC::PrototypeFunction(exec, prototypeFunctionStructure, /*length=*/0, exec->propertyNames().toString, qobjectProtoFuncToString), JSC::DontEnum);
    putDirectFunction(exec, new (exec) JSC::PrototypeFunction(exec, prototypeFunctionStructure, /*length=*/1, JSC::Identifier(exec, "findChild"), qobjectProtoFuncFindChild), JSC::DontEnum);
    putDirectFunction(exec, new (exec) JSC::PrototypeFunction(exec, prototypeFunctionStructure, /*length=*/1, JSC::Identifier(exec, "findChildren"), qobjectProtoFuncFindChildren), JSC::DontEnum);
}

const JSC::ClassInfo QMetaObjectWrapperObject::info = { "QMetaObject", 0, 0, 0 };

QMetaObjectWrapperObject::QMetaObjectWrapperObject(
    const QMetaObject *metaObject, JSC::JSValue ctor,
    WTF::PassRefPtr<JSC::Structure> sid)
    : JSC::JSObject(sid), data(new Data(metaObject, ctor))
{
}

QMetaObjectWrapperObject::~QMetaObjectWrapperObject()
{
}
    
bool QMetaObjectWrapperObject::getOwnPropertySlot(
    JSC::ExecState *exec, const JSC::Identifier& propertyName,
    JSC::PropertySlot &slot)
{
    const QMetaObject *meta = data->value;
    if (!meta)
        return false;

    QByteArray name = qtStringFromJSCUString(propertyName.ustring()).toLatin1();
    if (name == "prototype") {
        qWarning("getting of metaobject.prototype not implemented");
        return false;
    }

    for (int i = 0; i < meta->enumeratorCount(); ++i) {
        QMetaEnum e = meta->enumerator(i);
        for (int j = 0; j < e.keyCount(); ++j) {
            const char *key = e.key(j);
            if (!qstrcmp(key, name.constData())) {
                slot.setValue(JSC::JSValue(exec, e.value(j)));
                return true;
            }
        }
    }

    return JSC::JSObject::getOwnPropertySlot(exec, propertyName, slot);
}

void QMetaObjectWrapperObject::put(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                                   JSC::JSValue value, JSC::PutPropertySlot &slot)
{
    const QMetaObject *meta = data->value;
    if (meta) {
        QByteArray name = qtStringFromJSCUString(propertyName.ustring()).toLatin1();
        for (int i = 0; i < meta->enumeratorCount(); ++i) {
            QMetaEnum e = meta->enumerator(i);
            for (int j = 0; j < e.keyCount(); ++j) {
                if (!qstrcmp(e.key(j), name.constData()))
                    return;
            }
        }
    }
    JSC::JSObject::put(exec, propertyName, value, slot);
}

bool QMetaObjectWrapperObject::deleteProperty(
    JSC::ExecState *exec, const JSC::Identifier& propertyName)
{
    const QMetaObject *meta = data->value;
    if (meta) {
        QByteArray name = qtStringFromJSCUString(propertyName.ustring()).toLatin1();
        for (int i = 0; i < meta->enumeratorCount(); ++i) {
            QMetaEnum e = meta->enumerator(i);
            for (int j = 0; j < e.keyCount(); ++j) {
                if (!qstrcmp(e.key(j), name.constData()))
                    return false;
            }
        }
    }
    return JSC::JSObject::deleteProperty(exec, propertyName);
}

bool QMetaObjectWrapperObject::getPropertyAttributes(JSC::ExecState *exec,
                                                     const JSC::Identifier &propertyName,
                                                     unsigned &attributes) const
{
    const QMetaObject *meta = data->value;
    if (meta) {
        QByteArray name = qtStringFromJSCUString(propertyName.ustring()).toLatin1();
        for (int i = 0; i < meta->enumeratorCount(); ++i) {
            QMetaEnum e = meta->enumerator(i);
            for (int j = 0; j < e.keyCount(); ++j) {
                if (!qstrcmp(e.key(j), name.constData())) {
                    attributes = JSC::ReadOnly | JSC::DontDelete;
                    return true;
                }
            }
        }
    }
    return JSC::JSObject::getPropertyAttributes(exec, propertyName, attributes);
}

void QMetaObjectWrapperObject::getPropertyNames(JSC::ExecState *exec, JSC::PropertyNameArray &propertyNames)
{
    const QMetaObject *meta = data->value;
    if (!meta)
        return;
    for (int i = 0; i < meta->enumeratorCount(); ++i) {
        QMetaEnum e = meta->enumerator(i);
        for (int j = 0; j < e.keyCount(); ++j)
            propertyNames.add(JSC::Identifier(exec, e.key(j)));
    }
    JSC::JSObject::getPropertyNames(exec, propertyNames);
}

struct StaticQtMetaObject : public QObject
{
    static const QMetaObject *get()
        { return &static_cast<StaticQtMetaObject*> (0)->staticQtMetaObject; }
};

static JSC::JSValue JSC_HOST_CALL qmetaobjectProtoFuncClassName(
    JSC::ExecState *exec, JSC::JSObject*, JSC::JSValue thisValue, const JSC::ArgList&)
{
    if (!thisValue.isObject(&QMetaObjectWrapperObject::info))
        return throwError(exec, JSC::TypeError, "this object is not a QMetaObject");
    const QMetaObject *meta = static_cast<QMetaObjectWrapperObject*>(JSC::asObject(thisValue))->value();
    return JSC::jsString(exec, meta->className());
}

QMetaObjectPrototype::QMetaObjectPrototype(
    JSC::ExecState *exec, WTF::PassRefPtr<JSC::Structure> structure,
    JSC::Structure* prototypeFunctionStructure)
    : QMetaObjectWrapperObject(StaticQtMetaObject::get(), /*ctor=*/JSC::JSValue(), structure)
{
    putDirectFunction(exec, new (exec) JSC::PrototypeFunction(exec, prototypeFunctionStructure, /*length=*/0, JSC::Identifier(exec, "className"), qmetaobjectProtoFuncClassName), JSC::DontEnum);
}

static const uint qt_meta_data_QObjectConnectionManager[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      35,   34,   34,   34, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QObjectConnectionManager[] = {
    "QScript::QObjectConnectionManager\0\0execute()\0"
};

const QMetaObject QObjectConnectionManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QObjectConnectionManager,
      qt_meta_data_QObjectConnectionManager, 0 }
};

const QMetaObject *QObjectConnectionManager::metaObject() const
{
    return &staticMetaObject;
}

void *QObjectConnectionManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QObjectConnectionManager))
        return static_cast<void*>(const_cast<QObjectConnectionManager*>(this));
    return QObject::qt_metacast(_clname);
}

int QObjectConnectionManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        execute(_id, _a);
        _id -= slotCounter;
    }
    return _id;
}

void QObjectConnectionManager::execute(int slotIndex, void **argv)
{
    JSC::JSValue receiver;
    JSC::JSValue slot;
    JSC::JSValue senderWrapper;
    int signalIndex = -1;
    for (int i = 0; i < connections.size(); ++i) {
        const QVector<QObjectConnection> &cs = connections.at(i);
        for (int j = 0; j < cs.size(); ++j) {
            const QObjectConnection &c = cs.at(j);
            if (c.slotIndex == slotIndex) {
                receiver = c.receiver;
                slot = c.slot;
                senderWrapper = c.senderWrapper;
                signalIndex = i;
                break;
            }
        }
    }
//    Q_ASSERT(slot != 0);

#if 0
    // ### fixme
    if (engine->isCollecting()) {
        // we can't do a script function call during GC,
        // so we're forced to ignore this signal
        return;
    }

    QScriptFunction *fun = engine->convertToNativeFunction(slot);
    if (fun == 0) {
        // the signal handler has been GC'ed. This can only happen when
        // a QObject is owned by the engine, the engine is destroyed, and
        // there is a script function connected to the destroyed() signal
        Q_ASSERT(signalIndex <= 1); // destroyed(QObject*)
        return;
    }
#endif

    const QMetaObject *meta = sender()->metaObject();
    const QMetaMethod method = meta->method(signalIndex);

    QList<QByteArray> parameterTypes = method.parameterTypes();
    int argc = parameterTypes.count();

    JSC::ExecState *exec = engine->currentFrame;
    QVector<JSC::JSValue> argsVector;
    argsVector.resize(argc);
    for (int i = 0; i < argc; ++i) {
        // ### optimize -- no need to convert via QScriptValue
        QScriptValue actual;
        void *arg = argv[i + 1];
        QByteArray typeName = parameterTypes.at(i);
        int argType = QMetaType::type(parameterTypes.at(i));
        if (!argType) {
            if (typeName == "QVariant") {
                actual = engine->scriptValueFromVariant(*reinterpret_cast<QVariant*>(arg));
            } else {
                qWarning("QScriptEngine: Unable to handle unregistered datatype '%s' "
                         "when invoking handler of signal %s::%s",
                         typeName.constData(), meta->className(), method.signature());
                actual = QScriptValue(QScriptValue::UndefinedValue);
            }
        } else {
            actual = engine->create(argType, arg);
        }
        argsVector[i] = engine->scriptValueToJSCValue(actual);
    }
    JSC::ArgList jscArgs(argsVector.data(), argsVector.size());

    JSC::JSValue senderObject;
    if (senderWrapper && senderWrapper.isObject(&QObjectWrapperObject::info))
        senderObject = senderWrapper;
    else {
        QScriptEngine::QObjectWrapOptions opt = QScriptEngine::PreferExistingWrapperObject;
        senderObject = engine->newQObject(sender(), QScriptEngine::QtOwnership, opt);
    }

    JSC::JSValue thisObject;
    if (receiver && receiver.isObject())
        thisObject = receiver;
    else
        thisObject = exec->lexicalGlobalObject();

    JSC::CallData callData;
    JSC::CallType callType = slot.getCallData(callData);
    if (callType == JSC::CallTypeJS) {
        if (exec->hadException())
            exec->clearException(); // ### otherwise JSC asserts
        (void)JSC::asFunction(slot)->call(exec, thisObject, jscArgs);
    } else if (callType == JSC::CallTypeHost) {
        (void)callData.native.function(exec, JSC::asObject(slot), thisObject, jscArgs);
    }

    if (exec->hadException())
        engine->emitSignalHandlerException();
}

QObjectConnectionManager::QObjectConnectionManager(QScriptEnginePrivate *eng)
    : engine(eng), slotCounter(0)
{
}

QObjectConnectionManager::~QObjectConnectionManager()
{
}

void QObjectConnectionManager::mark()
{
    for (int i = 0; i < connections.size(); ++i) {
        QVector<QObjectConnection> &cs = connections[i];
        for (int j = 0; j < cs.size(); ++j)
            cs[j].mark();
    }
}

bool QObjectConnectionManager::addSignalHandler(
    QObject *sender, int signalIndex, JSC::JSValue receiver,
    JSC::JSValue function, JSC::JSValue senderWrapper)
{
    if (connections.size() <= signalIndex)
        connections.resize(signalIndex+1);
    QVector<QObjectConnection> &cs = connections[signalIndex];
    int absSlotIndex = slotCounter + metaObject()->methodOffset();
    bool ok = QMetaObject::connect(sender, signalIndex, this, absSlotIndex);
    if (ok) {
        cs.append(QObjectConnection(slotCounter++, receiver, function, senderWrapper));
        QMetaMethod signal = sender->metaObject()->method(signalIndex);
        QByteArray signalString;
        signalString.append('2'); // signal code
        signalString.append(signal.signature());
        static_cast<QObjectNotifyCaller*>(sender)->callConnectNotify(signalString);
    }
    return ok;
}

bool QObjectConnectionManager::removeSignalHandler(
    QObject *sender, int signalIndex,
    JSC::JSValue receiver, JSC::JSValue slot)
{
    if (connections.size() <= signalIndex)
        return false;
    QVector<QObjectConnection> &cs = connections[signalIndex];
    for (int i = 0; i < cs.size(); ++i) {
        const QObjectConnection &c = cs.at(i);
        if (c.hasTarget(receiver, slot)) {
            int absSlotIndex = c.slotIndex + metaObject()->methodOffset();
            bool ok = QMetaObject::disconnect(sender, signalIndex, this, absSlotIndex);
            if (ok) {
                cs.remove(i);
                QMetaMethod signal = sender->metaObject()->method(signalIndex);
                QByteArray signalString;
                signalString.append('2'); // signal code
                signalString.append(signal.signature());
                static_cast<QScript::QObjectNotifyCaller*>(sender)->callDisconnectNotify(signalString);
            }
            return ok;
        }
    }
    return false;
}

QObjectData::QObjectData(QScriptEnginePrivate *eng)
    : engine(eng), connectionManager(0)
{
}

QObjectData::~QObjectData()
{
    if (connectionManager) {
        delete connectionManager;
        connectionManager = 0;
    }
}

void QObjectData::mark()
{
    if (connectionManager)
        connectionManager->mark();
}

bool QObjectData::addSignalHandler(QObject *sender,
                                   int signalIndex,
                                   JSC::JSValue receiver,
                                   JSC::JSValue slot,
                                   JSC::JSValue senderWrapper)
{
    if (!connectionManager)
        connectionManager = new QObjectConnectionManager(engine);
    return connectionManager->addSignalHandler(
        sender, signalIndex, receiver, slot, senderWrapper);
}

bool QObjectData::removeSignalHandler(QObject *sender,
                                      int signalIndex,
                                      JSC::JSValue receiver,
                                      JSC::JSValue slot)
{
    if (!connectionManager)
        return false;
    return connectionManager->removeSignalHandler(
        sender, signalIndex, receiver, slot);
}

} // namespace QScript

namespace JSC
{
    ASSERT_CLASS_FITS_IN_CELL(QScript::QtFunction);
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
