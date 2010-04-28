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

#ifndef QDECLARATIVEEXPRESSION_P_H
#define QDECLARATIVEEXPRESSION_P_H

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

#include "qdeclarativeexpression.h"

#include "private/qdeclarativeengine_p.h"
#include "private/qdeclarativeguard_p.h"

#include <QtScript/qscriptvalue.h>

QT_BEGIN_NAMESPACE

class QDeclarativeAbstractExpression
{
public:
    QDeclarativeAbstractExpression();
    virtual ~QDeclarativeAbstractExpression();

    bool isValid() const;

    QDeclarativeContextData *context() const;
    void setContext(QDeclarativeContextData *);

    virtual void refresh();

private:
    friend class QDeclarativeContext;
    friend class QDeclarativeContextData;
    friend class QDeclarativeContextPrivate;
    QDeclarativeContextData *m_context;
    QDeclarativeAbstractExpression **m_prevExpression;
    QDeclarativeAbstractExpression  *m_nextExpression;
};

class QDeclarativeDelayedError 
{
public:
    inline QDeclarativeDelayedError() : nextError(0), prevError(0) {}
    inline ~QDeclarativeDelayedError() { removeError(); }

    QDeclarativeError error;

    bool addError(QDeclarativeEnginePrivate *);

    inline void removeError() {
        if (!prevError) return;
        if (nextError) nextError->prevError = prevError;
        *prevError = nextError;
        nextError = 0;
        prevError = 0;
    }

private:
    QDeclarativeDelayedError  *nextError;
    QDeclarativeDelayedError **prevError;
};

class QDeclarativeExpressionData : public QDeclarativeAbstractExpression, public QDeclarativeDelayedError, public QDeclarativeRefCount
{
public:
    QDeclarativeExpressionData();
    virtual ~QDeclarativeExpressionData();

    QDeclarativeExpressionPrivate *q;

    QDeclarativeRefCount *dataRef;
    QString expression;
    bool expressionFunctionValid:1;
    bool expressionRewritten:1;
    QScriptValue expressionFunction;
    QScriptValue expressionContext;

    QObject *me;
    bool trackChange;

    bool isShared;

    QString url; // This is a QString for a reason.  QUrls are slooooooow...
    int line;

    QDeclarativeNotifierEndpoint *guardList;
    int guardListLength;
};

class QDeclarativeExpression;
class QString;
class QDeclarativeExpressionPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeExpression)
public:
    QDeclarativeExpressionPrivate();
    QDeclarativeExpressionPrivate(QDeclarativeExpressionData *);
    ~QDeclarativeExpressionPrivate();

    void init(QDeclarativeContextData *, const QString &, QObject *);
    void init(QDeclarativeContextData *, void *, QDeclarativeRefCount *, QObject *, const QString &, int);

    QDeclarativeExpressionData *data;

    QVariant value(QObject *secondaryScope = 0, bool *isUndefined = 0);
    QScriptValue scriptValue(QObject *secondaryScope = 0, bool *isUndefined = 0);

    QScriptValue eval(QObject *secondaryScope, bool *isUndefined = 0);

    void updateGuards(const QPODVector<QDeclarativeEnginePrivate::CapturedProperty> &properties);
    void clearGuards();

    static QDeclarativeExpressionPrivate *get(QDeclarativeExpression *expr) {
        return static_cast<QDeclarativeExpressionPrivate *>(QObjectPrivate::get(expr));
    }
    static QDeclarativeExpression *get(QDeclarativeExpressionPrivate *expr) {
        return expr->q_func();
    }

    void _q_notify();
    virtual void emitValueChanged();

    static void exceptionToError(QScriptEngine *, QDeclarativeError &);
    static QScriptValue evalInObjectScope(QDeclarativeContextData *, QObject *, const QString &, const QString &,
                                          int, QScriptValue *);
    static QScriptValue evalInObjectScope(QDeclarativeContextData *, QObject *, const QScriptProgram &, 
                                          QScriptValue *);
};

QT_END_NAMESPACE

#endif // QDECLARATIVEEXPRESSION_P_H
