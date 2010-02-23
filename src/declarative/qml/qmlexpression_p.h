/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMLEXPRESSION_P_H
#define QMLEXPRESSION_P_H

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

#include "qmlexpression.h"

#include "qmlengine_p.h"
#include "qmlguard_p.h"

#include <QtScript/qscriptvalue.h>

QT_BEGIN_NAMESPACE

class QmlAbstractExpression
{
public:
    QmlAbstractExpression();
    virtual ~QmlAbstractExpression();

    bool isValid() const;

    QmlContext *context() const;
    void setContext(QmlContext *);

    virtual void refresh();

private:
    friend class QmlContext;
    friend class QmlContextPrivate;
    QmlContext *m_context;
    QmlAbstractExpression **m_prevExpression;
    QmlAbstractExpression  *m_nextExpression;
};

class QmlDelayedError 
{
public:
    inline QmlDelayedError() : nextError(0), prevError(0) {}
    inline ~QmlDelayedError() { removeError(); }

    QmlError error;

    bool addError(QmlEnginePrivate *);

    inline void removeError() {
        if (!prevError) return;
        if (nextError) nextError->prevError = prevError;
        *prevError = nextError;
        nextError = 0;
        prevError = 0;
    }

private:
    QmlDelayedError  *nextError;
    QmlDelayedError **prevError;
};

class QmlExpressionData : public QmlAbstractExpression, public QmlDelayedError, public QmlRefCount
{
public:
    QmlExpressionData();
    virtual ~QmlExpressionData();

    QmlExpressionPrivate *q;

    QmlRefCount *dataRef;
    QString expression;
    bool expressionFunctionValid:1;
    bool expressionRewritten:1;
    QScriptValue expressionFunction;

    QObject *me;
    bool trackChange;

    bool isShared;

    QString url; // This is a QString for a reason.  QUrls are slooooooow...
    int line;

    struct SignalGuard : public QmlGuard<QObject> {
        SignalGuard() : isDuplicate(false), notifyIndex(-1) {}

        SignalGuard &operator=(QObject *obj) {
            QmlGuard<QObject>::operator=(obj);
            return *this;
        }
        SignalGuard &operator=(const SignalGuard &o) {
            QmlGuard<QObject>::operator=(o);
            isDuplicate = o.isDuplicate;
            notifyIndex = o.notifyIndex;
            return *this;
        }

        bool isDuplicate:1;
        int notifyIndex:31;
    };
    SignalGuard *guardList;
    int guardListLength;
};

class QmlExpression;
class QString;
class QmlExpressionPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlExpression)
public:
    QmlExpressionPrivate();
    QmlExpressionPrivate(QmlExpressionData *);
    ~QmlExpressionPrivate();

    void init(QmlContext *, const QString &, QObject *);
    void init(QmlContext *, void *, QmlRefCount *, QObject *, const QString &, int);

    QmlExpressionData *data;

    QVariant value(QObject *secondaryScope = 0, bool *isUndefined = 0);
    QVariant evalQtScript(QObject *secondaryScope, bool *isUndefined = 0);

    void updateGuards(const QPODVector<QmlEnginePrivate::CapturedProperty> &properties);
    void clearGuards();

    static QmlExpressionPrivate *get(QmlExpression *expr) {
        return static_cast<QmlExpressionPrivate *>(QObjectPrivate::get(expr));
    }
    static QmlExpression *get(QmlExpressionPrivate *expr) {
        return expr->q_func();
    }

    static void exceptionToError(QScriptEngine *, QmlError &);
    static QScriptValue evalInObjectScope(QmlContext *, QObject *, const QString &);
    static QScriptValue evalInObjectScope(QmlContext *, QObject *, const QScriptProgram &);
};

QT_END_NAMESPACE

#endif // QMLEXPRESSION_P_H
