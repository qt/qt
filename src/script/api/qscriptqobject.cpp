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

#include "qscriptconverter_p.h"
#include "qscriptisolate_p.h"
#include "qscriptengine.h"
#include "qscriptengine_p.h"
#include "qscriptqobject_p.h"
#include "qscriptv8objectwrapper_p.h"

#include <QtCore/qmetaobject.h>
#include <QtCore/qvarlengtharray.h>

#include <QtCore/qdebug.h>
#include "qscriptvalue_p.h"
#include "qscriptcontext_p.h"
#include "qscriptable_p.h"

#include "qscript_impl_p.h"
#include <v8.h>

QT_BEGIN_NAMESPACE

static inline bool methodNameEquals(const QMetaMethod &method,
                                    const char *signature, int nameLength)
{
    const char *otherSignature = method.signature();
    return !qstrncmp(otherSignature, signature, nameLength)
        && (otherSignature[nameLength] == '(');
}

static inline int methodNameLength(const char *signature)
{
    const char *s = signature;
    while (*s && (*s != '('))
        ++s;
    return s - signature;
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

static v8::Handle<v8::Value> throwAmbiguousError(const QMetaObject *meta, const QByteArray &functionName, const QString &errorString)
{
    int nameLength = methodNameLength(functionName);
    QString string = errorString + QLatin1String(" %1(); candidates were");
    string = string.arg(QString::fromLatin1(functionName, nameLength));

    for (int index = 0; index < meta->methodCount(); ++index) {
        QMetaMethod method = meta->method(index);
        if (!methodNameEquals(method, functionName, nameLength))
            continue;
        string += QLatin1String("\n    ");
        string += QLatin1String(method.signature());
    }
    return v8::ThrowException(v8::Exception::TypeError(QScriptConverter::toString(string)));
}

// Generic implementation of Qt meta-method invocation.
// Uses QMetaType and friends to resolve types and convert arguments.
static v8::Handle<v8::Value> callQtMetaMethod(QScriptEnginePrivate *engine, QObject *qobject,
                                              const QMetaObject *meta, int methodIndex,
                                              const v8::Arguments& args)
{
    QMetaMethod method = meta->method(methodIndex);
    QList<QByteArray> parameterTypeNames = method.parameterTypes();

    if (args.Length() < parameterTypeNames.size()) {
        return throwAmbiguousError(meta, method.signature(), QString::fromLatin1("too few arguments in call to"));
    }

    const char *returnTypeName = method.typeName();
    int rtype = QMetaType::type(returnTypeName);
    QVarLengthArray<QVariant, 10> cppArgs(1 + parameterTypeNames.size());
    if (rtype > 0) {
        cppArgs[0] = QVariant(rtype, (void *)0);
    } else if (*returnTypeName) { //empty is not void*
        return v8::ThrowException(v8::Exception::TypeError(QScriptConverter::toString(
            QString::fromLatin1("cannot call %0(): unknown return type `%1' (register the type with qScriptRegisterMetaType())")
                .arg(QString::fromLatin1(method.signature(), methodNameLength(method.signature())),
                     QLatin1String(returnTypeName)))));
    }

    // Convert arguments to C++ types.
    for (int i = 0; i < parameterTypeNames.size() && !engine->hasUncaughtException(); ++i) {
        v8::Handle<v8::Value> actual = args[i];

        int targetType = QMetaType::type(parameterTypeNames.at(i));
        if (!targetType) {

            int enumIndex = indexOfMetaEnum(meta, parameterTypeNames.at(i));
            if (enumIndex != -1) {
                const QMetaEnum m(meta->enumerator(enumIndex));
                Q_ASSERT(m.isValid());
                if (actual->IsNumber()) {
                    int ival = actual->ToInt32()->Value();
                    if (m.valueToKey(ival) != 0) {
                        cppArgs[1+i] = ival;
                        continue;
                    }
                } else if (actual->IsString()) {
                    int ival = m.keyToValue(QScriptConverter::toString(actual->ToString()).toLatin1());
                    if (ival != -1) {
                        cppArgs[1+i] = ival;
                        continue;
                    }
                }
            }

            QVariant v(QMetaType::QObjectStar, (void *)0);
            if (engine->convertToNativeQObject(actual, parameterTypeNames.at(i), reinterpret_cast<void* *>(v.data()))) {
                cppArgs[1+i] = v;
                continue;
            }

            return v8::ThrowException(v8::Exception::TypeError(QScriptConverter::toString(
                QString::fromLatin1("cannot call %0(): argument %1 has unknown type `%2' (register the type with qScriptRegisterMetaType())")
                    .arg(QString::fromLatin1(method.signature(), methodNameLength(method.signature())),
                         QString::number(i+1), QLatin1String(parameterTypeNames.at(i))))));
        }


        QVariant v(targetType, (void *)0);
        if (engine->metaTypeFromJS(actual, targetType, v.data())) {
            cppArgs[1+i] = v;
            continue;
        }

        if(engine->isQtVariant(actual)) {
            QVariant &var = engine->variantValue(actual);
            if (var.userType() == targetType) {
                cppArgs[1+i] = var;
                continue;
            } else if (var.canConvert(QVariant::Type(targetType))) {
                v = var;
                v.convert(QVariant::Type(targetType));
                cppArgs[1+i] = v;
                continue;
            }
            QByteArray typeName = var.typeName();
            if (typeName.endsWith('*')
                && (QMetaType::type(typeName.left(typeName.size()-1)) == targetType)) {
                cppArgs[1+i] = QVariant(targetType, *reinterpret_cast<void* *>(var.data()));
                continue;
            }
        }

        return throwAmbiguousError(meta, method.signature(), QString::fromLatin1("incompatible type of argument(s) in call to"));
    }

    if (engine->hasUncaughtException()) {
        return engine->uncaughtException();
    }

    // Prepare void** array for metacall.
    QVarLengthArray<void *, 10> argv(cppArgs.size());
    void **argvData = argv.data();
    for (int i = 0; i < cppArgs.size(); ++i)
        argvData[i] = const_cast<void *>(cppArgs[i].constData());
    if (rtype <= 0)
        argvData[0] = 0;

    // Call the C++ method!
    QMetaObject::metacall(qobject, QMetaObject::InvokeMetaMethod, methodIndex, argvData);

    // Convert and return result.
    if (rtype <= 0)
        return v8::Undefined();
    return engine->metaTypeToJS(rtype, argvData[0]);
}

static int conversionDistance(QScriptEnginePrivate *engine, v8::Handle<v8::Value> actual, int targetType)
{
    if (actual->IsNumber()) {
        switch (targetType) {
        case QMetaType::Double:
            // perfect
            return 0;
        case QMetaType::Float:
            return 1;
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            return 2;
        case QMetaType::Long:
        case QMetaType::ULong:
            return 3;
        case QMetaType::Int:
        case QMetaType::UInt:
            return 4;
        case QMetaType::Short:
        case QMetaType::UShort:
            return 5;
        case QMetaType::Char:
        case QMetaType::UChar:
            return 6;
        default:
            return 10;
        }
    } else if (actual->IsString()) {
        switch (targetType) {
        case QMetaType::QString:
            // perfect
            return 0;
        default:
            return 10;
        }
    } else if (actual->IsBoolean()) {
        switch (targetType) {
        case QMetaType::Bool:
            // perfect
            return 0;
        default:
            return 10;
        }
    } else if (actual->IsDate()) {
        switch (targetType) {
        case QMetaType::QDateTime:
            return 0;
        case QMetaType::QDate:
            return 1;
        case QMetaType::QTime:
            return 2;
        default:
            return 10;
        }
    }
#if 0
    else if (actual->IsRegExp()) {
        switch (targetType) {
        case QMetaType::QRegExp:
            // perfect
            return 0;
        default:
            return 10;
        }
    }
#endif
    else if (engine->isQtVariant(actual)) {
        if ((targetType == QMetaType::QVariant)
            || (engine->variantFromJS(actual).userType() == targetType)) {
            // perfect
            return 0;
        }
        return 10;
    } else if (actual->IsArray()) {
        switch (targetType) {
        case QMetaType::QStringList:
        case QMetaType::QVariantList:
            return 5;
        default:
            return 10;
        }
    } else if (engine->isQtObject(actual)) {
        switch (targetType) {
        case QMetaType::QObjectStar:
        case QMetaType::QWidgetStar:
            // perfect
            return 0;
        default:
            return 10;
        }
    } else if (actual->IsNull()) {
        switch (targetType) {
        case QMetaType::VoidStar:
        case QMetaType::QObjectStar:
        case QMetaType::QWidgetStar:
            // perfect
            return 0;
        default:
            return 10;
        }
    }
    return 100;
}

static int resolveOverloadedQtMetaMethodCall(QScriptEnginePrivate *engine, const QMetaObject *meta, int initialIndex, const v8::Arguments& args)
{
    int bestIndex = -1;
    int bestDistance = INT_MAX;
    int nameLength = 0;
    const char *initialMethodSignature = 0;
    for (int index = initialIndex; index >= 0; --index) {
        QMetaMethod method = meta->method(index);
        if (index == initialIndex) {
            initialMethodSignature = method.signature();
            nameLength = methodNameLength(initialMethodSignature);
        } else {
            if (!methodNameEquals(method, initialMethodSignature, nameLength))
                continue;
        }

        QList<QByteArray> parameterTypeNames = method.parameterTypes();
        int parameterCount = parameterTypeNames.size();
        if (args.Length() != parameterCount)
            continue;

        int distance = 0;
        for (int i = 0; (distance < bestDistance) && (i < parameterCount); ++i) {
            int targetType = QMetaType::type(parameterTypeNames.at(i));
            Q_ASSERT(targetType != 0);
            distance += conversionDistance(engine, args[i], targetType);
        }

        if (distance == 0) {
            // Perfect match, no need to look further.
            return index;
        }

        if (distance < bestDistance) {
            bestIndex = index;
            bestDistance = distance;
        } else if (distance == bestDistance && bestIndex >= 0) {
            // it is possible that a virtual method is redeclared in a subclass,
            // in which case we want to ignore the superclass declaration
            if (qstrcmp(method.signature(), meta->method(bestIndex).signature())) {
                // different signature with the same distance, this is ambiguous;
                bestIndex = -1;
            }
        }
    }
    return bestIndex;
}


// Helper class to invoke QObject::{,dis}connectNotify() (they are protected).
class QtObjectNotifyCaller : public QObject
{
public:
    void callConnectNotify(const char *signal)
        { connectNotify(signal); }
    void callDisconnectNotify(const char *signal)
        { disconnectNotify(signal); }

    static void callConnectNotify(QObject *sender, int signalIndex)
    {
        QMetaMethod signal = sender->metaObject()->method(signalIndex);
        QByteArray signalString;
        signalString.append('2'); // signal code
        signalString.append(signal.signature());
        static_cast<QtObjectNotifyCaller*>(sender)->callConnectNotify(signalString);
    }

    static void callDisconnectNotify(QObject *sender, int signalIndex)
    {
        QMetaMethod signal = sender->metaObject()->method(signalIndex);
        QByteArray signalString;
        signalString.append('2'); // signal code
        signalString.append(signal.signature());
        static_cast<QtObjectNotifyCaller*>(sender)->callDisconnectNotify(signalString);
    }
};

// A C++ signal-to-JS handler connection.
//
// Acts as a middle-man; intercepts a C++ signal,
// and invokes a JS callback function.
//
class QScriptConnection : public QObject
{
public:
    QScriptConnection(QScriptSignalData *signal);
    ~QScriptConnection();

    bool connect(v8::Handle<v8::Object> receiver, v8::Handle<v8::Object> callback, Qt::ConnectionType type);
    bool disconnect();

    QScriptEnginePrivate *engine() const
    {
        return m_signal->engine;
    }
    v8::Handle<v8::Object> callback() const
    { return m_callback; }

    // This class implements qt_metacall() and friends manually; moc should
    // not process it. Qt Meta System doesn't allow to connect ta a slot with undefined arguments
    // as the type safety check should be always performed. This is a hack that allow to skip the check.
    // The Q_OBJECT macro must be manually expanded:
    static const QMetaObject staticMetaObject;
    Q_OBJECT_GETSTATICMETAOBJECT
    virtual const QMetaObject *metaObject() const;
    virtual void *qt_metacast(const char *);
    virtual int qt_metacall(QMetaObject::Call, int, void **);

    // Slot.
    void onSignal(void **);
    void deleteNow()
    {
        // This will be called if underlaying qobject instance is destroyed so there is no guarantee
        // that an isolate was setup correctly (delete can be called outside of public QtScript API).
        QScriptEnginePrivate *engine = this->engine();
        QScriptIsolate api(engine, QScriptIsolate::NotNullEngine);
        Q_ASSERT_X(engine == QScriptQObjectData::get(m_signal->object())->engine(),
                   Q_FUNC_INFO,
                   "Mismatch of QScriptEngines detected");
        delete this;
    }

private:
    QScriptSignalData *m_signal;
    v8::Persistent<v8::Object> m_callback;
    v8::Persistent<v8::Object> m_receiver;
};

QScriptSignalData::~QScriptSignalData()
{
    foreach (QScriptConnection *connection, m_connections) {
        delete connection;
    }
}

v8::Handle<v8::FunctionTemplate> QScriptSignalData::createFunctionTemplate(QScriptEnginePrivate* engine)
{
    v8::HandleScope handleScope;
    v8::Handle<v8::FunctionTemplate> funcTempl = v8::FunctionTemplate::New();
    funcTempl->SetClassName(v8::String::New("QtSignal"));

    v8::Handle<v8::ObjectTemplate> instTempl = funcTempl->InstanceTemplate();
    instTempl->SetCallAsFunctionHandler(QScriptV8ObjectWrapperHelper::callAsFunction<QScriptSignalData>);
    instTempl->SetInternalFieldCount(2); // QScriptSignalData* , and the 'self'

    v8::Handle<v8::ObjectTemplate> protoTempl = funcTempl->PrototypeTemplate();
    v8::Handle<v8::Signature> sig = v8::Signature::New(funcTempl);
    protoTempl->Set(v8::String::New("connect"), v8::FunctionTemplate::New(QtConnectCallback, v8::Handle<v8::Value>(), sig));
    protoTempl->Set(v8::String::New("disconnect"), v8::FunctionTemplate::New(QtDisconnectCallback, v8::Handle<v8::Value>(), sig));

    return handleScope.Close(funcTempl);
}

v8::Handle<v8::FunctionTemplate> QScriptMetaMethodData::createFunctionTemplate(QScriptEnginePrivate* engine)
{
    v8::HandleScope handleScope;
    v8::Handle<v8::FunctionTemplate> funcTempl = v8::FunctionTemplate::New();
    funcTempl->SetClassName(v8::String::New("QtMetaMethod"));

    v8::Handle<v8::ObjectTemplate> instTempl = funcTempl->InstanceTemplate();
    instTempl->SetCallAsFunctionHandler(QScriptV8ObjectWrapperHelper::callAsFunction<QScriptMetaMethodData>);
    instTempl->SetInternalFieldCount(2); // QScriptMetaMethodData* and the 'self'

    return handleScope.Close(funcTempl);
}


// Connects this signal to the given callback.
// receiver might be empty
// Returns undefined if the connection succeeded, otherwise throws an error.
v8::Handle<v8::Value> QScriptSignalData::connect(v8::Handle<v8::Object> receiver,
                                            v8::Handle<v8::Object> slot, Qt::ConnectionType type)
{
     if (QScriptMetaMethodData *cppSlot = QScriptMetaMethodData::safeGet(slot, engine)) {
         if (receiver.IsEmpty())
             receiver = cppSlot->object();
         QObject *recv = QScriptQObjectData::get(receiver)->cppObject(QScriptQObjectData::IgnoreException);
         if (recv) {
             QObject *sender = QScriptQObjectData::get(object())->cppObject();
             if (QMetaObject::connect(sender, index(), recv, cppSlot->index(), type))
                 return v8::Undefined();
             else
                 return v8::ThrowException(v8::Exception::Error(v8::String::New("QtSignal.connect(): failed to connect")));
         }
     }
     QScriptConnection *connection = new QScriptConnection(this);
     if (!connection->connect(receiver, slot, type)) {
         delete connection;
         return v8::ThrowException(v8::Exception::Error(v8::String::New("QtSignal.connect(): failed to connect")));
     }
     m_connections.append(connection);
     return v8::Undefined();
}

// Disconnect this signal from the given callback.
// Returns undefined if the disconnection succeeded, otherwise throws an error.
v8::Handle<v8::Value> QScriptSignalData::disconnect(v8::Handle<v8::Function> callback)
{
    for (int i = 0; i < m_connections.size(); ++i) {
        QScriptConnection *connection = m_connections.at(i);
        if (connection->callback()->StrictEquals(callback)) {
            if (!connection->disconnect())
                return v8::ThrowException(v8::Exception::Error(v8::String::New("QtSignal.disconnect(): failed to disconnect")));
            m_connections.removeAt(i);
            delete connection;
            return v8::Undefined();
        }
    }
    return v8::ThrowException(v8::Exception::Error(v8::String::New("QtSignal.disconnect(): function not connected to this signal")));
}

//Call the method
template <typename T,  v8::Persistent<v8::FunctionTemplate> QScriptEnginePrivate::*functionTemplate>
v8::Handle<v8::Value> QScriptGenericMetaMethodData<T, functionTemplate>::call()
{
    QScriptQObjectData *instance = QScriptQObjectData::get(object());
    v8::Local<v8::Value> error;
    QObject *qobject = instance->cppObject(&error);
    if (!qobject)
        return error;

    const QMetaObject *meta = qobject->metaObject();
    const v8::Arguments *args = this->engine->currentContext()->arguments;

    int methodIndex = m_info.index;
    if (m_info.overloaded)
        methodIndex = resolveOverloadedQtMetaMethodCall(this->engine, meta, methodIndex, *args);

    if (methodIndex < 0) { // Ambiguous call.
        return throwAmbiguousError(meta, meta->method(m_info.index).signature(), QString::fromLatin1("ambiguous call of overloaded function"));
    }

    QScriptEnginePrivate *oldEngine = 0;
    QScriptable *scriptable = instance->toQScriptable();
    if (scriptable)
        oldEngine = QScriptablePrivate::get(scriptable)->swapEngine(instance->engine());

    v8::Handle<v8::Value> result;
    if (m_info.voidvoid) {
        QMetaObject::metacall(qobject, QMetaObject::InvokeMetaMethod, methodIndex, 0);
    } else {
        result = callQtMetaMethod(this->engine, qobject, meta, methodIndex, *args);
    }

    if (scriptable)
        QScriptablePrivate::get(scriptable)->swapEngine(oldEngine);

    return result;
}

QScriptConnection::QScriptConnection(QScriptSignalData *signal)
    : m_signal(signal)
{
    Q_ASSERT(m_signal);
    Q_ASSERT(m_signal->engine);
}

QScriptConnection::~QScriptConnection()
{
    m_signal->unregisterQScriptConnection(this);
    m_callback.Dispose();
    m_receiver.Dispose();
}


// Connects to this connection's signal, and binds this connection to the
// given callback.
// Returns true if the connection succeeded, otherwise returns false.
bool QScriptConnection::connect(v8::Handle<v8::Object> receiver, v8::Handle<v8::Object> callback, Qt::ConnectionType type)
{
    Q_ASSERT(m_callback.IsEmpty());
    QScriptQObjectData *instance = QScriptQObjectData::get(m_signal->object());
    QObject *sender = instance->cppObject();
    bool ok;
    if (sender) {
        if (QMetaObject::connect(sender, m_signal->index(), this, staticMetaObject.methodOffset(), type)) {
            if (!receiver.IsEmpty() && m_signal->engine->isQtObject(receiver)) {
                instance = QScriptQObjectData::get(receiver);
                QObject *recv = instance->cppObject(QScriptQObjectData::IgnoreException);
                if (recv) {
                    // FIXME: we are connecting to qobjects, can we try to connect them directly?
                    QObject::connect(recv, SIGNAL(destroyed()), this, SLOT(deleteNow()));
                }
            }

            QtObjectNotifyCaller::callConnectNotify(sender, m_signal->index());
            m_callback = v8::Persistent<v8::Object>::New(callback);
            m_receiver = v8::Persistent<v8::Object>::New(receiver);
            ok = true;
        } else
            ok = false;
    } else
        ok = false;
    return ok;
}

// Disconnects from this connection's signal, and unbinds the callback.
bool QScriptConnection::disconnect()
{
    Q_ASSERT(!m_callback.IsEmpty());
    QScriptQObjectData *instance = QScriptQObjectData::get(m_signal->object());
    QObject *sender = instance->cppObject();
    bool ok = sender && QMetaObject::disconnect(sender, m_signal->index(),
                                      this, staticMetaObject.methodOffset());
    if (ok) {
        QtObjectNotifyCaller::callDisconnectNotify(sender, m_signal->index());
        m_callback.Dispose();
        m_callback.Clear();
    }
    return ok;
}

// This slot is called when the C++ signal is emitted.
// It forwards the call to the JS callback.
void QScriptConnection::onSignal(void **argv)
{
    Q_ASSERT(!m_callback.IsEmpty());

    QScriptEnginePrivate *engine = this->engine();
    QScriptIsolate api(engine, QScriptIsolate::NotNullEngine);
    Q_ASSERT_X(engine == QScriptQObjectData::get(m_signal->object())->engine(),
               Q_FUNC_INFO,
               "Mismatch of QScriptEngines detected");
    v8::HandleScope handleScope;

    const QMetaObject *meta = sender()->metaObject();
    QMetaMethod method = meta->method(m_signal->index());


    QList<QByteArray> parameterTypes = method.parameterTypes();
    int argc = parameterTypes.count();

    // Convert arguments to JS.
    QVarLengthArray<v8::Handle<v8::Value>, 8> jsArgv(argc);
    for (int i = 0; i < argc; ++i) {
        v8::Handle<v8::Value> convertedArg;
        void *arg = argv[i + 1];
        QByteArray typeName = parameterTypes.at(i);
        int type = QMetaType::type(typeName);
        if (!type) {
            qWarning("Unable to handle unregistered datatype '%s' "
                     "when invoking signal handler for %s::%s",
                     typeName.constData(), meta->className(), method.signature());
            convertedArg = v8::Undefined();
        } else {
            convertedArg = engine->metaTypeToJS(type, arg);
        }
        jsArgv[i] = convertedArg;
    }

    v8::TryCatch tryCatch;
    v8::Handle<v8::Object> receiver = m_receiver;
    if (receiver.IsEmpty())
        receiver = v8::Context::GetCurrent()->Global();

    /*QScriptQObjectData *instance = QtInstanceData::safeGet(receiver);
    if (instance) {
        QObject *obj = instance->cppObject(QScriptQObjectData::IgnoreException);
        if (!obj)
            return;
    }*/
    m_callback->Call(receiver, argc, const_cast<v8::Handle<v8::Value>*>(jsArgv.constData()));

    if (tryCatch.HasCaught()) {
        v8::Local<v8::Value> result = tryCatch.Exception();
        engine->setException(result, tryCatch.Message());
        engine->emitSignalHandlerException();
    }
}

// moc-generated code.
// DO NOT EDIT!

static const uint qt_meta_data_QScriptConnection[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x0a,
      30,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QScriptConnection[] = {
    "QScriptConnection\0\0onSignal()\0deleteNow()\0"
};

const QMetaObject QScriptConnection::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QScriptConnection,
      qt_meta_data_QScriptConnection, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QScriptConnection::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QScriptConnection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QScriptConnection::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QScriptConnection))
        return static_cast<void*>(const_cast< QScriptConnection*>(this));
    return QObject::qt_metacast(_clname);
}

