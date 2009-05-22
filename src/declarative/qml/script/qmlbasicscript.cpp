/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "qmlbasicscript.h"
#include "qmlbasicscript_p.h"
#include "lexer.h"
#include <QColor>
#include <QDebug>
#include <private/qmlengine_p.h>
#include <private/qmlcontext_p.h>
#include <QStack>
#include <qfxperf.h>
#include <private/qmlrefcount_p.h>


QT_BEGIN_NAMESPACE
DEFINE_BOOL_CONFIG_OPTION(scriptWarnings, QML_SCRIPT_WARNINGS);

class QmlBasicScriptPrivate
{
public:
    enum Flags { OwnData = 0x00000001 };
    
    int size;
    int stateSize;
    int instructionCount;
    int exprLen;

    ScriptInstruction *instructions() const { return (ScriptInstruction *)((char *)this + sizeof(QmlBasicScriptPrivate)); }

    const char *expr() const
    {
        return (const char *)(instructions() + instructionCount);
    }

    const char *data() const 
    {
        return (const char *)(instructions() + instructionCount) + exprLen + 1;
    }

    static unsigned int alignRound(int s)
    {
        if (s % 4) 
            s += 4 - (s % 4);
        return s;
    }
};

QDebug operator<<(QDebug lhs, const QmlBasicScriptNodeCache &rhs)
{
    switch(rhs.type) {
    case QmlBasicScriptNodeCache::Invalid:
        lhs << "Invalid";
        break;
    case QmlBasicScriptNodeCache::Core:
        lhs << "Core" << rhs.object << rhs.core;
        break;
    case QmlBasicScriptNodeCache::Attached: 
        lhs << "Attached" << rhs.object << rhs.attached;
        break;
    case QmlBasicScriptNodeCache::Signal: 
        lhs << "Signal" << rhs.object << rhs.core;
        break;
    case QmlBasicScriptNodeCache::SignalProperty: 
        lhs << "SignalProperty" << rhs.object << rhs.core;
        break;
    case QmlBasicScriptNodeCache::Variant: 
        lhs << "Variant" << rhs.context;
        break;
    }

    return lhs;
}

void QmlBasicScriptNodeCache::clear()
{
    object = 0;
    metaObject = 0;
    type = Invalid;
}

static QVariant toObjectOrVariant(const QVariant &v)
{
    switch(v.type()) {
        case QVariant::String:
        case QVariant::UInt:
        case QVariant::Int:
        case 135:
        case QVariant::Double:
        case QVariant::Color:
        case QVariant::Bool:
        default:
            return v;
        case QVariant::UserType:
            {
                QObject *o = QmlMetaType::toQObject(v);
                if (o)
                    return qVariantFromValue(o);
                else
                    return v;
            }
            break;
    }
}

static QVariant fetch_value(QObject *o, int idx, int type)
{
    switch(type) {
        case QVariant::String:
            {
                QString val;
                void *args[] = { &val, 0 };
                QMetaObject::metacall(o, QMetaObject::ReadProperty, idx, args);
                return QVariant(val);
            }
            break;
        case QVariant::UInt:
            {
                uint val;
                void *args[] = { &val, 0 };
                QMetaObject::metacall(o, QMetaObject::ReadProperty, idx, args);
                return QVariant(val);
            }
            break;
        case QVariant::Int:
            {
                int val;
                void *args[] = { &val, 0 };
                QMetaObject::metacall(o, QMetaObject::ReadProperty, idx, args);
                return QVariant(val);
            }
            break;
        case 135:
            {
                float val;
                void *args[] = { &val, 0 };
                QMetaObject::metacall(o, QMetaObject::ReadProperty, idx, args);
                return QVariant(val);
            }
            break;
        case QVariant::Double:
            {
                double val;
                void *args[] = { &val, 0 };
                QMetaObject::metacall(o, QMetaObject::ReadProperty, idx, args);
                return QVariant(val);
            }
            break;
        case QVariant::Color:
            {
                QColor val;
                void *args[] = { &val, 0 };
                QMetaObject::metacall(o, QMetaObject::ReadProperty, idx, args);
                return QVariant(val);
            }
            break;
        case QVariant::Bool:
            {
                bool val;
                void *args[] = { &val, 0 };
                QMetaObject::metacall(o, QMetaObject::ReadProperty, idx, args);
                return QVariant(val);
            }
            break;
        default:  
            {
                if (QmlMetaType::isObject(type)) {
                    // NOTE: This assumes a cast to QObject does not alter the 
                    // object pointer
                    QObject *val = 0;
                    void *args[] = { &val, 0 };
                    QMetaObject::metacall(o, QMetaObject::ReadProperty, idx, args);
                    return QVariant::fromValue(val);
                } else {
                    QVariant var = o->metaObject()->property(idx).read(o);
                    if (QmlMetaType::isObject(var.userType())) {
                        QObject *obj = 0;
                        obj = *(QObject **)var.data();
                        var = QVariant::fromValue(obj);
                    }
                    return var;
                }
            }
            break;
    };
}

