/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTVALUE_P_H
#define QSCRIPTVALUE_P_H

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

#include <QtCore/qobjectdefs.h>

#include "wtf/Platform.h"
#include "JSValue.h"

QT_BEGIN_NAMESPACE

class QString;
class QScriptEnginePrivate;

class QScriptValue;
class QScriptValuePrivate
{
    Q_DISABLE_COPY(QScriptValuePrivate);
public:
    void* operator new(size_t, QScriptEnginePrivate*);
    void operator delete(void*);

    enum Type {
        JSC,
        Number,
        String
    };

    inline QScriptValuePrivate(QScriptEnginePrivate*);
    inline ~QScriptValuePrivate();

    inline void initFrom(JSC::JSValue value);
    inline void initFrom(double value);
    inline void initFrom(const QString &value);

    inline bool isJSC() const;

    QVariant &variantValue() const;
    void setVariantValue(const QVariant &value);

    static inline QScriptValuePrivate *get(const QScriptValue &q)
    {
        return q.d_ptr.data();
    }

    static inline QScriptValue toPublic(QScriptValuePrivate *d)
    {
        return QScriptValue(d);
    }

    QScriptValue property(const JSC::Identifier &id, int resolveMode) const;
    QScriptValue property(quint32 index, int resolveMode) const;
    inline QScriptValue property(const QString &, int resolveMode) const;

    void detachFromEngine();

    qint64 objectId()
    {
        if ( (type == JSC) && (engine) )
            return (qint64)jscValue.asCell();
        else
            return -1;
    }

    static void saveException(JSC::ExecState*, JSC::JSValue*);
    static void restoreException(JSC::ExecState*, JSC::JSValue);

    QScriptEnginePrivate *engine;
    Type type;
    JSC::JSValue jscValue;
    double numberValue;
    QString stringValue;

    // linked list of engine's script values
    QScriptValuePrivate *prev;
    QScriptValuePrivate *next;

    QBasicAtomicInt ref;
};

inline QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate *e)
    : engine(e), prev(0), next(0)
{
    ref = 0;
}

inline bool QScriptValuePrivate::isJSC() const
{
    return (type == JSC);
}

// Rest of inline functions implemented in qscriptengine_p.h

QT_END_NAMESPACE

#endif