int QScriptConnection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: onSignal(_a); break;
        case 1: deleteNow(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

#if 0
struct StringType
{
    typedef QString Type;

    static v8::Handle<v8::Value> toJS(const QString &s)
    { return qtStringToJS(s); }

    static QString fromJS(v8::Handle<v8::Value> v)
    { return qtStringFromJS(v->ToString()); }
};

struct DoubleType
{
    typedef double Type;

    static v8::Handle<v8::Value> toJS(double d)
    { return v8::Number::New(d); }

    static double fromJS(v8::Handle<v8::Value> v)
    { return v->ToNumber()->Value(); }
};

struct BoolType
{
    typedef bool Type;

    static v8::Handle<v8::Value> toJS(bool b)
    { return v8::Boolean::New(b); }

    static bool fromJS(v8::Handle<v8::Value> v)
    { return v->ToBoolean()->Value(); }
};

template <class Type>
static v8::Handle<v8::Value> QtMetaPropertyFastGetter(v8::Local<v8::String> /*property*/,
                                                      const v8::AccessorInfo& info)
{
    v8::Local<v8::Object> self = info.Holder();
    QScriptQObjectData *data = QtInstanceData::get(self);

    QObject *qobject = data->cppObject();

    int propertyIndex = v8::Int32::Cast(*info.Data())->Value();

    typename Type::Type v;
    void *argv[] = { &v };

    QMetaObject::metacall(qobject, QMetaObject::ReadProperty, propertyIndex, argv);

    return Type::toJS(v);
}

template <class Type>
static void QtMetaPropertyFastSetter(v8::Local<v8::String> /*property*/,
                                     v8::Local<v8::Value> value,
                                     const v8::AccessorInfo& info)
{
    v8::Local<v8::Object> self = info.Holder();
    QScriptQObjectData *data = QtInstanceData::get(self);

    QObject *qobject = data->cppObject();

    int propertyIndex = v8::Int32::Cast(*info.Data())->Value();

    typename Type::Type v = Type::fromJS(value);
    void *argv[] = { &v };

    QMetaObject::metacall(qobject, QMetaObject::WriteProperty, propertyIndex, argv);
}
#endif


// This callback implements meta-object-defined property reads for objects
// that don't inherit QScriptable.
// - info.Holder() is a QObject wrapper
// - info.Data() is the meta-property index
static v8::Handle<v8::Value> QtMetaPropertyGetter(v8::Local<v8::String> property,
                                                  const v8::AccessorInfo& info)
{
    v8::Local<v8::Object> self = info.Holder();
    QScriptQObjectData *data = QScriptQObjectData::get(self);
    QScriptEnginePrivate *engine = data->engine();
    QScriptContextPrivate context(engine, &info);

    v8::Local<v8::Value> error;
    QObject *qobject = data->cppObject(&error);
    if (!qobject)
        return error;

    const QMetaObject *meta = qobject->metaObject();

    int propertyIndex = v8::Int32::Cast(*info.Data())->Value();

    QMetaProperty prop = meta->property(propertyIndex);
    if (!prop.isReadable())
        return v8::Undefined();

    QScriptEnginePrivate *oldEngine = 0;
    QScriptable *scriptable = data->toQScriptable();
    if (scriptable)
        oldEngine = QScriptablePrivate::get(scriptable)->swapEngine(data->engine());

    QVariant value = prop.read(qobject);

    if (scriptable)
        QScriptablePrivate::get(scriptable)->swapEngine(oldEngine);

    return engine->variantToJS(value);
}

// This callback implements meta-object-defined property writes for objects
// that don't inherit QScriptable.
// - info.Holder() is a QObject wrapper
// - info.Data() is the meta-property index
static void QtMetaPropertySetter(v8::Local<v8::String> /*property*/,
                                 v8::Local<v8::Value> value,
                                 const v8::AccessorInfo& info)
{
    v8::Local<v8::Object> self = info.Holder(); // This?
    QScriptQObjectData *data = QScriptQObjectData::get(self);
    QScriptEnginePrivate *engine = data->engine();
    QScriptContextPrivate context(engine, &info);

    QObject *qobject = data->cppObject();
    if (!qobject)
        return;

    const QMetaObject *meta = qobject->metaObject();

    int propertyIndex = v8::Int32::Cast(*info.Data())->Value();

    QMetaProperty prop = meta->property(propertyIndex);
    Q_ASSERT(prop.isWritable());

    QVariant cppValue;

    if (prop.isEnumType() && value->IsString()
        && !engine->hasDemarshalFunction(prop.userType())) {
        // Give QMetaProperty::write() a chance to convert from
        // string to enum value.
        cppValue = QScriptConverter::toString(value->ToString());
    } else {
        int type = prop.userType();
        if (type == -1) { // -1 is a QVariant.
            cppValue = engine->variantFromJS(value);
        } else {
            cppValue = QVariant(type, (void *)0);
            if (!engine->metaTypeFromJS(value, type, cppValue.data())) {
                cppValue = engine->variantFromJS(value);
                if (cppValue.userType() != type) {
                    QByteArray typeName = cppValue.typeName();
                    if (typeName.endsWith('*') && (QMetaType::type(typeName.left(typeName.size()-1)) == type)) {
                        cppValue = QVariant(type, *reinterpret_cast<void* *>(cppValue.data()));
                    }
                }
            }
        }
    }

    QScriptEnginePrivate *oldEngine = 0;
    QScriptable *scriptable = data->toQScriptable();
    if (scriptable)
        oldEngine = QScriptablePrivate::get(scriptable)->swapEngine(data->engine());

    prop.write(qobject, cppValue);

    if (scriptable)
        QScriptablePrivate::get(scriptable)->swapEngine(oldEngine);
}

// This callback implements reading a presumably existing dynamic property.
// Unlike meta-object-defined properties, dynamic properties are specific to
// an instance (not tied to a class). Dynamic properties can be added,
// changed and removed at any time. If the dynamic property with the given
// name no longer exists, this accessor will be uninstalled.
v8::Handle<v8::Value> QtDynamicPropertyGetter(v8::Local<v8::String> property,
                                                     const v8::AccessorInfo& info)
{
    v8::Local<v8::Object> self = info.Holder(); // This?
    QScriptQObjectData *data = QScriptQObjectData::get(self);
    QScriptEnginePrivate *engine = data->engine();
    QScriptContextPrivate context(engine, &info);

    v8::Local<v8::Value> error;
    QObject *qobject = data->cppObject(&error);
    if (!qobject)
        return error;

    QByteArray name = QScriptConverter::toString(property).toLatin1();
    QVariant value = qobject->property(name);
    if (!value.isValid()) {
        // The property no longer exists. Remove this accessor.
        self->ForceDelete(property);

        // Fallback to V8 to get our property again. Unless other
        // property is found on the prototype chain, we'll end up
        // in QObject interceptor again.
        return self->Get(property);
    }
    return engine->variantToJS(value);
}

// This callback implements writing a presumably existing dynamic property.
// If the dynamic property with the given name no longer exists, this accessor
// will be uninstalled.
void QtDynamicPropertySetter(v8::Local<v8::String> property,
                                    v8::Local<v8::Value> value,
                                    const v8::AccessorInfo& info)
{
    v8::Local<v8::Object> self = info.Holder(); // This?
    QScriptQObjectData *data = QScriptQObjectData::get(self);
    QScriptEnginePrivate *engine = data->engine();
    QScriptContextPrivate context(engine, &info);

    QObject *qobject = data->cppObject();
    if (!qobject)
        return;

    QByteArray name = QScriptConverter::toString(property).toLatin1();
    if (qobject->dynamicPropertyNames().indexOf(name) == -1) {
        // The property no longer exists. Remove this accessor.
        self->ForceDelete(property);
        // Call normal logic for property writes.
        Q_UNIMPLEMENTED();
    }

    QVariant cppValue = engine->variantFromJS(value);
    qobject->setProperty(name, cppValue);
}

// This callback implements a fallback property getter for Qt wrapper objects.
// This is only called if the property is not a QMetaObject-defined property,
// Q_INVOKABLE method or slot. It handles signals (which are methods that must
// be bound to an object), and dynamic properties and child objects (since they
// are instance-specific, not defined by the QMetaObject).
v8::Handle<v8::Value> QtLazyPropertyGetter(v8::Local<v8::String> property,
                                                  const v8::AccessorInfo& info)
{
    QScriptEnginePrivate *engine = reinterpret_cast<QScriptEnginePrivate *>(v8::External::Unwrap(info.Data()));
    Q_ASSERT(engine);
    v8::Local<v8::Object> self = info.This();
    if (!engine->qobjectTemplate()->HasInstance(self))
        return v8::Handle<v8::Value>(); //the QObject prototype is being used on another object.
    QScriptQObjectData *data = QScriptQObjectData::get(self);
    Q_ASSERT(engine == data->engine());
    QScriptContextPrivate context(engine, &info);

    v8::Local<v8::Value> error;
    QObject *qobject = data->cppObject(&error);
    if (!qobject)
        return error;

    QByteArray name = QScriptConverter::toString(property).toLatin1();

    // Look up dynamic property.
    {
        int index = qobject->dynamicPropertyNames().indexOf(name);
        if (index != -1) {
            QVariant value = qobject->property(name);
            // Install accessor for this dynamic property.
            // Dynamic properties can be removed from the C++ object without
            // us knowing it (well, we could install an event filter, but
            // that seems overkill). The getter makes sure that the property
            // is still there the next time a read is attempted, and
            // uninstalls itself if not, so that we fall back to this
            // interceptor again.
            self->SetAccessor(property,
                              QtDynamicPropertyGetter,
                              QtDynamicPropertySetter);

            return engine->variantToJS(value);
        }
    }

    // Look up child.
    if (!(data->options() & QScriptEngine::ExcludeChildObjects)) {
        QList<QObject*> children = qobject->children();
        for (int index = 0; index < children.count(); ++index) {
            QObject *child = children.at(index);
            QString childName = child->objectName();
            if (childName == QString::fromLatin1(name)) {
                v8::Handle<v8::Value> result = engine->newQObject(child);
                //TODO: We could set the property, but then we should also make sure it goes away when the object is destroyed.
                //  and we should not hide the dynamic properties
                //self->Set(property, result, v8::PropertyAttribute(v8::DontEnum | v8::DontDelete));
                return result;
            }
        }
    }

    return v8::Handle<v8::Value>();
}

v8::Handle<v8::Value> QtLazyPropertySetter(v8::Local<v8::String> property,
                                                  v8::Local<v8::Value> value,
                                                  const v8::AccessorInfo& info)
{
    QScriptEnginePrivate *engine = reinterpret_cast<QScriptEnginePrivate *>(v8::External::Unwrap(info.Data()));
    Q_ASSERT(engine);
    v8::Local<v8::Object> self = info.This();
    if (!engine->qobjectTemplate()->HasInstance(self))
        return v8::Handle<v8::Value>(); //the QObject prototype is being used on another object.
    QScriptQObjectData *data = QScriptQObjectData::get(self);
    if (!(data->options() & QScriptEngine::AutoCreateDynamicProperties))
        return v8::Handle<v8::Value>();
    Q_ASSERT(engine == data->engine());

    v8::Local<v8::Value> error;
    QObject *qobject = data->cppObject(&error);
    if (!qobject)
        return error;

    QByteArray name = QScriptConverter::toString(property).toLatin1();
    qobject->setProperty(name, engine->variantFromJS(value));

    self->SetAccessor(property, QtDynamicPropertyGetter, QtDynamicPropertySetter);
    
    return value;
}

// This callback implements a catch-all property getter for QMetaObject wrapper objects.
v8::Handle<v8::Value> QtMetaObjectPropertyGetter(v8::Local<v8::String> property,
                                                 const v8::AccessorInfo& info)
{
    v8::Local<v8::Object> self = info.Holder();
    QtMetaObjectData *data = QtMetaObjectData::get(self);
    QScriptEnginePrivate *engine = data->engine();
    QScriptContextPrivate context(engine, &info);

    const QMetaObject *meta = data->metaObject();
    QString propertyName = QScriptConverter::toString(property);

    if (propertyName == QLatin1String("prototype")) {
        v8::Handle<v8::Value> ctor = data->constructor();
        if (ctor->IsObject())
            return ctor->ToObject()->Get(property);
    }

    QByteArray name = propertyName.toLatin1();

    for (int i = 0; i < meta->enumeratorCount(); ++i) {
        QMetaEnum e = meta->enumerator(i);
        for (int j = 0; j < e.keyCount(); ++j) {
            const char *key = e.key(j);
            if (!qstrcmp(key, name.constData()))
                return v8::Int32::New(e.value(j));
        }
    }

    return v8::Handle<v8::Value>();
}

v8::Handle<v8::Array> QtMetaObjectEnumerator(const v8::AccessorInfo& info)
{
    v8::HandleScope handleScope;

    v8::Local<v8::Object> self = info.Holder();
    QtMetaObjectData *data = QtMetaObjectData::get(self);
    QScriptEnginePrivate *engine = data->engine();
    QScriptContextPrivate context(engine, &info);

    const QMetaObject *meta = data->metaObject();
    v8::Handle<v8::Array> names = v8::Array::New(0);
    int index = 0;
    for (int i = 0; i < meta->enumeratorCount(); ++i) {
        QMetaEnum e = meta->enumerator(i);
        for (int j = 0; j < e.keyCount(); ++j)
            names->Set(index++, v8::String::New(e.key(j)));
    }

    return handleScope.Close(names);
}

// This callback is called when the QMetaObject is invoked
v8::Handle<v8::Value> QtMetaObjectCallback(const v8::Arguments& args)
{
    v8::Local<v8::Object> self = args.Holder();
    QtMetaObjectData *data = QtMetaObjectData::get(self);
    QScriptEnginePrivate *engine = data->engine();
    QScriptContextPrivate context(engine, &args);

//    const QMetaObject *meta = data->metaObject();
//    qDebug() << Q_FUNC_INFO << meta->className();
//     if (!args.IsConstructCall())
//         return v8::Handle<v8::Value>();
//

    v8::Handle<v8::Value> ctor = data->constructor();
    if (ctor->IsFunction()) {
        QVarLengthArray<v8::Handle<v8::Value>, 8> newArgs;
        newArgs.reserve(args.Length());
        for (int i = 0; i < args.Length(); i++) {
            newArgs.append(args[i]);
        }
        return v8::Function::Cast(*ctor)->NewInstance(newArgs.count(), newArgs.data());
    }
    return v8::Handle<v8::Value>();
}

// This callback implements the connect() method of signal wrapper objects.
// The this-object is a QtSignal wrapper.
// If the connect succeeds, this function returns undefined; otherwise,
// an error is thrown.
// If the connect succeeds, the associated JS function will be invoked
// when the C++ object emits the signal.
v8::Handle<v8::Value> QScriptSignalData::QtConnectCallback(const v8::Arguments& args)
{
    v8::HandleScope handleScope;
    v8::Local<v8::Object> self = args.Holder();
    QScriptSignalData *data = QScriptSignalData::get(self);

    if (args.Length() == 0)
        return v8::ThrowException(v8::Exception::SyntaxError(v8::String::New("QtSignal.connect(): no arguments given")));

    if (data->resolveMode() == QScriptSignalData::ResolvedByName) {
        // ### Check if the signal is overloaded. If it is, throw an error,
        // since it's not possible to know which of the overloads we should connect to.
        // Can probably figure this out at class/instance construction time
    }

    v8::Handle<v8::Object> receiver;
    v8::Handle<v8::Object> slot;
    if (args.Length() == 1) {
        //simple function
        if (!args[0]->IsObject())
            return handleScope.Close(v8::ThrowException(v8::Exception::TypeError(v8::String::New("QtSignal.connect(): argument is not a function"))));
        slot = v8::Handle<v8::Object>(v8::Object::Cast(*args[0]));
    } else {
        receiver = v8::Handle<v8::Object>(v8::Object::Cast(*args[0]));
        v8::Local<v8::Value> arg1 = args[1];
        if (arg1->IsObject() && !arg1->IsString()) {
            slot = v8::Handle<v8::Object>(v8::Object::Cast(*arg1));
        } else if (!receiver.IsEmpty() && arg1->IsString()) {
            v8::Local<v8::String> propertyName = arg1->ToString();
            slot = v8::Handle<v8::Object>(v8::Object::Cast(*receiver->Get(propertyName)));
        }
    }

    if (slot.IsEmpty() || !slot->IsCallable()) {
        return handleScope.Close(v8::ThrowException(v8::Exception::TypeError(v8::String::New("QtSignal.connect(): target is not a function"))));
    }

    // Options:
    // 1) Connection manager per C++ object.
    // 2) Connection manager per Qt wrapper object.
    // 3) Connection manager per signal wrapper object.
    // 4) Connection object per connection.

    // disconnect() needs to be able to go introspect connections
    // of that signal only, for that wrapper only.

    // qDebug() << "connect" << data->object() << data->index();
    return handleScope.Close(data->connect(receiver, slot));
}

// This callback implements the disconnect() method of signal wrapper objects.
// The this-object is a QtSignal wrapper.
// If the disconnect succeeds, this function returns undefined; otherwise,
// an error is thrown.
v8::Handle<v8::Value> QScriptSignalData::QtDisconnectCallback(const v8::Arguments& args)
{
    v8::Local<v8::Object> self = args.Holder();
    QScriptSignalData *data = QScriptSignalData::get(self);

    if (args.Length() == 0)
        return v8::ThrowException(v8::Exception::SyntaxError(v8::String::New("QtSignal.disconnect(): no arguments given")));

    // ### Should be able to take any [[Callable]], but there is no v8 API for that.
    if (!args[0]->IsFunction())
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New("QtSignal.disconnect(): argument is not a function")));

    return data->disconnect(v8::Handle<v8::Function>(v8::Function::Cast(*args[0])));
}

