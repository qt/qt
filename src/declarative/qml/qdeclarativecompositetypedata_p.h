/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVECOMPOSITETYPEDATA_P_H
#define QDECLARATIVECOMPOSITETYPEDATA_P_H

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

#include "private/qdeclarativeengine_p.h"

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

class QDeclarativeCompositeTypeResource;
class QDeclarativeCompositeTypeData : public QDeclarativeRefCount
{
public:
    QDeclarativeCompositeTypeData();
    virtual ~QDeclarativeCompositeTypeData();

    enum Status {
        Invalid,
        Complete,
        Error,
        Waiting,
        WaitingResources
    };
    Status status;
    enum ErrorType {
        NoError,
        AccessError,
        GeneralError
    };
    ErrorType errorType;

    QList<QDeclarativeError> errors;

    QDeclarativeImports imports;

    QList<QDeclarativeCompositeTypeData *> dependants;

    // Return a QDeclarativeComponent if the QDeclarativeCompositeTypeData is not in the Waiting
    // state.  The QDeclarativeComponent is owned by the QDeclarativeCompositeTypeData, so a
    // reference should be kept to keep the QDeclarativeComponent alive.
    QDeclarativeComponent *toComponent(QDeclarativeEngine *);
    // Return a QDeclarativeCompiledData if possible, or 0 if an error
    // occurs
    QDeclarativeCompiledData *toCompiledComponent(QDeclarativeEngine *);

    struct TypeReference
    {
        TypeReference();

        QDeclarativeType *type;
        QDeclarativeCompositeTypeData *unit;
    };

    struct ScriptReference 
    {
        ScriptReference();

        QString qualifier;
        QDeclarativeCompositeTypeResource *resource;
    };

    QList<TypeReference> types;
    QList<ScriptReference> scripts;
    QList<QDeclarativeCompositeTypeResource *> resources;

    // Add or remove p as a waiter.  When the QDeclarativeCompositeTypeData becomes
    // ready, the QDeclarativeComponentPrivate::typeDataReady() method will be invoked on
    // p.  The waiter is automatically removed when the typeDataReady() method
    // is invoked, so there is no need to call remWaiter() in this case.
    void addWaiter(QDeclarativeComponentPrivate *p);
    void remWaiter(QDeclarativeComponentPrivate *p);

    qreal progress;

private:
    friend class QDeclarativeCompositeTypeManager;
    friend class QDeclarativeCompiler;
    friend class QDeclarativeDomDocument;

    QDeclarativeScriptParser data;
    QList<QDeclarativeComponentPrivate *> waiters;
    QDeclarativeComponent *component;
    QDeclarativeCompiledData *compiledComponent;
};

class QDeclarativeCompositeTypeResource : public QDeclarativeRefCount
{
public:
    QDeclarativeCompositeTypeResource();
    virtual ~QDeclarativeCompositeTypeResource();

    enum Status {
        Invalid,
        Complete,
        Error,
        Waiting
    };
    Status status;

    QList<QDeclarativeCompositeTypeData *> dependants;

    QString url;
    QByteArray data;
};

QT_END_NAMESPACE

#endif // QDECLARATIVECOMPOSITETYPEDATA_P_H

