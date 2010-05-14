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

#ifndef QDECLARATIVECOMPONENT_P_H
#define QDECLARATIVECOMPONENT_P_H

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

#include "qdeclarativecomponent.h"

#include "private/qdeclarativeengine_p.h"
#include "private/qdeclarativecompositetypemanager_p.h"
#include "private/qbitfield_p.h"
#include "qdeclarativeerror.h"
#include "qdeclarative.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeComponent;
class QDeclarativeEngine;
class QDeclarativeCompiledData;

class QDeclarativeComponentAttached;
class QDeclarativeComponentPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeComponent)
        
public:
    QDeclarativeComponentPrivate() : typeData(0), progress(0.), start(-1), count(-1), cc(0), engine(0), creationContext(0) {}

    QObject *create(QDeclarativeContextData *, const QBitField &);
    QObject *beginCreate(QDeclarativeContextData *, const QBitField &);
    void completeCreate();

    QDeclarativeCompositeTypeData *typeData;
    void typeDataReady();
    void updateProgress(qreal);
    
    void fromTypeData(QDeclarativeCompositeTypeData *data);

    QUrl url;
    qreal progress;

    int start;
    int count;
    QDeclarativeCompiledData *cc;

    struct ConstructionState {
        ConstructionState() : componentAttached(0), completePending(false) {}
        QList<QDeclarativeEnginePrivate::SimpleList<QDeclarativeAbstractBinding> > bindValues;
        QList<QDeclarativeEnginePrivate::SimpleList<QDeclarativeParserStatus> > parserStatus;
        QList<QPair<QDeclarativeGuard<QObject>, int> > finalizedParserStatus;
        QDeclarativeComponentAttached *componentAttached;
        QList<QDeclarativeError> errors;
        bool completePending;
    };
    ConstructionState state;

    static QObject *begin(QDeclarativeContextData *ctxt, QDeclarativeEnginePrivate *enginePriv,
                          QDeclarativeCompiledData *component, int start, int count,
                          ConstructionState *state, const QBitField &bindings = QBitField());
    static void beginDeferred(QDeclarativeEnginePrivate *enginePriv, QObject *object, 
                              ConstructionState *state);
    static void complete(QDeclarativeEnginePrivate *enginePriv, ConstructionState *state);

    QDeclarativeEngine *engine;
    QDeclarativeGuardedContextData creationContext;

    void clear();

    static QDeclarativeComponentPrivate *get(QDeclarativeComponent *c) {
        return static_cast<QDeclarativeComponentPrivate *>(QObjectPrivate::get(c));
    }
};

class QDeclarativeComponentAttached : public QObject
{
    Q_OBJECT
public:
    QDeclarativeComponentAttached(QObject *parent = 0);
    ~QDeclarativeComponentAttached();

    void add(QDeclarativeComponentAttached **a) {
        prev = a; next = *a; *a = this;
        if (next) next->prev = &next;
    }
    void rem() {
        if (next) next->prev = prev;
        *prev = next;
        next = 0; prev = 0;
    }
    QDeclarativeComponentAttached **prev;
    QDeclarativeComponentAttached *next;

Q_SIGNALS:
    void completed();
    void destruction();

private:
    friend class QDeclarativeContextData;
    friend class QDeclarativeComponentPrivate;
};

QT_END_NAMESPACE

#endif // QDECLARATIVECOMPONENT_P_H