static v8::Handle<v8::Value> QtGetMetaMethod(v8::Local<v8::String> property, const v8::AccessorInfo& info)
{
    v8::Local<v8::Object> self = info.This();

    v8::Local<v8::Value> cached = self->GetHiddenValue(property);
    if (!cached.IsEmpty())
        return cached;

    v8::Local<v8::Array> dataArray = v8::Array::Cast(*info.Data());
    QScriptEnginePrivate *engine = static_cast<QScriptEnginePrivate *>(v8::External::Unwrap(dataArray->Get(0)));
    if (!engine->isQtObject(self)) {
        //If we are not called on a QObject (ie, the prototype is used in another object, we cannot do anything
        return v8::Handle<v8::Value>();
    }
    QScriptQObjectData *instance = QScriptQObjectData::get(self);
    Q_ASSERT(engine == instance->engine());
    //QScriptContextPrivate context(engine, &info);

    v8::Local<v8::Value> error;
    QObject *qobject = instance->cppObject(&error);
    if (!qobject)
        return error;

    const QMetaObject *meta = qobject->metaObject();
    QScriptMetaMethodInfo mmInfo;
    mmInfo.intData = v8::Uint32::Cast(*dataArray->Get(1))->Value();
    const QMetaMethod &method = meta->method(mmInfo.index);

    v8::Handle<v8::Object> result;
    if (method.methodType() == QMetaMethod::Signal) {
        QScriptSignalData *data = new QScriptSignalData(engine, self, mmInfo);
        result = QScriptSignalData::createInstance(data);
    } else {
        QScriptMetaMethodData *data = new QScriptMetaMethodData(engine, self, mmInfo);
        result = QScriptMetaMethodData::createInstance(data);
    }

    self->SetHiddenValue(property, result);
    //make a reference so QScriptGenericMetaMethodData::m_object is not garbage collected too early
    result->SetInternalField(1, self);
    return result;
}

