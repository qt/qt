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

#include "qmlbindingvme_p.h"
#include <private/qmlcontext_p.h>
#include <private/qmljsast_p.h>
#include <private/qmljsengine_p.h>
#include <QtCore/qdebug.h>
#include <private/qmlgraphicsanchors_p_p.h>

QT_BEGIN_NAMESPACE

using namespace QmlJS;

namespace {
// Supported types: int, qreal, QString (needs constr/destr), QObject*, bool
struct Register {
    void setQObject(QObject *o) { *((QObject **)data) = o; }
    QObject *getQObject() { return *((QObject **)data); }

    void setqreal(qreal v) { *((qreal *)data) = v; }
    qreal getqreal() { return *((qreal *)data); }

    void setint(int v) { *((int *)data) = v; }
    int getint() { return *((int *)data); }

    void setbool(bool v) { *((bool *)data) = v; }
    bool getbool() { return *((bool *)data); }

    QVariant *getvariantptr() { return (QVariant *)typeDataPtr(); }
    QString *getstringptr() { return (QString *)typeDataPtr(); }
    QUrl *geturlptr() { return (QUrl *)typeDataPtr(); }

    void *typeDataPtr() { return (void *)&data; }
    void *typeMemory() { return (void *)data; }

    void *data[2];     // Object stored here
};

struct Instr {
    enum {
        Noop,

        Init,                    // init

        Subscribe,               // subscribe
        SubscribeId,             // subscribe

        LoadId,                  // load
        LoadScope,               // load
        LoadRoot,                // load
        LoadAttached,            // attached

        ConvertIntToReal,        // unaryop
        ConvertRealToInt,        // unaryop

        Real,                    // real_value
        Int,                     // int_value
        Bool,                    // bool_value

        AddReal,                 // binaryop
        AddInt,                  // binaryop

        MinusReal,               // binaryop
        MinusInt,                // binaryop

        CompareReal,             // binaryop
        CompareString,           // binaryop

        NotCompareReal,          // binaryop
        NotCompareString,        // binaryop
        
        GreaterThanReal,         // binaryop

        NewString,               // construct
        NewUrl,                  // construct

        CleanupUrl,              // cleanup
        CleanupString,           // cleanup

        Copy,                    // copy
        Fetch,                   // fetch
        Store,                   // store

        Skip,                    // skip

        Done,

        // Speculative property resolution
        InitString,              // initstring
        FindGeneric,             // find 
        FindGenericTerminal,     // find 
        FindProperty,            // find 
        FindPropertyTerminal,    // find 
        CleanupGeneric,          // cleanup
        ConvertGenericToReal,    // genericunaryop
        ConvertGenericToBool,    // genericunaryop
        ConvertGenericToString,  // genericunaryop
        ConvertGenericToUrl,     // genericunaryop

    } type;

    union {
        struct {
            int subscriptions;
            int identifiers;
        } init;
        struct {
            int offset;
            int reg;
            int index;
        } subscribe;
        struct {
            int index;
            int reg;
        } load;
        struct {
            int output;
            int reg;
            int index;
        } attached;
        struct {
            int output;
            int reg;
            int index;
        } store;
        struct {
            int output;
            int objectReg;
            int index;
        } fetch;
        struct {
            int reg;
            int src;
        } copy;
        struct {
            int reg;
        } construct;
        struct {
            int reg;
            qreal value;
        } real_value;
        struct {
            int reg;
            int value;
        } int_value;
        struct {
            int reg;
            bool value;
        } bool_value;
        struct {
            int output;
            int src1;
            int src2;
        } binaryop;
        struct {
            int output;
            int src;
        } unaryop;
        struct {
            int output;
            int src;
            int srcType;
        } genericunaryop;
        struct {
            int reg;
            int count;
        } skip;
        struct {
            int reg;
            int src;
            int typeReg;
            int name; 
            int subscribeIndex;
        } find;
        struct {
            int reg;
            int typeReg;
        } cleanup;
        struct {
            int offset;
            int dataIdx;
            int length;
        } initstring;
    };
};

struct Program {
    int dataLength;
    const char *data() const { return ((const char *)this) + sizeof(Program); }
    const Instr *instructions() const { return (const Instr *)(data() + dataLength); }
};
}

struct QmlBindingCompiler
{
    struct Result {
        Result() : unknownType(false), metaObject(0), type(-1), reg(-1) {}
        bool operator==(const Result &o) const { 
            return unknownType == o.unknownType &&
                   metaObject == o.metaObject && 
                   type == o.type &&
                   reg == o.reg; 
        }
        bool operator!=(const Result &o) const { 
            return !(*this == o);
        }
        bool unknownType;
        const QMetaObject *metaObject;
        int type;
        int reg;

        QSet<QString> subscriptionSet;
    };

    QmlBindingCompiler() : registers(0), strings(0) {}
    void reset();

    QmlParser::Object *context;
    QmlParser::Object *component;
    QmlParser::Property *destination;
    QHash<QString, QmlParser::Object *> ids;
    QmlEnginePrivate::Imports imports;
    QmlEnginePrivate *engine;

    bool compile(QmlJS::AST::Node *);

    bool parseExpression(QmlJS::AST::Node *, Result &);

    bool tryName(QmlJS::AST::Node *);
    bool parseName(QmlJS::AST::Node *, Result &);

    bool tryArith(QmlJS::AST::Node *);
    bool parseArith(QmlJS::AST::Node *, Result &);

    bool tryLogic(QmlJS::AST::Node *);
    bool parseLogic(QmlJS::AST::Node *, Result &);

    bool tryConditional(QmlJS::AST::Node *);
    bool parseConditional(QmlJS::AST::Node *, Result &);

    bool tryConstant(QmlJS::AST::Node *);
    bool parseConstant(QmlJS::AST::Node *, Result &);

    bool buildName(QStringList &, QmlJS::AST::Node *);
    bool fetch(Result &type, const QMetaObject *, int reg, int idx, const QStringList &);

    quint32 registers;
    QHash<int, QPair<int, int> > registerCleanups;
    int acquireReg(int cleanup = Instr::Noop, int cleanupType = 0);
    void registerCleanup(int reg, int cleanup, int cleanupType = 0);
    void releaseReg(int);

    int registerString(const QString &);
    int strings;
    QByteArray data;

    QSet<QString> subscriptionSet;
    QHash<QString, int> subscriptionIds;
    bool subscription(const QStringList &, Result *);
    int subscriptionIndex(const QStringList &);
    bool subscriptionNeutral(const QSet<QString> &base, const QSet<QString> &lhs, const QSet<QString> &rhs);
    QVector<Instr> bytecode;
};

