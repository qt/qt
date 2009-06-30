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

#ifndef QMLBASICSCRIPT_P_H
#define QMLBASICSCRIPT_P_H

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