QVariant QmlBasicScriptNodeCache::value(const char *name) const
{
    //QFxPerfTimer<QFxPerf::BasicScriptValue> pt;
    switch(type) {
    case Invalid:
        break;
    case Core:
        return fetch_value(object, core, coreType);
        break;
    case Attached:
        return qVariantFromValue(static_cast<QObject *>(attached));
        break;
    case Signal:
        // XXX
        Q_ASSERT(!"Not implemented");
        break;
    case SignalProperty:
        break;
    case Variant:
        return context->propertyValues[contextIndex];
    };
    return QVariant();
}

struct QmlBasicScriptCompiler
{
    QmlBasicScriptCompiler()
    : script(0), stateSize(0), src(0), idx(0) {}
    QmlBasicScript *script;
    QList<LexerToken> tokens;
    int stateSize;
    const char *src;
    int idx;

    bool compile();
    bool compileExpr();

    bool parseFetch();
    bool parseName();
    bool parseConstant();
    void skipWhitespace();

    QByteArray data;
    QList<ScriptInstruction> bytecode;

    QByteArray string(int, int);
    Token token() const;
    bool atEnd() const;
    void adv();
    int index() const;
};

/*!
    \internal
    \class QmlBasicScript
    \brief The QmlBasicScript class provides a fast implementation of a limited subset of JavaScript bindings.

    QmlBasicScript instances are used to accelerate binding.  Instead of using
    the slower, fully fledged JavaScript engine, many simple bindings can be
    evaluated using the QmlBasicScript engine.

    To see if the QmlBasicScript engine can handle a binding, call compile()
    and check the return value, or isValid() afterwards.  

    To evaluate the binding, the QmlBasicScript instance needs some memory in
    which to cache state.  This may be allocated by calling newScriptState()
    and destroyed by calling deleteScriptState().  The state data is then passed
    to the run() method when evaluating the binding.

    To further accelerate binding, QmlBasicScript can return a precompiled 
    version of itself that can be saved for future use.  Call compileData() to
    get an opaque pointer to the compiled state, and compileDataSize() for the
    size of this data in bytes.  This data can be saved and passed to future
    instances of the QmlBasicScript constructor.  The initial copy of compile
    data is owned by the QmlBindScript instance on which compile() was called.
*/

/*!
    Create a new QmlBasicScript instance.
*/
QmlBasicScript::QmlBasicScript()
: flags(0), d(0), rc(0)
{
}

/*!
    Create a new QmlBasicScript instance from saved \a data.

    \a data \b must be data previously acquired from calling compileData() on a
    previously created QmlBasicScript instance.  Any other data will almost
    certainly cause the QmlBasicScript engine to crash.

    \a data must continue to be valid throughout the QmlBasicScript instance 
    life.  It does not assume ownership of the memory.

    If \a owner is set, it is referenced on creation and dereferenced on 
    destruction of this instance.
*/
QmlBasicScript::QmlBasicScript(const char *data, QmlRefCount *owner)
: flags(0), d((QmlBasicScriptPrivate *)data), rc(owner)
{
    if (rc) rc->addref();
}

