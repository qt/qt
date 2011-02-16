/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QSCRIPTCLASSPRIVATE_IMPL_P_H
#define QSCRIPTCLASSPRIVATE_IMPL_P_H

#include "qscriptclass_p.h"

QT_BEGIN_NAMESPACE

inline QScriptClassPrivate* QScriptClassPrivate::get(const QScriptClass* q)
{
    Q_ASSERT(q);
    Q_ASSERT(q->d_ptr);
    return q->d_ptr.data();
}

inline QScriptClassPrivate* QScriptClassPrivate::safeGet(const QScriptClass* q)
{
    if (q && q->d_ptr)
        return q->d_ptr.data();
    return 0;
}

inline QScriptClass* QScriptClassPrivate::get(const QScriptClassPrivate* d)
{
    Q_ASSERT(d);
    return d->q_ptr;
}

inline QScriptClass* QScriptClassPrivate::safeGet(const QScriptClassPrivate* d)
{
    if (d)
        return d->q_ptr;
    return 0;
}

inline QScriptClassPrivate::QScriptClassPrivate(QScriptEnginePrivate* engine, QScriptClass* q)
    : q_ptr(q)
    , m_engine(engine)
{
    Q_ASSERT(q_ptr);
}

inline QScriptEnginePrivate* QScriptClassPrivate::engine() const
{
    return m_engine;
}

inline QScriptClass* QScriptClassPrivate::userCallback() const
{
    return q_ptr;
}

QT_END_NAMESPACE

#endif // QSCRIPTCLASSPRIVATE_IMPL_P_H