QByteArray QmlBindingVME::compile(const QmlBasicScript::Expression &expression, QmlEnginePrivate *engine)
{
    if (!expression.expression.asAST()) return false;

    QmlBindingCompiler bsc;
    bsc.context = expression.context;
    bsc.component = expression.component;
    bsc.destination = expression.property;
    bsc.ids = expression.ids;
    bsc.imports = expression.imports;
    bsc.engine = engine;

    bool ok = bsc.compile(expression.expression.asAST());

    if (ok) {
        Program prog;
        prog.dataLength = 4 * ((bsc.data.size() + 3) / 4);
        int size = sizeof(Program) + bsc.bytecode.count() * sizeof(Instr);
        size += prog.dataLength;

        QByteArray data;
        data.resize(size);
        memcpy(data.data(), &prog, sizeof(Program));
        if (prog.dataLength)
            memcpy((char *)((Program *)data.data())->data(), bsc.data.constData(),
                   bsc.data.size());
        memcpy((char *)((Program *)data.data())->instructions(), bsc.bytecode.constData(), 
               bsc.bytecode.count() * sizeof(Instr));
        return data;
    } else {
        return QByteArray();
    }
}

inline void subscribe(QObject *o, int notifyIndex, QmlBindingVME::Config::Subscription *s, 
                      QmlBindingVME::Config *config)
{
    if (o != s->source || notifyIndex != s->notifyIndex)  {
        if (s->source)
            QMetaObject::disconnect(s->source, s->notifyIndex, 
                                    config->target, config->targetSlot);
        s->source = o;
        s->notifyIndex = notifyIndex;
        if (s->source && s->notifyIndex != -1)
            QMetaObject::connect(s->source, s->notifyIndex, config->target, 
                                 config->targetSlot, Qt::DirectConnection);
    } 
}

static QObject *variantToQObject(const QVariant &value, bool *ok)
{
    *ok = false;
    Q_UNUSED(value);
    return 0;
}

static QmlPropertyCache::Data *findproperty(QObject *obj, 
                                            const QScriptDeclarativeClass::Identifier &name,
                                            QmlEnginePrivate *enginePriv,
                                            QmlPropertyCache::Data &local)
{
    QmlPropertyCache *cache = 0;
    QmlDeclarativeData *ddata = QmlDeclarativeData::get(obj);
    if (ddata)
        cache = ddata->propertyCache;
    if (!cache) {
        cache = enginePriv->cache(obj);
        if (cache && ddata) { cache->addref(); ddata->propertyCache = cache; }
    }

    QmlPropertyCache::Data *property = 0;

    if (cache) {
        property = cache->property(name);
    } else {
        qWarning() << "QmlBindingVME: Slow search" << enginePriv->objectClass->toString(name);
        local = QmlPropertyCache::create(obj->metaObject(),  
                                         enginePriv->objectClass->toString(name));
        if (local.isValid())
            property = &local;
    }

    return property;
}

static bool findproperty(QObject *obj, 
                         Register *output, Register *type, 
                         QmlEnginePrivate *enginePriv,
                         QmlBindingVME::Config *config, int subIdx,
                         const QScriptDeclarativeClass::Identifier &name,
                         bool isTerminal)
{
    if (!obj)
        return false;

    QmlPropertyCache::Data local;
    QmlPropertyCache::Data *property = findproperty(obj, name, enginePriv, local);

    if (property) {
        if (property->flags & QmlPropertyCache::Data::IsQObjectDerived) {
            void *args[] = { output->typeDataPtr(), 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, property->coreIndex, args);
            type->setint(QMetaType::QObjectStar);
        } else if (property->propType == qMetaTypeId<QVariant>()) {
            QVariant v;
            void *args[] = { &v, 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, property->coreIndex, args);

            if (isTerminal) {
                new (output->typeDataPtr()) QVariant(v);
                type->setint(qMetaTypeId<QVariant>());
            } else {
                bool ok;
                output->setQObject(variantToQObject(v, &ok));
                if (!ok) return false;
                type->setint(QMetaType::QObjectStar);
            }

        } else {
            if (!isTerminal)
                return false;

            if (property->propType == QMetaType::QReal) {
                void *args[] = { output->typeDataPtr(), 0 };
                QMetaObject::metacall(obj, QMetaObject::ReadProperty, property->coreIndex, args);
                type->setint(QMetaType::QReal);
            } else {
                new (output->typeDataPtr()) 
                    QVariant(obj->metaObject()->property(property->coreIndex).read(obj));
                type->setint(qMetaTypeId<QVariant>());
            }

        }

        if (subIdx != -1)
            subscribe(obj, property->notifyIndex, config->subscriptions + subIdx, config);

        return true;
    } else {
        return false;
    }
}

static bool findproperty(Register *input, 
                         Register *output, Register *type, 
                         QmlEnginePrivate *enginePriv,
                         QmlBindingVME::Config *config, int subIdx,
                         const QScriptDeclarativeClass::Identifier &name,
                         bool isTerminal)
{
    return findproperty(input->getQObject(), output, type, enginePriv, 
                        config, subIdx, name, isTerminal);
}

static bool findgeneric(Register *output,                                 // value output
                        Register *type,                                   // value type output
                        QmlBindingVME::Config *config,           
                        int subIdx,                                       // Subscription index in config
                        QmlContextPrivate *context,                       // Context to search in
                        const QScriptDeclarativeClass::Identifier &name, 
                        bool isTerminal)
{
    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(context->engine);

    while (context) {

        int contextPropertyIndex = context->propertyNames?context->propertyNames->value(name):-1;
        if (contextPropertyIndex != -1) {

            if (contextPropertyIndex < context->idValueCount) {
                output->setQObject(context->idValues[contextPropertyIndex]);
                type->setint(QMetaType::QObjectStar);
            } else {
                const QVariant &value = context->propertyValues.at(contextPropertyIndex);
                if (isTerminal) {
                    new (output->typeDataPtr()) QVariant(value);
                    type->setint(qMetaTypeId<QVariant>());
                } else {
                    bool ok;
                    output->setQObject(variantToQObject(value, &ok));
                    if (!ok) return false;
                    type->setint(QMetaType::QObjectStar);
                }
            }

            return true;
        }

        for (int ii = 0; ii < context->scripts.count(); ++ii) {
            QScriptValue function = QScriptDeclarativeClass::function(context->scripts.at(ii), name);
            if (function.isValid()) {
                qFatal("Binding optimizer resolved name to QScript method");
                return false;
            }
        }

        if (QObject *root = context->defaultObjects.isEmpty()?0:context->defaultObjects.first()) {

            if (findproperty(root, output, type, enginePriv, config, subIdx, name, isTerminal))
                return true;

        }

        if (context->parent) {
            context = QmlContextPrivate::get(context->parent);
        } else {
            context = 0;
        }
    }

    return false;
}


// Conversion functions - these MUST match the QtScript expression path
inline static qreal toReal(Register *reg, int type, bool *ok = 0)
{
    if (ok) *ok = true;

    if (type == QMetaType::QReal) {
        return reg->getqreal();
    } else if (type == qMetaTypeId<QVariant>()) {
        *ok = true;
        return reg->getvariantptr()->toReal();
    } else {
        if (ok) *ok = false;
        return 0;
    }
}

inline static QString toString(Register *reg, int type, bool *ok = 0)
{
    if (ok) *ok = true;

    if (type == QMetaType::QReal) {
        return QString::number(reg->getqreal());
    } else if (type == qMetaTypeId<QVariant>()) {
        return reg->getvariantptr()->toString();
    } else {
        if (ok) *ok = false;
        return QString();
    }
}

