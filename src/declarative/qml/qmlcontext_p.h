/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QMLCONTEXT_P_H
#define QMLCONTEXT_P_H

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

#include "qmlcontext.h"

#include "qmldeclarativedata_p.h"
#include "qmlengine_p.h"
#include "qmlintegercache_p.h"
#include "qmltypenamecache_p.h"

#include <QtCore/qhash.h>
#include <QtScript/qscriptvalue.h>
#include <QtCore/qset.h>

#include <private/qobject_p.h>
#include "qmlguard_p.h"

QT_BEGIN_NAMESPACE

class QmlContext;
class QmlExpression;
class QmlEngine;
class QmlExpression;
class QmlExpressionPrivate;
class QmlAbstractExpression;
class QmlBinding_Id;
class QmlCompiledBindings;

class Q_DECLARATIVE_EXPORT QmlContextPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlContext)
public:
    QmlContextPrivate();

    QmlContext *parent;
    QmlEngine *engine;

    bool isInternal;

    QmlIntegerCache *propertyNames;
    QList<QVariant> propertyValues;
    int notifyIndex;

    QObjectList defaultObjects;
    int highPriorityCount;

    QList<QScriptValue> scripts;
    void addScript(const QmlParser::Object::ScriptBlock &, QObject *);

    QUrl url;

    QmlTypeNameCache *imports;

    void init();

    void invalidateEngines();
    void refreshExpressions();
    QSet<QmlContext *> childContexts;

    QmlAbstractExpression *expressions;

    QmlDeclarativeData *contextObjects;

    struct IdNotifier 
    {
        inline IdNotifier();
        inline ~IdNotifier();
        
        inline void clear();

        IdNotifier *next;
        IdNotifier**prev;
        QObject *target;
        int methodIndex;
    };

    struct ContextGuard : public QmlGuard<QObject>
    {
        inline ContextGuard();
        inline ContextGuard &operator=(QObject *obj);
        inline virtual void objectDestroyed(QObject *);

        QmlContextPrivate *priv;
        IdNotifier *bindings;
    };
    ContextGuard *idValues;
    int idValueCount;
    void setIdProperty(int, QObject *);
    void setIdPropertyData(QmlIntegerCache *);
    void destroyed(ContextGuard *);

    static QmlContextPrivate *get(QmlContext *context) {
        return static_cast<QmlContextPrivate *>(QObjectPrivate::get(context));
    }
    static QmlContext *get(QmlContextPrivate *context) {
        return static_cast<QmlContext *>(context->q_func());
    }

    QmlCompiledBindings *optimizedBindings;

    // Only used for debugging
    QList<QPointer<QObject> > instances;

    static int context_count(QmlListProperty<QObject> *);
    static QObject *context_at(QmlListProperty<QObject> *, int);
};

QmlContextPrivate::IdNotifier::IdNotifier()
: next(0), prev(0), target(0), methodIndex(-1)
{
}

QmlContextPrivate::IdNotifier::~IdNotifier()
{
    clear();
}

void QmlContextPrivate::IdNotifier::clear()
{
    if (next) next->prev = prev;
    if (prev) *prev = next;
    next = 0; prev = 0; target = 0;
    methodIndex = -1;
}

QmlContextPrivate::ContextGuard::ContextGuard() 
: priv(0), bindings(0) 
{
}

QmlContextPrivate::ContextGuard &QmlContextPrivate::ContextGuard::operator=(QObject *obj) 
{
    (QmlGuard<QObject>&)*this = obj; return *this;
}

void QmlContextPrivate::ContextGuard::objectDestroyed(QObject *) 
{ 
    priv->destroyed(this); 
}

QT_END_NAMESPACE

#endif // QMLCONTEXT_P_H
