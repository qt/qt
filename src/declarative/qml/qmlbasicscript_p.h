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

#ifndef QMLBASICSCRIPT_P_H
#define QMLBASICSCRIPT_P_H

#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtCore/QVariant>
#include <private/qmlparser_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QmlRefCount;
class QmlContext;
class QmlBasicScriptPrivate;
class QmlBasicScriptNodeCache;
class QmlBasicScript
{
public:
    QmlBasicScript();
    QmlBasicScript(const char *, QmlRefCount * = 0);
    ~QmlBasicScript();

    // Always 4-byte aligned
    const char *compileData() const;
    unsigned int compileDataSize() const;

    QByteArray expression() const;

    struct Expression
    {
        QmlParser::Object *component;
        QmlParser::Object *context;
        QmlParser::Property *property;
        QmlParser::Variant expression;
        QHash<QString, QPair<QmlParser::Object *, int> > ids;
    };

    bool compile(const Expression &);
    bool isValid() const;

    void clear();

    void dump();
    void *newScriptState();
    void deleteScriptState(void *);

    enum CacheState { NoChange, Incremental, Reset };
    QVariant run(QmlContext *, void *, CacheState *);

    // Optimization opportunities
    bool isSingleLoad() const;
    QByteArray singleLoadTarget() const;

private:
    int flags;
    QmlBasicScriptPrivate *d;
    QmlRefCount *rc;

    void clearCache(void *);
    void guard(QmlBasicScriptNodeCache &);
    bool valid(QmlBasicScriptNodeCache &, QObject *);
};

class QmlContextPrivate;
class QDebug;
class QmlBasicScriptNodeCache
{
public:
    QObject *object;
    const QMetaObject *metaObject;
    enum { Invalid,
           Core, 
           Attached, 
           Signal,
           SignalProperty, 
           Variant
    } type;
    union {
        int core;
        QObject *attached;
        QmlContextPrivate *context;
    };
    int coreType;
    int contextIndex;

    bool isValid() const { return type != Invalid; }
    bool isCore() const { return type == Core; }
    bool isVariant() const { return type == Variant; }
    void clear();
    QVariant value(const char *) const;
};

QDebug operator<<(QDebug, const QmlBasicScriptNodeCache &);

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLBASICSCRIPT_P_H


