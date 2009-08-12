/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmlbindingoptimizations_p.h"
#include <private/qmlcontext_p.h>

QT_BEGIN_NAMESPACE

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

void QmlBinding_Id::setEnabled(bool e)
{
    if (e) {
        addToObject(m_object);
        update();
    } else {
        removeFromObject();
    }

    QmlAbstractBinding::setEnabled(e);
}

int QmlBinding_Id::propertyIndex()
{
    return m_propertyIdx;
}

void QmlBinding_Id::update()
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
        void *a[] = { &o, 0 };
        QMetaObject::metacall(m_object, QMetaObject::WriteProperty, 
                              m_propertyIdx, a);
    } 
}

void QmlBinding_Id::reset()
{
    if (m_prev) {
        *m_prev = m_next;
        if (m_next) m_next->m_prev = m_prev;
        m_next = 0;
        m_prev = 0;
    }

    QObject *o = 0;
    void *a[] = { &o, 0 };
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

void QmlBinding_ObjProperty::setEnabled(bool e)
{
    m_enabled = e;
    if (e) {
        addToObject(m_object);
        update();
    } else {
        removeFromObject();
    }

    QmlAbstractBinding::setEnabled(e);
}

int QmlBinding_ObjProperty::propertyIndex()
{
    return m_propertyIdx;
}

void QmlBinding_ObjProperty::update()
{
    if (!m_enabled)
        return;

    QObject *value = 0;
    void *a[] = { &value, 0 };

    // Read
    QMetaObject::metacall(m_context, QMetaObject::ReadProperty, 
                          m_contextIdx, a);

    // Write
    QMetaObject::metacall(m_object, QMetaObject::WriteProperty, 
                          m_propertyIdx, a);

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
