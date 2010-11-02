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

#include "qscriptoriginalglobalobject_p.h"
#include "qscriptvalue_p.h"

QT_BEGIN_NAMESPACE

bool QScriptOriginalGlobalObject::isType(const QScriptValuePrivate* value, v8::Handle<v8::Object> constructor, v8::Handle<v8::Value> prototype) const
{
    // FIXME re-check this comment
    // V8 API doesn't export the [[Class]] information for the builtins. But we know that a value
    // is an object of the Type if it was created with the Type constructor or if it is the Type.prototype.
    Q_ASSERT(value->isObject());

    bool result = value->instanceOf(constructor);
    if (result)
        return true;
    // FIXME Creating QSVP just for comparision is a waste of time.
    QScriptValuePrivate* tmp = new QScriptValuePrivate(value->engine(), prototype);
    // value is pointing to an object so strictlyEquals can't change it. We can safely const_cast.
    result = const_cast<QScriptValuePrivate*>(value)->strictlyEquals(tmp);
    delete tmp;
    return result;
}

QT_END_NAMESPACE
