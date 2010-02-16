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

#ifndef QMLCOMPOSITETYPEDATA_P_H
#define QMLCOMPOSITETYPEDATA_P_H

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

#include "qmlengine_p.h"

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

class QmlCompositeTypeResource;
class QmlCompositeTypeData : public QmlRefCount
{
public:
    QmlCompositeTypeData();
    virtual ~QmlCompositeTypeData();

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

    QList<QmlError> errors;

    QmlEnginePrivate::Imports imports;

    QList<QmlCompositeTypeData *> dependants;

    // Return a QmlComponent if the QmlCompositeTypeData is not in the Waiting
    // state.  The QmlComponent is owned by the QmlCompositeTypeData, so a
    // reference should be kept to keep the QmlComponent alive.
    QmlComponent *toComponent(QmlEngine *);
    // Return a QmlCompiledData if possible, or 0 if an error
    // occurs
    QmlCompiledData *toCompiledComponent(QmlEngine *);

    struct TypeReference
    {
        TypeReference();

        QmlType *type;
        QmlCompositeTypeData *unit;
    };

    QList<TypeReference> types;
    QList<QmlCompositeTypeResource *> resources;

    // Add or remove p as a waiter.  When the QmlCompositeTypeData becomes
    // ready, the QmlComponentPrivate::typeDataReady() method will be invoked on
    // p.  The waiter is automatically removed when the typeDataReady() method
    // is invoked, so there is no need to call remWaiter() in this case.
    void addWaiter(QmlComponentPrivate *p);
    void remWaiter(QmlComponentPrivate *p);

    qreal progress;

private:
    friend class QmlCompositeTypeManager;
    friend class QmlCompiler;
    friend class QmlDomDocument;

    QmlScriptParser data;
    QList<QmlComponentPrivate *> waiters;
    QmlComponent *component;
    QmlCompiledData *compiledComponent;
};

class QmlCompositeTypeResource : public QmlRefCount
{
public:
    QmlCompositeTypeResource();
    virtual ~QmlCompositeTypeResource();

    enum Status {
        Invalid,
        Complete,
        Error,
        Waiting
    };
    Status status;

    QList<QmlCompositeTypeData *> dependants;

    QString url;
    QByteArray data;
};

QT_END_NAMESPACE

#endif // QMLCOMPOSITETYPEDATA_P_H