inline static bool toBool(Register *reg, int type, bool *ok = 0)
{
    if (ok) *ok = true;

    if (type == QMetaType::Bool) {
        return reg->getbool();
    } else if (type == qMetaTypeId<QVariant>()) {
        return reg->getvariantptr()->toBool();
    } else {
        if (ok) *ok = false;
        return false;
    }
}

inline static QUrl toUrl(Register *reg, int type, QmlContextPrivate *context, bool *ok = 0)
{
    if (ok) *ok = true;

    QUrl base;
    if (type == qMetaTypeId<QVariant>()) {
        QVariant *var = reg->getvariantptr();
        int vt = var->type();
        if (vt == QVariant::Url) {
            base = var->toUrl();
        } else if (vt == QVariant::ByteArray) {
            base = QUrl(QString::fromUtf8(var->toByteArray()));
        } else if (vt == QVariant::String) {
            base = QUrl(var->toString());
        } else {
            if (ok) *ok = false;
            return QUrl();
        }
    } else {
        if (ok) *ok = false;
        return QUrl();
    }

    if (base.isRelative())
        return context->url.resolved(base);
    else
        return base;
}

void QmlBindingVME::run(const char *programData, Config *config, 
                        QmlContextPrivate *context, 
                        QObject **scopes, QObject **outputs)
{
    Register registers[32];
    int storeFlags = 0;

    QmlEnginePrivate *engine = QmlEnginePrivate::get(context->engine);
    Program *program = (Program *)programData;
    const Instr *instr = program->instructions();
    const char *data = program->data();

    while (instr) {

    switch (instr->type) {
    case Instr::Noop:
        break;
    case Instr::Init:
        if (!config->subscriptions && instr->init.subscriptions)
            config->subscriptions = new Config::Subscription[instr->init.subscriptions];
        if (!config->identifiers && instr->init.identifiers)
            config->identifiers = new QScriptDeclarativeClass::PersistentIdentifier[instr->init.identifiers];
        break;

    case Instr::SubscribeId:
    case Instr::Subscribe:
        {
            QObject *o = registers[instr->subscribe.reg].getQObject();
            Config::Subscription *s = config->subscriptions + instr->subscribe.offset;
            int notifyIndex = instr->subscribe.index;

            if (instr->type == Instr::SubscribeId) {
                o = QmlContextPrivate::get(context);
                notifyIndex += context->notifyIndex;
            }

            subscribe(o, instr->subscribe.index, s, config);
        }
        break;

    case Instr::LoadId:
        registers[instr->load.reg].setQObject(context->idValues[instr->load.index].data());
        break;

    case Instr::LoadScope:
        registers[instr->load.reg].setQObject(scopes[instr->load.index]);
        break;

    case Instr::LoadRoot:
        registers[instr->load.reg].setQObject(context->defaultObjects.at(0));
        break;

    case Instr::LoadAttached:
        {
            QObject *o = qmlAttachedPropertiesObjectById(instr->attached.index, 
                                                         registers[instr->attached.reg].getQObject(), 
                                                         true);
            registers[instr->attached.output].setQObject(o);
        }
        break;

    case Instr::ConvertIntToReal:
        registers[instr->unaryop.output].setqreal(qreal(registers[instr->unaryop.src].getint()));
        break;

    case Instr::ConvertRealToInt:
        registers[instr->unaryop.output].setint(int(registers[instr->unaryop.src].getqreal()));
        break;

    case Instr::Real:
        registers[instr->real_value.reg].setqreal(instr->real_value.value);
        break;

    case Instr::Int:
        registers[instr->int_value.reg].setint(instr->int_value.value);
        break;
        
    case Instr::Bool:
        registers[instr->bool_value.reg].setbool(instr->bool_value.value);
        break;

    case Instr::AddReal:
        registers[instr->binaryop.output].setqreal(registers[instr->binaryop.src1].getqreal() + 
                                                   registers[instr->binaryop.src2].getqreal());
        break;

    case Instr::AddInt:
        registers[instr->binaryop.output].setint(registers[instr->binaryop.src1].getint() + 
                                                 registers[instr->binaryop.src2].getint());
        break;

    case Instr::MinusReal:
        registers[instr->binaryop.output].setqreal(registers[instr->binaryop.src1].getqreal() - 
                                                   registers[instr->binaryop.src2].getqreal());
        break;

    case Instr::MinusInt:
        registers[instr->binaryop.output].setint(registers[instr->binaryop.src1].getint() - 
                                                 registers[instr->binaryop.src2].getint());
        break;

    case Instr::CompareReal:
        registers[instr->binaryop.output].setbool(registers[instr->binaryop.src1].getqreal() ==
                                                  registers[instr->binaryop.src2].getqreal());
        break;

    case Instr::CompareString:
        registers[instr->binaryop.output].setbool(*registers[instr->binaryop.src1].getstringptr() ==
                                                  *registers[instr->binaryop.src2].getstringptr());
        break;

    case Instr::NotCompareReal:
        registers[instr->binaryop.output].setbool(registers[instr->binaryop.src1].getqreal() !=
                                                  registers[instr->binaryop.src2].getqreal());
        break;

    case Instr::NotCompareString:
        registers[instr->binaryop.output].setbool(*registers[instr->binaryop.src1].getstringptr() !=
                                                  *registers[instr->binaryop.src2].getstringptr());
        break;

    case Instr::GreaterThanReal:
        registers[instr->binaryop.output].setbool(registers[instr->binaryop.src1].getqreal() > 
                                                  registers[instr->binaryop.src2].getqreal());
        break;

    case Instr::NewString:
        new (registers[instr->construct.reg].typeMemory()) QString;
        break;

    case Instr::NewUrl:
        new (registers[instr->construct.reg].typeMemory()) QUrl;
        break;

    case Instr::CleanupString:
        registers[instr->cleanup.reg].getstringptr()->~QString();
        break;

    case Instr::CleanupUrl:
        registers[instr->cleanup.reg].geturlptr()->~QUrl();
        break;

    case Instr::Fetch:
        {
            QObject *object = registers[instr->fetch.objectReg].getQObject();
            if (!object) {
                qWarning() << "ERROR - Fetch";
                return; 
            }
            void *argv[] = { registers[instr->fetch.output].typeDataPtr(), 0 };
            QMetaObject::metacall(object, QMetaObject::ReadProperty, instr->fetch.index, argv);
        }
        break;

    case Instr::Store:
        {
            int status = -1;
            void *argv[] = { registers[instr->store.reg].typeDataPtr(),
                             0, &status, &storeFlags };
            QMetaObject::metacall(outputs[instr->store.output], QMetaObject::WriteProperty, 
                                  instr->store.index, argv);
        }
        break;

    case Instr::Copy:
        registers[instr->copy.reg] = registers[instr->copy.src];
        break;

    case Instr::Skip:
        if (instr->skip.reg == -1 || !registers[instr->skip.reg].getbool()) 
            instr += instr->skip.count;
        break;

    case Instr::Done:
        return;

    case Instr::InitString:
        if (!config->identifiers[instr->initstring.offset].identifier) {
            QString str = QString::fromRawData((QChar *)(data + instr->initstring.dataIdx), 
                                               instr->initstring.length);
            config->identifiers[instr->initstring.offset] = 
                engine->objectClass->createPersistentIdentifier(str);
        }
        break;

    case Instr::FindGenericTerminal:
    case Instr::FindGeneric:
        // We start the search in the parent context, as we know that the 
        // name is not present in the current context or it would have been
        // found during the static compile
        if (!findgeneric(registers + instr->find.reg, registers + instr->find.typeReg,
                         config, instr->find.subscribeIndex, QmlContextPrivate::get(context->parent),
                         config->identifiers[instr->find.name].identifier, 
                         instr->type == Instr::FindGenericTerminal)) {
            qWarning() << "ERROR - FindGeneric*";
            return;
        }
        break;

    case Instr::FindPropertyTerminal:
    case Instr::FindProperty:
        if (!findproperty(registers + instr->find.src,
                          registers + instr->find.reg, registers + instr->find.typeReg,
                          QmlEnginePrivate::get(context->engine),
                          config, instr->find.subscribeIndex, 
                          config->identifiers[instr->find.name].identifier, 
                          instr->type == Instr::FindPropertyTerminal)) {
            qWarning() << "ERROR - FindProperty*";
            return;
        }
        break;

    case Instr::CleanupGeneric:
        {
            int type = registers[instr->cleanup.typeReg].getint();
            if (type == qMetaTypeId<QVariant>()) {
                ((QVariant *)registers[instr->cleanup.reg].typeDataPtr())->~QVariant();
            }
        }
        break;

    case Instr::ConvertGenericToReal:
        {
            int type = registers[instr->genericunaryop.srcType].getint();
            registers[instr->genericunaryop.output].setqreal(toReal(registers + instr->genericunaryop.src, type));
        }
        break;

    case Instr::ConvertGenericToBool:
        {
            int type = registers[instr->genericunaryop.srcType].getint();
            registers[instr->genericunaryop.output].setbool(toBool(registers + instr->genericunaryop.src, type));
        }
        break;

    case Instr::ConvertGenericToString:
        {
            int type = registers[instr->genericunaryop.srcType].getint();
            void *regPtr = registers[instr->genericunaryop.output].typeDataPtr();
            new (regPtr) QString(toString(registers + instr->genericunaryop.src, type));
        }
        break;

    case Instr::ConvertGenericToUrl:
        {
            int type = registers[instr->genericunaryop.srcType].getint();
            void *regPtr = registers[instr->genericunaryop.output].typeDataPtr();
            new (regPtr) QUrl(toUrl(registers + instr->genericunaryop.src, type, context));
        }
        break;

    default:
        qFatal("EEK");
        break;
    }

    instr++;
    }
}

