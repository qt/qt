/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVESCARCERESOURCESCRIPTCLASS_P_H
#define QDECLARATIVESCARCERESOURCESCRIPTCLASS_P_H

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

#include "private/qdeclarativepropertycache_p.h"
#include "private/qdeclarativetypenamecache_p.h"

#include <private/qscriptdeclarativeclass_p.h>
#include <QtScript/qscriptengine.h>

QT_BEGIN_NAMESPACE

class QDeclarativeEngine;

/*
   Scarce resources (like pixmaps and textures) are managed manually
   in that the variant will be set to the invalid variant once the
   JavaScript engine has finished using the JavaScript object whose
   instance data is the ScarceResourceData (but before the garbage
   collector frees the JavaScript object itself).

   The engine stores a doubly-linked-list of scarce resources which
   will to be cleaned up after a binding is successfully evaluated
   (unless the user explicitly preserves the scarce resource).

   A ScarceResourceData pointer should not be deleted manually, as
   all instances of a ScarceResourceData should be owned by the
   JavaScript engine.
 */
struct ScarceResourceData : public QScriptDeclarativeClass::Object {
    ScarceResourceData(const QVariant &v) : resource(v), prev(0), next(0)
    {
    }

    virtual ~ScarceResourceData()
    {
        releaseResource();
    }

    // Insert this resource into the given list of resources.
    void insertInto(ScarceResourceData **list)
    {
        // This node becomes the head of the list.
        next = *list; // so our next = old list head
        *list = this; // list now points to us (we're the head)
        prev = list;  // as we're the head, our prev ptr becomes the list ptr.

        // and the next node's prev pointer must contain a ptr to our next ptr,
        // since per definition, prev always contains a pointer to the previous node's "next" ptr,
        // and the "this" node is the "this->next" node's "prev" node.
        if (next) next->prev = &next;
    }

    // Remove this resource from the list of resources, without releasing the resource.
    void removeNode()
    {
        // whatever previously pointed to this node (ie, as that node's "next" node)
        // should now point to our next node (since we no longer exist in the list).
        // and the next node's prev ptr should point to our prev node.
        if (prev) *prev = next;
        if (next) next->prev = prev;
        prev = 0;
        next = 0;
    }

    // Release this resource, and remove from the list.
    void releaseResource()
    {
        resource = QVariant();
        removeNode();
    }

    QVariant resource;

    // prev always contains a pointer to the previous node's "next" ptr.
    // :. for the head node, [*prev] will be engine->scarceResources
    // :. for every other node, [*prev] will be the previous node's "next" ptr.
    ScarceResourceData **prev;
    ScarceResourceData  *next;
};

class Q_AUTOTEST_EXPORT QDeclarativeScarceResourceScriptClass : public QScriptDeclarativeClass
{
public:
    QDeclarativeScarceResourceScriptClass(QDeclarativeEngine *);
    ~QDeclarativeScarceResourceScriptClass();

    // Creates a new JavaScript object whose instance data is the scarce resource v
    QScriptValue newScarceResource(const QVariant &v);

    // inherited from QScriptDeclarativeClass
    virtual QScriptClass::QueryFlags queryProperty(Object *, const Identifier &,
                                                   QScriptClass::QueryFlags flags);
    virtual Value property(Object *, const Identifier &);
    virtual QVariant toVariant(Object *, bool *ok = 0);
    QVariant toVariant(const QScriptValue &value);

private:
    PersistentIdentifier m_preserveId;
    PersistentIdentifier m_destroyId;
    QScriptValue m_preserve;
    QScriptValue m_destroy;

    static QScriptValue preserve(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue destroy(QScriptContext *context, QScriptEngine *engine);

    QDeclarativeEngine *engine;
};

QT_END_NAMESPACE

#endif // QDECLARATIVESCARCERESOURCESCRIPTCLASS_P_H