static v8::Handle<v8::Value> findChildCallback(const v8::Arguments& args)
{
    v8::HandleScope handleScope;
    QScriptEnginePrivate *engine = reinterpret_cast<QScriptEnginePrivate *>(v8::External::Unwrap(args.Data()));
    Q_ASSERT(engine);
    v8::Local<v8::Object> self = args.This();
    if (!engine->qobjectTemplate()->HasInstance(self))
        return v8::Handle<v8::Value>(); //the QObject prototype is being used on another object.
        QScriptQObjectData *data = QScriptQObjectData::get(self);
    Q_ASSERT(engine == data->engine());
    QScriptContextPrivate context(engine, &args);

    v8::Local<v8::Value> error;
    QObject *qobject = data->cppObject(&error);
    if (!qobject)
        return error;

    QString name;
    if (args.Length() != 0)
        name = QScriptConverter::toString(args[0]->ToString());
    QObject *child = qobject->findChild<QObject *>(name);
    QScriptEngine::QObjectWrapOptions opt = QScriptEngine::PreferExistingWrapperObject;
    return handleScope.Close(engine->newQObject(child, QScriptEngine::QtOwnership, opt));
}

static v8::Handle<v8::Value> findChildrenWithRegExp(QScriptEnginePrivate *engine, QObject *qobject, v8::Handle<v8::Object> regExp)
{
    const QList<QObject *> children = qobject->findChildren<QObject *>();
    v8::Handle<v8::Object> testFunction = v8::Handle<v8::Object>::Cast(regExp->Get(v8::String::New("test")));

    v8::Local<v8::Array> result = v8::Array::New(children.length());
    v8::Handle<v8::Value> argv[1];

    for (int i = 0, resultIndex = 0; i < children.length(); i++) {
        argv[0] = QScriptConverter::toString(children.at(i)->objectName());
        if (testFunction->Call(regExp, 1, argv)->IsTrue()) {
            result->Set(resultIndex, engine->newQObject(children.at(i), QScriptEngine::QtOwnership,
                                                          QScriptEngine::PreferExistingWrapperObject));
            resultIndex++;
        }
    }

    return result;
}

