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

QT_END_NAMESPACE
