/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
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
                                const JSC::Identifier& propertyName,
                                bool checkDontDelete = true);
    virtual bool getPropertyAttributes(JSC::ExecState*, const JSC::Identifier&,
                                       unsigned&) const;
    virtual void getPropertyNames(JSC::ExecState*, JSC::PropertyNameArray&, unsigned listedAttributes = JSC::Structure::Prototype);
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
                                const JSC::Identifier& propertyName,
                                bool checkDontDelete = true);
    virtual bool getPropertyAttributes(const QScriptObject*, JSC::ExecState*,
                                       const JSC::Identifier&, unsigned&) const;
    virtual void getPropertyNames(QScriptObject*, JSC::ExecState*, JSC::PropertyNameArray&,
                                  unsigned listedAttributes = JSC::Structure::Prototype);
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
