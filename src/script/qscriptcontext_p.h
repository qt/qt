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
    inline ~QScriptContextPrivate();

    inline QScriptPassPointer<QScriptValuePrivate> argument(int index) const;
    inline int argumentCount() const;
    inline QScriptPassPointer<QScriptValuePrivate> thisObject() const;
    inline QScriptPassPointer<QScriptValuePrivate> callee() const;

    QScriptContext* q_ptr;
    QScriptEnginePrivate *engine;
    const v8::Arguments *arguments;
    const v8::AccessorInfo *accessorInfo;
    QScriptContextPrivate *parent;
    QVarLengthArray<QPair<v8::Persistent<v8::Context>, v8::Persistent<v8::Value> >, 4> v8Scopes;
};


QT_BEGIN_INCLUDE_NAMESPACE
#include "qscriptvalue_p.h"
#include "qscriptengine_p.h"
QT_END_INCLUDE_NAMESPACE


inline QScriptContextPrivate::QScriptContextPrivate(QScriptEnginePrivate *engine, const v8::Arguments *args)
: q_ptr(this), engine(engine), arguments(args), accessorInfo(0), parent(engine->setCurrentQSContext(this))
{
}

inline QScriptContextPrivate::QScriptContextPrivate(QScriptEnginePrivate *engine, const v8::AccessorInfo *accessor)
: q_ptr(this), engine(engine), arguments(0), accessorInfo(accessor), parent(engine->setCurrentQSContext(this))
{
}

QScriptContextPrivate::~QScriptContextPrivate()
{
    if (!parent)
        return;
    QScriptContextPrivate *old = engine->setCurrentQSContext(parent);
    if (old != this) {
        qWarning("QScriptEngine::pushContext() doesn't match with popContext()");
        old->parent = 0;
        //old is most likely leaking.
    }
    while (!v8Scopes.isEmpty())
        popScope();
}



inline QScriptPassPointer<QScriptValuePrivate> QScriptContextPrivate::argument(int index) const
{
    if (index < 0)
        return new QScriptValuePrivate();

    if (arguments) {
        if (index >= arguments->Length())
            return new QScriptValuePrivate(engine, QScriptValue::UndefinedValue);

        return new QScriptValuePrivate(engine, (*arguments)[index]);
    }

    Q_UNIMPLEMENTED();
    return new QScriptValuePrivate();
}

inline int QScriptContextPrivate::argumentCount() const
{
    if (arguments) {
        return arguments->Length();
    }

    Q_UNIMPLEMENTED();
    return -1;
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptContextPrivate::thisObject() const
{
    if (arguments) {
        return new QScriptValuePrivate(engine, arguments->This());
    } else if (accessorInfo) {
        return new QScriptValuePrivate(engine, accessorInfo->This());
    }

    return new QScriptValuePrivate();
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptContextPrivate::callee() const
{
    if (arguments)
        return new QScriptValuePrivate(engine, arguments->Callee());

    Q_UNIMPLEMENTED();
    return new QScriptValuePrivate();
}

QT_END_NAMESPACE

#endif