/*!
    Return the text of the script expression.
 */
QByteArray QmlBasicScript::expression() const
{
    if (!d)
        return QByteArray();
    else
        return QByteArray(d->expr());
}

/*!
    Destroy the script instance.
*/
QmlBasicScript::~QmlBasicScript()
{
    if (flags & QmlBasicScriptPrivate::OwnData)
        free(d);
    if (rc) rc->release();
    d = 0;
    rc = 0;
}

/*!
    Clear this script.  The object will then be in its initial state, as though
    it were freshly constructed with default constructor.
*/
void QmlBasicScript::clear()
{
    if (flags & QmlBasicScriptPrivate::OwnData)
        free(d);
    if (rc) rc->release();
    d = 0;
    rc = 0;
    flags = 0;
}

/*!
    Return the script state memory for this script instance.  This memory should
    only be destroyed by calling deleteScriptState().
 */
void *QmlBasicScript::newScriptState()
{
    if (!d) {
        return 0;
    } else  {
        void *rv = ::malloc(d->stateSize * sizeof(QmlBasicScriptNodeCache));
        ::memset(rv, 0, d->stateSize * sizeof(QmlBasicScriptNodeCache));
        return rv;
    }
}

/*!
    Delete the \a data previously allocated by newScriptState().
 */
void QmlBasicScript::deleteScriptState(void *data)
{
    if (!data) return;
    Q_ASSERT(d);
    clearCache(data);
    free(data);
}

/*!
    Dump the script instructions to stderr for debugging.
 */
void QmlBasicScript::dump()
{
    if (!d)
        return;

    qWarning() << d->instructionCount << "instructions:";
    const char *data = d->data();
    for (int ii = 0; ii < d->instructionCount; ++ii) {
        const ScriptInstruction &instr = d->instructions()[ii];

        switch(instr.type) {
        case ScriptInstruction::Load:
            qWarning().nospace() << "LOAD\t\t" << instr.fetch.idx << "\t\t" 
                                 << QByteArray(data + instr.fetch.idx);
            break;
        case ScriptInstruction::Fetch:
            qWarning().nospace() << "FETCH\t\t" << instr.fetch.idx << "\t\t" 
                                 << QByteArray(data + instr.fetch.idx);
            break;
        case ScriptInstruction::Add:
            qWarning().nospace() << "ADD";
            break;
        case ScriptInstruction::Subtract:
            qWarning().nospace() << "SUBTRACT";
            break;
        case ScriptInstruction::Multiply:
            qWarning().nospace() << "MULTIPLY";
            break;
        case ScriptInstruction::Equals:
            qWarning().nospace() << "EQUALS";
            break;
        case ScriptInstruction::Int:
            qWarning().nospace() << "INT\t\t" << instr.integer.value;
            break;
        case ScriptInstruction::Bool:
            qWarning().nospace() << "BOOL\t\t" << instr.boolean.value;
            break;
        default:
            qWarning().nospace() << "UNKNOWN";
            break;
        }
    }
}

/*!
    Return true if this is a valid script binding, otherwise returns false.
 */
bool QmlBasicScript::isValid() const
{
    return d != 0;
}

/*!
    Compile \a src and return true if the compilation is successful, otherwise
    returns false.
 */
bool QmlBasicScript::compile(const QByteArray &src)
{
    bool rv = compile(src.constData());
    return rv;
}

/*!
    \overload

    Compile \a src and return true if the compilation is successful, otherwise
    returns false.
 */