static v8::Handle<v8::Value> findChildrenCallback(const v8::Arguments& args)
{
    v8::HandleScope handleScope;
    QScriptEnginePrivate *engine = reinterpret_cast<QScriptEnginePrivate *>(v8::External::Unwrap(args.Data()));
    Q_ASSERT(engine);
    v8::Local<v8::Object> self = args.This();
    if (!engine->qobjectTemplate()->HasInstance(self))
        return v8::Handle<v8::Value>(); //the QObject prototype is being used on another object.
        QScriptQObjectData *data = QScriptQObjectData::get(self);
    Q_ASSERT(engine == data->engine());
    QScriptContextPrivate context(engine, &args);

    v8::Local<v8::Value> error;
    QObject *qobject = data->cppObject(&error);
    if (!qobject)
        return error;

    QString name;
    if (args.Length() != 0) {
        if (args[0]->IsRegExp()) {
            v8::Handle<v8::Object> regExp = v8::Handle<v8::Object>::Cast(args[0]);
            return handleScope.Close(findChildrenWithRegExp(engine, qobject, regExp));
        }
        name = QScriptConverter::toString(args[0]->ToString());
    }
    const QList<QObject *> children = qobject->findChildren<QObject *>(name);
    v8::Local<v8::Array> array = v8::Array::New(children.length());
    const QScriptEngine::QObjectWrapOptions opt = QScriptEngine::PreferExistingWrapperObject;
    for (int i = 0; i < children.length(); i++) {
        array->Set(i , engine->newQObject(children.at(i), QScriptEngine::QtOwnership, opt));
    }
    return handleScope.Close(array);
}

