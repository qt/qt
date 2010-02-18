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

#include "qmlcompiledbindings_p.h"

#include <QtDeclarative/qmlinfo.h>
#include <private/qmlcontext_p.h>
#include <private/qmljsast_p.h>
#include <private/qmljsengine_p.h>
#include <private/qmlexpression_p.h>
#include <QtCore/qdebug.h>
#include <QtCore/qnumeric.h>
#include <private/qmlgraphicsanchors_p_p.h>

QT_BEGIN_NAMESPACE

using namespace QmlJS;

namespace {
// Supported types: int, qreal, QString (needs constr/destr), QObject*, bool
struct Register {
    void setUndefined() { type = 0; }
    void setUnknownButDefined() { type = -1; }
    void setNaN() { setqreal(qSNaN()); }
    bool isUndefined() const { return type == 0; }

    void setQObject(QObject *o) { *((QObject **)data) = o; type = QMetaType::QObjectStar; }
    QObject *getQObject() const { return *((QObject **)data); }

    void setqreal(qreal v) { *((qreal *)data) = v; type = QMetaType::QReal; }
    qreal getqreal() const { return *((qreal *)data); }

    void setint(int v) { *((int *)data) = v; type = QMetaType::Int; }
    int getint() const { return *((int *)data); }

    void setbool(bool v) { *((bool *)data) = v; type = QMetaType::Bool; }
    bool getbool() const { return *((bool *)data); }

    QVariant *getvariantptr() { return (QVariant *)typeDataPtr(); }
    QString *getstringptr() { return (QString *)typeDataPtr(); }
    QUrl *geturlptr() { return (QUrl *)typeDataPtr(); }
    const QVariant *getvariantptr() const { return (QVariant *)typeDataPtr(); }
    const QString *getstringptr() const { return (QString *)typeDataPtr(); }
    const QUrl *geturlptr() const { return (QUrl *)typeDataPtr(); }

    void *typeDataPtr() { return (void *)&data; }
    void *typeMemory() { return (void *)data; }
    const void *typeDataPtr() const { return (void *)&data; }
    const void *typeMemory() const { return (void *)data; }

    int gettype() const { return type; }
    void settype(int t) { type = t; }

    int type;          // Optional type
    void *data[2];     // Object stored here
};
}

class QmlCompiledBindingsPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlCompiledBindings)

public:
    QmlCompiledBindingsPrivate();
    virtual ~QmlCompiledBindingsPrivate();

    struct Binding : public QmlAbstractBinding, public QmlDelayedError {
        Binding() : enabled(false), updating(0), property(0),
                    scope(0), target(0), parent(0) {}

        // Inherited from QmlAbstractBinding
        virtual void setEnabled(bool, QmlMetaProperty::WriteFlags flags);
        virtual int propertyIndex();
        virtual void update(QmlMetaProperty::WriteFlags flags);
        virtual void destroy();

        int index:30;
        bool enabled:1;
        bool updating:1;
        int property;
        QObject *scope;
        QObject *target;

        QmlCompiledBindingsPrivate *parent;
    };

    struct Subscription {
        struct Signal {
            QmlGuard<QObject> source;
            int notifyIndex;
        };

        enum { InvalidType, SignalType, IdType } type;
        inline Subscription();
        inline ~Subscription();
        bool isSignal() const { return type == SignalType; }
        bool isId() const { return type == IdType; }
        inline Signal *signal();
        inline QmlContextPrivate::IdNotifier *id();
        union {
            char signalData[sizeof(Signal)];
            char idData[sizeof(QmlContextPrivate::IdNotifier)];
        };
    };
    Subscription *subscriptions;
    QScriptDeclarativeClass::PersistentIdentifier *identifiers;

    void run(Binding *);

    const char *programData;
    Binding *m_bindings;
    quint32 *m_signalTable;

    static int methodCount;

    void init();
    void run(int instr, QmlContextPrivate *context, 
             QmlDelayedError *error, QObject *scope, QObject *output);


    inline void unsubscribe(int subIndex);
    inline void subscribeId(QmlContextPrivate *p, int idIndex, int subIndex);
    inline void subscribe(QObject *o, int notifyIndex, int subIndex);

    QmlPropertyCache::Data *findproperty(QObject *obj, 
                                         const QScriptDeclarativeClass::Identifier &name,
                                         QmlEnginePrivate *enginePriv, 
                                         QmlPropertyCache::Data &local);
    bool findproperty(QObject *obj, 
                      Register *output, 
                      QmlEnginePrivate *enginePriv,
                      int subIdx, 
                      const QScriptDeclarativeClass::Identifier &name,
                      bool isTerminal);
    void findgeneric(Register *output,                                 // value output
                     int subIdx,                                       // Subscription index in config
                     QmlContextPrivate *context,                       // Context to search in
                     const QScriptDeclarativeClass::Identifier &name, 
                     bool isTerminal);
};

QmlCompiledBindingsPrivate::QmlCompiledBindingsPrivate()
: subscriptions(0), identifiers(0)
{
}

QmlCompiledBindingsPrivate::~QmlCompiledBindingsPrivate()
{
    delete [] subscriptions; subscriptions = 0;
    delete [] identifiers; identifiers = 0;
}

QmlCompiledBindingsPrivate::Subscription::Subscription()
: type(InvalidType)
{
}

QmlCompiledBindingsPrivate::Subscription::~Subscription()
{
    if (type == SignalType) ((Signal *)signalData)->~Signal();
    else if (type == IdType) ((QmlContextPrivate::IdNotifier *)idData)->~IdNotifier();
}


int QmlCompiledBindingsPrivate::methodCount = -1;

QmlCompiledBindings::QmlCompiledBindings(const char *program, QmlContext *context)
: QObject(*(new QmlCompiledBindingsPrivate))
{
    Q_D(QmlCompiledBindings);

    if (d->methodCount == -1)
        d->methodCount = QmlCompiledBindings::staticMetaObject.methodCount();

    d->programData = program;

    d->init();

    QmlAbstractExpression::setContext(context);
}

QmlCompiledBindings::~QmlCompiledBindings()
{
    Q_D(QmlCompiledBindings);

    delete [] d->m_bindings;
}

QmlAbstractBinding *QmlCompiledBindings::configBinding(int index, QObject *target, 
                                                        QObject *scope, int property)
{
    Q_D(QmlCompiledBindings);

    QmlCompiledBindingsPrivate::Binding *rv = d->m_bindings + index;

    rv->index = index;
    rv->property = property;
    rv->target = target;
    rv->scope = scope;
    rv->parent = d;

    addref(); // This is decremented in Binding::destroy()

    return rv;
}

void QmlCompiledBindingsPrivate::Binding::setEnabled(bool e, QmlMetaProperty::WriteFlags flags)
{
    if (e) {
        addToObject(target);
        update(flags);
    } else {
        removeFromObject();
    }

    QmlAbstractBinding::setEnabled(e, flags);

    if (enabled != e) {
        enabled = e;

        if (e) update(flags);
    }
}

int QmlCompiledBindingsPrivate::Binding::propertyIndex()
{
    return property & 0xFFFF;
}

void QmlCompiledBindingsPrivate::Binding::update(QmlMetaProperty::WriteFlags)
{
    parent->run(this);
}

void QmlCompiledBindingsPrivate::Binding::destroy()
{
    enabled = false;
    removeFromObject();
    parent->q_func()->release();
}

int QmlCompiledBindings::qt_metacall(QMetaObject::Call c, int id, void **)
{
    Q_D(QmlCompiledBindings);

    if (c == QMetaObject::InvokeMetaMethod && id >= d->methodCount) {
        id -= d->methodCount;

        quint32 *reeval = d->m_signalTable + d->m_signalTable[id];
        quint32 count = *reeval;
        ++reeval;
        for (quint32 ii = 0; ii < count; ++ii) {
            d->run(d->m_bindings + reeval[ii]);
        }
    }
    return -1;
}

void QmlCompiledBindingsPrivate::run(Binding *binding)
{
    Q_Q(QmlCompiledBindings);

    if (!binding->enabled)
        return;
    if (binding->updating)
        qWarning("ERROR: Circular binding");

    QmlContext *context = q->QmlAbstractExpression::context();
    if (!context) {
        qWarning("QmlCompiledBindings: Attempted to evaluate an expression in an invalid context");
        return;
    }
    QmlContextPrivate *cp = QmlContextPrivate::get(context);

    if (binding->property & 0xFFFF0000) {
        QmlEnginePrivate *ep = QmlEnginePrivate::get(cp->engine);

        QmlValueType *vt = ep->valueTypes[(binding->property >> 16) & 0xFF];
        Q_ASSERT(vt);
        vt->read(binding->target, binding->property & 0xFFFF);

        QObject *target = vt;
        run(binding->index, cp, binding, binding->scope, target);

        vt->write(binding->target, binding->property & 0xFFFF, 
                  QmlMetaProperty::DontRemoveBinding);
    } else {
        run(binding->index, cp, binding, binding->scope, binding->target);
    }
}

