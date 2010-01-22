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

#ifndef QMLBINDINGOPTIMIZATIONS_P_H
#define QMLBINDINGOPTIMIZATIONS_P_H

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

#include "qmlexpression_p.h"
#include "qmlbinding.h"
#include "qmlbasicscript_p.h"
#include "qmlbindingvme_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QmlOptimizedBindings : public QObject, public QmlAbstractExpression, public QmlRefCount
{
public:
    QmlOptimizedBindings(const char *program, QmlContext *context);
    virtual ~QmlOptimizedBindings();
    QmlAbstractBinding *configBinding(int index, QObject *target, QObject *scope, int property);

protected:
    int qt_metacall(QMetaObject::Call, int, void **);

private:
    struct Binding : public QmlAbstractBinding, public QmlDelayedError {
        Binding() : enabled(false), updating(0), property(0),
                    scope(0), target(0), parent(0) {}

        // Inherited from QmlAbstractBinding
        virtual void setEnabled(bool, QmlMetaProperty::WriteFlags flags);
        virtual int propertyIndex();
        virtual void update(QmlMetaProperty::WriteFlags flags);
        virtual void destroy();

        int index:30;
        bool enabled:1;
        bool updating:1;
        int property;
        QObject *scope;
        QObject *target;

        QmlOptimizedBindings *parent;
    };
    void run(Binding *);

    QmlBindingVME::Config m_config;
    const char *m_program;
    Binding *m_bindings;
    quint32 *m_signalTable;

    static int methodCount;
};

class QmlBinding_Id : public QmlAbstractExpression, 
                      public QmlAbstractBinding
{
public:
    QmlBinding_Id(QObject *object, int propertyIdx,
                  QmlContext *context, int id);
    virtual ~QmlBinding_Id();

    // Inherited from QmlAbstractBinding
    virtual void setEnabled(bool, QmlMetaProperty::WriteFlags flags);
    virtual int propertyIndex();
    virtual void update(QmlMetaProperty::WriteFlags flags);

    void reset();

private:
    void removeFromContext();

    QmlBinding_Id **m_prev;
    QmlBinding_Id  *m_next;

    QObject *m_object;
    int m_propertyIdx;
    int m_id;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLBINDINGOPTIMIZATIONS_P_H