void QmlBindingVME::dump(const QByteArray &programData)
{
    const Program *program = (const Program *)programData.constData();

    int count = (programData.size() - sizeof(Program) - program->dataLength) / sizeof(Instr);
    const Instr *instr = program->instructions();

    while (count--) {

        switch (instr->type) {
        case Instr::Noop:
            qWarning().nospace() << "Noop";
            break;
        case Instr::Init:
            qWarning().nospace() << "Init" << "\t\t\t" << instr->init.subscriptions << "\t" << instr->init.identifiers;
            break;
        case Instr::Subscribe:
            qWarning().nospace() << "Subscribe" << "\t\t" << instr->subscribe.offset << "\t" << instr->subscribe.reg << "\t" << instr->subscribe.index;
            break;
        case Instr::SubscribeId:
            qWarning().nospace() << "SubscribeId" << "\t\t" << instr->subscribe.offset << "\t" << instr->subscribe.reg << "\t" << instr->subscribe.index;
            break;
        case Instr::LoadId:
            qWarning().nospace() << "LoadId" << "\t\t\t" << instr->load.index << "\t" << instr->load.reg;
            break;
        case Instr::LoadScope:
            qWarning().nospace() << "LoadScope" << "\t\t" << instr->load.index << "\t" << instr->load.reg;
            break;
        case Instr::LoadRoot:
            qWarning().nospace() << "LoadRoot" << "\t\t" << instr->load.index << "\t" << instr->load.reg;
            break;
        case Instr::LoadAttached:
            qWarning().nospace() << "LoadAttached" << "\t\t" << instr->attached.output << "\t" << instr->attached.reg << "\t" << instr->attached.index;
            break;
        case Instr::ConvertIntToReal:
            qWarning().nospace() << "ConvertIntToReal" << "\t" << instr->unaryop.output << "\t" << instr->unaryop.src;
            break;
        case Instr::ConvertRealToInt:
            qWarning().nospace() << "ConvertRealToInt" << "\t" << instr->unaryop.output << "\t" << instr->unaryop.src;
            break;
        case Instr::Real:
            qWarning().nospace() << "Real" << "\t\t\t" << instr->real_value.reg << "\t" << instr->real_value.value;
            break;
        case Instr::Int:
            qWarning().nospace() << "Int" << "\t\t\t" << instr->int_value.reg << "\t" << instr->int_value.value;
            break;
        case Instr::Bool:
            qWarning().nospace() << "Bool" << "\t\t\t" << instr->bool_value.reg << "\t" << instr->bool_value.value;
            break;
        case Instr::AddReal:
            qWarning().nospace() << "AddReal" << "\t\t\t" << instr->binaryop.output << "\t" << instr->binaryop.src1 << "\t" << instr->binaryop.src2;
            break;
        case Instr::AddInt:
            qWarning().nospace() << "AddInt" << "\t\t\t" << instr->binaryop.output << "\t" << instr->binaryop.src1 << "\t" << instr->binaryop.src2;
            break;
        case Instr::MinusReal:
            qWarning().nospace() << "MinusReal" << "\t\t" << instr->binaryop.output << "\t" << instr->binaryop.src1 << "\t" << instr->binaryop.src2;
            break;
        case Instr::MinusInt:
            qWarning().nospace() << "MinusInt" << "\t\t" << instr->binaryop.output << "\t" << instr->binaryop.src1 << "\t" << instr->binaryop.src2;
            break;
        case Instr::CompareReal:
            qWarning().nospace() << "CompareReal" << "\t\t" << instr->binaryop.output << "\t" << instr->binaryop.src1 << "\t" << instr->binaryop.src2;
            break;
        case Instr::CompareString:
            qWarning().nospace() << "CompareString" << "\t\t" << instr->binaryop.output << "\t" << instr->binaryop.src1 << "\t" << instr->binaryop.src2;
            break;
        case Instr::NotCompareReal:
            qWarning().nospace() << "NotCompareReal" << "\t\t" << instr->binaryop.output << "\t" << instr->binaryop.src1 << "\t" << instr->binaryop.src2;
            break;
        case Instr::NotCompareString:
            qWarning().nospace() << "NotCompareString" << "\t\t" << instr->binaryop.output << "\t" << instr->binaryop.src1 << "\t" << instr->binaryop.src2;
            break;
        case Instr::GreaterThanReal:
            qWarning().nospace() << "GreaterThanReal" << "\t\t" << instr->binaryop.output << "\t" << instr->binaryop.src1 << "\t" << instr->binaryop.src2;
            break;
        case Instr::NewString:
            qWarning().nospace() << "NewString" << "\t\t" << instr->construct.reg;
            break;
        case Instr::NewUrl:
            qWarning().nospace() << "NewUrl" << "\t\t\t" << instr->construct.reg;
            break;
        case Instr::CleanupString:
            qWarning().nospace() << "CleanupString" << "\t\t" << instr->cleanup.reg << "\t" << instr->cleanup.typeReg;
            break;
        case Instr::CleanupUrl:
            qWarning().nospace() << "CleanupUrl" << "\t\t" << instr->cleanup.reg << "\t" << instr->cleanup.typeReg;
            break;
        case Instr::Fetch:
            qWarning().nospace() << "Fetch" << "\t\t\t" << instr->fetch.output << "\t" << instr->fetch.index << "\t" << instr->fetch.objectReg;
            break;
        case Instr::Store:
            qWarning().nospace() << "Store" << "\t\t\t" << instr->store.output << "\t" << instr->store.index << "\t" << instr->store.reg;
            break;
        case Instr::Copy:
            qWarning().nospace() << "Copy" << "\t\t\t" << instr->copy.reg << "\t" << instr->copy.src;
            break;
        case Instr::Skip:
            qWarning().nospace() << "Skip" << "\t\t\t" << instr->skip.reg << "\t" << instr->skip.count;
            break;
        case Instr::Done:
            qWarning().nospace() << "Done";
            break;
        case Instr::InitString:
            qWarning().nospace() << "InitString" << "\t\t" << instr->initstring.offset << "\t" << instr->initstring.dataIdx << "\t" << instr->initstring.length;
            break;
        case Instr::FindGeneric:
            qWarning().nospace() << "FindGeneric" << "\t\t" << instr->find.reg << "\t" << instr->find.typeReg << "\t" << instr->find.name;
            break;
        case Instr::FindGenericTerminal:
            qWarning().nospace() << "FindGenericTerminal" << "\t" << instr->find.reg << "\t" << instr->find.typeReg << "\t" << instr->find.name;
            break;
        case Instr::FindProperty:
            qWarning().nospace() << "FindProperty" << "\t\t" << instr->find.reg << "\t" << instr->find.src << "\t" << instr->find.typeReg << "\t" << instr->find.name;
            break;
        case Instr::FindPropertyTerminal:
            qWarning().nospace() << "FindPropertyTerminal" << "\t" << instr->find.reg << "\t" << instr->find.src << "\t" << instr->find.typeReg << "\t" << instr->find.name;
            break;
        case Instr::CleanupGeneric:
            qWarning().nospace() << "CleanupGeneric" << "\t\t" << instr->cleanup.reg << "\t" << instr->cleanup.typeReg;
            break;
        case Instr::ConvertGenericToReal:
            qWarning().nospace() << "ConvertGenericToReal" << "\t" << instr->genericunaryop.output << "\t" << instr->genericunaryop.src << "\t" << instr->genericunaryop.srcType;
            break;
        case Instr::ConvertGenericToBool:
            qWarning().nospace() << "ConvertGenericToBool" << "\t" << instr->genericunaryop.output << "\t" << instr->genericunaryop.src << "\t" << instr->genericunaryop.srcType;
            break;
        case Instr::ConvertGenericToString:
            qWarning().nospace() << "ConvertGenericToString" << "\t" << instr->genericunaryop.output << "\t" << instr->genericunaryop.src << "\t" << instr->genericunaryop.srcType;
            break;
        case Instr::ConvertGenericToUrl:
            qWarning().nospace() << "ConvertGenericToUrl" << "\t" << instr->genericunaryop.output << "\t" << instr->genericunaryop.src << "\t" << instr->genericunaryop.srcType;
            break;
        default:
            qWarning().nospace() << "Unknown";
            break;
        }

        ++instr;
    }
}

