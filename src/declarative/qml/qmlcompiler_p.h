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

#ifndef QMLCOMPILER_P_H
#define QMLCOMPILER_P_H

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

#include <QtCore/qbytearray.h>
#include <QtCore/qset.h>
#include <QtDeclarative/qml.h>
#include <QtDeclarative/qmlerror.h>
#include <private/qmlinstruction_p.h>
#include <private/qmlcompositetypemanager_p.h>
#include <private/qmlparser_p.h>

QT_BEGIN_NAMESPACE

class QmlEngine;
class QmlComponent;
class QmlCompiledComponent;
class QmlContext;

class QmlCompiledData 
{
public:
    QmlCompiledData();
    QmlCompiledData(const QmlCompiledData &other);
    QmlCompiledData &operator=(const QmlCompiledData &other);
    virtual ~QmlCompiledData();

    QByteArray name;
    QUrl url;

    struct TypeReference 
    {
        TypeReference()
        : type(0), component(0), ref(0) {}

        QByteArray className;
        QmlType *type;
        QmlComponent *component;

        QmlRefCount *ref;
        QObject *createInstance(QmlContext *) const;
        const QMetaObject *metaObject() const;
    };
    QList<TypeReference> types;
    struct CustomTypeData
    {
        int index;
        int type;
    };
    QAbstractDynamicMetaObject root;
    QList<QString> primitives;
    QList<float> floatData;
    QList<int> intData;
    QList<CustomTypeData> customTypeData;
    QList<QByteArray> datas;
    QList<QMetaObject *> synthesizedMetaObjects;
    QList<QmlParser::Location> locations;
    QList<QmlInstruction> bytecode;

private:
    QByteArray packData;
    friend class QmlCompiler;
    int pack(const char *, size_t);

    int indexForString(const QString &);
    int indexForByteArray(const QByteArray &);
    int indexForFloat(float *, int);
    int indexForInt(int *, int);
    int indexForLocation(const QmlParser::Location &);
    int indexForLocation(const QmlParser::LocationSpan &);
};

class QMetaObjectBuilder;
class Q_DECLARATIVE_EXPORT QmlCompiler 
{
public:
    QmlCompiler();

    bool compile(QmlEngine *, QmlCompositeTypeData *, QmlCompiledComponent *);

    bool isError() const;
    QList<QmlError> errors() const;

    static bool isValidId(const QString &);
    static bool isAttachedPropertyName(const QByteArray &);
    static bool isSignalPropertyName(const QByteArray &);

private:
    void reset(QmlCompiledComponent *, bool);

    struct BindingContext {
        BindingContext()
            : stack(0), object(0) {}
        BindingContext(QmlParser::Object *o)
            : stack(0), object(o) {}
        BindingContext incr() const {
            BindingContext rv(object);
            rv.stack = stack + 1;
            return rv;
        }
        bool isSubContext() const { return stack != 0; }
        int stack;
        QmlParser::Object *object;
    };

    void compileTree(QmlParser::Object *tree);


    bool buildObject(QmlParser::Object *obj, const BindingContext &);
    bool buildComponent(QmlParser::Object *obj, const BindingContext &);
    bool buildSubObject(QmlParser::Object *obj, const BindingContext &);
    bool buildSignal(QmlParser::Property *prop, QmlParser::Object *obj, 
                     const BindingContext &);
    bool buildProperty(QmlParser::Property *prop, QmlParser::Object *obj, 
                       const BindingContext &);
    bool buildIdProperty(QmlParser::Property *prop, QmlParser::Object *obj);
    bool buildAttachedProperty(QmlParser::Property *prop, 
                               QmlParser::Object *obj,
                               const BindingContext &ctxt);
    bool buildGroupedProperty(QmlParser::Property *prop,
                              QmlParser::Object *obj,
                              const BindingContext &ctxt);
    bool buildListProperty(QmlParser::Property *prop,
                           QmlParser::Object *obj,
                           const BindingContext &ctxt);
    bool buildPropertyAssignment(QmlParser::Property *prop,
                                 QmlParser::Object *obj,
                                 const BindingContext &ctxt);
    bool buildPropertyObjectAssignment(QmlParser::Property *prop,
                                       QmlParser::Object *obj,
                                       QmlParser::Value *value,
                                       const BindingContext &ctxt);
    bool buildPropertyLiteralAssignment(QmlParser::Property *prop,
                                        QmlParser::Object *obj,
                                        QmlParser::Value *value,
                                        const BindingContext &ctxt);
    bool testProperty(QmlParser::Property *prop, QmlParser::Object *obj);
    bool testLiteralAssignment(const QMetaProperty &prop, 
                               QmlParser::Value *value);
    enum DynamicMetaMode { IgnoreAliases, ResolveAliases };
    bool mergeDynamicMetaProperties(QmlParser::Object *obj);
    bool buildDynamicMeta(QmlParser::Object *obj, DynamicMetaMode mode);
    bool checkDynamicMeta(QmlParser::Object *obj);
    bool buildBinding(QmlParser::Value *, QmlParser::Property *prop,
                      const BindingContext &ctxt);
    bool buildComponentFromRoot(QmlParser::Object *obj, const BindingContext &);
    bool compileAlias(QMetaObjectBuilder &, 
                      QByteArray &data,
                      QmlParser::Object *obj, 
                      const QmlParser::Object::DynamicProperty &);
    bool completeComponentBuild();


    void genObject(QmlParser::Object *obj);
    void genObjectBody(QmlParser::Object *obj);
    void genComponent(QmlParser::Object *obj);
    void genValueProperty(QmlParser::Property *prop, QmlParser::Object *obj);
    void genListProperty(QmlParser::Property *prop, QmlParser::Object *obj);
    void genPropertyAssignment(QmlParser::Property *prop, 
                               QmlParser::Object *obj);
    void genLiteralAssignment(const QMetaProperty &prop, 
                              QmlParser::Value *value);
    void genBindingAssignment(QmlParser::Value *binding, 
                              QmlParser::Property *prop, 
                              QmlParser::Object *obj);


    int componentTypeRef();

    static int findSignalByName(const QMetaObject *, const QByteArray &name);
    static bool canCoerce(int to, QmlParser::Object *from);
    static QmlType *toQmlType(QmlParser::Object *from);

    QStringList deferredProperties(QmlParser::Object *);

    struct IdReference {
        QString id;
        QmlParser::Object *object;
        int instructionIdx;
        int idx;
    };
    void addId(const QString &, QmlParser::Object *);

    struct AliasReference {
        QmlParser::Object *object;
        int instructionIdx;
    };

    struct BindingReference {
        QmlParser::Variant expression;
        QmlParser::Property *property;
        QmlParser::Value *value;
        QByteArray compiledData;
        int instructionIdx;
        BindingContext bindingContext;
    };
    void addBindingReference(const BindingReference &);

    struct ComponentCompileState
    {
        ComponentCompileState() 
            : parserStatusCount(0), savedObjects(0), 
              pushedProperties(0), root(0) {}
        QHash<QString, IdReference> ids;
        int parserStatusCount;
        int savedObjects;
        int pushedProperties;
        QList<BindingReference> bindings;
        QHash<QmlParser::Value *, int> bindingMap;
        QList<AliasReference> aliases;
        QmlParser::Object *root;
    };
    ComponentCompileState compileState;

    void saveComponentState();
    ComponentCompileState componentState(QmlParser::Object *);
    QHash<QmlParser::Object *, ComponentCompileState> savedCompileStates;

    QList<QmlError> exceptions;
    QmlCompiledData *output;
};
QT_END_NAMESPACE

#endif // QMLCOMPILER_P_H
