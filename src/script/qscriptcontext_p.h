/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTCONTEXT_P_H
#define QSCRIPTCONTEXT_P_H

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

#include <QtCore/QVarLengthArray>
#include <QtCore/QPair>
#include "qscriptcontext.h"
#include "qscriptvalue.h"
#include "v8.h"

QT_BEGIN_NAMESPACE

class QScriptEnginePrivate;
class QScriptContext;

class QScriptContextPrivate : public QScriptContext
{
    Q_DECLARE_PUBLIC(QScriptContext);
public:
    static QScriptContextPrivate *get(const QScriptContext *q) { Q_ASSERT(q->d_ptr); return q->d_ptr; }
    static QScriptContext *get(QScriptContextPrivate *d) { return d->q_func(); }

    inline QScriptContextPrivate(QScriptEnginePrivate *engine, const v8::Arguments *args = 0);
    inline QScriptContextPrivate(QScriptEnginePrivate *engine, const v8::AccessorInfo *accessor);
    inline QScriptContextPrivate(QScriptEnginePrivate *engine, v8::Handle<v8::Context> context);
    inline ~QScriptContextPrivate();

    inline QScriptPassPointer<QScriptValuePrivate> argument(int index) const;
    inline int argumentCount() const;
    inline QScriptPassPointer<QScriptValuePrivate> argumentsObject() const;
    inline QScriptPassPointer<QScriptValuePrivate> thisObject() const;
    inline QScriptPassPointer<QScriptValuePrivate> callee() const;

    inline QScriptPassPointer<QScriptValuePrivate> activationObject() const;
    inline QScriptValueList scopeChain() const;
    inline void pushScope(QScriptValuePrivate *object);
    inline QScriptPassPointer<QScriptValuePrivate> popScope();
    inline void setInheritedScope(v8::Handle<v8::Context>);

    inline v8::Handle<v8::Value> throwError(Error error, const QString &text);

    QScriptContext* q_ptr;
    QScriptEnginePrivate *engine;
    const v8::Arguments *arguments;
    const v8::AccessorInfo *accessorInfo;
    v8::Persistent<v8::Context> context;
    QList<v8::Persistent<v8::Context> > scopes;
    v8::Persistent<v8::Context> inheritedScope;
    QScriptContextPrivate *parent;
private:
    Q_DISABLE_COPY(QScriptContextPrivate)
};


QT_END_NAMESPACE

#endif