bool QmlBasicScript::compile(const char *src)
{
    if (!src) return false;

    QmlBasicScriptCompiler bsc;
    bsc.script = this;
    bsc.tokens = tokenize(src);
    bsc.src = src;
    // dumpTokens(src, bsc.tokens);

    if (d) {
        if (flags & QmlBasicScriptPrivate::OwnData)
            free(d);
        d = 0;
        flags = 0;
    }

    if (bsc.compile()) {
        int len = ::strlen(src);
        flags = QmlBasicScriptPrivate::OwnData;
        int size = sizeof(QmlBasicScriptPrivate) + 
                   bsc.bytecode.count() * sizeof(ScriptInstruction) + 
                   QmlBasicScriptPrivate::alignRound(bsc.data.count() + len + 1);
        d = (QmlBasicScriptPrivate *) malloc(size);
        d->size = size;
        d->stateSize = bsc.stateSize;
        d->instructionCount = bsc.bytecode.count();
        d->exprLen = len;
        ::memcpy((char *)d->expr(), src, len + 1);
        for (int ii = 0; ii < d->instructionCount; ++ii) 
            d->instructions()[ii] = bsc.bytecode.at(ii);
        ::memcpy((char *)d->data(), bsc.data.constData(), bsc.data.count());
    } 

    return d != 0;
}

void QmlBasicScriptCompiler::skipWhitespace()
{
    while(idx < tokens.count() && tokens.at(idx).token == WHITESPACE)
        ++idx;
}

bool QmlBasicScriptCompiler::compile()
{
    if (!compileExpr())
        return false;

    skipWhitespace();

    if (atEnd())
        return true;

    int t = token();
    if (t != AND) 
        return false;

    adv();
    skipWhitespace();
    if (!compileExpr())
        return false;

    ScriptInstruction instr;
    instr.type = ScriptInstruction::And;
    bytecode.append(instr);

    skipWhitespace();

    return atEnd();
}

bool QmlBasicScriptCompiler::compileExpr()
{
    /*
        EXPRESSION := <NAME><OPERATOR>[<CONSTANT>|<NAME>]
     */

    if (!parseName())
        return false;

    skipWhitespace();

    if (atEnd())
        return true;

    int t = token();
    switch(t) {
    case PLUS:
    case MINUS:
        /*
    case LANGLE:
    case RANGLE:
    */
    case STAR:
    case EQUALS:
        break;
    default:
        return true;
    }
    adv();

    skipWhitespace();

    if (!parseConstant() &&
       !parseName())
        return false;

    ScriptInstruction instr;
    switch(t) {
    case PLUS:
        instr.type = ScriptInstruction::Add;
        break;
    case MINUS:
        instr.type = ScriptInstruction::Subtract;
        break;
    case STAR:
        instr.type = ScriptInstruction::Multiply;
        break;
    case EQUALS:
        instr.type = ScriptInstruction::Equals;
        break;
    default:
        break;
    }
    bytecode.append(instr);

    skipWhitespace();

    return true;
}

bool QmlBasicScriptCompiler::parseName()
{
    skipWhitespace();
    
    bool named = false;
    bool seenchar = false;
    bool seendot = false;
    int namestart = -1;
    bool pushed = false;
    while(!atEnd()) {
        int t = token();
        if (t == CHARACTER) {
            named = true;
            seendot = false;
            seenchar = true;
            namestart = index();
            adv();
        } else if (t == DIGIT) {
            if (!seenchar) break;
            adv();
        } else if (t == DOT) {
            seendot = true;
            if (namestart == -1)
                break;

            seenchar = false;
            QByteArray name = string(namestart, index() - 1);
            int nref = data.count();
            data.append(name);
            data.append('\0');
            ScriptInstruction instr;
            if (pushed)
                instr.type = ScriptInstruction::Fetch;
            else
                instr.type = ScriptInstruction::Load;
            pushed = true;
            instr.fetch.idx = nref;
            bytecode.append(instr);
            ++stateSize;
            namestart = -1;
            adv();
        } else {
            break;
        }
    }

    if (namestart != -1) {
        QByteArray name = string(namestart, index() - 1);
        int nref = data.count();
        data.append(name);
        data.append('\0');
        ScriptInstruction instr;
        if (pushed)
            instr.type = ScriptInstruction::Fetch;
        else
            instr.type = ScriptInstruction::Load;
        pushed = true;
        instr.fetch.idx = nref;
        bytecode.append(instr);
        ++stateSize;
    }

    if (seendot)
        return false;
    else
        return named;
}