v8::Handle<v8::FunctionTemplate> createQtClassTemplate(QScriptEnginePrivate *engine, const QMetaObject *mo, const QScriptEngine::QObjectWrapOptions &options)
{
    v8::HandleScope handleScope;
    v8::Handle<v8::FunctionTemplate> funcTempl = v8::FunctionTemplate::New();
    funcTempl->SetClassName(v8::String::New(mo->className()));

    // Make the template inherit the super class's template.
    // This is different from the old back-end, where every wrapped
    // object exposed everything as own properties.
    const QMetaObject *superMo = mo->superClass();
    if (superMo && !(options & QScriptEngine::ExcludeSuperClassContents))
        funcTempl->Inherit(engine->qtClassTemplate(superMo, options));
    else
        funcTempl->Inherit(engine->qobjectTemplate());

    v8::Handle<v8::ObjectTemplate> instTempl = funcTempl->InstanceTemplate();
    // Internal field is used to hold QScriptQObjectData*.
    instTempl->SetInternalFieldCount(1);

    // Figure out method names (own and inherited).
    QHash<QByteArray, QList<int> > ownMethodNameToIndexes;
    QHash<QByteArray, QList<int> > methodNameToIndexes;
    int methodOffset = mo->methodOffset();
    if ((options & QScriptEngine::ExcludeSuperClassContents) == QScriptEngine::ExcludeSuperClassProperties)
        methodOffset = 0;  //if we excluded the superclass for the properties, we need to include the superclass methods
    for (int i = 0; i < mo->methodCount(); ++i) {
        QMetaMethod method = mo->method(i);
        // Ignore private methods.
        if (method.access() == QMetaMethod::Private)
            continue;

        if (options & QScriptEngine::ExcludeSlots && method.methodType() == QMetaMethod::Slot)
            continue;

        if (i == 2 && options & QScriptEngine::ExcludeDeleteLater) {
            Q_ASSERT(!qstrcmp(method.signature(), "deleteLater()"));
            continue;
        }

        QByteArray signature = method.signature();
        QByteArray name = signature.left(signature.indexOf('('));
        if (i >= methodOffset)
            ownMethodNameToIndexes[name].append(i);
        methodNameToIndexes[name].append(i);
    }

    // Add accessors for own meta-methods.
    {
        v8::PropertyAttribute methodAttributes = (options & QScriptEngine::SkipMethodsInEnumeration) ? v8::DontEnum : v8::None ;
        QHash<QByteArray, QList<int> >::const_iterator it;
        v8::Handle<v8::Signature> sig = v8::Signature::New(funcTempl);
        for (it = ownMethodNameToIndexes.constBegin(); it != ownMethodNameToIndexes.constEnd(); ++it) {
            QByteArray name = it.key();
            QList<int> indexes = it.value();

            // Choose appropriate callback based on return type and parameter types.
            QMetaMethod method;
            foreach(int methodIndex, indexes) {
                method = mo->method(methodIndex);
                QScriptMetaMethodInfo info;
                info.index = methodIndex;
                info.voidvoid = !method.typeName()[0] && method.parameterTypes().isEmpty();
                info.resolveMode = 1;
                v8::Local<v8::Array> dataArray = v8::Array::New(2);
                dataArray->Set(0, v8::External::Wrap(engine));
                dataArray->Set(1, v8::Uint32::New(info.intData));
                instTempl->SetAccessor(v8::String::New(method.signature()), QtGetMetaMethod, 0, dataArray, v8::DEFAULT, methodAttributes);
            }
            QScriptMetaMethodInfo info;
            info.index = indexes.last(); // The largest index by that name.
            info.voidvoid = !method.typeName()[0] && method.parameterTypes().isEmpty();
            info.overloaded = methodNameToIndexes[name].size() > 1;
            v8::Local<v8::Array> dataArray = v8::Array::New(2);
            dataArray->Set(0, v8::External::Wrap(engine));
            dataArray->Set(1, v8::Uint32::New(info.intData));
            instTempl->SetAccessor(v8::String::New(name), QtGetMetaMethod, 0, dataArray, v8::DEFAULT, v8::DontEnum);

        }
    }

    // Add accessors for meta-properties.
    int propertyOffset = mo->propertyOffset();
    if ((options & QScriptEngine::ExcludeSuperClassContents) == QScriptEngine::ExcludeSuperClassMethods)
        propertyOffset = 0;  //if we excluded the superclass for the methods, we need to include the superclass properties
    for (int i = propertyOffset; i < mo->propertyCount(); ++i) {
        QMetaProperty prop = mo->property(i);
        if (!prop.isScriptable())
            continue;
        // Choose suitable callbacks for type.
        v8::AccessorGetter getter;
        v8::AccessorSetter setter;
#if 0
        if (prop.userType() == QMetaType::QString) {
            getter = QtMetaPropertyFastGetter<StringType>;
            setter = QtMetaPropertyFastSetter<StringType>;
        } else
#endif
        {
            getter = QtMetaPropertyGetter;
            setter = QtMetaPropertySetter;
        }

        v8::PropertyAttribute attribute = v8::DontDelete;
        if (!prop.isWritable()) {
            setter = 0;
            attribute = v8::PropertyAttribute(v8::DontDelete | v8::ReadOnly);
        }
        instTempl->SetAccessor(v8::String::New(prop.name()),
                               getter, setter, v8::Int32::New(i),
                               v8::DEFAULT, attribute);
    }

    instTempl->SetNamedPropertyHandler(QtLazyPropertyGetter,
                                        QtLazyPropertySetter,
                                        0, 0, 0,
                                        /*data=*/v8::External::Wrap(engine));

    return handleScope.Close(funcTempl);
}