QmlCompiledBindingsPrivate::Subscription::Signal *QmlCompiledBindingsPrivate::Subscription::signal() 
{
    if (type == IdType) ((QmlContextPrivate::IdNotifier *)idData)->~IdNotifier();
    if (type != SignalType) new (signalData) Signal;
    type = SignalType;
    return (Signal *)signalData;
}

QmlContextPrivate::IdNotifier *QmlCompiledBindingsPrivate::Subscription::id()
{
    if (type == SignalType) ((Signal *)signalData)->~Signal();
    if (type != IdType) new (idData) QmlContextPrivate::IdNotifier;
    type = IdType;
    return (QmlContextPrivate::IdNotifier *)idData;
}

namespace {
// This structure is exactly 8-bytes in size
struct Instr {
    enum {
        Noop,

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
        String,                  // string_value

        AddReal,                 // binaryop
        AddInt,                  // binaryop
        AddString,               // binaryop

        MinusReal,               // binaryop
        MinusInt,                // binaryop

        CompareReal,             // binaryop
        CompareString,           // binaryop

        NotCompareReal,          // binaryop
        NotCompareString,        // binaryop
        
        GreaterThanReal,         // binaryop
        MaxReal,                 // binaryop 
        MinReal,                 // binaryop

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
        ConvertGenericToReal,    // unaryop
        ConvertGenericToBool,    // unaryop
        ConvertGenericToString,  // unaryop
        ConvertGenericToUrl,     // unaryop
    };

    union {
        struct {
            quint8 type;
            quint8 packing[7];
        } common;
        struct {
            quint8 type;
            quint8 packing[3];
            quint16 subscriptions;
            quint16 identifiers;
        } init;
        struct {
            quint8 type;
            qint8 reg;
            quint16 offset;
            quint32 index;
        } subscribe;
        struct {
            quint8 type;
            qint8 reg;
            quint8 packing[2];
            quint32 index;
        } load;
        struct {
            quint8 type;
            qint8 output;
            qint8 reg;
            quint8 exceptionId;
            quint32 index;
        } attached;
        struct {
            quint8 type;
            qint8 output;
            qint8 reg;
            quint8 exceptionId;
            quint32 index;
        } store;
        struct {
            quint8 type;
            qint8 output;
            qint8 objectReg;
            quint8 exceptionId;
            quint32 index;
        } fetch;
        struct {
            quint8 type;
            qint8 reg;
            qint8 src;
            quint8 packing[5];
        } copy;
        struct {
            quint8 type;
            qint8 reg;
            quint8 packing[6];
        } construct;
        struct {
            quint8 type;
            qint8 reg;
            quint8 packing[2];
            float value;
        } real_value;
        struct {
            quint8 type;
            qint8 reg;
            quint8 packing[2];
            int value;
        } int_value;
        struct {
            quint8 type;
            qint8 reg;
            bool value;
            quint8 packing[5];
        } bool_value;
        struct {
            quint8 type;
            qint8 reg;
            quint16 length;
            quint32 offset;
        } string_value;
        struct {
            quint8 type;
            qint8 output;
            qint8 src1;
            qint8 src2;
            quint8 packing[4];
        } binaryop;
        struct {
            quint8 type;
            qint8 output;
            qint8 src;
            quint8 packing[5];
        } unaryop;
        struct {
            quint8 type;
            qint8 reg;
            quint8 packing[2];
            quint32 count;
        } skip;
        struct {
            quint8 type;
            qint8 reg;
            qint8 src;
            quint8 exceptionId;
            quint16 name; 
            quint16 subscribeIndex;
        } find;
        struct {
            quint8 type;
            qint8 reg;
            quint8 packing[6];
        } cleanup;
        struct {
            quint8 type;
            quint8 packing[1];
            quint16 offset;
            quint32 dataIdx;
        } initstring;
    };
};

struct Program {
    quint32 bindings;
    quint32 dataLength;
    quint32 signalTableOffset;
    quint32 exceptionDataOffset;
    quint16 subscriptions;
    quint16 identifiers;
    quint16 instructionCount;
    quint16 dummy;

    const char *data() const { return ((const char *)this) + sizeof(Program); }
    const Instr *instructions() const { return (const Instr *)(data() + dataLength); }
};
}

struct QmlBindingCompilerPrivate
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

    QmlBindingCompilerPrivate() : registers(0) {}

    void resetInstanceState();
    int commitCompile();

    QmlParser::Object *context;
    QmlParser::Object *component;
    QmlParser::Property *destination;
    QHash<QString, QmlParser::Object *> ids;
    QmlEnginePrivate::Imports imports;
    QmlEnginePrivate *engine;

    QString contextName() const { return QLatin1String("$$$SCOPE_") + QString::number((intptr_t)context, 16); }

    bool compile(QmlJS::AST::Node *);

    bool parseExpression(QmlJS::AST::Node *, Result &);

    bool tryName(QmlJS::AST::Node *);
    bool parseName(QmlJS::AST::Node *, Result &);

    bool tryArith(QmlJS::AST::Node *);
    bool parseArith(QmlJS::AST::Node *, Result &);
    bool numberArith(Result &, const Result &, const Result &, QSOperator::Op op);
    bool stringArith(Result &, const Result &, const Result &, QSOperator::Op op);

    bool tryLogic(QmlJS::AST::Node *);
    bool parseLogic(QmlJS::AST::Node *, Result &);

    bool tryConditional(QmlJS::AST::Node *);
    bool parseConditional(QmlJS::AST::Node *, Result &);

    bool tryConstant(QmlJS::AST::Node *);
    bool parseConstant(QmlJS::AST::Node *, Result &);

    bool tryMethod(QmlJS::AST::Node *);
    bool parseMethod(QmlJS::AST::Node *, Result &);

    bool buildName(QStringList &, QmlJS::AST::Node *, QList<QmlJS::AST::ExpressionNode *> *nodes = 0);
    bool fetch(Result &type, const QMetaObject *, int reg, int idx, const QStringList &, QmlJS::AST::ExpressionNode *);

    quint32 registers;
    QHash<int, QPair<int, int> > registerCleanups;
    int acquireReg(int cleanup = Instr::Noop, int cleanupType = 0);
    void registerCleanup(int reg, int cleanup, int cleanupType = 0);
    void releaseReg(int);

    int registerLiteralString(const QString &);
    int registerString(const QString &);
    QHash<QString, QPair<int, int> > registeredStrings;
    QByteArray data;

    bool subscription(const QStringList &, Result *);
    int subscriptionIndex(const QStringList &);
    bool subscriptionNeutral(const QSet<QString> &base, const QSet<QString> &lhs, const QSet<QString> &rhs);

    quint8 exceptionId(QmlJS::AST::ExpressionNode *);
    QVector<quint64> exceptions;

    QSet<int> usedSubscriptionIds;
    QSet<QString> subscriptionSet;
    QHash<QString, int> subscriptionIds;
    QVector<Instr> bytecode;

    // Committed binding data
    struct {
        QList<int> offsets;
        QList<QSet<int> > dependencies;

        QVector<Instr> bytecode;
        QByteArray data;
        QHash<QString, int> subscriptionIds;
        QVector<quint64> exceptions;

        QHash<QString, QPair<int, int> > registeredStrings;

        int count() const { return offsets.count(); }
    } committed;

    QByteArray buildSignalTable() const;
    QByteArray buildExceptionData() const;
};

void QmlCompiledBindingsPrivate::unsubscribe(int subIndex)
{
    Q_Q(QmlCompiledBindings);

    QmlCompiledBindingsPrivate::Subscription *sub = (subscriptions + subIndex);
    if (sub->isSignal()) {
        QmlCompiledBindingsPrivate::Subscription::Signal *s = sub->signal();
        if (s->source)
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 2))
            QMetaObject::disconnectOne(s->source, s->notifyIndex, 
                                       q, methodCount + subIndex);
#else
            // QTBUG-6781
            QMetaObject::disconnect(s->source, s->notifyIndex, 
                                    q, methodCount + subIndex);
#endif
    } else if (sub->isId()) {
        sub->id()->clear();
    }
}

void QmlCompiledBindingsPrivate::subscribeId(QmlContextPrivate *p, int idIndex, int subIndex)
{
    Q_Q(QmlCompiledBindings);

    unsubscribe(subIndex);

    if (p->idValues[idIndex]) {
        QmlCompiledBindingsPrivate::Subscription *sub = (subscriptions + subIndex);
        QmlContextPrivate::IdNotifier *i = sub->id();

        i->next = p->idValues[idIndex].bindings;
        i->prev = &p->idValues[idIndex].bindings;
        p->idValues[idIndex].bindings = i;
        if (i->next) i->next->prev = &i->next;

        i->target = q;
        i->methodIndex = methodCount + subIndex;
    }
}
 
