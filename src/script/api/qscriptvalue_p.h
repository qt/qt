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
** contact the sales department at http://www.qtsoftware.com/contact.
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

    static QScriptValuePrivate *get(const QScriptValue &q) { return q.d_ptr; }

    QScriptValue property(const JSC::Identifier &id, int resolveMode) const;
    QScriptValue property(const QString &, int resolveMode) const;
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

    static void saveException(JSC::ExecState*, JSC::JSValue*);
    static void restoreException(JSC::ExecState*, JSC::JSValue);

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

#endif