void QmlBindingCompiler::reset()
{
    registers = 0;
    strings = 0;
    data.clear();
    subscriptionSet.clear();
    bytecode.clear();
}

bool QmlBindingCompiler::compile(QmlJS::AST::Node *node)
{
    reset();

    Result type;

    if (!parseExpression(node, type)) 
        return false;

    if (subscriptionSet.count() || strings) {
        if (subscriptionSet.count() > 0xFFFF ||
            strings > 0xFFFF)
            return false;

        Instr init;
        init.type = Instr::Init;
        init.init.subscriptions = subscriptionIds.count();
        init.init.identifiers = strings;
        bytecode.prepend(init);
    }

    if (type.unknownType) {
        if (destination->type != QMetaType::QReal &&
            destination->type != QVariant::String &&
            destination->type != QMetaType::Bool &&
            destination->type != QVariant::Url)
            return false;

        int convertReg = acquireReg();

        if (destination->type == QMetaType::QReal) {
            Instr convert;
            convert.type = Instr::ConvertGenericToReal;
            convert.genericunaryop.output = convertReg;
            convert.genericunaryop.src = type.reg;
            convert.genericunaryop.srcType = 2; // XXX
            bytecode << convert;
        } else if (destination->type == QVariant::String) {
            Instr convert;
            convert.type = Instr::ConvertGenericToString;
            convert.genericunaryop.output = convertReg;
            convert.genericunaryop.src = type.reg;
            convert.genericunaryop.srcType = 2; // XXX
            bytecode << convert;
        } else if (destination->type == QMetaType::Bool) {
            Instr convert;
            convert.type = Instr::ConvertGenericToBool;
            convert.genericunaryop.output = convertReg;
            convert.genericunaryop.src = type.reg;
            convert.genericunaryop.srcType = 2; // XXX
            bytecode << convert;
        } else if (destination->type == QVariant::Url) {
            Instr convert;
            convert.type = Instr::ConvertGenericToUrl;
            convert.genericunaryop.output = convertReg;
            convert.genericunaryop.src = type.reg;
            convert.genericunaryop.srcType = 2; // XXX
            bytecode << convert;
        }

        Instr cleanup;
        cleanup.type = Instr::CleanupGeneric;
        cleanup.cleanup.reg = type.reg;
        cleanup.cleanup.typeReg = 2; // XXX
        bytecode << cleanup;

        Instr instr;
        instr.type = Instr::Store;
        instr.store.output = 0;
        instr.store.index = destination->index;
        instr.store.reg = convertReg;
        bytecode << instr;

        if (destination->type == QVariant::String) {
            Instr cleanup;
            cleanup.type = Instr::CleanupString;
            cleanup.cleanup.reg = convertReg;
            cleanup.cleanup.typeReg = -1;
            bytecode << cleanup;
        } else if (destination->type == QVariant::Url) {
            Instr cleanup;
            cleanup.type = Instr::CleanupUrl;
            cleanup.cleanup.reg = convertReg;
            cleanup.cleanup.typeReg = -1;
            bytecode << cleanup;
        }

        releaseReg(convertReg);

        Instr done;
        done.type = Instr::Done;
        bytecode << done;

        return true;
    } else {
        // Can we store the final value?
        if (type.type == QVariant::Int &&
            destination->type == QMetaType::QReal) {
            Instr instr;
            instr.type = Instr::ConvertIntToReal;
            instr.unaryop.output = type.reg;
            instr.unaryop.src = type.reg;
            bytecode << instr;
            type.type = QMetaType::QReal;
        } else if (type.type == QMetaType::QReal &&
                   destination->type == QVariant::Int) {
            Instr instr;
            instr.type = Instr::ConvertRealToInt;
            instr.unaryop.output = type.reg;
            instr.unaryop.src = type.reg;
            bytecode << instr;
            type.type = QVariant::Int;
        } else if (type.type == destination->type) {
        } else {
            const QMetaObject *from = type.metaObject;
            const QMetaObject *to = QmlMetaType::rawMetaObjectForType(destination->type);

            if (QmlMetaPropertyPrivate::canConvert(from, to))
                type.type = destination->type;
        }

        if (type.type == destination->type) {
            Instr instr;
            instr.type = Instr::Store;
            instr.store.output = 0;
            instr.store.index = destination->index;
            instr.store.reg = type.reg;
            bytecode << instr;

            releaseReg(type.reg);

            Instr done;
            done.type = Instr::Done;
            bytecode << done;

            return true;
        } else {
            return false;
        }
    }
}