bool QmlBasicScriptCompiler::parseConstant()
{
    switch(token()) {
    case DIGIT:
        {
            ScriptInstruction instr;
            instr.type = ScriptInstruction::Int;
            instr.integer.value = string(index(), index()).toUInt();
            bytecode.append(instr);
            adv();
        }
        break;
    case TOKEN_TRUE:
    case TOKEN_FALSE:
        {
            ScriptInstruction instr;
            instr.type = ScriptInstruction::Bool;
            instr.boolean.value = (token() == TOKEN_TRUE);
            bytecode.append(instr);
            adv();
        }
        break;

    default:
        return false;
    }

    return true;
}

bool QmlBasicScriptCompiler::atEnd() const
{
    return idx >= tokens.count();
}

Token QmlBasicScriptCompiler::token() const
{
    return tokens.at(idx).token;
}

void QmlBasicScriptCompiler::adv()
{
    ++idx;
}

int QmlBasicScriptCompiler::index() const
{
    return idx;
}

QByteArray QmlBasicScriptCompiler::string(int from, int to)
{
    QByteArray rv;
    for (int ii = from; ii <= to; ++ii) {
        const LexerToken &token = tokens.at(ii);
        rv.append(QByteArray(src + token.start, token.end - token.start + 1));
    }
    return rv;
}

/*!
    \internal
*/
void QmlBasicScript::clearCache(void *voidCache)
{
    QmlBasicScriptNodeCache *dataCache =
        reinterpret_cast<QmlBasicScriptNodeCache *>(voidCache);

    for (int ii = 0; ii < d->stateSize; ++ii) {
        if (!dataCache[ii].isCore() && !dataCache[ii].isVariant() && 
            dataCache[ii].object) {
            QMetaObject::removeGuard(&dataCache[ii].object);
            dataCache[ii].object = 0;
        }
        dataCache[ii].clear();
    }
}

void QmlBasicScript::guard(QmlBasicScriptNodeCache &n)
{
    if (n.object) {
        if (n.isVariant()) {
        } else if (n.isCore()) {
            n.metaObject = 
                n.object->metaObject();
        } else {
            QMetaObject::addGuard(&n.object);
        }
    }
}

bool QmlBasicScript::valid(QmlBasicScriptNodeCache &n, QObject *obj)
{
    return n.object == obj && 
                       (!n.isCore() || obj->metaObject() == n.metaObject); 
}


/*!
    \enum QmlBasicScript::CacheState
    \value NoChange The query has not change.  Any previous monitoring is still
    valid.
    \value Incremental The query has been incrementally changed.  Any previous 
    monitoring is still valid, but needs to have the fresh properties added to
    it.
    \value Reset The entire query has been reset from the beginning.  Any previous
    monitoring is now invalid.
*/

/*!
    Run the script in \a context and return the result.  \a voidCache should 
    contain state memory previously acquired from newScript. 
 */
