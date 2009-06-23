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

#ifndef QT_NO_SCRIPT

QT_BEGIN_NAMESPACE

#include "wtf/Platform.h"
#include "JSValue.h"

class QString;
class QScriptEngine;
class QScriptValue;
class QScriptValuePrivate
{
public:
    enum Type {
        JSC,
        Number,
        String
    };

    QScriptValuePrivate();
    ~QScriptValuePrivate();

    void initFromJSCValue(JSC::JSValue value);
    void initFromNumber(double value);
    void initFromString(const QString &value);

    static void initFromJSCValue(QScriptValue &result,
                                 QScriptEngine *engine,
                                 JSC::JSValue value);

    bool isJSC() const;

    QVariant &variantValue() const;

    static QScriptValuePrivate *get(const QScriptValue &q);

    QScriptValue property(const QString &name, int resolveMode) const;
    QScriptValue property(quint32 index, int resolveMode) const;

    QScriptEngine *engine;
    Type type;
    JSC::JSValue jscValue;
    double numberValue;
    QString *stringValue;

    QBasicAtomicInt ref;
};

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
