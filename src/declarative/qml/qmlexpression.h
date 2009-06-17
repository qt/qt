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

#ifndef QMLEXPRESSION_H
#define QMLEXPRESSION_H

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QString;
class QmlRefCount;
class QmlEngine;
class QmlContext;
class QmlExpressionPrivate;
class QmlBasicScript;
class Q_DECLARATIVE_EXPORT QmlExpression 
{
public:
    QmlExpression();
    QmlExpression(QmlContext *, const QString &, QObject *);
    QmlExpression(QmlContext *, void *, QmlRefCount *rc, QObject *me);
    virtual ~QmlExpression();

    QmlEngine *engine() const;
    QmlContext *context() const;

    QString expression() const;
    void clearExpression();
    virtual void setExpression(const QString &);
    QVariant value();
    bool isConstant() const;

    bool trackChange() const;
    void setTrackChange(bool);

    void setSourceLocation(const QUrl &fileName, int line);

    QObject *scopeObject() const;

    quint32 id() const;
protected:
    virtual void valueChanged();

private:
    friend class BindExpressionProxy;
    friend class QmlDebugger;
    friend class QmlContext;
    QmlExpressionPrivate *d;
};

// LK: can't we merge with QmlExpression????
class Q_DECLARATIVE_EXPORT QmlExpressionObject : public QObject, 
                                                 public QmlExpression
{
    Q_OBJECT
public:
    QmlExpressionObject(QObject *parent = 0);
    QmlExpressionObject(QmlContext *, const QString &, QObject *scope, QObject *parent = 0);
    QmlExpressionObject(QmlContext *, void *, QmlRefCount *, QObject *);

public Q_SLOTS:
    QVariant value();

Q_SIGNALS:
    void valueChanged();
};


QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLEXPRESSION_H

