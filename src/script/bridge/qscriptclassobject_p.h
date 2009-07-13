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

#ifndef QSCRIPTCLASSOBJECT_P_H
#define QSCRIPTCLASSOBJECT_P_H

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

#include "qscriptobject_p.h"

QT_BEGIN_NAMESPACE

class QScriptClass;

namespace QScript
{

class ClassObjectDelegate : public QScriptObjectDelegate
{
public:
    ClassObjectDelegate(QScriptClass *scriptClass);
    ~ClassObjectDelegate();

    QScriptClass *scriptClass() const;
    void setScriptClass(QScriptClass *scriptClass);

    virtual Type type() const;

    virtual bool getOwnPropertySlot(QScriptObject*, JSC::ExecState*,
                                    const JSC::Identifier& propertyName,
                                    JSC::PropertySlot&);
    virtual void put(QScriptObject*, JSC::ExecState* exec,
                     const JSC::Identifier& propertyName,
                     JSC::JSValue, JSC::PutPropertySlot&);
    virtual bool deleteProperty(QScriptObject*, JSC::ExecState*,
                                const JSC::Identifier& propertyName);
    virtual bool getPropertyAttributes(const QScriptObject*, JSC::ExecState*,
                                       const JSC::Identifier&,
                                       unsigned&) const;
    virtual void getPropertyNames(QScriptObject*, JSC::ExecState*,
                                  JSC::PropertyNameArray&);

    virtual JSC::CallType getCallData(QScriptObject*, JSC::CallData&);
    static JSC::JSValue JSC_HOST_CALL call(JSC::ExecState*, JSC::JSObject*,
                                           JSC::JSValue, const JSC::ArgList&);
    virtual JSC::ConstructType getConstructData(QScriptObject*, JSC::ConstructData&);
    static JSC::JSObject* construct(JSC::ExecState*, JSC::JSObject*,
                                    const JSC::ArgList&);

    virtual bool hasInstance(QScriptObject*, JSC::ExecState*,
                             JSC::JSValue value, JSC::JSValue proto);

private:
    QScriptClass *m_scriptClass;
};

} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
