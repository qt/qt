/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
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

#include <QtCore/qobjectdefs.h>

#ifndef QT_NO_SCRIPT

QT_BEGIN_NAMESPACE

namespace JSC
{
    class JSObject;
    class ArgList;
}

#include "wtf/Platform.h"
#include "JSValue.h"

class QScriptEnginePrivate;

class QScriptContext;
class QScriptContextPrivate
{
public:
    QScriptContextPrivate(JSC::JSObject *callee,
                          JSC::JSValue thisObject,
                          const JSC::ArgList &args,
                          bool calledAsConstructor,
                          QScriptEnginePrivate *engine);
    ~QScriptContextPrivate();

    static QScriptContext *create(QScriptContextPrivate &dd);

    JSC::JSObject *callee;
    JSC::JSValue thisObject;
    const JSC::ArgList &args;
    bool calledAsConstructor;
    QScriptEnginePrivate *engine;
};

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