void QmlCompiledBindingsPrivate::subscribe(QObject *o, int notifyIndex, int subIndex)
{
    Q_Q(QmlCompiledBindings);

    QmlCompiledBindingsPrivate::Subscription *sub = (subscriptions + subIndex);
    
    if (sub->isId())
        unsubscribe(subIndex);

    QmlCompiledBindingsPrivate::Subscription::Signal *s = sub->signal();
    if (o != s->source || notifyIndex != s->notifyIndex)  {
        if (s->source)
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 2))
            QMetaObject::disconnectOne(s->source, s->notifyIndex, 
                                       q, methodCount + subIndex);
#else
            // QTBUG-6781
            QMetaObject::disconnect(s->source, s->notifyIndex, 
                                    q, methodCount + subIndex);
#endif
        s->source = o;
        s->notifyIndex = notifyIndex;
        if (s->source && s->notifyIndex != -1) 
            QMetaObject::connect(s->source, s->notifyIndex, q,
                                 methodCount + subIndex, Qt::DirectConnection);
    } 
}

// Conversion functions - these MUST match the QtScript expression path
inline static qreal toReal(Register *reg, int type, bool *ok = 0)
{
    if (ok) *ok = true;

    if (type == QMetaType::QReal) {
        return reg->getqreal();
    } else if (type == qMetaTypeId<QVariant>()) {
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
    } else if (type == QMetaType::Int) {
        return QString::number(reg->getint());
    } else if (type == qMetaTypeId<QVariant>()) {
        return reg->getvariantptr()->toString();
    } else if (type == QMetaType::QString) {
        return *reg->getstringptr();
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
    } else if (type == QMetaType::QString) {
        base = QUrl(*reg->getstringptr());
    } else {
        if (ok) *ok = false;
        return QUrl();
    }

    if (!base.isEmpty() && base.isRelative())
        return context->url.resolved(base);
    else
        return base;
}

static QObject *variantToQObject(const QVariant &value, bool *ok)
{
    if (ok) *ok = true;

    if (value.userType() == QMetaType::QObjectStar) {
        return qvariant_cast<QObject*>(value);
    } else {
        if (ok) *ok = false;
        return 0;
    }
}

bool QmlCompiledBindingsPrivate::findproperty(QObject *obj, Register *output, 
                                              QmlEnginePrivate *enginePriv,
                                              int subIdx, const QScriptDeclarativeClass::Identifier &name,
                                              bool isTerminal)
{
    if (!obj) {
        output->setUndefined();
        return false;
    }

    QmlPropertyCache::Data local;
    QmlPropertyCache::Data *property = 
        QmlPropertyCache::property(QmlEnginePrivate::get(enginePriv), obj, name, local);

    if (property) {
        if (subIdx != -1)
            subscribe(obj, property->notifyIndex, subIdx);

        if (property->flags & QmlPropertyCache::Data::IsQObjectDerived) {
            void *args[] = { output->typeDataPtr(), 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, property->coreIndex, args);
            output->settype(QMetaType::QObjectStar);
        } else if (property->propType == qMetaTypeId<QVariant>()) {
            QVariant v;
            void *args[] = { &v, 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, property->coreIndex, args);

            if (isTerminal) {
                new (output->typeDataPtr()) QVariant(v);
                output->settype(qMetaTypeId<QVariant>());
            } else {
                bool ok;
                output->setQObject(variantToQObject(v, &ok));
                if (!ok) 
                    output->setUndefined();
                else
                    output->settype(QMetaType::QObjectStar);
            }

        } else {
            if (!isTerminal) {
                output->setUndefined();
            } else if (property->propType == QMetaType::QReal) {
                void *args[] = { output->typeDataPtr(), 0 };
                QMetaObject::metacall(obj, QMetaObject::ReadProperty, property->coreIndex, args);
                output->settype(QMetaType::QReal);
            } else if (property->propType == QMetaType::Int) {
                void *args[] = { output->typeDataPtr(), 0 };
                QMetaObject::metacall(obj, QMetaObject::ReadProperty, property->coreIndex, args);
                output->settype(QMetaType::Int);
            } else if (property->propType == QMetaType::Bool) {
                void *args[] = { output->typeDataPtr(), 0 };
                QMetaObject::metacall(obj, QMetaObject::ReadProperty, property->coreIndex, args);
                output->settype(QMetaType::Bool);
            } else if (property->propType == QMetaType::QString) {
                new (output->typeDataPtr()) QString();
                void *args[] = { output->typeDataPtr(), 0 };
                QMetaObject::metacall(obj, QMetaObject::ReadProperty, property->coreIndex, args);
                output->settype(QMetaType::QString);
            } else {
                new (output->typeDataPtr()) 
                    QVariant(obj->metaObject()->property(property->coreIndex).read(obj));
                output->settype(qMetaTypeId<QVariant>());
            }
        }

        return true;
    } else {
        output->setUndefined();
        return false;
    }
}

void QmlCompiledBindingsPrivate::findgeneric(Register *output, 
                                             int subIdx,      
                                             QmlContextPrivate *context,
                                             const QScriptDeclarativeClass::Identifier &name, 
                                             bool isTerminal)
{
    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(context->engine);

    while (context) {

        int contextPropertyIndex = context->propertyNames?context->propertyNames->value(name):-1;


        if (contextPropertyIndex != -1) {

            if (subIdx != -1) 
                subscribe(QmlContextPrivate::get(context), contextPropertyIndex + context->notifyIndex, subIdx);

            if (contextPropertyIndex < context->idValueCount) {
                output->setQObject(context->idValues[contextPropertyIndex]);
                output->settype(QMetaType::QObjectStar);
            } else {
                const QVariant &value = context->propertyValues.at(contextPropertyIndex);
                if (isTerminal) {
                    new (output->typeDataPtr()) QVariant(value);
                    output->settype(qMetaTypeId<QVariant>());
                } else {
                    bool ok;
                    output->setQObject(variantToQObject(value, &ok));
                    if (!ok) { output->setUndefined(); }
                    else { output->settype(QMetaType::QObjectStar); }
                    return;
                }
            }

            return;
        }

        for (int ii = 0; ii < context->scripts.count(); ++ii) {
            QScriptValue function = QScriptDeclarativeClass::function(context->scripts.at(ii), name);
            if (function.isValid()) {
                qFatal("Binding optimizer resolved name to QScript method");
            }
        }

        if (QObject *root = context->defaultObjects.isEmpty()?0:context->defaultObjects.first()) {

            if (findproperty(root, output, enginePriv, subIdx, name, isTerminal))
                return;

        }

        if (context->parent) {
            context = QmlContextPrivate::get(context->parent);
        } else {
            context = 0;
        }
    }

    output->setUndefined();
}

void QmlCompiledBindingsPrivate::init()
{
    Program *program = (Program *)programData;
    if (program->subscriptions)
        subscriptions = new QmlCompiledBindingsPrivate::Subscription[program->subscriptions];
    if (program->identifiers)
        identifiers = new QScriptDeclarativeClass::PersistentIdentifier[program->identifiers];

    m_signalTable = (quint32 *)(program->data() + program->signalTableOffset);
    m_bindings = new QmlCompiledBindingsPrivate::Binding[program->bindings];
}

static void throwException(int id, QmlDelayedError *error, 
                           Program *program, QmlContextPrivate *context,
                           const QString &description = QString())
{
    error->error.setUrl(context->url);
    if (description.isEmpty())
        error->error.setDescription(QLatin1String("TypeError: Result of expression is not an object"));
    else
        error->error.setDescription(description);
    if (id != 0xFF) {
        quint64 e = *((quint64 *)(program->data() + program->exceptionDataOffset) + id); 
        error->error.setLine((e >> 32) & 0xFFFFFFFF);
        error->error.setColumn(e & 0xFFFFFFFF); 
    } else {
        error->error.setLine(-1);
        error->error.setColumn(-1);
    }
    if (!context->engine || !error->addError(QmlEnginePrivate::get(context->engine)))
        qWarning() << error->error;
}