bool QmlBindingCompiler::parseExpression(QmlJS::AST::Node *node, Result &type)
{
    while (node->kind == AST::Node::Kind_NestedExpression)
        node = static_cast<AST::NestedExpression *>(node)->expression;

    if (tryArith(node)) {
        if (!parseArith(node, type)) return false;
    } else if (tryLogic(node)) {
        if (!parseLogic(node, type)) return false;
    } else if (tryConditional(node)) {
        if (!parseConditional(node, type)) return false;
    } else if (tryName(node)) {
        if (!parseName(node, type)) return false;
    } else if (tryConstant(node)) {
        if (!parseConstant(node, type)) return false;
    } else {
        return false;
    }
    return true;
}

bool QmlBindingCompiler::tryName(QmlJS::AST::Node *node)
{
    return node->kind == AST::Node::Kind_IdentifierExpression ||
           node->kind == AST::Node::Kind_FieldMemberExpression;
}

bool QmlBindingCompiler::parseName(AST::Node *node, Result &type)
{
    QStringList nameParts;
    if (!buildName(nameParts, node))
        return false;

    int reg = acquireReg();
    if (reg == -1)
        return false;
    type.reg = reg;

    QmlParser::Object *absType = 0;

    QStringList subscribeName;

    bool wasAttachedObject = false;

    for (int ii = 0; ii < nameParts.count(); ++ii) {
        const QString &name = nameParts.at(ii);

        // We don't handle signal properties or attached properties
        if (name.length() > 2 && name.startsWith(QLatin1String("on")) &&
            name.at(2).isUpper())
            return false;

        QmlType *attachType = 0;
        if (name.at(0).isUpper()) {
            // Could be an attached property
            if (ii == nameParts.count() - 1)
                return false;
            if (nameParts.at(ii + 1).at(0).isUpper())
                return false;

            QmlEnginePrivate::ImportedNamespace *ns = 0;
            if (!engine->resolveType(imports, name.toUtf8(), &attachType, 0, 0, 0, &ns))
                return false;
            if (ns || !attachType || !attachType->attachedPropertiesType())
                return false;

            wasAttachedObject = true;
        } 

        if (ii == 0) {

            if (attachType) {
                Instr instr;
                instr.type = Instr::LoadScope;
                instr.load.index = 0;
                instr.load.reg = reg;
                bytecode << instr;

                Instr attach;
                attach.type = Instr::LoadAttached;
                attach.attached.output = reg;
                attach.attached.reg = reg;
                attach.attached.index = attachType->index();
                bytecode << attach;

                absType = 0;
                type.metaObject = attachType->attachedPropertiesType();

                subscribeName << QLatin1String("$$$ATTACH_") + name;

                continue;
            } else if (ids.contains(name)) {
                QmlParser::Object *idObject = ids.value(name);
                absType = idObject;
                type.metaObject = absType->metaObject();

                // We check if the id object is the root or 
                // scope object to avoid a subscription
                if (idObject == component) {
                    Instr instr;
                    instr.type = Instr::LoadRoot;
                    instr.load.index = 0;
                    instr.load.reg = reg;
                    bytecode << instr;
                } else if (idObject == context) {
                    Instr instr;
                    instr.type = Instr::LoadScope;
                    instr.load.index = 0;
                    instr.load.reg = reg;
                    bytecode << instr;
                } else {
                    Instr instr;
                    instr.type = Instr::LoadId;
                    instr.load.index = idObject->idIndex;
                    instr.load.reg = reg;
                    bytecode << instr;

                    subscribeName << QLatin1String("$$$ID_") + name;

                    if (subscription(subscribeName, &type)) {
                        Instr sub;
                        sub.type = Instr::SubscribeId;
                        sub.subscribe.offset = subscriptionIndex(subscribeName);
                        sub.subscribe.reg = reg;
                        sub.subscribe.index = instr.load.index;
                        bytecode << sub;
                    }
                }

            } else {

                QByteArray utf8Name = name.toUtf8();
                const char *cname = utf8Name.constData();

                int d0Idx = (context == component)?-1:context->metaObject()->indexOfProperty(cname);
                int d1Idx = -1;
                if (d0Idx == -1)
                    d1Idx = component->metaObject()->indexOfProperty(cname);

                if (d0Idx != -1) {
                    Instr instr;
                    instr.type = Instr::LoadScope;
                    instr.load.index = 0;
                    instr.load.reg = reg;
                    bytecode << instr;

                    subscribeName << QLatin1String("$$$Scope");
                    subscribeName << name;

                    fetch(type, context->metaObject(), reg, d0Idx, subscribeName);
                } else if(d1Idx != -1) {
                    Instr instr;
                    instr.type = Instr::LoadRoot;
                    instr.load.index = 0;
                    instr.load.reg = reg;
                    bytecode << instr;

                    subscribeName << QLatin1String("$$$Root");
                    subscribeName << name;

                    fetch(type, component->metaObject(), reg, d1Idx, subscribeName);
                } else {
                    Instr find;
                    if (nameParts.count() == 1)
                        find.type = Instr::FindGenericTerminal;
                    else
                        find.type = Instr::FindGeneric;

                    find.find.reg = reg;
                    find.find.src = -1;
                    find.find.typeReg = 2; // XXX
                    find.find.name = registerString(name);

                    subscribeName << QString(QLatin1String("$$$Generic_") + name);
                    if (subscription(subscribeName, &type)) 
                        find.find.subscribeIndex = subscriptionIndex(subscribeName);
                    else
                        find.find.subscribeIndex = -1;

                    bytecode << find;
                    type.unknownType = true;
                } 

                if (!type.unknownType && type.type == -1)
                    return false; // Couldn't fetch that type
            } 

        } else {

            if (attachType) {
                Instr attach;
                attach.type = Instr::LoadAttached;
                attach.attached.output = reg;
                attach.attached.reg = reg;
                attach.attached.index = attachType->index();
                bytecode << attach;

                absType = 0;
                type.metaObject = attachType->attachedPropertiesType();

                subscribeName << QLatin1String("$$$ATTACH_") + name;
                continue;
            }

            const QMetaObject *mo = 0;
            if (absType)
                mo = absType->metaObject();
            else if (type.metaObject)
                mo = type.metaObject;

            QByteArray utf8Name = name.toUtf8();
            const char *cname = utf8Name.constData();
            int idx = mo?mo->indexOfProperty(cname):-1;
            if (absType && idx == -1)
                return false;

            subscribeName << name;

            if (absType || (wasAttachedObject && idx != -1) || (mo && mo->property(idx).isFinal())) {
                absType = 0; 
                fetch(type, mo, reg, idx, subscribeName);
                if (type.type == -1)
                    return false;
            } else {

                Instr prop;
                if (ii == nameParts.count() -1 ) 
                    prop.type = Instr::FindPropertyTerminal;
                else
                    prop.type = Instr::FindProperty;

                prop.find.reg = reg;
                prop.find.src = reg;
                prop.find.typeReg = 2; // XXX 
                prop.find.name = registerString(name);
                if (subscription(subscribeName, &type))
                    prop.find.subscribeIndex = subscriptionIndex(subscribeName);
                else
                    prop.find.subscribeIndex = -1;

                type.unknownType = true;
                type.metaObject = 0;
                type.type = -1;
                type.reg = reg;
                bytecode << prop;
            }
        }

        wasAttachedObject = false;
    }

    return true;
}