QVariant QmlBasicScript::run(QmlContext *context, void *voidCache, CacheState *cached)
{
    if (!isValid())
        return QVariant();

    QmlBasicScriptNodeCache *dataCache =
        reinterpret_cast<QmlBasicScriptNodeCache *>(voidCache);
    int dataCacheItem; 
    QStack<QVariant> stack;

    bool resetting = false;
    bool hasReset = false;

    const char *data = d->data();

    if (dataCache[0].type == QmlBasicScriptNodeCache::Invalid) {
        resetting = true;
        hasReset = true;
    }

    CacheState state = NoChange;

    dataCacheItem = 0;
    for (int idx = 0; idx < d->instructionCount; ++idx) {
        const ScriptInstruction &instr = d->instructions()[idx];

        switch(instr.type) {
            case ScriptInstruction::Load: // either an object or a property
            case ScriptInstruction::Fetch: // can only be a property
            {
                const char *id = data + instr.fetch.idx;
                QmlBasicScriptNodeCache &n = dataCache[dataCacheItem];

                if (instr.type == ScriptInstruction::Load) {

                    if (n.type == QmlBasicScriptNodeCache::Invalid) {
                        context->engine()->d_func()->loadCache(n, QLatin1String(id), static_cast<QmlContextPrivate*>(context->d_ptr));
                        state = Incremental;
                    }

                    if(!n.isValid())
                        qWarning("ReferenceError: %s is not defined", id);

                } else { // instr.type == ScriptInstruction::Fetch

                    QVariant o = stack.pop();
                    QObject *obj = qvariant_cast<QObject *>(o);
                    if (!obj) {
                        if (n.type == QmlBasicScriptNodeCache::Invalid) {
                            if (scriptWarnings())
                                qWarning() << "QmlBasicScript: Unable to convert" << o;
                            *cached = state;
                            return QVariant();
                        } else {
                            clearCache(dataCache);
                            *cached = Reset;
                            CacheState dummy;
                            return run(context, voidCache, &dummy);
                        }
                    } else if (n.type == QmlBasicScriptNodeCache::Invalid) {
                        context->engine()->d_func()->fetchCache(n, QLatin1String(id), obj);
                        guard(n);
                        state = Incremental;
                    } else if (!valid(n, obj)) {
                        clearCache(dataCache);
                        *cached = Reset;
                        CacheState dummy;
                        return run(context, voidCache, &dummy);
                    }

                }

                QVariant var = n.value(id);
                stack.push(var);
                ++dataCacheItem;
            }
                break;
            case ScriptInstruction::Int:
                stack.push(QVariant(instr.integer.value));
                break;
            case ScriptInstruction::Bool:
                stack.push(QVariant(instr.boolean.value));
                break;
            case ScriptInstruction::Add:
                {
                    QVariant rhs = stack.pop();
                    QVariant lhs = stack.pop();

                    stack.push(rhs.toDouble() + lhs.toDouble());
                }
                break;
            case ScriptInstruction::Subtract:
                {
                    QVariant rhs = stack.pop();
                    QVariant lhs = stack.pop();

                    stack.push(lhs.toDouble() - rhs.toDouble());
                }
                break;
            case ScriptInstruction::Multiply:
                {
                    QVariant rhs = stack.pop();
                    QVariant lhs = stack.pop();

                    stack.push(rhs.toDouble() * lhs.toDouble());
                }
                break;
            case ScriptInstruction::Equals:
                {
                    QVariant rhs = stack.pop();
                    QVariant lhs = stack.pop();

                    stack.push(rhs == lhs);
                }
                break;
            case ScriptInstruction::And:
                {
                    QVariant rhs = stack.pop();
                    QVariant lhs = stack.pop();

                    stack.push(rhs.toBool() && lhs.toBool());
                }
                break;
            default:
                break;
        }
    }

    *cached = state;

    if (stack.isEmpty())
        return QVariant();
    else
        return stack.top();
}

/*!
    Return a pointer to the script's compile data, or null if there is no data.
 */
const char *QmlBasicScript::compileData() const
{
    return (const char *)d;
}

/*!
    Return the size of the script's compile data, or zero if there is no data.
    The size will always be a multiple of 4.
 */
unsigned int QmlBasicScript::compileDataSize() const
{
    if (d)
        return d->size;
    else
        return 0;
}

bool QmlBasicScript::isSingleLoad() const
{
    if (!d)
        return false;

    return d->instructionCount == 1 &&
           d->instructions()[0].type == ScriptInstruction::Load;
}

QByteArray QmlBasicScript::singleLoadTarget() const
{
    if (!isSingleLoad())
        return QByteArray();

    // We know there is one instruction and it is a load
    return QByteArray(d->data() + d->instructions()[0].fetch.idx);
}


QT_END_NAMESPACE
