/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qmlbindingoptimizations_p.h"

#include "qmlcontext_p.h"
#include <QtDeclarative/qmlinfo.h>
#include "qmlbindingvme_p.h"

QT_BEGIN_NAMESPACE

int QmlOptimizedBindings::methodCount = -1;

QmlOptimizedBindings::QmlOptimizedBindings(const char *program, QmlContext *context)
: m_program(program)
{
    if (methodCount == -1)
        methodCount = QmlOptimizedBindings::staticMetaObject.methodCount();

    m_config.target = this;
    m_config.targetSlot = metaObject()->methodCount();

    quint32 bindings = 0;
    QmlBindingVME::init(m_program, &m_config, &m_signalTable, &bindings);

    m_bindings = new Binding[bindings];

    QmlAbstractExpression::setContext(context);
}

QmlOptimizedBindings::~QmlOptimizedBindings()
{
    delete [] m_bindings;
}

QmlAbstractBinding *QmlOptimizedBindings::configBinding(int index, QObject *target, 
                                                        QObject *scope, int property)
{
    Binding *rv = m_bindings + index;

    rv->index = index;
    rv->property = property;
    rv->target = target;
    rv->scope = scope;
    rv->parent = this;

    addref(); // This is decremented in Binding::destroy()

    return rv;
}

void QmlOptimizedBindings::Binding::setEnabled(bool e, QmlMetaProperty::WriteFlags flags)
{
    if (e) {
        addToObject(target);
        update(flags);
    } else {
        removeFromObject();
    }

    QmlAbstractBinding::setEnabled(e, flags);

    if (enabled != e) {
        enabled = e;

        if (e) update(flags);
    }
}

int QmlOptimizedBindings::Binding::propertyIndex()
{
    return property & 0xFFFF;
}

void QmlOptimizedBindings::Binding::update(QmlMetaProperty::WriteFlags)
{
    parent->run(this);
}

void QmlOptimizedBindings::Binding::destroy()
{
    enabled = false;
    removeFromObject();
    parent->release();
}

int QmlOptimizedBindings::qt_metacall(QMetaObject::Call c, int id, void **)
{
    if (c == QMetaObject::InvokeMetaMethod && id >= methodCount) {
        id -= methodCount;

        quint32 *reeval = m_signalTable + m_signalTable[id];
        quint32 count = *reeval;
        ++reeval;
        for (quint32 ii = 0; ii < count; ++ii) {
            run(m_bindings + reeval[ii]);
        }
    }
    return -1;
}

void QmlOptimizedBindings::run(Binding *binding)
{
    if (!binding->enabled)
        return;
    if (binding->updating)
        qWarning("ERROR: Circular binding");

    QmlContext *context = QmlAbstractExpression::context();
    if (!context) {
        qWarning("QmlOptimizedBindings: Attempted to evaluate an expression in an invalid context");
        return;
    }
    QmlContextPrivate *cp = QmlContextPrivate::get(context);

    if (binding->property & 0xFFFF0000) {
        QmlEnginePrivate *ep = QmlEnginePrivate::get(cp->engine);

        QmlValueType *vt = ep->valueTypes[(binding->property >> 16) & 0xFF];
        Q_ASSERT(vt);
        vt->read(binding->target, binding->property & 0xFFFF);

        QObject *target = vt;
        QmlBindingVME::run(m_program, binding->index, &m_config, cp, binding,
                           binding->scope, target);

        vt->write(binding->target, binding->property & 0xFFFF, 
                  QmlMetaProperty::DontRemoveBinding);
    } else {
        QmlBindingVME::run(m_program, binding->index, &m_config, cp, binding,
                           binding->scope, binding->target);
    }
}

/*
    The QmlBinding_Id optimization handles expressions of the type:

        property: id

    where id is a local context id, and property is an object property.  
    Coercian between id and property must be checked outside the QmlBinding_Id -
    it assumes that they coerce successfully.

    The QmlBinding_Id class avoids any signal slot connections, through the
    special "bindings" linked list maintained in the 
    QmlContextPrivate::ContextGuard instance for each id object.
*/
QmlBinding_Id::QmlBinding_Id(QObject *object, int propertyIdx,
                             QmlContext *context, int id)
: m_prev(0), m_next(0), m_object(object), m_propertyIdx(propertyIdx), m_id(id)
{
    QmlAbstractExpression::setContext(context);
}

QmlBinding_Id::~QmlBinding_Id()
{
    removeFromContext();
}

void QmlBinding_Id::setEnabled(bool e, QmlMetaProperty::WriteFlags flags)
{
    if (e) {
        addToObject(m_object);
        update(flags);
    } else {
        removeFromObject();
    }

    QmlAbstractBinding::setEnabled(e, flags);
}

int QmlBinding_Id::propertyIndex()
{
    return m_propertyIdx;
}

void QmlBinding_Id::update(QmlMetaProperty::WriteFlags flags)
{
    QmlContextPrivate *ctxtPriv = 
        static_cast<QmlContextPrivate *>(QObjectPrivate::get(context()));

    if (ctxtPriv) {

        if (!m_prev) {
            m_next = ctxtPriv->idValues[m_id].bindings;
            if (m_next) m_next->m_prev = &m_next;

            m_prev = &ctxtPriv->idValues[m_id].bindings;
            ctxtPriv->idValues[m_id].bindings = this;
        }

        QObject *o = ctxtPriv->idValues[m_id].data();
        int status = -1;
        void *a[] = { &o, 0, &status, &flags };
        QMetaObject::metacall(m_object, QMetaObject::WriteProperty, 
                              m_propertyIdx, a);
    } 
}

void QmlBinding_Id::removeFromContext()
{
    if (m_prev) {
        *m_prev = m_next;
        if (m_next) m_next->m_prev = m_prev;
        m_next = 0;
        m_prev = 0;
    }
}

void QmlBinding_Id::reset()
{
    removeFromContext();

    QObject *o = 0;
    int status = -1;
    QmlMetaProperty::WriteFlags flags = QmlMetaProperty::DontRemoveBinding;
    void *a[] = { &o, 0, &status, &flags };
    QMetaObject::metacall(m_object, QMetaObject::WriteProperty, 
                          m_propertyIdx, a);
}

QT_END_NAMESPACE
