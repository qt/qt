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

#ifndef QMLBASICSCRIPT_H
#define QMLBASICSCRIPT_H

#include "instructions.h"
#include <QList>
#include <QByteArray>
#include "lexer.h"
#include <QVariant>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
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

    bool compile(const QByteArray &);
    bool compile(const char *);
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

#endif // QMLBASICSCRIPT_H


QT_END_NAMESPACE

QT_END_HEADER
