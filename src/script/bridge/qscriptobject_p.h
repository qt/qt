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

#ifndef QSCRIPTOBJECT_P_H
#define QSCRIPTOBJECT_P_H

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

#include <QtCore/qobjectdefs.h>

#ifndef QT_NO_SCRIPT

#include "JSObject.h"

QT_BEGIN_NAMESPACE

class QScriptObjectDelegate;

class QScriptObject : public JSC::JSObject
{
public:
     // work around CELL_SIZE limitation
    struct Data
    {
        JSC::JSValue data; // QScriptValue::data
        QScriptObjectDelegate *delegate;

        Data() : delegate(0) {}
        ~Data();
    };

    explicit QScriptObject(WTF::PassRefPtr<JSC::Structure> sid);
    virtual ~QScriptObject();
    
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
    virtual JSC::JSValue lookupGetter(JSC::ExecState*, const JSC::Identifier& propertyName);
    virtual JSC::JSValue lookupSetter(JSC::ExecState*, const JSC::Identifier& propertyName);
    virtual void mark();
    virtual JSC::CallType getCallData(JSC::CallData&);
    virtual JSC::ConstructType getConstructData(JSC::ConstructData&);
    virtual bool hasInstance(JSC::ExecState*, JSC::JSValue value, JSC::JSValue proto);

    virtual const JSC::ClassInfo* classInfo() const { return &info; }
    static const JSC::ClassInfo info;

    static WTF::PassRefPtr<JSC::Structure> createStructure(JSC::JSValue prototype)
    {
        return JSC::Structure::create(prototype, JSC::TypeInfo(JSC::ObjectType, JSC::ImplementsHasInstance));
    }

    JSC::JSValue data() const;
    void setData(JSC::JSValue data);

    QScriptObjectDelegate *delegate() const;
    void setDelegate(QScriptObjectDelegate *delegate);

protected:
    Data *d;
};

class QScriptObjectPrototype : public QScriptObject
{
public:
    QScriptObjectPrototype(JSC::ExecState*, WTF::PassRefPtr<JSC::Structure>,
                           JSC::Structure* prototypeFunctionStructure);
};

class QScriptObjectDelegate
{
public:
    enum Type {
        QtObject,
        Variant,
        ClassObject
    };

    QScriptObjectDelegate();
    virtual ~QScriptObjectDelegate();

    virtual Type type() const = 0;

    virtual bool getOwnPropertySlot(QScriptObject*, JSC::ExecState*,
                                    const JSC::Identifier& propertyName,
                                    JSC::PropertySlot&);
    virtual void put(QScriptObject*, JSC::ExecState* exec, const JSC::Identifier& propertyName,
                     JSC::JSValue, JSC::PutPropertySlot&);
    virtual bool deleteProperty(QScriptObject*, JSC::ExecState*,
                                const JSC::Identifier& propertyName);
    virtual bool getPropertyAttributes(const QScriptObject*, JSC::ExecState*,
                                       const JSC::Identifier&, unsigned&) const;
    virtual void getPropertyNames(QScriptObject*, JSC::ExecState*, JSC::PropertyNameArray&);
    virtual void mark(QScriptObject*);
    virtual JSC::CallType getCallData(QScriptObject*, JSC::CallData&);
    virtual JSC::ConstructType getConstructData(QScriptObject*, JSC::ConstructData&);
    virtual bool hasInstance(QScriptObject*, JSC::ExecState*,
                             JSC::JSValue value, JSC::JSValue proto);

private:
    Q_DISABLE_COPY(QScriptObjectDelegate)
};

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
