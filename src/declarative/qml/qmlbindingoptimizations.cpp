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

int QmlBinding_Basic::reevalIndex = -1;

QmlBinding_Basic::QmlBinding_Basic(QObject *target, int property,
                                   const char *data, QmlRefCount *ref,
                                   QObject *scope, QmlContext *context)
: m_enabled(false), m_updating(false), m_scope(scope), m_target(target), 
  m_property(property), m_data(data)
{
    if (reevalIndex == -1) 
        reevalIndex = QmlBinding_Basic::staticMetaObject.indexOfSlot("reeval()");

    m_config.target = this;
    m_config.targetSlot = reevalIndex;
    m_scope2 = m_scope;

    QmlAbstractExpression::setContext(context);
}

QmlBinding_Basic::~QmlBinding_Basic()
{
}

void QmlBinding_Basic::setEnabled(bool e, QmlMetaProperty::WriteFlags flags)
{
    if (e) {
        addToObject(m_target);
        update(flags);
    } else {
        removeFromObject();
    }

    QmlAbstractBinding::setEnabled(e, flags);

    if (m_enabled != e) {
        m_enabled = e;

        if (e) update(flags);
    }
}

int QmlBinding_Basic::propertyIndex()
{
    return m_property & 0xFFFF;
}

void QmlBinding_Basic::update(QmlMetaProperty::WriteFlags flags)
{
    if (!m_enabled)
        return;

    if (m_updating) {
        qmlInfo(m_target) << tr("Binding loop detected");
        return;
    }

    QmlContext *context = QmlAbstractExpression::context();
    if (!context)
        return;
    QmlContextPrivate *cp = QmlContextPrivate::get(context);

    m_updating = true;

    if (m_property & 0xFFFF0000) {
        QmlEnginePrivate *ep = QmlEnginePrivate::get(cp->engine);

        QmlValueType *vt = ep->valueTypes[(m_property >> 16) & 0xFF];
        Q_ASSERT(vt);
        vt->read(m_target, m_property & 0xFFFF);

        QObject *target = vt;
        QmlBindingVME::run(m_data, &m_config, cp, &m_scope, &target);

        vt->write(m_target, m_property & 0xFFFF, flags);
    } else {
        QmlBindingVME::run(m_data, &m_config, cp, &m_scope, &m_target);
    }

    m_updating = false;
}

void QmlBinding_Basic::reeval()
{
    update(QmlMetaProperty::DontRemoveBinding);
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

/*
    The QmlBinding_ObjectProperty optimization handles expressions of the type:

        property: objectProperty

    where both property and objectProperty are object properties on the target
    object.  Coercian between the two must be checked outside the 
    QmlBinding_ObjectProperty - it assumes that they coerce successfully.

    Due to dot properties, property does not have to be on the same object as 
    objectProperty.  For example:

        anchors.fill: parent
*/
QmlBinding_ObjProperty::QmlBinding_ObjProperty(QObject *object, int propertyIdx,
                                               QObject *context, int contextIdx,
                                               int notifyIdx)
: m_enabled(false), m_object(object), m_propertyIdx(propertyIdx), 
  m_context(context), m_contextIdx(contextIdx), m_notifyIdx(notifyIdx)
{
}

void QmlBinding_ObjProperty::setEnabled(bool e, QmlMetaProperty::WriteFlags flags)
{
    m_enabled = e;
    if (e) {
        addToObject(m_object);
        update(flags);
    } else {
        removeFromObject();
    }

    QmlAbstractBinding::setEnabled(e, flags);
}

int QmlBinding_ObjProperty::propertyIndex()
{
    return m_propertyIdx;
}

void QmlBinding_ObjProperty::update(QmlMetaProperty::WriteFlags flags)
{
    if (!m_enabled)
        return;

    QObject *value = 0;
    int status = -1;
    void *ra[] = { &value, 0, &status };

    // Read
    QMetaObject::metacall(m_context, QMetaObject::ReadProperty, 
                          m_contextIdx, ra);

    void *wa[] = { &value, 0, &status, &flags };

    // Write
    QMetaObject::metacall(m_object, QMetaObject::WriteProperty, 
                          m_propertyIdx, wa);

    // Connect notify if needed.  Only need to connect once, so we set
    // m_notifyIdx back to -1 afterwards
    static int slotIdx = -1;
    if (m_notifyIdx != -1) {
        if (slotIdx == -1) 
            slotIdx = QmlBinding_ObjProperty::staticMetaObject.indexOfMethod("update()");

        QMetaObject::connect(m_context, m_notifyIdx, this, slotIdx);
        m_notifyIdx = -1;
    }
}

QT_END_NAMESPACE