// Returns the template for the almighty QObject class.
v8::Handle<v8::FunctionTemplate> QScriptEnginePrivate::qobjectTemplate()
{
    if (m_qobjectBaseTemplate.IsEmpty()) {
        v8::HandleScope handleScope;
        v8::Handle<v8::FunctionTemplate> funcTempl = v8::FunctionTemplate::New();
        funcTempl->SetClassName(v8::String::New("QObject"));

        v8::Handle<v8::ObjectTemplate> protoTempl = funcTempl->PrototypeTemplate();
        v8::Local<v8::Value> wEngine = v8::External::Wrap(this);
        protoTempl->Set(v8::String::New("findChild"), v8::FunctionTemplate::New(findChildCallback, wEngine)->GetFunction(), v8::DontEnum);
        protoTempl->Set(v8::String::New("findChildren"), v8::FunctionTemplate::New(findChildrenCallback, wEngine)->GetFunction(), v8::DontEnum);

        // Install QObject interceptor.
        // This interceptor will only get called if the access is not handled by the instance
        // itself nor other objects in the prototype chain.
        // The interceptor handles access to signals, dynamic properties and child objects.
        // The reason for putting the interceptor near the "back" of the prototype chain
        // is to avoid "slow" interceptor calls in what should be the common cases (accessing
        // meta-object-defined properties, and Q_INVOKABLE methods and slots).
        protoTempl->SetNamedPropertyHandler(QtLazyPropertyGetter,
                                            QtLazyPropertySetter,
                                            0, 0, 0,
                                            /*data=*/wEngine);

        m_qobjectBaseTemplate = v8::Persistent<v8::FunctionTemplate>::New(funcTempl);
    }
    return m_qobjectBaseTemplate;
}


