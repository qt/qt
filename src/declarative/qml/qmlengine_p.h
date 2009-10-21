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
#include <private/qmlmetaproperty_p.h>
#include <private/qmlpropertycache_p.h>
#include <private/qmlobjectscriptclass_p.h>
#include <private/qmlcontextscriptclass_p.h>
#include <private/qmlvaluetypescriptclass_p.h>

QT_BEGIN_NAMESPACE

class QmlContext;
class QmlEngine;
class QmlContextPrivate;
class QmlExpression;
class QmlBasicScriptNodeCache;
class QmlContextScriptClass;
class QmlObjectScriptClass;
class QmlTypeNameScriptClass;
class QmlValueTypeScriptClass;
class QScriptEngineDebugger;
class QNetworkReply;
class QNetworkAccessManager;
class QmlAbstractBinding;
class QScriptDeclarativeClass;
class QmlTypeNameScriptClass;
class QmlTypeNameCache;
class QmlComponentAttached;
class QmlListScriptClass;

class QmlEnginePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlEngine)
public:
    QmlEnginePrivate(QmlEngine *);
    ~QmlEnginePrivate();

    void init();

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
    QmlExpression *currentExpression;
    bool isDebugging;
#ifdef QT_SCRIPTTOOLS_LIB
    QScriptEngineDebugger *debugger;
#endif

    struct ImportedNamespace;
    QmlContextScriptClass *contextClass;
    QmlObjectScriptClass *objectClass;
    QmlValueTypeScriptClass *valueTypeClass;
    QmlTypeNameScriptClass *typeNameClass;
    QmlListScriptClass *listClass;
    // Global script class
    QScriptClass *globalClass;
    // Used by DOM Core 3 API
    QScriptClass *nodeListClass;
    QScriptClass *namedNodeMapClass;
    // Used by SQL database API
    QScriptClass *sqlQueryClass;

    struct QmlScriptEngine : public QScriptEngine
    {
        QmlScriptEngine(QmlEnginePrivate *priv)
            : p(priv) {}
        QmlEnginePrivate *p;
    };
    QmlScriptEngine scriptEngine;

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

    static void clear(SimpleList<QmlAbstractBinding> &);
    static void clear(SimpleList<QmlParserStatus> &);

    QList<SimpleList<QmlAbstractBinding> > bindValues;
    QList<SimpleList<QmlParserStatus> > parserStatus;
    QmlComponentAttached *componentAttacheds;

    QmlComponent *rootComponent;
    mutable QNetworkAccessManager *networkAccessManager;

    QmlCompositeTypeManager typeManager;
    QStringList fileImportPath;
    QString offlineStoragePath;

    mutable quint32 uniqueId;
    quint32 getUniqueId() const {
        return uniqueId++;
    }

    QmlValueTypeFactory valueTypes;

    QHash<const QMetaObject *, QmlPropertyCache *> propertyCache;
    QmlPropertyCache *cache(QObject *obj) { 
        Q_Q(QmlEngine);
        if (!obj || QObjectPrivate::get(obj)->metaObject) return 0;
        const QMetaObject *mo = obj->metaObject();
        QmlPropertyCache *rv = propertyCache.value(mo);
        if (!rv) {
            rv = QmlPropertyCache::create(q, mo);
            propertyCache.insert(mo, rv);
        }
        return rv;
    }

    // ### This whole class is embarrassing
    struct Imports {
        Imports();
        ~Imports();
        Imports(const Imports &copy);
        Imports &operator =(const Imports &copy);

        void setBaseUrl(const QUrl& url);
        QUrl baseUrl() const;

        QmlTypeNameCache *cache(QmlEngine *) const;

    private:
        friend class QmlEnginePrivate;
        QmlImportsPrivate *d;
    };

    bool addToImport(Imports*, const QString& uri, const QString& prefix, int vmaj, int vmin, QmlScriptParser::Import::Type importType) const;
    bool resolveType(const Imports&, const QByteArray& type,
                     QmlType** type_return, QUrl* url_return,
                     int *version_major, int *version_minor,
                     ImportedNamespace** ns_return) const;
    void resolveTypeInNamespace(ImportedNamespace*, const QByteArray& type,
                                QmlType** type_return, QUrl* url_return,
                                int *version_major, int *version_minor ) const;


    void registerCompositeType(QmlCompiledData *);
    bool isQmlList(int) const;
    bool isObject(int);
    int qmlListType(int) const;
    const QMetaObject *rawMetaObjectForType(int) const;
    const QMetaObject *metaObjectForType(int) const;
    QHash<int, int> m_qmlLists;
    QHash<int, QmlCompiledData *> m_compositeTypes;

    QScriptValue scriptValueFromVariant(const QVariant &);
    QVariant scriptValueToVariant(const QScriptValue &);

    static QScriptValue qmlScriptObject(QObject*, QmlEngine*);
    static QScriptValue createComponent(QScriptContext*, QScriptEngine*);
    static QScriptValue createQmlObject(QScriptContext*, QScriptEngine*);
    static QScriptValue vector(QScriptContext*, QScriptEngine*);
    static QScriptValue rgba(QScriptContext*, QScriptEngine*);
    static QScriptValue hsla(QScriptContext*, QScriptEngine*);
    static QScriptValue point(QScriptContext*, QScriptEngine*);
    static QScriptValue size(QScriptContext*, QScriptEngine*);
    static QScriptValue rect(QScriptContext*, QScriptEngine*);

    static QScriptValue lighter(QScriptContext*, QScriptEngine*);
    static QScriptValue darker(QScriptContext*, QScriptEngine*);
    static QScriptValue tint(QScriptContext*, QScriptEngine*);

    static QScriptValue playSound(QScriptContext*, QScriptEngine*);

    static QScriptEngine *getScriptEngine(QmlEngine *e) { return &e->d_func()->scriptEngine; }
    static QmlEngine *getEngine(QScriptEngine *e) { return static_cast<QmlScriptEngine*>(e)->p->q_func(); }
    static QmlEnginePrivate *get(QmlEngine *e) { return e->d_func(); }
    static QmlEnginePrivate *get(QScriptEngine *e) { return static_cast<QmlScriptEngine*>(e)->p; }
};


class QmlScriptClass : public QScriptClass
{
public:
    QmlScriptClass(QmlEngine *);

    static QVariant toVariant(QmlEngine *, const QScriptValue &);
protected:
    QmlEngine *engine;
};

QT_END_NAMESPACE

#endif // QMLENGINE_P_H
