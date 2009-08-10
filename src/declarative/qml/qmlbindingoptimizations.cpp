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


QmlBindingIdOptimization::QmlBindingIdOptimization(QObject *object, 
                                                   int propertyIdx,
                                                   QmlContext *context, 
                                                   int id)
: m_prev(0), m_next(0), m_object(object), m_propertyIdx(propertyIdx), m_id(id)
{
    QmlAbstractExpression::setContext(context);
}

void QmlBindingIdOptimization::setEnabled(bool e)
{
    if (e) {
        addToObject(m_object);
        update();
    } else {
        removeFromObject();
    }
}

int QmlBindingIdOptimization::propertyIndex()
{
    return m_propertyIdx;
}

void QmlBindingIdOptimization::update()
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

void QmlBindingIdOptimization::reset()
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

QT_END_NAMESPACE