v8::Handle<v8::Object> QScriptEnginePrivate::newQMetaObject(const QMetaObject *mo, const QScriptValue &ctor)
{
    v8::Handle<v8::FunctionTemplate> templ = metaObjectTemplate();
    Q_ASSERT(!templ.IsEmpty());
    v8::Handle<v8::ObjectTemplate> instanceTempl = templ->InstanceTemplate();
    Q_ASSERT(!instanceTempl.IsEmpty());
    v8::Handle<v8::Object> instance = instanceTempl->NewInstance();

    Q_ASSERT(instance->InternalFieldCount() == 1);

    // FIXME I'm not sure about that logic at least it is better than before as we are checking for an empty handle
    QScriptValuePrivate *tmp = QScriptValuePrivate::get(ctor);
    v8::Handle<v8::Value> ctorHandle;
    if (tmp->isObject())
        ctorHandle = *tmp;
    else
        ctorHandle = v8::Null();
    // FIXME that is likely to leak
    QtMetaObjectData *data = new QtMetaObjectData(this, mo, ctorHandle);
    instance->SetPointerInInternalField(0, data);

    v8::Persistent<v8::Object> persistent = v8::Persistent<v8::Object>::New(instance);
    persistent.MakeWeak(data, QScriptV8ObjectWrapperHelper::weakCallback<QtMetaObjectData>);
    return instance;
}

QT_END_NAMESPACE
