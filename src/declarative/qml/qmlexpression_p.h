/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
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

#include "qmlbasicscript_p.h"
#include "qmlexpression.h"
#include <QtScript/qscriptvalue.h>

QT_BEGIN_NAMESPACE

class QmlExpression;
class QString;
class QmlExpressionLog;
class QmlExpressionBindProxy;
class QmlExpressionPrivate
{
public:
    QmlExpressionPrivate(QmlExpression *);
    QmlExpressionPrivate(QmlExpression *, const QString &expr);
    QmlExpressionPrivate(QmlExpression *, void *expr, QmlRefCount *rc);
    ~QmlExpressionPrivate();

    QmlExpression *q;
    QmlContext *ctxt;
    QString expression;
    bool expressionFunctionValid;
    QScriptValue expressionFunction;

    QmlBasicScript sse;
    void *sseData;
    QmlExpressionBindProxy *proxy;
    QObject *me;
    bool trackChange;

    QUrl fileName;
    int line;

    quint32 id;

    void addLog(const QmlExpressionLog &);
    QList<QmlExpressionLog> *log;

    QVariant evalSSE(QmlBasicScript::CacheState &cacheState);
    QVariant evalQtScript();
};

class QmlExpressionBindProxy : public QObject
{
Q_OBJECT
public:
    QmlExpressionBindProxy(QmlExpression *be)
    :e(be) { }

private:
    QmlExpression *e;

private Q_SLOTS:
    void changed() { e->valueChanged(); }
};

class QmlExpressionLog
{
public:
    QmlExpressionLog();
    QmlExpressionLog(const QmlExpressionLog &);
    ~QmlExpressionLog();

    QmlExpressionLog &operator=(const QmlExpressionLog &);

    void setTime(quint32);
    quint32 time() const;

    QString expression() const;
    void setExpression(const QString &);

    QStringList warnings() const;
    void addWarning(const QString &);

    QVariant result() const;
    void setResult(const QVariant &);

private:
    quint32 m_time;
    QString m_expression;
    QVariant m_result;
    QStringList m_warnings;
};

QT_END_NAMESPACE

#endif // QMLEXPRESSION_P_H