void QmlCompiledBindingsPrivate::run(int instrIndex,
                                     QmlContextPrivate *context, QmlDelayedError *error,
                                     QObject *scope, QObject *output)
{
    error->removeError();

    Register registers[32];
    int storeFlags = 0;

    QmlEnginePrivate *engine = QmlEnginePrivate::get(context->engine);
    Program *program = (Program *)programData;
    const Instr *instr = program->instructions();
    instr += instrIndex;
    const char *data = program->data();

    while (instr) {

    switch (instr->common.type) {
    case Instr::Noop:
        break;

    case Instr::SubscribeId:
        subscribeId(context, instr->subscribe.index, instr->subscribe.offset);
        break;

    case Instr::Subscribe:
    {
        QObject *o = 0;
        const Register &object = registers[instr->subscribe.reg];
        if (!object.isUndefined()) o = object.getQObject();
        subscribe(o, instr->subscribe.index, instr->subscribe.offset);
    }
        break;

    case Instr::LoadId:
        registers[instr->load.reg].setQObject(context->idValues[instr->load.index].data());
        break;

    case Instr::LoadScope:
        registers[instr->load.reg].setQObject(scope);
        break;

    case Instr::LoadRoot:
        registers[instr->load.reg].setQObject(context->defaultObjects.at(0));
        break;

    case Instr::LoadAttached:
    {
        const Register &input = registers[instr->attached.reg];
        Register &output = registers[instr->attached.output];
        if (input.isUndefined()) {
            throwException(instr->attached.exceptionId, error, program, context);
            return;
        }

        QObject *object = registers[instr->attached.reg].getQObject();
        if (!object) {
            output.setUndefined();
        } else {
            QObject *attached = 
                qmlAttachedPropertiesObjectById(instr->attached.index, 
                                                registers[instr->attached.reg].getQObject(), 
                                                true);
            Q_ASSERT(attached);
            output.setQObject(attached);
        }
    }
        break;

    case Instr::ConvertIntToReal:
    {
        const Register &input = registers[instr->unaryop.src];
        Register &output = registers[instr->unaryop.output];
        if (input.isUndefined()) output.setUndefined();
        else output.setqreal(qreal(input.getint()));
    }
        break;

    case Instr::ConvertRealToInt:
    {
        const Register &input = registers[instr->unaryop.src];
        Register &output = registers[instr->unaryop.output];
        if (input.isUndefined()) output.setUndefined();
        else output.setint(int(input.getqreal()));
    }
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

    case Instr::String:
    {
        Register &output = registers[instr->string_value.reg];
        new (output.getstringptr()) 
            QString((QChar *)(data + instr->string_value.offset), instr->string_value.length);
        output.settype(QMetaType::QString);
    }
        break;

    case Instr::AddReal:
    {
        const Register &lhs = registers[instr->binaryop.src1];
        const Register &rhs = registers[instr->binaryop.src2];
        Register &output = registers[instr->binaryop.output];
        if (lhs.isUndefined() || rhs.isUndefined()) output.setNaN();
        else output.setqreal(lhs.getqreal() + rhs.getqreal());
    }
        break;

    case Instr::AddInt:
    {
        const Register &lhs = registers[instr->binaryop.src1];
        const Register &rhs = registers[instr->binaryop.src2];
        Register &output = registers[instr->binaryop.output];
        if (lhs.isUndefined() || rhs.isUndefined()) output.setNaN();
        else output.setint(lhs.getint() + rhs.getint());
    }
        break;
        
    case Instr::AddString:
    {
        const Register &lhs = registers[instr->binaryop.src1];
        const Register &rhs = registers[instr->binaryop.src2];
        Register &output = registers[instr->binaryop.output];
        if (lhs.isUndefined() && rhs.isUndefined()) { output.setNaN(); }
        else {
            if (lhs.isUndefined())
                new (output.getstringptr())
                    QString(QLatin1String("undefined") + *registers[instr->binaryop.src2].getstringptr());
            else if (rhs.isUndefined())
                new (output.getstringptr())
                    QString(*registers[instr->binaryop.src1].getstringptr() + QLatin1String("undefined"));
            else
                new (output.getstringptr()) 
                    QString(*registers[instr->binaryop.src1].getstringptr() + 
                            *registers[instr->binaryop.src2].getstringptr());
            output.settype(QMetaType::QString);
        }
    }
        break;

    case Instr::MinusReal:
    {
        const Register &lhs = registers[instr->binaryop.src1];
        const Register &rhs = registers[instr->binaryop.src2];
        Register &output = registers[instr->binaryop.output];
        if (lhs.isUndefined() || rhs.isUndefined()) output.setNaN();
        else output.setqreal(lhs.getqreal() - rhs.getqreal());
    }
        break;

    case Instr::MinusInt:
    {
        const Register &lhs = registers[instr->binaryop.src1];
        const Register &rhs = registers[instr->binaryop.src2];
        Register &output = registers[instr->binaryop.output];
        if (lhs.isUndefined() || rhs.isUndefined()) output.setNaN();
        else output.setint(lhs.getint() - rhs.getint());
    }
        break;

    case Instr::CompareReal:
    {
        const Register &lhs = registers[instr->binaryop.src1];
        const Register &rhs = registers[instr->binaryop.src2];
        Register &output = registers[instr->binaryop.output];
        if (lhs.isUndefined() || rhs.isUndefined()) output.setbool(lhs.isUndefined() == rhs.isUndefined());
        else output.setbool(lhs.getqreal() == rhs.getqreal());
    }
        break;

    case Instr::CompareString:
    {
        const Register &lhs = registers[instr->binaryop.src1];
        const Register &rhs = registers[instr->binaryop.src2];
        Register &output = registers[instr->binaryop.output];
        if (lhs.isUndefined() || rhs.isUndefined()) output.setbool(lhs.isUndefined() == rhs.isUndefined());
        else output.setbool(*lhs.getstringptr() == *rhs.getstringptr());
    }
        break;

    case Instr::NotCompareReal:
    {
        const Register &lhs = registers[instr->binaryop.src1];
        const Register &rhs = registers[instr->binaryop.src2];
        Register &output = registers[instr->binaryop.output];
        if (lhs.isUndefined() || rhs.isUndefined()) output.setbool(lhs.isUndefined() != rhs.isUndefined());
        else output.setbool(lhs.getqreal() != rhs.getqreal());
    }
        break;

    case Instr::NotCompareString:
    {
        const Register &lhs = registers[instr->binaryop.src1];
        const Register &rhs = registers[instr->binaryop.src2];
        Register &output = registers[instr->binaryop.output];
        if (lhs.isUndefined() || rhs.isUndefined()) output.setbool(lhs.isUndefined() != rhs.isUndefined());
        else output.setbool(*lhs.getstringptr() != *rhs.getstringptr());
    }
        break;

    case Instr::GreaterThanReal:
    {
        const Register &lhs = registers[instr->binaryop.src1];
        const Register &rhs = registers[instr->binaryop.src2];
        Register &output = registers[instr->binaryop.output];
        if (lhs.isUndefined() || rhs.isUndefined()) output.setbool(false);
        else output.setbool(lhs.getqreal() > rhs.getqreal());
    }
        break;

    case Instr::MaxReal:
    {
        const Register &lhs = registers[instr->binaryop.src1];
        const Register &rhs = registers[instr->binaryop.src2];
        Register &output = registers[instr->binaryop.output];
        if (lhs.isUndefined() || rhs.isUndefined()) output.setNaN();
        else output.setqreal(qMax(lhs.getqreal(), rhs.getqreal()));
    }
        break;

    case Instr::MinReal:
    {
        const Register &lhs = registers[instr->binaryop.src1];
        const Register &rhs = registers[instr->binaryop.src2];
        Register &output = registers[instr->binaryop.output];
        if (lhs.isUndefined() || rhs.isUndefined()) output.setNaN();
        else output.setqreal(qMin(lhs.getqreal(), rhs.getqreal()));
    }
        break;

    case Instr::NewString:
    {
        Register &output = registers[instr->construct.reg];
        new (output.getstringptr()) QString;
        output.settype(QMetaType::QString);
    }
        break;

    case Instr::NewUrl:
    {
        Register &output = registers[instr->construct.reg];
        new (output.geturlptr()) QUrl;
        output.settype(QMetaType::QUrl);
    }
        break;

    case Instr::CleanupString:
        registers[instr->cleanup.reg].getstringptr()->~QString();
        break;

    case Instr::CleanupUrl:
        registers[instr->cleanup.reg].geturlptr()->~QUrl();
        break;

    case Instr::Fetch:
    {
        const Register &input = registers[instr->fetch.objectReg];
        Register &output = registers[instr->fetch.output];

        if (input.isUndefined()) {
            throwException(instr->fetch.exceptionId, error, program, context);
            return;
        }

        QObject *object = input.getQObject();
        if (!object) {
            output.setUndefined();
        } else {
            void *argv[] = { output.typeDataPtr(), 0 };
            QMetaObject::metacall(object, QMetaObject::ReadProperty, instr->fetch.index, argv);
        }
    }
        break;

    case Instr::Store:
    {
        Register &data = registers[instr->store.reg];
        if (data.isUndefined()) {
            throwException(instr->store.exceptionId, error, program, context,
                           QLatin1String("Unable to assign undefined value"));
            return;
        }

        int status = -1;
        void *argv[] = { data.typeDataPtr(), 0, &status, &storeFlags };
        QMetaObject::metacall(output, QMetaObject::WriteProperty, 
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
        if (!identifiers[instr->initstring.offset].identifier) {
            quint32 len = *(quint32 *)(data + instr->initstring.dataIdx);
            QChar *strdata = (QChar *)(data + instr->initstring.dataIdx + sizeof(quint32)); 

            QString str = QString::fromRawData(strdata, len);

            identifiers[instr->initstring.offset] = engine->objectClass->createPersistentIdentifier(str);
        }
        break;

    case Instr::FindGenericTerminal:
    case Instr::FindGeneric:
        // We start the search in the parent context, as we know that the 
        // name is not present in the current context or it would have been
        // found during the static compile
        findgeneric(registers + instr->find.reg, instr->find.subscribeIndex, 
                    QmlContextPrivate::get(context->parent),
                    identifiers[instr->find.name].identifier, 
                    instr->common.type == Instr::FindGenericTerminal);
        break;

    case Instr::FindPropertyTerminal:
    case Instr::FindProperty:
    {
        const Register &object = registers[instr->find.src];
        if (object.isUndefined()) {
            throwException(instr->find.exceptionId, error, program, context);
            return;
        }

        findproperty(object.getQObject(), registers + instr->find.reg, 
                     QmlEnginePrivate::get(context->engine), 
                     instr->find.subscribeIndex, identifiers[instr->find.name].identifier, 
                     instr->common.type == Instr::FindPropertyTerminal);
    }
        break;

    case Instr::CleanupGeneric:
    {
        int type = registers[instr->cleanup.reg].gettype();
        if (type == qMetaTypeId<QVariant>()) {
            registers[instr->cleanup.reg].getvariantptr()->~QVariant();
        } else if (type == QMetaType::QString) {
            registers[instr->cleanup.reg].getstringptr()->~QString();
        } else if (type == QMetaType::QUrl) {
            registers[instr->cleanup.reg].geturlptr()->~QUrl();
        }
    }
        break;

    case Instr::ConvertGenericToReal:
    {
        Register &output = registers[instr->unaryop.output];
        Register &input = registers[instr->unaryop.src];
        bool ok = true;
        output.setqreal(toReal(&input, input.gettype(), &ok));
        if (!ok) output.setUndefined();
    }
        break;

    case Instr::ConvertGenericToBool:
    {
        Register &output = registers[instr->unaryop.output];
        Register &input = registers[instr->unaryop.src];
        bool ok = true;
        output.setbool(toBool(&input, input.gettype(), &ok));
        if (!ok) output.setUndefined();
    }
        break;

    case Instr::ConvertGenericToString:
    {
        Register &output = registers[instr->unaryop.output];
        Register &input = registers[instr->unaryop.src];
        bool ok = true;
        QString str = toString(&input, input.gettype(), &ok);
        if (ok) { new (output.getstringptr()) QString(str); output.settype(QMetaType::QString); }
        else { output.setUndefined(); }
    }
        break;

    case Instr::ConvertGenericToUrl:
    {
        Register &output = registers[instr->unaryop.output];
        Register &input = registers[instr->unaryop.src];
        bool ok = true;
        QUrl url = toUrl(&input, input.gettype(), context, &ok);
        if (ok) { new (output.geturlptr()) QUrl(url); output.settype(QMetaType::QUrl); }
        else { output.setUndefined(); }
    }
        break;

    default:
        qFatal("EEK");
        break;
    }

    instr++;
    }
}

void QmlBindingCompiler::dump(const QByteArray &programData)
{
    const Program *program = (const Program *)programData.constData();

    qWarning() << "Program.bindings:" << program->bindings;
    qWarning() << "Program.dataLength:" << program->dataLength;
    qWarning() << "Program.subscriptions:" << program->subscriptions;
    qWarning() << "Program.indentifiers:" << program->identifiers;

    int count = program->instructionCount;
    const Instr *instr = program->instructions();

    while (count--) {

        switch (instr->common.type) {
        case Instr::Noop:
            qWarning().nospace() << "Noop";
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
        case Instr::String:
            qWarning().nospace() << "String" << "\t\t\t" << instr->string_value.reg << "\t" << instr->string_value.offset << "\t" << instr->string_value.length;
            break;
        case Instr::AddReal:
            qWarning().nospace() << "AddReal" << "\t\t\t" << instr->binaryop.output << "\t" << instr->binaryop.src1 << "\t" << instr->binaryop.src2;
            break;
        case Instr::AddInt:
            qWarning().nospace() << "AddInt" << "\t\t\t" << instr->binaryop.output << "\t" << instr->binaryop.src1 << "\t" << instr->binaryop.src2;
            break;
        case Instr::AddString:
            qWarning().nospace() << "AddString" << "\t\t" << instr->binaryop.output << "\t" << instr->binaryop.src1 << "\t" << instr->binaryop.src2;
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
        case Instr::MaxReal:
            qWarning().nospace() << "MaxReal" << "\t\t\t" << instr->binaryop.output << "\t" << instr->binaryop.src1 << "\t" << instr->binaryop.src2;
            break;
        case Instr::MinReal:
            qWarning().nospace() << "MinReal" << "\t\t\t" << instr->binaryop.output << "\t" << instr->binaryop.src1 << "\t" << instr->binaryop.src2;
            break;
        case Instr::NewString:
            qWarning().nospace() << "NewString" << "\t\t" << instr->construct.reg;
            break;
        case Instr::NewUrl:
            qWarning().nospace() << "NewUrl" << "\t\t\t" << instr->construct.reg;
            break;
        case Instr::CleanupString:
            qWarning().nospace() << "CleanupString" << "\t\t" << instr->cleanup.reg;
            break;
        case Instr::CleanupUrl:
            qWarning().nospace() << "CleanupUrl" << "\t\t" << instr->cleanup.reg;
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
            qWarning().nospace() << "InitString" << "\t\t" << instr->initstring.offset << "\t" << instr->initstring.dataIdx;
            break;
        case Instr::FindGeneric:
            qWarning().nospace() << "FindGeneric" << "\t\t" << instr->find.reg << "\t" << instr->find.name;
            break;
        case Instr::FindGenericTerminal:
            qWarning().nospace() << "FindGenericTerminal" << "\t" << instr->find.reg << "\t" <<  instr->find.name;
            break;
        case Instr::FindProperty:
            qWarning().nospace() << "FindProperty" << "\t\t" << instr->find.reg << "\t" << instr->find.src << "\t" << instr->find.name;
            break;
        case Instr::FindPropertyTerminal:
            qWarning().nospace() << "FindPropertyTerminal" << "\t" << instr->find.reg << "\t" << instr->find.src << "\t" << instr->find.name;
            break;
        case Instr::CleanupGeneric:
            qWarning().nospace() << "CleanupGeneric" << "\t\t" << instr->cleanup.reg;
            break;
        case Instr::ConvertGenericToReal:
            qWarning().nospace() << "ConvertGenericToReal" << "\t" << instr->unaryop.output << "\t" << instr->unaryop.src;
            break;
        case Instr::ConvertGenericToBool:
            qWarning().nospace() << "ConvertGenericToBool" << "\t" << instr->unaryop.output << "\t" << instr->unaryop.src;
            break;
        case Instr::ConvertGenericToString:
            qWarning().nospace() << "ConvertGenericToString" << "\t" << instr->unaryop.output << "\t" << instr->unaryop.src;
            break;
        case Instr::ConvertGenericToUrl:
            qWarning().nospace() << "ConvertGenericToUrl" << "\t" << instr->unaryop.output << "\t" << instr->unaryop.src;
            break;
        default:
            qWarning().nospace() << "Unknown";
            break;
        }

        ++instr;
    }
}

/*!
Clear the state associated with attempting to compile a specific binding.
This does not clear the global "commited binding" states.
*/
void QmlBindingCompilerPrivate::resetInstanceState()
{
    registers = 0;
    registerCleanups.clear();
    data = committed.data;
    exceptions = committed.exceptions;
    usedSubscriptionIds.clear();
    subscriptionSet.clear();
    subscriptionIds = committed.subscriptionIds;
    registeredStrings = committed.registeredStrings;
    bytecode.clear();
}

/*!
Mark the last compile as successful, and add it to the "committed data"
section.

Returns the index for the committed binding.
*/
int QmlBindingCompilerPrivate::commitCompile()
{
    int rv = committed.count();
    committed.offsets << committed.bytecode.count();
    committed.dependencies << usedSubscriptionIds;
    committed.bytecode << bytecode;
    committed.data = data;
    committed.exceptions = exceptions;
    committed.subscriptionIds = subscriptionIds;
    committed.registeredStrings = registeredStrings;
    return rv;
}

bool QmlBindingCompilerPrivate::compile(QmlJS::AST::Node *node)
{
    resetInstanceState();

    Result type;

    if (!parseExpression(node, type)) 
        return false;

    if (subscriptionSet.count() > 0xFFFF ||
            registeredStrings.count() > 0xFFFF)
        return false;

    if (type.unknownType) {
        if (destination->type != QMetaType::QReal &&
            destination->type != QVariant::String &&
            destination->type != QMetaType::Bool &&
            destination->type != QVariant::Url)
            return false;

        int convertReg = acquireReg();

        if (destination->type == QMetaType::QReal) {
            Instr convert;
            convert.common.type = Instr::ConvertGenericToReal;
            convert.unaryop.output = convertReg;
            convert.unaryop.src = type.reg;
            bytecode << convert;
        } else if (destination->type == QVariant::String) {
            Instr convert;
            convert.common.type = Instr::ConvertGenericToString;
            convert.unaryop.output = convertReg;
            convert.unaryop.src = type.reg;
            bytecode << convert;
        } else if (destination->type == QMetaType::Bool) {
            Instr convert;
            convert.common.type = Instr::ConvertGenericToBool;
            convert.unaryop.output = convertReg;
            convert.unaryop.src = type.reg;
            bytecode << convert;
        } else if (destination->type == QVariant::Url) {
            Instr convert;
            convert.common.type = Instr::ConvertGenericToUrl;
            convert.unaryop.output = convertReg;
            convert.unaryop.src = type.reg;
            bytecode << convert;
        }

        Instr cleanup;
        cleanup.common.type = Instr::CleanupGeneric;
        cleanup.cleanup.reg = type.reg;
        bytecode << cleanup;

        Instr instr;
        instr.common.type = Instr::Store;
        instr.store.output = 0;
        instr.store.index = destination->index;
        instr.store.reg = convertReg;
        instr.store.exceptionId = exceptionId(node->expressionCast());
        bytecode << instr;

        if (destination->type == QVariant::String) {
            Instr cleanup;
            cleanup.common.type = Instr::CleanupString;
            cleanup.cleanup.reg = convertReg;
            bytecode << cleanup;
        } else if (destination->type == QVariant::Url) {
            Instr cleanup;
            cleanup.common.type = Instr::CleanupUrl;
            cleanup.cleanup.reg = convertReg;
            bytecode << cleanup;
        }

        releaseReg(convertReg);

        Instr done;
        done.common.type = Instr::Done;
        bytecode << done;

        return true;
    } else {
        // Can we store the final value?
        if (type.type == QVariant::Int &&
            destination->type == QMetaType::QReal) {
            Instr instr;
            instr.common.type = Instr::ConvertIntToReal;
            instr.unaryop.output = type.reg;
            instr.unaryop.src = type.reg;
            bytecode << instr;
            type.type = QMetaType::QReal;
        } else if (type.type == QMetaType::QReal &&
                   destination->type == QVariant::Int) {
            Instr instr;
            instr.common.type = Instr::ConvertRealToInt;
            instr.unaryop.output = type.reg;
            instr.unaryop.src = type.reg;
            bytecode << instr;
            type.type = QVariant::Int;
        } else if (type.type == destination->type) {
        } else {
            const QMetaObject *from = type.metaObject;
            const QMetaObject *to = engine->rawMetaObjectForType(destination->type);

            if (QmlMetaPropertyPrivate::canConvert(from, to))
                type.type = destination->type;
        }

        if (type.type == destination->type) {
            Instr instr;
            instr.common.type = Instr::Store;
            instr.store.output = 0;
            instr.store.index = destination->index;
            instr.store.reg = type.reg;
            instr.store.exceptionId = exceptionId(node->expressionCast());
            bytecode << instr;

            releaseReg(type.reg);

            Instr done;
            done.common.type = Instr::Done;
            bytecode << done;

            return true;
        } else {
            return false;
        }
    }
}

bool QmlBindingCompilerPrivate::parseExpression(QmlJS::AST::Node *node, Result &type)
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
    } else if (tryMethod(node)) {
        if (!parseMethod(node, type)) return false;
    } else {
        return false;
    }
    return true;
}

