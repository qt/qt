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
#include "qscriptengine.h"

class QString;
class QScriptValue;


class QScriptValuePrivate
{
    /** Helper class used only in QScriptValuePrivate. Supports atomatic invalidation of all script
        values evaluated and contained by QScriptEngine when the engine is above to be deleted.
        On change it call QScriptEnginePrivate to unregister old and register new pointer value.
        It should behave as pointer to QScriptEngine.
        */
    class QScriptValueAutoRegister
    {
        QScriptValuePrivate *val;
        QScriptEngine *ptr;
        public:
            QScriptValueAutoRegister(QScriptValuePrivate *value) : val(value), ptr(0) {};
            QScriptValueAutoRegister(QScriptValuePrivate *value, const QScriptEngine *engine);
            ~QScriptValueAutoRegister();

            QScriptValueAutoRegister& operator=(const QScriptEngine* pointer);

            operator QScriptEngine*() const
            {
                return ptr;
            }

            operator bool() const
            {
                return ptr;
            }

            bool operator==(const int i) const
            {
                return ptr == (void *)i;
            }

            bool operator!=(const int i) const
            {
                return ptr !=   (void *)i;
            }

        private:
            QScriptValueAutoRegister(QScriptValueAutoRegister &/*enigne*/){}; //block it
    };

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
    void setVariantValue(const QVariant &value);

    static QScriptValuePrivate *get(const QScriptValue &q);

    QScriptValue property(const QString &name, int resolveMode) const;
    QScriptValue property(quint32 index, int resolveMode) const;

    QScriptValue toPublic() {
        QScriptValue tmp;
        tmp.d_ptr = this;
        return tmp;
    };

    bool isValid() const {return valid;}
    void detachEngine()
    {
        // if type is not developed in js engine there is no
        // need to invalidate the object
        if (isJSC()) valid=false;
        engine=0;
    }

    QScriptValueAutoRegister engine;
    Type type;
    JSC::JSValue jscValue;
    double numberValue;
    QString stringValue;

    QBasicAtomicInt ref;
    bool valid; //object is valid ?

    qint64 id;  //object unique id number
};


QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
