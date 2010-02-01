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

#ifndef QMLBINDINGOPTIMIZATIONS_P_H
#define QMLBINDINGOPTIMIZATIONS_P_H

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

#include "qmlexpression_p.h"
#include "qmlbinding.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

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

    static void dump(const QByteArray &);
private:
    QmlBindingCompilerPrivate *d;
};

class QmlCompiledBindingsPrivate;
class QmlCompiledBindings : public QObject, public QmlAbstractExpression, public QmlRefCount
{
public:
    QmlCompiledBindings(const char *program, QmlContext *context);
    virtual ~QmlCompiledBindings();

    QmlAbstractBinding *configBinding(int index, QObject *target, QObject *scope, int property);

protected:
    int qt_metacall(QMetaObject::Call, int, void **);

private:
    Q_DISABLE_COPY(QmlCompiledBindings);
    Q_DECLARE_PRIVATE(QmlCompiledBindings);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLBINDINGOPTIMIZATIONS_P_H

