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

#ifndef QSCRIPTSTRING__IMPL_P_H
#define QSCRIPTSTRING__IMPL_P_H

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

#include "qscriptconverter_p.h"
#include "qscriptstring_p.h"
#include "qscriptengine_p.h"
#include <QtCore/qnumeric.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qhash.h>
#include "qscriptisolate_p.h"

QT_BEGIN_NAMESPACE

QScriptStringPrivate::QScriptStringPrivate()
    : m_engine(0)
{}

QScriptStringPrivate::QScriptStringPrivate(QScriptEnginePrivate *engine, v8::Handle<v8::String> str)
    : m_engine(engine)
{
    const v8::String::Utf8Value utf8(str);
    m_string = v8::Persistent<v8::String>::New(v8::String::NewSymbol(*utf8, utf8.length()));
    Q_ASSERT(!m_string.IsEmpty());
    m_engine->registerString(this);
}

QScriptStringPrivate::~QScriptStringPrivate()
{
    if (isValid()) {
        QScriptIsolate api(m_engine);
        m_engine->unregisterString(this);
        m_string.Dispose();
    }
}

/*!
  \internal
  Change this string to an invalid one
*/
inline void QScriptStringPrivate::reinitialize()
{
    if (isValid()) {
        m_engine->unregisterString(this);
        m_string.Dispose();
        m_string.Clear();
        m_engine = 0;
    }
    Q_ASSERT(!isValid());
}

QScriptString QScriptStringPrivate::get(QScriptStringPrivate* d)
{
    Q_ASSERT(d);
    return QScriptString(d);
}

QScriptString QScriptStringPrivate::get(QScriptPassPointer<QScriptStringPrivate> d)
{
    Q_ASSERT(d);
    return QScriptString(d);
}

QScriptStringPrivate* QScriptStringPrivate::get(const QScriptString& p)
{
    return p.d_ptr.data();
}

bool QScriptStringPrivate::isValid() const
{
    return !m_string.IsEmpty();
}

bool QScriptStringPrivate::operator==(const QScriptStringPrivate& other) const
{
    v8::HandleScope handleScope;
    return isValid() && other.isValid() && m_string->Equals(other.m_string);
}

bool QScriptStringPrivate::operator!=(const QScriptStringPrivate& other) const
{
    v8::HandleScope handleScope;
    return isValid() && other.isValid() && !m_string->Equals(other.m_string);
}

quint32 QScriptStringPrivate::toArrayIndex(bool* ok) const
{
    quint32 idx = 0xffffffff;
    if (isValid()) {
        v8::HandleScope handleScope;
        v8::Handle<v8::Uint32> converted = m_string->ToArrayIndex();
        if (!converted.IsEmpty())
            idx = converted->Uint32Value();
    }
    if (ok)
        *ok = (idx != 0xffffffff);
    return idx;
}

QString QScriptStringPrivate::toString() const
{
    return QScriptConverter::toString(m_string);
}

quint64 QScriptStringPrivate::id() const
{
    return m_string->Hash();
}

inline QScriptStringPrivate::operator v8::Handle<v8::String>() const
{
    Q_ASSERT(isValid());
    return m_string;
}

inline v8::Handle<v8::String> QScriptStringPrivate::asV8Value()const
{
    return m_string;
}

inline QScriptEnginePrivate* QScriptStringPrivate::engine() const
{
    return m_engine;
}

QT_END_NAMESPACE

#endif
