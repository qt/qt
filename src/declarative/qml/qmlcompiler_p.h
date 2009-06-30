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

class QStringList;

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
    friend class QmlCompiler;
    int indexForString(const QString &);
    int indexForByteArray(const QByteArray &);
    int indexForFloat(float *, int);
    int indexForInt(int *, int);
    int indexForLocation(const QmlParser::Location &);
    int indexForLocation(const QmlParser::LocationSpan &);
};

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
        int stack;
        QmlParser::Object *object;
    };

    void compileTree(QmlParser::Object *tree);
    bool compileObject(QmlParser::Object *obj, const BindingContext &);
    bool compileComponent(QmlParser::Object *obj, const BindingContext &);
    bool compileComponentFromRoot(QmlParser::Object *obj, const BindingContext &);
    bool compileFetchedObject(QmlParser::Object *obj, const BindingContext &);
    bool compileSignal(QmlParser::Property *prop, QmlParser::Object *obj);
    bool testProperty(QmlParser::Property *prop, QmlParser::Object *obj);
    int signalByName(const QMetaObject *, const QByteArray &name);
    bool compileProperty(QmlParser::Property *prop, QmlParser::Object *obj, const BindingContext &);
    bool compileIdProperty(QmlParser::Property *prop, 
                           QmlParser::Object *obj);
    bool compileAttachedProperty(QmlParser::Property *prop, 
                                 const BindingContext &ctxt);
    bool compileNestedProperty(QmlParser::Property *prop,
                               const BindingContext &ctxt);
    bool compileListProperty(QmlParser::Property *prop,
                             QmlParser::Object *obj,
                             const BindingContext &ctxt);
    bool compilePropertyAssignment(QmlParser::Property *prop,
                                   QmlParser::Object *obj,
                                   const BindingContext &ctxt);
    bool compilePropertyObjectAssignment(QmlParser::Property *prop,
                                         QmlParser::Value *value,
                                         const BindingContext &ctxt);
    bool compilePropertyLiteralAssignment(QmlParser::Property *prop,
                                          QmlParser::Object *obj,
                                          QmlParser::Value *value,
                                          const BindingContext &ctxt);
    bool compileStoreInstruction(QmlInstruction &instr, 
                                 const QMetaProperty &prop, 
                                 QmlParser::Value *value);

    bool compileDynamicMeta(QmlParser::Object *obj);
    bool compileBinding(QmlParser::Value *, QmlParser::Property *prop,
                        const BindingContext &ctxt);

    void finalizeComponent(int patch);
    struct BindingReference;
    void finalizeBinding(const BindingReference &); 

    bool canConvert(int, QmlParser::Object *);
    QStringList deferredProperties(QmlParser::Object *);

    struct IdReference {
        QString id;
        QmlParser::Object *object;
        int instructionIdx;
        int idx;
    };

    struct BindingReference {
        QmlParser::Variant expression;
        QmlParser::Property *property;
        QmlParser::Value *value;
        int instructionIdx;
        BindingContext bindingContext;
    };

    struct ComponentCompileState
    {
        ComponentCompileState() : parserStatusCount(0), savedObjects(0), pushedProperties(0), root(0) {}
        QHash<QString, IdReference> ids;
        int parserStatusCount;
        int savedObjects;
        int pushedProperties;
        QList<BindingReference> bindings;
        QmlParser::Object *root;
    };
    ComponentCompileState compileState;

    QList<QmlError> exceptions;
    QmlCompiledData *output;

};

QT_END_NAMESPACE

#endif // QMLCOMPILER_P_H
