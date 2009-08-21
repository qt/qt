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

#include <QtCore/qglobal.h>
#include <private/qmlengine_p.h>

QT_BEGIN_NAMESPACE

struct QmlCompositeTypeData : public QmlRefCount
{
    QmlCompositeTypeData();
    virtual ~QmlCompositeTypeData();

    enum Status { 
        Invalid,
        Complete,
        Error,
        Waiting
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

    // Add or remove p as a waiter.  When the QmlCompositeTypeData becomes 
    // ready, the QmlComponentPrivate::typeDataReady() method will be invoked on
    // p.  The waiter is automatically removed when the typeDataReady() method
    // is invoked, so there is no need to call remWaiter() in this case.
    void addWaiter(QmlComponentPrivate *p);
    void remWaiter(QmlComponentPrivate *p);

private:
    friend class QmlCompositeTypeManager;
    friend class QmlCompiler;
    friend class QmlDomDocument;

    QmlScriptParser data;
    QList<QmlComponentPrivate *> waiters;
    QmlComponent *component;
    QmlCompiledData *compiledComponent;
};

#endif // QMLCOMPOSITETYPEDATA_P_H

