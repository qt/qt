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

#ifndef QSCRIPTQOBJECT_P_H
#define QSCRIPTQOBJECT_P_H

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

#include <QtCore/qobject.h>

#ifndef QT_NO_SCRIPT

#include "qscriptengine.h"
#include <QtCore/qpointer.h>

#include "JSObject.h"
#include "InternalFunction.h"

QT_BEGIN_NAMESPACE

namespace QScript
{

enum AttributeExtension {
    // ### Make sure there's no conflict with JSC::Attribute
    QObjectMemberAttribute = 1 << 12
};

class QObjectWrapperObject : public JSC::JSObject
{
public:
    // work around CELL_SIZE limitation
    struct Data
    {
        QPointer<QObject> value;
        QScriptEngine::ValueOwnership ownership;
        QScriptEngine::QObjectWrapOptions options;

        QHash<QByteArray, JSC::JSValue> cachedMembers;

        Data(QObject *o, QScriptEngine::ValueOwnership own,
             QScriptEngine::QObjectWrapOptions opt)
            : value(o), ownership(own), options(opt) {}
    };

    explicit QObjectWrapperObject(
        QObject *object, QScriptEngine::ValueOwnership ownership,
        const QScriptEngine::QObjectWrapOptions &options,
        WTF::PassRefPtr<JSC::Structure> sid);
    ~QObjectWrapperObject();
    
    virtual bool getOwnPropertySlot(JSC::ExecState*,
                                    const JSC::Identifier& propertyName,
                                    JSC::PropertySlot&);
    virtual void put(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                     JSC::JSValue, JSC::PutPropertySlot&);
    virtual bool deleteProperty(JSC::ExecState*,
                                const JSC::Identifier& propertyName);
    virtual bool getPropertyAttributes(JSC::ExecState*, const JSC::Identifier&,
                                       unsigned&) const;
    virtual void getPropertyNames(JSC::ExecState*, JSC::PropertyNameArray&);
    virtual void mark();

    virtual const JSC::ClassInfo* classInfo() const { return &info; }
    static const JSC::ClassInfo info;

    inline QObject *value() const { return data->value; }
    inline void setValue(QObject* value) { data->value = value; }

    inline QScriptEngine::ValueOwnership ownership() const
        { return data->ownership; }
    inline void setOwnership(QScriptEngine::ValueOwnership ownership)
        { data->ownership = ownership; }

    inline QScriptEngine::QObjectWrapOptions options() const
        { return data->options; }
    inline void setOptions(QScriptEngine::QObjectWrapOptions options)
        { data->options = options; }

    static WTF::PassRefPtr<JSC::Structure> createStructure(JSC::JSValue prototype)
    {
        return JSC::Structure::create(prototype, JSC::TypeInfo(JSC::ObjectType));
    }

protected:
    Data *data;
};

class QObjectPrototype : public QObjectWrapperObject
{
public:
    QObjectPrototype(JSC::ExecState*, WTF::PassRefPtr<JSC::Structure>,
                     JSC::Structure* prototypeFunctionStructure);
};

class QObjectConnectionManager;

class QObjectData // : public QObjectUserData
{
public:
    QObjectData(QScriptEnginePrivate *engine);
    ~QObjectData();

    bool addSignalHandler(QObject *sender,
                          int signalIndex,
                          JSC::JSValue receiver,
                          JSC::JSValue slot,
                          JSC::JSValue senderWrapper = 0);
    bool removeSignalHandler(QObject *sender,
                             int signalIndex,
                             JSC::JSValue receiver,
                             JSC::JSValue slot);

    void mark();

private:
    QScriptEnginePrivate *engine;
    QScript::QObjectConnectionManager *connectionManager;
//    QList<QScriptQObjectWrapperInfo> wrappers;
};

class QtFunction: public JSC::InternalFunction
{
public:
    // work around CELL_SIZE limitation
    struct Data
    {
        JSC::JSValue object;
        int initialIndex;
        bool maybeOverloaded;

        Data(JSC::JSValue o, int ii, bool mo)
            : object(o), initialIndex(ii), maybeOverloaded(mo) {}
    };

    QtFunction(JSC::JSValue object, int initialIndex, bool maybeOverloaded,
               JSC::JSGlobalData*, WTF::PassRefPtr<JSC::Structure>, const JSC::Identifier&);
    virtual ~QtFunction();

    virtual JSC::CallType getCallData(JSC::CallData&);
    virtual void mark();

    virtual const JSC::ClassInfo* classInfo() const { return &info; }
    static const JSC::ClassInfo info;

    JSC::JSValue call(JSC::ExecState *exec, JSC::JSValue thisValue,
                       const JSC::ArgList &args);

    QObjectWrapperObject *wrapperObject() const;
    QObject *qobject() const;
    const QMetaObject *metaObject() const;
    int initialIndex() const;
    bool maybeOverloaded() const;
    int mostGeneralMethod(QMetaMethod *out = 0) const;
    QList<int> overloadedIndexes() const;
    QString functionName() const;

private:
    Data *data;
};

class QMetaObjectWrapperObject : public JSC::JSObject
{
public:
    // work around CELL_SIZE limitation
    struct Data
    {
        const QMetaObject *value;
        JSC::JSValue ctor;

        Data(const QMetaObject *mo, JSC::JSValue c)
            : value(mo), ctor(c) {}
    };

    explicit QMetaObjectWrapperObject(
        const QMetaObject *metaobject, JSC::JSValue ctor,
        WTF::PassRefPtr<JSC::Structure> sid);
    ~QMetaObjectWrapperObject();
    
    virtual bool getOwnPropertySlot(JSC::ExecState*,
                                    const JSC::Identifier& propertyName,
                                    JSC::PropertySlot&);
    virtual void put(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                     JSC::JSValue, JSC::PutPropertySlot&);
    virtual bool deleteProperty(JSC::ExecState*,
                                const JSC::Identifier& propertyName);
    virtual bool getPropertyAttributes(JSC::ExecState*, const JSC::Identifier&,
                                       unsigned&) const;
    virtual void getPropertyNames(JSC::ExecState*, JSC::PropertyNameArray&);

    virtual const JSC::ClassInfo* classInfo() const { return &info; }
    static const JSC::ClassInfo info;

    inline const QMetaObject *value() const { return data->value; }
    inline void setValue(const QMetaObject* value) { data->value = value; }

    static WTF::PassRefPtr<JSC::Structure> createStructure(JSC::JSValue prototype)
    {
        return JSC::Structure::create(prototype, JSC::TypeInfo(JSC::ObjectType));
    }

protected:
    Data *data;
};

class QMetaObjectPrototype : public QMetaObjectWrapperObject
{
public:
    QMetaObjectPrototype(JSC::ExecState*, WTF::PassRefPtr<JSC::Structure>,
                         JSC::Structure* prototypeFunctionStructure);
};

} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
