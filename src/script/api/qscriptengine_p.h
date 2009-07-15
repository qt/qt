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

#ifndef QSCRIPTENGINE_P_H
#define QSCRIPTENGINE_P_H

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

#include "private/qobject_p.h"

#ifndef QT_NO_SCRIPT

#include <QtCore/qhash.h>
#include <QtCore/qset.h>

#include "RefPtr.h"
#include "Structure.h"
#include "JSGlobalObject.h"
#include "JSValue.h"

QT_BEGIN_NAMESPACE

namespace JSC
{
    class ExecState;
    class JSCell;
    class JSGlobalObject;
    class UString;
}

namespace QScript
{
    class QObjectPrototype;
    class QMetaObjectPrototype;
    class QVariantPrototype;
#ifndef QT_NO_QOBJECT
    class QObjectData;
#endif
}

class QString;
class QStringList;
class QScriptContext;
class QScriptValue;
class QScriptValuePrivate;
class QScriptTypeInfo;
class QScriptEngineAgent;

class QScriptSyntaxCheckResult;

class QScriptEngine;
class QScriptEnginePrivate
#ifndef QT_NO_QOBJECT
    : public QObjectPrivate
#endif
{
    Q_DECLARE_PUBLIC(QScriptEngine)
public:
    QScriptEnginePrivate();
    virtual ~QScriptEnginePrivate();

    static QScriptEnginePrivate *get(QScriptEngine*);
    static QScriptEngine *get(QScriptEnginePrivate*);

    static bool convert(const QScriptValue &value,
                        int type, void *ptr,
                        QScriptEnginePrivate *eng);
    QScriptValue create(int type, const void *ptr);
    bool hasDemarshalFunction(int type) const;

    QScriptValue scriptValueFromJSCValue(JSC::JSValue value);
    JSC::JSValue scriptValueToJSCValue(const QScriptValue &value);
    void releaseJSCValue(JSC::JSValue value);

    QScriptValue scriptValueFromVariant(const QVariant &value);
    QVariant scriptValueToVariant(const QScriptValue &value, int targetType);

    JSC::JSValue jscValueFromVariant(const QVariant &value);
    QVariant jscValueToVariant(JSC::JSValue value, int targetType);

    QScriptValue arrayFromStringList(const QStringList &lst);
    static QStringList stringListFromArray(const QScriptValue &arr);

    QScriptValue arrayFromVariantList(const QVariantList &lst);
    static QVariantList variantListFromArray(const QScriptValue &arr);

    QScriptValue objectFromVariantMap(const QVariantMap &vmap);
    static QVariantMap variantMapFromObject(const QScriptValue &obj);

    JSC::JSValue defaultPrototype(int metaTypeId) const;
    void setDefaultPrototype(int metaTypeId, JSC::JSValue prototype);

    QScriptContext *contextForFrame(JSC::ExecState *frame);
    void releaseContextForFrame(JSC::ExecState *frame);

    bool isCollecting() const;

#ifndef QT_NO_QOBJECT
    JSC::JSValue newQObject(QObject *object,
        QScriptEngine::ValueOwnership ownership = QScriptEngine::QtOwnership,
        const QScriptEngine:: QObjectWrapOptions &options = 0);
    JSC::JSValue newQMetaObject(const QMetaObject *metaObject,
                                JSC::JSValue ctor);

    static QScriptSyntaxCheckResult checkSyntax(const QString &program);
    static bool canEvaluate(const QString &program);
    static bool convertToNativeQObject(const QScriptValue &value,
                                       const QByteArray &targetType,
                                       void **result);

    QScript::QObjectData *qobjectData(QObject *object);
    void disposeQObject(QObject *object);
    void emitSignalHandlerException();

    bool scriptConnect(QObject *sender, const char *signal,
                       JSC::JSValue receiver, JSC::JSValue function);
    bool scriptDisconnect(QObject *sender, const char *signal,
                          JSC::JSValue receiver, JSC::JSValue function);

    bool scriptConnect(QObject *sender, int index,
                       JSC::JSValue receiver, JSC::JSValue function,
                       JSC::JSValue senderWrapper = 0);
    bool scriptDisconnect(QObject *sender, int index,
                          JSC::JSValue receiver, JSC::JSValue function);

    bool scriptConnect(JSC::JSValue signal, JSC::JSValue receiver,
                       JSC::JSValue function);
    bool scriptDisconnect(JSC::JSValue signal, JSC::JSValue receiver,
                          JSC::JSValue function);

    void registerScriptValue(QScriptValuePrivate *value);
    void unregisterScriptValue(QScriptValuePrivate *value)
    {
        attachedScriptValues.remove(value);
    }
    void detachAllRegisteredScriptValues();

    // private slots
    void _q_objectDestroyed(QObject *);
#endif

    JSC::JSGlobalObject *globalObject;
    JSC::JSObject *customGlobalObject;
    JSC::ExecState *currentFrame;
    QHash<JSC::ExecState*, QScriptContext*> contextForFrameHash;
    JSC::JSValue uncaughtException;

    WTF::RefPtr<JSC::Structure> scriptObjectStructure;

    QScript::QObjectPrototype *qobjectPrototype;
    WTF::RefPtr<JSC::Structure> qobjectWrapperObjectStructure;

    QScript::QMetaObjectPrototype *qmetaobjectPrototype;
    WTF::RefPtr<JSC::Structure> qmetaobjectWrapperObjectStructure;

    QScript::QVariantPrototype *variantPrototype;
    WTF::RefPtr<JSC::Structure> variantWrapperObjectStructure;

    QScriptEngineAgent *agent;
    QHash<JSC::JSCell*, QBasicAtomicInt> keepAliveValues;
    QHash<int, QScriptTypeInfo*> m_typeInfos;
    int processEventsInterval;

    QSet<QString> importedExtensions;
    QSet<QString> extensionsBeingImported;

    QSet<QScriptValuePrivate*> attachedScriptValues;  //keep trace to all QScriptValue evalueted

    QAtomicInt idGenerator;  //generate id for QScriptValue

#ifndef QT_NO_QOBJECT
    QHash<QObject*, QScript::QObjectData*> m_qobjectData;
#endif

#ifdef QT_NO_QOBJECT
    QScriptEngine *q_ptr;
#endif
};

namespace QScript
{

class GlobalObject : public JSC::JSGlobalObject
{
public:
    GlobalObject(QScriptEnginePrivate*);
    virtual ~GlobalObject();
    virtual JSC::UString className() const { return "global"; }
    virtual void mark();
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

public:
    QScriptEnginePrivate *engine;
};

} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