bool QmlBindingCompiler::tryArith(QmlJS::AST::Node *node)
{
    if (node->kind != AST::Node::Kind_BinaryExpression)
        return false;

    AST::BinaryExpression *expression = static_cast<AST::BinaryExpression *>(node);
    if (expression->op == QSOperator::Add ||
        expression->op == QSOperator::Sub)
        return true;
    else
        return false;
}

bool QmlBindingCompiler::parseArith(QmlJS::AST::Node *node, Result &type)
{
    AST::BinaryExpression *expression = static_cast<AST::BinaryExpression *>(node);

    type.reg = acquireReg();

    Result lhs;
    Result rhs;

    if (!parseExpression(expression->left, lhs)) return false;
    if (!parseExpression(expression->right, rhs)) return false;

    if (lhs.type != QVariant::Int &
        lhs.type != QMetaType::QReal)
        return false;

    if (rhs.type != QVariant::Int &
        rhs.type != QMetaType::QReal)
        return false;

    bool nativeReal = rhs.type == QMetaType::QReal ||
                      lhs.type == QMetaType::QReal;


    if (nativeReal && lhs.type == QMetaType::Int) {
        Instr convert;
        convert.type = Instr::ConvertIntToReal;
        convert.unaryop.output = lhs.reg;
        convert.unaryop.src = lhs.reg;
        bytecode << convert;
    }

    if (nativeReal && rhs.type == QMetaType::Int) {
        Instr convert;
        convert.type = Instr::ConvertIntToReal;
        convert.unaryop.output = rhs.reg;
        convert.unaryop.src = rhs.reg;
        bytecode << convert;
    }

    Instr arith;
    if (expression->op == QSOperator::Add) {
        arith.type = nativeReal?Instr::AddReal:Instr::AddInt;
    } else if (expression->op == QSOperator::Sub) {
        arith.type = nativeReal?Instr::MinusReal:Instr::MinusInt;
    } else {
        qFatal("Unsupported arithmetic operator");
    }

    arith.binaryop.output = type.reg;
    arith.binaryop.src1 = lhs.reg;
    arith.binaryop.src2 = rhs.reg;
    bytecode << arith;

    type.metaObject = 0;
    type.type = nativeReal?QMetaType::QReal:QMetaType::Int;
    type.subscriptionSet.unite(lhs.subscriptionSet);
    type.subscriptionSet.unite(rhs.subscriptionSet);

    releaseReg(lhs.reg);
    releaseReg(rhs.reg);

    return true;
}

bool QmlBindingCompiler::tryLogic(QmlJS::AST::Node *node)
{
    if (node->kind != AST::Node::Kind_BinaryExpression)
        return false;

    AST::BinaryExpression *expression = static_cast<AST::BinaryExpression *>(node);
    if (expression->op == QSOperator::Gt ||
        expression->op == QSOperator::Equal ||
        expression->op == QSOperator::NotEqual)
        return true;
    else
        return false;
}

bool QmlBindingCompiler::parseLogic(QmlJS::AST::Node *node, Result &type)
{
    AST::BinaryExpression *expression = static_cast<AST::BinaryExpression *>(node);

    Result lhs;
    Result rhs;

    if (!parseExpression(expression->left, lhs)) return false;
    if (!parseExpression(expression->right, rhs)) return false;

    type.reg = acquireReg();
    type.metaObject = 0;
    type.type = QVariant::Bool;

    if (lhs.type == QMetaType::QReal && rhs.type == QMetaType::QReal) {

        Instr op;
        if (expression->op == QSOperator::Gt)
            op.type = Instr::GreaterThanReal;
        else if (expression->op == QSOperator::Equal)
            op.type = Instr::CompareReal;
        else if (expression->op == QSOperator::NotEqual)
            op.type = Instr::NotCompareReal;
        else
            return false;
        op.binaryop.output = type.reg;
        op.binaryop.src1 = lhs.reg;
        op.binaryop.src2 = rhs.reg;
        bytecode << op;


    } else if (lhs.type == QMetaType::QString && rhs.type == QMetaType::QString) {

        Instr op;
        if (expression->op == QSOperator::Equal)
            op.type = Instr::CompareString;
        else if (expression->op == QSOperator::NotEqual)
            op.type = Instr::NotCompareString;
        else
            return false;
        op.binaryop.output = type.reg;
        op.binaryop.src1 = lhs.reg;
        op.binaryop.src2 = rhs.reg;
        bytecode << op;

    } else {
        return false;
    }

    releaseReg(lhs.reg);
    releaseReg(rhs.reg);

    return true;
}

bool QmlBindingCompiler::tryConditional(QmlJS::AST::Node *node)
{
    return (node->kind == AST::Node::Kind_ConditionalExpression);
}

bool QmlBindingCompiler::parseConditional(QmlJS::AST::Node *node, Result &type)
{
    AST::ConditionalExpression *expression = static_cast<AST::ConditionalExpression *>(node);

    AST::Node *test = expression->expression;
    if (test->kind == AST::Node::Kind_NestedExpression)
        test = static_cast<AST::NestedExpression*>(test)->expression;

    Result etype;
    if (!parseExpression(test, etype)) return false;

    if (etype.type != QVariant::Bool) 
        return false;

    Instr skip;
    skip.type = Instr::Skip;
    skip.skip.reg = etype.reg;
    skip.skip.count = 0;
    int skipIdx = bytecode.count();
    bytecode << skip;

    // Release to allow reuse of reg
    releaseReg(etype.reg);

    QSet<QString> preSubSet = subscriptionSet;

    // int preConditionalSubscriptions = subscriptionSet.count();

    Result ok;
    if (!parseExpression(expression->ok, ok)) return false;
    if (ok.unknownType) return false;

    int skipIdx2 = bytecode.count();
    skip.skip.reg = -1;
    bytecode << skip;

    // Release to allow reuse of reg
    releaseReg(ok.reg);
    bytecode[skipIdx].skip.count = bytecode.count() - skipIdx - 1;

    subscriptionSet = preSubSet;

    Result ko;
    if (!parseExpression(expression->ko, ko)) return false;
    if (ko.unknownType) return false;

    // Release to allow reuse of reg
    releaseReg(ko.reg);
    bytecode[skipIdx2].skip.count = bytecode.count() - skipIdx2 - 1;

    if (ok != ko)
        return false; // Must be same type and in same register

    subscriptionSet = preSubSet;

    if (!subscriptionNeutral(subscriptionSet, ok.subscriptionSet, ko.subscriptionSet))
        return false; // Conditionals cannot introduce new subscriptions

    type = ok;

    return true;
}

