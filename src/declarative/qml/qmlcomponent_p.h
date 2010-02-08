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

#ifndef QMLCOMPONENT_P_H
#define QMLCOMPONENT_P_H

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

#include "qmlcomponent.h"

#include "qmlengine_p.h"
#include "qmlcompositetypemanager_p.h"
#include "qbitfield_p.h"
#include "qmlerror.h"
#include "qml.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QmlComponent;
class QmlEngine;
class QmlCompiledData;

class QmlComponentAttached;
class QmlComponentPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlComponent)
        
public:
    QmlComponentPrivate() : typeData(0), progress(0.), start(-1), count(-1), cc(0), engine(0), creationContext(0) {}

    QObject *create(QmlContext *context, const QBitField &);
    QObject *beginCreate(QmlContext *, const QBitField &);
    void completeCreate();

    QmlCompositeTypeData *typeData;
    void typeDataReady();
    void updateProgress(qreal);
    
    void fromTypeData(QmlCompositeTypeData *data);

    QUrl url;
    qreal progress;

    int start;
    int count;
    QmlCompiledData *cc;

    struct ConstructionState {
        ConstructionState() : componentAttacheds(0), completePending(false) {}
        QList<QmlEnginePrivate::SimpleList<QmlAbstractBinding> > bindValues;
        QList<QmlEnginePrivate::SimpleList<QmlParserStatus> > parserStatus;
        QmlComponentAttached *componentAttacheds;
        QList<QmlError> errors;
        bool completePending;
    };
    ConstructionState state;

    static QObject *begin(QmlContext *ctxt, QmlEnginePrivate *enginePriv,
                          QmlCompiledData *component, int start, int count,
                          ConstructionState *state, const QBitField &bindings = QBitField());
    static void beginDeferred(QmlContext *ctxt, QmlEnginePrivate *enginePriv,
                              QObject *object, ConstructionState *state);
    static void complete(QmlEnginePrivate *enginePriv, ConstructionState *state);

    QmlEngine *engine;
    QmlContext *creationContext;

    void clear();

    static QmlComponentPrivate *get(QmlComponent *c) {
        return static_cast<QmlComponentPrivate *>(QObjectPrivate::get(c));
    }
};

class QmlComponentAttached : public QObject
{
    Q_OBJECT
public:
    QmlComponentAttached(QObject *parent = 0);
    ~QmlComponentAttached();

    QmlComponentAttached **prev;
    QmlComponentAttached *next;

Q_SIGNALS:
    void completed();

private:
    friend class QmlComponentPrivate;
};

QT_END_NAMESPACE

#endif // QMLCOMPONENT_P_H
