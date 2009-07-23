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

#ifndef QSCRIPTSTRING_P_H
#define QSCRIPTSTRING_P_H

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

#include <QtCore/qstring.h>
#include <QtCore/qpointer.h>
#include "qscriptengine.h"

#include "Identifier.h"


QT_BEGIN_NAMESPACE

class QScriptString;
class QScriptEngine;
class QScriptStringPrivate
{
public:
    QScriptStringPrivate();
    ~QScriptStringPrivate();

    static void init(QScriptString &q, QScriptEngine *engine, const JSC::Identifier &value);

    QBasicAtomicInt ref;
#ifndef QT_NO_QOBJECT
    QPointer<QScriptEngine> engine;
#else
    void *engine;
#endif
    JSC::Identifier identifier;
};

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