bool QmlBindingCompiler::tryConstant(QmlJS::AST::Node *node)
{
    return node->kind == AST::Node::Kind_TrueLiteral ||
           node->kind == AST::Node::Kind_FalseLiteral ||
           node->kind == AST::Node::Kind_NumericLiteral;
}

bool QmlBindingCompiler::parseConstant(QmlJS::AST::Node *node, Result &type)
{
    type.metaObject = 0;
    type.type = -1;
    type.reg = acquireReg();

    if (node->kind == AST::Node::Kind_TrueLiteral) {
        type.type = QVariant::Bool;
        Instr instr;
        instr.type = Instr::Bool;
        instr.bool_value.reg = type.reg;
        instr.bool_value.value = true;
        bytecode << instr;
        return true;
    } else if (node->kind == AST::Node::Kind_FalseLiteral) {
        type.type = QVariant::Bool;
        Instr instr;
        instr.type = Instr::Bool;
        instr.bool_value.reg = type.reg;
        instr.bool_value.value = false;
        bytecode << instr;
        return true;
    } else if (node->kind == AST::Node::Kind_NumericLiteral) {
        type.type = QMetaType::QReal;
        Instr instr;
        instr.type = Instr::Real;
        instr.real_value.reg = type.reg;
        instr.real_value.value = qreal(static_cast<AST::NumericLiteral *>(node)->value);
        bytecode << instr;
        return true;
    } else {
        return false;
    }
}

bool QmlBindingCompiler::buildName(QStringList &name,
                                       QmlJS::AST::Node *node)
{
    if (node->kind == AST::Node::Kind_IdentifierExpression) {
        name << static_cast<AST::IdentifierExpression*>(node)->name->asString();
    } else if (node->kind == AST::Node::Kind_FieldMemberExpression) {
        AST::FieldMemberExpression *expr =
            static_cast<AST::FieldMemberExpression *>(node);

        if (!buildName(name, expr->base))
            return false;

        name << expr->name->asString();
    } else {
        return false;
    }

    return true;
}


bool QmlBindingCompiler::fetch(Result &rv, const QMetaObject *mo, int reg, int idx, const QStringList &subName)
{
    QMetaProperty prop = mo->property(idx);
    rv.metaObject = 0;
    rv.type = 0;

    if (subscription(subName, &rv) && prop.hasNotifySignal() && prop.notifySignalIndex() != -1) {
        Instr sub;
        sub.type = Instr::Subscribe;
        sub.subscribe.offset = subscriptionIndex(subName);
        sub.subscribe.reg = reg;
        sub.subscribe.index = prop.notifySignalIndex();
        bytecode << sub;
    }

    Instr fetch;
    fetch.type = Instr::Fetch;
    fetch.fetch.objectReg = reg;
    fetch.fetch.index = idx;
    fetch.fetch.output = reg;

    rv.type = prop.userType();
    rv.metaObject = QmlMetaType::metaObjectForType(rv.type);
    rv.reg = reg;

    if (rv.type == QMetaType::QString) {
        int tmp = acquireReg();
        Instr copy;
        copy.type = Instr::Copy;
        copy.copy.reg = tmp;
        copy.copy.src = reg;
        bytecode << copy;
        releaseReg(tmp);
        fetch.fetch.objectReg = tmp;

        Instr setup;
        setup.type = Instr::NewString;
        setup.construct.reg = reg;
        bytecode << setup;
        registerCleanup(reg, Instr::CleanupString);
    }

    bytecode << fetch;

    if (!rv.metaObject &&
        rv.type != QMetaType::QReal &&
        rv.type != QMetaType::Int &&
        rv.type != QMetaType::Bool &&
        rv.type != qMetaTypeId<QmlGraphicsAnchorLine>() &&
        rv.type != QMetaType::QString) {
        rv.metaObject = 0;
        rv.type = 0;
        return false; // Unsupported type (string not supported yet);
    }

    return true;
}

void QmlBindingCompiler::registerCleanup(int reg, int cleanup, int cleanupType)
{
    registerCleanups.insert(reg, qMakePair(cleanup, cleanupType));
}

int QmlBindingCompiler::acquireReg(int cleanup, int cleanupType)
{
    for (int ii = 0; ii < 32; ++ii) {
        if (!(registers & (1 << ii))) {
            registers |= (1 << ii);

            if (cleanup != Instr::Noop)
                registerCleanup(ii, cleanup, cleanupType);

            return ii;
        }
    }
    return -1;
}

void QmlBindingCompiler::releaseReg(int reg)
{
    Q_ASSERT(reg >= 0 && reg <= 31);

    if (registerCleanups.contains(reg)) {
        QPair<int, int> c = registerCleanups[reg];
        registerCleanups.remove(reg);
        Instr cleanup;
        (int &)cleanup.type = c.first;
        cleanup.cleanup.reg = reg;
        cleanup.cleanup.typeReg = c.second;
        bytecode << cleanup;
    }

    quint32 mask = 1 << reg;
    registers &= ~mask;
}

int QmlBindingCompiler::registerString(const QString &string)
{
    Q_ASSERT(!string.isEmpty());

    // ### Padding?
    QByteArray strdata((const char *)string.constData(), string.length() * sizeof(QChar));
    int rv = data.count();
    data += strdata;

    Instr reg;
    reg.type = Instr::InitString;
    reg.initstring.offset = strings++;
    reg.initstring.dataIdx = rv;
    reg.initstring.length = string.length();
    bytecode << reg;

    return strings - 1;
}

bool QmlBindingCompiler::subscription(const QStringList &sub, Result *result)
{
    QString str = sub.join(QLatin1String("."));
    result->subscriptionSet.insert(str);

    if (subscriptionSet.contains(str)) {
        return false;
    } else {
        subscriptionSet.insert(str);
        return true;
    }
}

int QmlBindingCompiler::subscriptionIndex(const QStringList &sub)
{
    QString str = sub.join(QLatin1String("."));
    QHash<QString, int>::ConstIterator iter = subscriptionIds.find(str);
    if (iter == subscriptionIds.end()) 
        iter = subscriptionIds.insert(str, subscriptionIds.count());
    return *iter;
}

/*
    Returns true if lhs contains no subscriptions that aren't also in base or rhs AND
    rhs contains no subscriptions that aren't also in base or lhs.
*/ 
bool QmlBindingCompiler::subscriptionNeutral(const QSet<QString> &base, 
                                             const QSet<QString> &lhs, 
                                             const QSet<QString> &rhs)
{
    QSet<QString> difflhs = lhs;
    difflhs.subtract(rhs);
    QSet<QString> diffrhs = rhs;
    diffrhs.subtract(lhs);

    difflhs.unite(diffrhs);
    difflhs.subtract(base);

    return difflhs.isEmpty();
}
QT_END_NAMESPACE

