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

#ifndef QDECLARATIVECONTEXT_P_H
#define QDECLARATIVECONTEXT_P_H

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

#include "qdeclarativecontext.h"

#include "qdeclarativedeclarativedata_p.h"
#include "qdeclarativeengine_p.h"
#include "qdeclarativeintegercache_p.h"
#include "qdeclarativetypenamecache_p.h"
#include "qdeclarativenotifier_p.h"

#include <QtCore/qhash.h>
#include <QtScript/qscriptvalue.h>
#include <QtCore/qset.h>

#include <private/qobject_p.h>
#include "qdeclarativeguard_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeContext;
class QDeclarativeExpression;
class QDeclarativeEngine;
class QDeclarativeExpression;
class QDeclarativeExpressionPrivate;
class QDeclarativeAbstractExpression;
class QDeclarativeBinding_Id;
class QDeclarativeCompiledBindings;

class Q_DECLARATIVE_EXPORT QDeclarativeContextPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeContext)
public:
    QDeclarativeContextPrivate();

    QDeclarativeContext *parent;
    QDeclarativeEngine *engine;

    bool isInternal;

    QDeclarativeIntegerCache *propertyNames;
    QList<QVariant> propertyValues;
    int notifyIndex;

    QObject *contextObject;

    QList<QScriptValue> scripts;
    void addScript(const QDeclarativeParser::Object::ScriptBlock &, QObject *);

    QUrl url;

    QDeclarativeTypeNameCache *imports;

    void init();

    void invalidateEngines();
    void refreshExpressions();
    QSet<QDeclarativeContext *> childContexts;

    QDeclarativeAbstractExpression *expressions;

    QDeclarativeDeclarativeData *contextObjects;

    struct ContextGuard : public QDeclarativeGuard<QObject>
    {
        inline ContextGuard();
        inline ContextGuard &operator=(QObject *obj);
        inline virtual void objectDestroyed(QObject *);

        QDeclarativeContextPrivate *priv;
        QDeclarativeNotifier bindings;
    };
    ContextGuard *idValues;
    int idValueCount;
    void setIdProperty(int, QObject *);
    void setIdPropertyData(QDeclarativeIntegerCache *);
    void destroyed(ContextGuard *);

    static QDeclarativeContextPrivate *get(QDeclarativeContext *context) {
        return static_cast<QDeclarativeContextPrivate *>(QObjectPrivate::get(context));
    }
    static QDeclarativeContext *get(QDeclarativeContextPrivate *context) {
        return static_cast<QDeclarativeContext *>(context->q_func());
    }

    QDeclarativeCompiledBindings *optimizedBindings;

    // Only used for debugging
    QList<QPointer<QObject> > instances;

    static int context_count(QDeclarativeListProperty<QObject> *);
    static QObject *context_at(QDeclarativeListProperty<QObject> *, int);
};

QDeclarativeContextPrivate::ContextGuard::ContextGuard() 
: priv(0)
{
}

QDeclarativeContextPrivate::ContextGuard &QDeclarativeContextPrivate::ContextGuard::operator=(QObject *obj) 
{
    (QDeclarativeGuard<QObject>&)*this = obj; return *this;
}

void QDeclarativeContextPrivate::ContextGuard::objectDestroyed(QObject *) 
{ 
    priv->destroyed(this); 
}

QT_END_NAMESPACE

#endif // QDECLARATIVECONTEXT_P_H
