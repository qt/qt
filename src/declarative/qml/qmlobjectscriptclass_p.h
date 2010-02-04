/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QMLOBJECTSCRIPTCLASS_P_H
#define QMLOBJECTSCRIPTCLASS_P_H

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

#include "qmlpropertycache_p.h"
#include "qmltypenamecache_p.h"

#include <private/qmlscriptclass_p.h>

QT_BEGIN_NAMESPACE

class QmlEngine;
class QScriptContext;
class QScriptEngine;
class QmlContext;

#if (QT_VERSION > QT_VERSION_CHECK(4, 6, 2)) || defined(QT_HAVE_QSCRIPTDECLARATIVECLASS_VALUE)
class Q_AUTOTEST_EXPORT QmlObjectMethodScriptClass : public QScriptDeclarativeClass
{
public:
    QmlObjectMethodScriptClass(QmlEngine *);
    ~QmlObjectMethodScriptClass();

    QScriptValue newMethod(QObject *, const QmlPropertyCache::Data *);
protected:
    virtual Value call(Object *, QScriptContext *);

private:
    QmlEngine *engine;
};
#endif

class Q_AUTOTEST_EXPORT QmlObjectScriptClass : public QmlScriptClass
{
public:
    QmlObjectScriptClass(QmlEngine *);
    ~QmlObjectScriptClass();

    QScriptValue newQObject(QObject *, int type = QMetaType::QObjectStar);
    QObject *toQObject(const QScriptValue &) const;
    int objectType(const QScriptValue &) const;

    enum QueryHint {
        ImplicitObject = 0x01,
        SkipAttachedProperties = 0x02
    };
    Q_DECLARE_FLAGS(QueryHints, QueryHint)

    QScriptClass::QueryFlags queryProperty(QObject *, const Identifier &, 
                                           QScriptClass::QueryFlags flags, 
                                           QmlContext *evalContext,
                                           QueryHints hints = 0);

    ScriptValue property(QObject *, const Identifier &);

    void setProperty(QObject *, const Identifier &name, const QScriptValue &,
                     QmlContext *evalContext = 0);
    virtual QStringList propertyNames(Object *);

protected:
    virtual QScriptClass::QueryFlags queryProperty(Object *, const Identifier &, 
                                                   QScriptClass::QueryFlags flags);

    virtual ScriptValue property(Object *, const Identifier &);
    virtual void setProperty(Object *, const Identifier &name, const QScriptValue &);
    virtual bool isQObject() const;
    virtual QObject *toQObject(Object *, bool *ok = 0);

private:
#if (QT_VERSION > QT_VERSION_CHECK(4, 6, 2)) || defined(QT_HAVE_QSCRIPTDECLARATIVECLASS_VALUE)
    QmlObjectMethodScriptClass methods;
#endif

    QmlTypeNameCache::Data *lastTNData;
    QmlPropertyCache::Data *lastData;
    QmlPropertyCache::Data local;

    PersistentIdentifier m_destroyId;
    PersistentIdentifier m_toStringId;
    QScriptValue m_destroy;
    QScriptValue m_toString;

    static QScriptValue tostring(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue destroy(QScriptContext *context, QScriptEngine *engine);

    QmlEngine *engine;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QmlObjectScriptClass::QueryHints);

QT_END_NAMESPACE

#endif // QMLOBJECTSCRIPTCLASS_P_H

