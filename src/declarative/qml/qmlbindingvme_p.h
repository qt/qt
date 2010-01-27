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

#ifndef QMLBINDINGVME_P_H
#define QMLBINDINGVME_P_H

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

#include <QtCore/qglobal.h>
#include <private/qscriptdeclarativeclass_p.h>
#include "qmlexpression_p.h"
#include "qmlguard_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QObject;
class QmlContextPrivate;
class QmlBindingVME 
{
public:
    struct Config {
        Config() : target(0), targetSlot(-1), subscriptions(0), identifiers(0) {}
        ~Config() { delete [] subscriptions; delete [] identifiers; }
        QObject *target;
        int targetSlot;

        struct Subscription {
            struct Signal {
                QmlGuard<QObject> source;
                int notifyIndex;
            };

            struct Id {
                inline Id();
                inline ~Id();
                inline void reset();
                Id *next;
                Id**prev;
                QObject *target;
                int methodIndex;
            };

            enum { InvalidType, SignalType, IdType } type;
            inline Subscription();
            inline ~Subscription();
            bool isSignal() const { return type == SignalType; }
            bool isId() const { return type == IdType; }
            inline Signal *signal();
            inline Id *id();
            union {
                char signalData[sizeof(Signal)];
                char idData[sizeof(Id)];
            };
        };
        Subscription *subscriptions;
        QScriptDeclarativeClass::PersistentIdentifier *identifiers;
    };

    static void init(const char *program, Config *config, 
                     quint32 **sigTable, quint32 *bindingCount);
    static void run(const char *program, int instr,
                    Config *config, QmlContextPrivate *context, QmlDelayedError *error, 
                    QObject *scope, QObject *output);
    static void dump(const char *);
};

QmlBindingVME::Config::Subscription::Subscription()
: type(InvalidType)
{
}

QmlBindingVME::Config::Subscription::~Subscription()
{
    if (type == SignalType) ((Signal *)signalData)->~Signal();
    else if (type == IdType) ((Id *)idData)->~Id();
}

QmlBindingVME::Config::Subscription::Id::Id()
: next(0), prev(0), target(0), methodIndex(-1)
{
}

QmlBindingVME::Config::Subscription::Id::~Id()
{
    reset();
}

void QmlBindingVME::Config::Subscription::Id::reset()
{
    if (next) next->prev = prev;
    if (prev) *prev = next;
    next = 0;
    prev = 0;
    target = 0;
    methodIndex = -1;
}

class QmlBindingCompilerPrivate;
class QmlBindingCompiler
{
public:
    QmlBindingCompiler();
    ~QmlBindingCompiler();

    // Returns true if bindings were compiled
    bool isValid() const;

    struct Expression
    {
        QmlParser::Object *component;
        QmlParser::Object *context;
        QmlParser::Property *property;
        QmlParser::Variant expression;
        QHash<QString, QmlParser::Object *> ids;
        QmlEnginePrivate::Imports imports;
    };

    // -1 on failure, otherwise the binding index to use
    int compile(const Expression &, QmlEnginePrivate *);

    // Returns the compiled program
    QByteArray program() const;

private:
    QmlBindingCompilerPrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLBINDINGVME_P_H

