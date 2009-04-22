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

#ifndef QMLENGINE_P_H
#define QMLENGINE_P_H

#include <QScriptClass>
#include <QScriptValue>
#include <QScriptString>
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qstack.h>
#include <private/qobject_p.h>
#include <private/qmlclassfactory_p.h>
#include <private/qmlcompositetypemanager_p.h>
#include <qml.h>
#include <qmlbasicscript.h>
#include <qmlcontext.h>
#include <qmlengine.h>
#include <qmlexpression.h>
#include <QtScript/qscriptengine.h>

QT_BEGIN_NAMESPACE
class QmlContext;
class QmlEngine;
class QmlContextPrivate;
class QmlExpression;
class QmlBasicScriptNodeCache;
class QmlContextScriptClass;
class QmlObjectScriptClass;
class QScriptEngineDebugger;
class QNetworkReply;
class QNetworkAccessManager;

class QmlEnginePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlEngine)
public:
    QmlEnginePrivate(QmlEngine *);
    ~QmlEnginePrivate();

    void init();

    void contextActivated(QmlContext *);
    void contextDeactivated(QmlContext *);

    bool fetchCache(QmlBasicScriptNodeCache &cache, const QString &propName, QObject *);
    bool loadCache(QmlBasicScriptNodeCache &cache, const QString &propName, QmlContextPrivate *context);

    QScriptClass::QueryFlags queryObject(const QString &name, uint *id, QObject *);
    QScriptValue propertyObject(const QScriptString &propName, QObject *, uint id = 0);

    QList<QmlMetaProperty> capturedProperties;

    QmlContext *rootContext;
    QmlContext *currentBindContext;
    QmlExpression *currentExpression;
    QmlEngine *q;
#ifdef QT_SCRIPTTOOLS_LIB
    QScriptEngineDebugger *debugger;
#endif

    QmlContextScriptClass *contextClass;
    QmlObjectScriptClass *objectClass;

    QmlContext *setCurrentBindContext(QmlContext *);
    QStack<QmlContext *> activeContexts;

    QScriptEngine scriptEngine;

    QList<QmlBindableValue *> currentBindValues;
    QList<QmlParserStatus *> currentParserStatus;
    QmlComponent *rootComponent;
    mutable QNetworkAccessManager *networkAccessManager;

    QmlCompositeTypeManager typeManager;
    QMap<QString,QString> nameSpacePaths;
};


class BindExpressionProxy : public QObject
{
Q_OBJECT
public:
    BindExpressionProxy(QmlExpression *be)
    :e(be)
    {
    }

private:
    QmlExpression *e;

private Q_SLOTS:
    void changed();
};

class QmlScriptClass : public QScriptClass
{
public:
    enum ClassId 
    {
        InvalidId = -1,

        ObjectListPropertyId = 0xC0000000,
        FunctionId           = 0x80000000,
        VariantPropertyId    = 0x40000000,
        PropertyId           = 0x00000000,

        ClassIdMask          = 0xC0000000,

        ClassIdSelectorMask  = 0x3F000000,
    };

    QmlScriptClass(QmlEngine *);

protected:
    QmlEngine *engine;
};

class QmlContextScriptClass : public QmlScriptClass
{
public:
    QmlContextScriptClass(QmlEngine *);
    ~QmlContextScriptClass();

    virtual QueryFlags queryProperty(const QScriptValue &object,
                                     const QScriptString &name,
                                     QueryFlags flags, uint *id);
    virtual QScriptValue property(const QScriptValue &object,
                                  const QScriptString &name, 
                                  uint id);
    virtual void setProperty(QScriptValue &object,
                             const QScriptString &name,
                             uint id,
                             const QScriptValue &value);
};

class QmlObjectScriptClass : public QmlScriptClass
{
public:
    QmlObjectScriptClass(QmlEngine *);
    ~QmlObjectScriptClass();

    virtual QueryFlags queryProperty(const QScriptValue &object,
                                     const QScriptString &name,
                                     QueryFlags flags, uint *id);
    virtual QScriptValue property(const QScriptValue &object,
                                  const QScriptString &name, 
                                  uint id);
    virtual void setProperty(QScriptValue &object,
                             const QScriptString &name,
                             uint id,
                             const QScriptValue &value);
};

class QmlExpressionPrivate
{
public:
    QmlExpressionPrivate(QmlExpression *);
    QmlExpressionPrivate(QmlExpression *, const QString &expr, bool);
    QmlExpressionPrivate(QmlExpression *, void *expr, QmlRefCount *rc);
    ~QmlExpressionPrivate();

    QmlExpression *q;
    QmlContext *ctxt;
    QString expression;
    QmlBasicScript sse;
    void *sseData;
    BindExpressionProxy *proxy;
    QObject *me;
    bool trackChange;
};
QT_END_NAMESPACE

#endif // QMLENGINE_P_H