bool QmlBindingCompilerPrivate::tryName(QmlJS::AST::Node *node)
{
    return node->kind == AST::Node::Kind_IdentifierExpression ||
           node->kind == AST::Node::Kind_FieldMemberExpression;
}

bool QmlBindingCompilerPrivate::parseName(AST::Node *node, Result &type)
{
    QStringList nameParts;
    QList<AST::ExpressionNode *> nameNodes;
    if (!buildName(nameParts, node, &nameNodes))
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
                instr.common.type = Instr::LoadScope;
                instr.load.index = 0;
                instr.load.reg = reg;
                bytecode << instr;

                Instr attach;
                attach.common.type = Instr::LoadAttached;
                attach.attached.output = reg;
                attach.attached.reg = reg;
                attach.attached.index = attachType->index();
                attach.attached.exceptionId = exceptionId(nameNodes.at(ii));
                bytecode << attach;

                subscribeName << contextName();
                subscribeName << QLatin1String("$$$ATTACH_") + name;

                absType = 0;
                type.metaObject = attachType->attachedPropertiesType();

                continue;
            } else if (ids.contains(name)) {
                QmlParser::Object *idObject = ids.value(name);
                absType = idObject;
                type.metaObject = absType->metaObject();

                // We check if the id object is the root or 
                // scope object to avoid a subscription
                if (idObject == component) {
                    Instr instr;
                    instr.common.type = Instr::LoadRoot;
                    instr.load.index = 0;
                    instr.load.reg = reg;
                    bytecode << instr;
                } else if (idObject == context) {
                    Instr instr;
                    instr.common.type = Instr::LoadScope;
                    instr.load.index = 0;
                    instr.load.reg = reg;
                    bytecode << instr;
                } else {
                    Instr instr;
                    instr.common.type = Instr::LoadId;
                    instr.load.index = idObject->idIndex;
                    instr.load.reg = reg;
                    bytecode << instr;

                    subscribeName << QLatin1String("$$$ID_") + name;

                    if (subscription(subscribeName, &type)) {
                        Instr sub;
                        sub.common.type = Instr::SubscribeId;
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
                    instr.common.type = Instr::LoadScope;
                    instr.load.index = 0;
                    instr.load.reg = reg;
                    bytecode << instr;

                    subscribeName << contextName();
                    subscribeName << name;

                    fetch(type, context->metaObject(), reg, d0Idx, subscribeName, nameNodes.at(ii));
                } else if(d1Idx != -1) {
                    Instr instr;
                    instr.common.type = Instr::LoadRoot;
                    instr.load.index = 0;
                    instr.load.reg = reg;
                    bytecode << instr;

                    subscribeName << QLatin1String("$$$ROOT");
                    subscribeName << name;

                    fetch(type, component->metaObject(), reg, d1Idx, subscribeName, nameNodes.at(ii));
                } else {
                    Instr find;
                    if (nameParts.count() == 1)
                        find.common.type = Instr::FindGenericTerminal;
                    else
                        find.common.type = Instr::FindGeneric;

                    find.find.reg = reg;
                    find.find.src = -1;
                    find.find.name = registerString(name);
                    find.find.exceptionId = exceptionId(nameNodes.at(ii));

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
                attach.common.type = Instr::LoadAttached;
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
                fetch(type, mo, reg, idx, subscribeName, nameNodes.at(ii));
                if (type.type == -1)
                    return false;
            } else {

                Instr prop;
                if (ii == nameParts.count() -1 ) 
                    prop.common.type = Instr::FindPropertyTerminal;
                else
                    prop.common.type = Instr::FindProperty;

                prop.find.reg = reg;
                prop.find.src = reg;
                prop.find.name = registerString(name);
                prop.find.exceptionId = exceptionId(nameNodes.at(ii));

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

bool QmlBindingCompilerPrivate::tryArith(QmlJS::AST::Node *node)
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

bool QmlBindingCompilerPrivate::parseArith(QmlJS::AST::Node *node, Result &type)
{
    AST::BinaryExpression *expression = static_cast<AST::BinaryExpression *>(node);

    type.reg = acquireReg();

    Result lhs;
    Result rhs;

    if (!parseExpression(expression->left, lhs)) return false;
    if (!parseExpression(expression->right, rhs)) return false;

    if ((lhs.type == QVariant::Int || lhs.type == QMetaType::QReal) &&
        (rhs.type == QVariant::Int || rhs.type == QMetaType::QReal))
        return numberArith(type, lhs, rhs, (QSOperator::Op)expression->op);
    else if(expression->op == QSOperator::Sub)
        return numberArith(type, lhs, rhs, (QSOperator::Op)expression->op);
    else if ((lhs.type == QMetaType::QString || lhs.unknownType) && 
             (rhs.type == QMetaType::QString || rhs.unknownType) && 
             (lhs.type == QMetaType::QString || rhs.type == QMetaType::QString))
        return stringArith(type, lhs, rhs, (QSOperator::Op)expression->op);
    else
        return false;
}

bool QmlBindingCompilerPrivate::numberArith(Result &type, const Result &lhs, const Result &rhs, QSOperator::Op op)
{
    bool nativeReal = rhs.type == QMetaType::QReal ||
                      lhs.type == QMetaType::QReal ||
                      lhs.unknownType ||
                      rhs.unknownType;

    if (nativeReal && lhs.type == QMetaType::Int) {
        Instr convert;
        convert.common.type = Instr::ConvertIntToReal;
        convert.unaryop.output = lhs.reg;
        convert.unaryop.src = lhs.reg;
        bytecode << convert;
    }

    if (nativeReal && rhs.type == QMetaType::Int) {
        Instr convert;
        convert.common.type = Instr::ConvertIntToReal;
        convert.unaryop.output = rhs.reg;
        convert.unaryop.src = rhs.reg;
        bytecode << convert;
    }

    int lhsTmp = -1;
    int rhsTmp = -1;
    if (lhs.unknownType) {
        lhsTmp = acquireReg();

        Instr conv;
        conv.common.type = Instr::ConvertGenericToReal;
        conv.unaryop.output = lhsTmp;
        conv.unaryop.src = lhs.reg;
        bytecode << conv;
    }

    if (rhs.unknownType) {
        rhsTmp = acquireReg();

        Instr conv;
        conv.common.type = Instr::ConvertGenericToReal;
        conv.unaryop.output = rhsTmp;
        conv.unaryop.src = rhs.reg;
        bytecode << conv;
    }

    Instr arith;
    if (op == QSOperator::Add) {
        arith.common.type = nativeReal?Instr::AddReal:Instr::AddInt;
    } else if (op == QSOperator::Sub) {
        arith.common.type = nativeReal?Instr::MinusReal:Instr::MinusInt;
    } else {
        qFatal("Unsupported arithmetic operator");
    }

    arith.binaryop.output = type.reg;
    arith.binaryop.src1 = (lhsTmp == -1)?lhs.reg:lhsTmp;
    arith.binaryop.src2 = (rhsTmp == -1)?rhs.reg:rhsTmp;
    bytecode << arith;

    type.metaObject = 0;
    type.type = nativeReal?QMetaType::QReal:QMetaType::Int;
    type.subscriptionSet.unite(lhs.subscriptionSet);
    type.subscriptionSet.unite(rhs.subscriptionSet);

    if (lhsTmp != -1) releaseReg(lhsTmp);
    if (rhsTmp != -1) releaseReg(rhsTmp);
    releaseReg(lhs.reg);
    releaseReg(rhs.reg);

    return true;
}

bool QmlBindingCompilerPrivate::stringArith(Result &type, const Result &lhs, const Result &rhs, QSOperator::Op op)
{
    if (op != QSOperator::Add)
        return false;

    int lhsTmp = -1;
    int rhsTmp = -1;

    if (lhs.unknownType) {
        lhsTmp = acquireReg(Instr::CleanupString);

        Instr convert;
        convert.common.type = Instr::ConvertGenericToString;
        convert.unaryop.output = lhsTmp;
        convert.unaryop.src = lhs.reg;
        bytecode << convert;
    }

    if (rhs.unknownType) {
        rhsTmp = acquireReg(Instr::CleanupString);

        Instr convert;
        convert.common.type = Instr::ConvertGenericToString;
        convert.unaryop.output = rhsTmp;
        convert.unaryop.src = rhs.reg;
        bytecode << convert;
    }

    type.reg = acquireReg(Instr::CleanupString);
    type.type = QMetaType::QString;

    Instr add;
    add.common.type = Instr::AddString;
    add.binaryop.output = type.reg;
    add.binaryop.src1 = (lhsTmp == -1)?lhs.reg:lhsTmp;
    add.binaryop.src2 = (rhsTmp == -1)?rhs.reg:rhsTmp;
    bytecode << add;

    if (lhsTmp != -1) releaseReg(lhsTmp);
    if (rhsTmp != -1) releaseReg(rhsTmp);

    return true;
}

bool QmlBindingCompilerPrivate::tryLogic(QmlJS::AST::Node *node)
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

bool QmlBindingCompilerPrivate::parseLogic(QmlJS::AST::Node *node, Result &type)
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
            op.common.type = Instr::GreaterThanReal;
        else if (expression->op == QSOperator::Equal)
            op.common.type = Instr::CompareReal;
        else if (expression->op == QSOperator::NotEqual)
            op.common.type = Instr::NotCompareReal;
        else
            return false;
        op.binaryop.output = type.reg;
        op.binaryop.src1 = lhs.reg;
        op.binaryop.src2 = rhs.reg;
        bytecode << op;


    } else if (lhs.type == QMetaType::QString && rhs.type == QMetaType::QString) {

        Instr op;
        if (expression->op == QSOperator::Equal)
            op.common.type = Instr::CompareString;
        else if (expression->op == QSOperator::NotEqual)
            op.common.type = Instr::NotCompareString;
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

bool QmlBindingCompilerPrivate::tryConditional(QmlJS::AST::Node *node)
{
    return (node->kind == AST::Node::Kind_ConditionalExpression);
}

bool QmlBindingCompilerPrivate::parseConditional(QmlJS::AST::Node *node, Result &type)
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
    skip.common.type = Instr::Skip;
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

bool QmlBindingCompilerPrivate::tryConstant(QmlJS::AST::Node *node)
{
    return node->kind == AST::Node::Kind_TrueLiteral ||
           node->kind == AST::Node::Kind_FalseLiteral ||
           node->kind == AST::Node::Kind_NumericLiteral ||
           node->kind == AST::Node::Kind_StringLiteral;
}

bool QmlBindingCompilerPrivate::parseConstant(QmlJS::AST::Node *node, Result &type)
{
    type.metaObject = 0;
    type.type = -1;
    type.reg = acquireReg();

    if (node->kind == AST::Node::Kind_TrueLiteral) {
        type.type = QVariant::Bool;
        Instr instr;
        instr.common.type = Instr::Bool;
        instr.bool_value.reg = type.reg;
        instr.bool_value.value = true;
        bytecode << instr;
        return true;
    } else if (node->kind == AST::Node::Kind_FalseLiteral) {
        type.type = QVariant::Bool;
        Instr instr;
        instr.common.type = Instr::Bool;
        instr.bool_value.reg = type.reg;
        instr.bool_value.value = false;
        bytecode << instr;
        return true;
    } else if (node->kind == AST::Node::Kind_NumericLiteral) {
        qreal value = qreal(static_cast<AST::NumericLiteral *>(node)->value);

        if (qreal(float(value)) != value)
            return false;

        type.type = QMetaType::QReal;
        Instr instr;
        instr.common.type = Instr::Real;
        instr.real_value.reg = type.reg;
        instr.real_value.value = float(value);
        bytecode << instr;
        return true;
    } else if (node->kind == AST::Node::Kind_StringLiteral) {
        QString str = static_cast<AST::StringLiteral *>(node)->value->asString();
        type.type = QMetaType::QString;
        type.reg = registerLiteralString(str);
        return true;
    } else {
        return false;
    }
}

bool QmlBindingCompilerPrivate::tryMethod(QmlJS::AST::Node *node)
{
    return node->kind == AST::Node::Kind_CallExpression; 
}

bool QmlBindingCompilerPrivate::parseMethod(QmlJS::AST::Node *node, Result &result)
{
    AST::CallExpression *expr = static_cast<AST::CallExpression *>(node);

    QStringList name;
    if (!buildName(name, expr->base))
        return false;

    if (name.count() != 2 || name.at(0) != QLatin1String("Math"))
        return false;

    QString method = name.at(1);

    AST::ArgumentList *args = expr->arguments;
    if (!args) return false;
    AST::ExpressionNode *arg0 = args->expression;
    args = args->next;
    if (!args) return false;
    AST::ExpressionNode *arg1 = args->expression;
    if (args->next != 0) return false;
    if (!arg0 || !arg1) return false;

    Result r0;
    if (!parseExpression(arg0, r0)) return false;
    Result r1;
    if (!parseExpression(arg1, r1)) return false;

    if (r0.type != QMetaType::QReal || r1.type != QMetaType::QReal)
        return false;

    Instr op;
    if (method == QLatin1String("max")) {
        op.common.type = Instr::MaxReal;
    } else if (method == QLatin1String("min")) {
        op.common.type = Instr::MinReal;
    } else {
        return false;
    }
    // We release early to reuse registers
    releaseReg(r0.reg);
    releaseReg(r1.reg);

    op.binaryop.output = acquireReg();
    op.binaryop.src1 = r0.reg;
    op.binaryop.src2 = r1.reg;
    bytecode << op;

    result.type = QMetaType::QReal;
    result.reg = op.binaryop.output;

    return true;
}

bool QmlBindingCompilerPrivate::buildName(QStringList &name,
                                       QmlJS::AST::Node *node,
                                       QList<QmlJS::AST::ExpressionNode *> *nodes)
{
    if (node->kind == AST::Node::Kind_IdentifierExpression) {
        name << static_cast<AST::IdentifierExpression*>(node)->name->asString();
        if (nodes) *nodes << static_cast<AST::IdentifierExpression*>(node);
    } else if (node->kind == AST::Node::Kind_FieldMemberExpression) {
        AST::FieldMemberExpression *expr =
            static_cast<AST::FieldMemberExpression *>(node);

        if (!buildName(name, expr->base, nodes))
            return false;

        name << expr->name->asString();
        if (nodes) *nodes << expr;
    } else {
        return false;
    }

    return true;
}


bool QmlBindingCompilerPrivate::fetch(Result &rv, const QMetaObject *mo, int reg, 
                                      int idx, const QStringList &subName, QmlJS::AST::ExpressionNode *node)
{
    QMetaProperty prop = mo->property(idx);
    rv.metaObject = 0;
    rv.type = 0;

    if (subscription(subName, &rv) && prop.hasNotifySignal() && prop.notifySignalIndex() != -1) {
        Instr sub;
        sub.common.type = Instr::Subscribe;
        sub.subscribe.offset = subscriptionIndex(subName);
        sub.subscribe.reg = reg;
        sub.subscribe.index = prop.notifySignalIndex();
        bytecode << sub;
    }

    Instr fetch;
    fetch.common.type = Instr::Fetch;
    fetch.fetch.objectReg = reg;
    fetch.fetch.index = idx;
    fetch.fetch.output = reg;
    fetch.fetch.exceptionId = exceptionId(node);

    rv.type = prop.userType();
    rv.metaObject = engine->metaObjectForType(rv.type);
    rv.reg = reg;

    if (rv.type == QMetaType::QString) {
        int tmp = acquireReg();
        Instr copy;
        copy.common.type = Instr::Copy;
        copy.copy.reg = tmp;
        copy.copy.src = reg;
        bytecode << copy;
        releaseReg(tmp);
        fetch.fetch.objectReg = tmp;

        Instr setup;
        setup.common.type = Instr::NewString;
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

void QmlBindingCompilerPrivate::registerCleanup(int reg, int cleanup, int cleanupType)
{
    registerCleanups.insert(reg, qMakePair(cleanup, cleanupType));
}

int QmlBindingCompilerPrivate::acquireReg(int cleanup, int cleanupType)
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

void QmlBindingCompilerPrivate::releaseReg(int reg)
{
    Q_ASSERT(reg >= 0 && reg <= 31);

    if (registerCleanups.contains(reg)) {
        QPair<int, int> c = registerCleanups[reg];
        registerCleanups.remove(reg);
        Instr cleanup;
        cleanup.common.type = (quint8)c.first;
        cleanup.cleanup.reg = reg;
        bytecode << cleanup;
    }

    quint32 mask = 1 << reg;
    registers &= ~mask;
}

// Returns a reg
int QmlBindingCompilerPrivate::registerLiteralString(const QString &str)
{
    QByteArray strdata((const char *)str.constData(), str.length() * sizeof(QChar));
    int offset = data.count();
    data += strdata;

    int reg = acquireReg(Instr::CleanupString);

    Instr string;
    string.common.type = Instr::String;
    string.string_value.reg = reg;
    string.string_value.offset = offset;
    string.string_value.length = str.length();
    bytecode << string;

    return reg;
}

// Returns an identifier offset
int QmlBindingCompilerPrivate::registerString(const QString &string)
{
    Q_ASSERT(!string.isEmpty());

    QHash<QString, QPair<int, int> >::ConstIterator iter = registeredStrings.find(string);

    if (iter == registeredStrings.end()) {
        quint32 len = string.length();
        QByteArray lendata((const char *)&len, sizeof(quint32));
        QByteArray strdata((const char *)string.constData(), string.length() * sizeof(QChar));
        strdata.prepend(lendata);
        int rv = data.count();
        data += strdata;

        iter = registeredStrings.insert(string, qMakePair(registeredStrings.count(), rv));
    } 

    Instr reg;
    reg.common.type = Instr::InitString;
    reg.initstring.offset = iter->first;
    reg.initstring.dataIdx = iter->second;
    bytecode << reg;
    return reg.initstring.offset;
}

bool QmlBindingCompilerPrivate::subscription(const QStringList &sub, Result *result)
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

int QmlBindingCompilerPrivate::subscriptionIndex(const QStringList &sub)
{
    QString str = sub.join(QLatin1String("."));
    QHash<QString, int>::ConstIterator iter = subscriptionIds.find(str);
    if (iter == subscriptionIds.end()) 
        iter = subscriptionIds.insert(str, subscriptionIds.count());
    usedSubscriptionIds.insert(*iter);
    return *iter;
}

/*
    Returns true if lhs contains no subscriptions that aren't also in base or rhs AND
    rhs contains no subscriptions that aren't also in base or lhs.
*/ 
bool QmlBindingCompilerPrivate::subscriptionNeutral(const QSet<QString> &base, 
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

quint8 QmlBindingCompilerPrivate::exceptionId(QmlJS::AST::ExpressionNode *n)
{
    quint8 rv = 0xFF;
    if (n && exceptions.count() < 0xFF) {
        rv = (quint8)exceptions.count();
        QmlJS::AST::SourceLocation l = n->firstSourceLocation();
        quint64 e = l.startLine;
        e <<= 32;
        e |= l.startColumn;
        exceptions.append(e);
    }
    return rv;
}

QmlBindingCompiler::QmlBindingCompiler()
: d(new QmlBindingCompilerPrivate)
{
}

QmlBindingCompiler::~QmlBindingCompiler()
{
    delete d; d = 0;
}

/* 
Returns true if any bindings were compiled.
*/
bool QmlBindingCompiler::isValid() const
{
    return !d->committed.bytecode.isEmpty();
}

/* 
-1 on failure, otherwise the binding index to use.
*/
int QmlBindingCompiler::compile(const Expression &expression, QmlEnginePrivate *engine)
{
    if (!expression.expression.asAST()) return false;

    d->context = expression.context;
    d->component = expression.component;
    d->destination = expression.property;
    d->ids = expression.ids;
    d->imports = expression.imports;
    d->engine = engine;

    if (d->compile(expression.expression.asAST())) {
        return d->commitCompile();
    } else {
        return -1;
    }
}


QByteArray QmlBindingCompilerPrivate::buildSignalTable() const
{
    QHash<int, QList<int> > table;

    for (int ii = 0; ii < committed.count(); ++ii) {
        const QSet<int> &deps = committed.dependencies.at(ii);
        for (QSet<int>::ConstIterator iter = deps.begin(); iter != deps.end(); ++iter) 
            table[*iter].append(ii);
    }

    QVector<quint32> header;
    QVector<quint32> data;
    for (int ii = 0; ii < committed.subscriptionIds.count(); ++ii) {
        header.append(committed.subscriptionIds.count() + data.count());
        const QList<int> &bindings = table[ii];
        data.append(bindings.count());
        for (int jj = 0; jj < bindings.count(); ++jj)
            data.append(bindings.at(jj));
    }
    header << data;

    return QByteArray((const char *)header.constData(), header.count() * sizeof(quint32));
}

QByteArray QmlBindingCompilerPrivate::buildExceptionData() const
{
    QByteArray rv;
    rv.resize(committed.exceptions.count() * sizeof(quint64));
    ::memcpy(rv.data(), committed.exceptions.constData(), rv.size());
    return rv;
}

/* 
Returns the compiled program.
*/
QByteArray QmlBindingCompiler::program() const
{
    QByteArray programData;

    if (isValid()) {
        Program prog;
        prog.bindings = d->committed.count();

        QVector<Instr> bytecode;
        Instr skip;
        skip.common.type = Instr::Skip;
        skip.skip.reg = -1;
        for (int ii = 0; ii < d->committed.count(); ++ii) {
            skip.skip.count = d->committed.count() - ii - 1;
            skip.skip.count+= d->committed.offsets.at(ii);
            bytecode << skip;
        }
        bytecode << d->committed.bytecode;

        QByteArray data = d->committed.data;
        while (data.count() % 4) data.append('\0');
        prog.signalTableOffset = data.count();
        data += d->buildSignalTable();
        while (data.count() % 4) data.append('\0');
        prog.exceptionDataOffset = data.count();
        data += d->buildExceptionData();

        prog.dataLength = 4 * ((data.size() + 3) / 4);
        prog.subscriptions = d->committed.subscriptionIds.count();
        prog.identifiers = d->committed.registeredStrings.count();
        prog.instructionCount = bytecode.count();
        int size = sizeof(Program) + bytecode.count() * sizeof(Instr);
        size += prog.dataLength;

        programData.resize(size);
        memcpy(programData.data(), &prog, sizeof(Program));
        if (prog.dataLength)
            memcpy((char *)((Program *)programData.data())->data(), data.constData(), 
                   data.size());
        memcpy((char *)((Program *)programData.data())->instructions(), bytecode.constData(), 
               bytecode.count() * sizeof(Instr));
    } 

    return programData;
}



QT_END_NAMESPACE
