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

#include <QtScript/QScriptClass>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptString>
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qstack.h>
#include <private/qobject_p.h>
#include <private/qmlclassfactory_p.h>
#include <private/qmlcompositetypemanager_p.h>
#include <private/qpodvector_p.h>
#include <QtDeclarative/qml.h>
#include <private/qmlbasicscript_p.h>
#include <private/qmlvaluetype_p.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlexpression.h>
#include <QtScript/qscriptengine.h>

QT_BEGIN_NAMESPACE

class QmlContext;
class QmlEngine;
class QmlContextPrivate;
class QmlExpression;
class QmlBasicScriptNodeCache;
class QmlContextScriptClass;
class QmlObjectScriptClass;
class QmlValueTypeScriptClass;
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

    QScriptClass::QueryFlags queryObject(const QString &name, uint *id, 
                                         QObject *);
    QScriptValue propertyObject(const QScriptString &propName, QObject *, 
                                uint id = 0);

    struct CapturedProperty {
        CapturedProperty(QObject *o, int c, int n)
            : object(o), coreIndex(c), notifyIndex(n) {}
        CapturedProperty(const QmlMetaProperty &);

        QObject *object;
        int coreIndex;
        int notifyIndex;
    };
    QPODVector<CapturedProperty> capturedProperties;

    QmlContext *rootContext;
    QmlContext *currentBindContext;
    QmlExpression *currentExpression;
    QmlEngine *q;
    bool isDebugging;
#ifdef QT_SCRIPTTOOLS_LIB
    QScriptEngineDebugger *debugger;
#endif

    QmlContextScriptClass *contextClass;
    QmlObjectScriptClass *objectClass;
    QmlValueTypeScriptClass *valueTypeClass;

    QmlContext *setCurrentBindContext(QmlContext *);
    QStack<QmlContext *> activeContexts;

    QScriptEngine scriptEngine;

    QUrl baseUrl;

    template<class T>
    struct SimpleList {
        SimpleList()
            : count(0), values(0) {}
        SimpleList(int r)
            : count(0), values(new T*[r]) {}

        int count;
        T **values;

        void append(T *v) {
            values[count++] = v;
        }

        T *at(int idx) const {
            return values[idx];
        }

        void clear() {
            delete [] values;
        }
    };

    static void clear(SimpleList<QmlBinding> &);
    static void clear(SimpleList<QmlParserStatus> &);

    QList<SimpleList<QmlBinding> > bindValues;
    QList<SimpleList<QmlParserStatus> > parserStatus;

    QmlComponent *rootComponent;
    mutable QNetworkAccessManager *networkAccessManager;

    QmlCompositeTypeManager typeManager;
    QStringList fileImportPath;

    mutable quint32 uniqueId;
    quint32 getUniqueId() const {
        return uniqueId++;
    }

    QmlValueTypeFactory valueTypes;
};

class QmlScriptClass : public QScriptClass
{
public:
    enum ClassId 
    {
        InvalidId = -1,

        FunctionId           = 0x80000000,
        VariantPropertyId    = 0x40000000,
        PropertyId           = 0x00000000,

        ClassIdMask          = 0xC0000000,

        ClassIdSelectorMask  = 0x3F000000,
    };

    QmlScriptClass(QmlEngine *);

    static QVariant toVariant(QmlEngine *, const QScriptValue &);
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

    virtual QScriptValue prototype () const;
    QScriptValue prototypeObject;

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

class QmlValueTypeScriptClass : public QmlScriptClass
{
public:
    QmlValueTypeScriptClass(QmlEngine *);
    ~QmlValueTypeScriptClass();

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

QT_END_NAMESPACE

#endif // QMLENGINE_P_H
