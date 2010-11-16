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
#include "qscriptengine_p.h"
#include "qscriptengine_impl_p.h"
#include "qscriptconverter_p.h"

QT_BEGIN_NAMESPACE

v8::Handle<v8::Value> functionPrint(const v8::Arguments& args)
{
    QString result;
    for (int i = 0; i < args.Length(); ++i) {
        if (i != 0)
            result.append(QLatin1Char(' '));
        QString s = QScriptConverter::toString(args[i]->ToString());
        result.append(s);
    }
    qDebug("%s", qPrintable(result));
    return v8::Handle<v8::Value>();
}

v8::Handle<v8::Value> functionGC(const v8::Arguments& args)
{
    QScriptEnginePrivate *engine = static_cast<QScriptEnginePrivate *>(v8::External::Unwrap(args.Data()));
    engine->collectGarbage();
    return v8::Handle<v8::Value>();
}

v8::Handle<v8::Value> functionVersion(const v8::Arguments& args)
{
    return v8::Number::New(1);
}

QT_END_NAMESPACE
