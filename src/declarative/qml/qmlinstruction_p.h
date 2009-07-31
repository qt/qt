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

#ifndef QMLINSTRUCTION_P_H
#define QMLINSTRUCTION_P_H

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

#include <QtDeclarative/qfxglobal.h>

QT_BEGIN_NAMESPACE

class QmlCompiledData;
class Q_DECLARATIVE_EXPORT QmlInstruction
{
public:
    enum Type { 
        //
        // Object Creation
        //
        //    CreateObject - Create a new object instance and push it on the 
        //                   object stack
        //    SetId - Set the id of the object on the top of the object stack
        //    SetDefault - Sets the instance on the top of the object stack to
        //                 be the context's default object.
        //    StoreMetaObject - Assign the dynamic metaobject to object on the
        //                      top of the stack.
        Init,                     /* init */
        CreateObject,             /* create */
        SetId,                    /* setId */
        SetDefault,
        CreateComponent,          /* createComponent */
        StoreMetaObject,          /* storeMeta */

        //
        // Precomputed single assignment
        //
        //    StoreFloat - Store a float in a core property
        //    StoreDouble - Store a double in a core property
        //    StoreInteger - Store a int or uint in a core property
        //    StoreBool - Store a bool in a core property
        //    StoreString - Store a QString in a core property
        //    StoreUrl - Store a QUrl in a core property
        //    StoreColor - Store a QColor in a core property
        //    StoreDate - Store a QDate in a core property
        //    StoreTime - Store a QTime in a core property
        //    StoreDateTime - Store a QDateTime in a core property
        //    StoreVariant - Store a QVariant in a core property
        //    StoreObject - Pop the object on the top of the object stack and
        //                  store it in a core property
        StoreFloat,               /* storeFloat */
        StoreDouble,              /* storeDouble */
        StoreInteger,             /* storeInteger */
        StoreBool,                /* storeBool */
        StoreString,              /* storeString */
        StoreUrl,                 /* storeUrl */
        StoreColor,               /* storeColor */
        StoreDate,                /* storeDate */
        StoreTime,                /* storeTime */
        StoreDateTime,            /* storeDateTime */
        StorePoint,               /* storeRealPair */
        StorePointF,              /* storeRealPair */
        StoreSize,                /* storeRealPair */
        StoreSizeF,               /* storeRealPair */
        StoreRect,                /* storeRect */
        StoreRectF,               /* storeRect */
        StoreVariant,             /* storeString */
        StoreObject,              /* storeObject */
        StoreVariantObject,       /* storeObject */
        StoreInterface,           /* storeObject */

        StoreSignal,              /* storeSignal */

        //
        // Unresolved single assignment
        //
        AssignSignalObject,       /* assignSignalObject */
        AssignCustomType,          /* assignCustomType */

        StoreBinding,             /* assignBinding */
        StoreCompiledBinding,     /* assignBinding */
        StoreValueSource,         /* assignValueSource */

        BeginObject,              /* begin */

        StoreObjectQmlList,       /* NA */
        StoreObjectQList,         /* NA */
        AssignObjectList,         /* NA */

        FetchAttached,            /* fetchAttached */
        FetchQmlList,             /* fetchQmlList */ 
        FetchQList,               /* fetch */
        FetchObject,              /* fetch */
        FetchValueType,           /* fetchValue */

        //
        // Stack manipulation
        // 
        //    PopFetchedObject - Remove an object from the object stack
        //    PopQList - Remove a list from the list stack
        PopFetchedObject,
        PopQList,
        PopValueType,            /* fetchValue */

        // 
        // Deferred creation
        //
        Defer,                    /* defer */
    };
    QmlInstruction()
        : line(0) {}

    Type type;
    unsigned short line;
    union {
        struct {
            int bindingsSize;
            int parserStatusSize;
        } init;
        struct {
            int type;
            int data;
        } create;
        struct {
            int data;
            int slotData;
            int aliasData;
        } storeMeta;
        struct {
            int value;
        } setId;
        struct {
            int property;
            int owner;
        } assignValueSource;
        struct {
            int property;
            int value;
            short context;
            short owner;
        } assignBinding;
        struct {
            int property;
        } fetch;
        struct {
            int property;
            int type;
        } fetchValue;
        struct {
            int property;
            int type;
        } fetchQmlList;
        struct {
            int castValue;
        } begin;
        struct {
            int propertyIndex;
            float value;
        } storeFloat;
        struct {
            int propertyIndex;
            double value;
        } storeDouble;
        struct {
            int propertyIndex;
            int value;
        } storeInteger;
        struct {
            int propertyIndex;
            bool value;
        } storeBool;
        struct {
            int propertyIndex;
            int value;
        } storeString;
        struct {
            int propertyIndex;
            int value;
        } storeUrl;
        struct {
            int propertyIndex;
            unsigned int value;
        } storeColor;
        struct {
            int propertyIndex;
            int value;
        } storeDate;
        struct {
            int propertyIndex;
            int valueIndex;
        } storeTime;
        struct {
            int propertyIndex;
            int valueIndex;
        } storeDateTime;
        struct {
            int propertyIndex;
            int valueIndex;
        } storeRealPair;
        struct {
            int propertyIndex;
            int valueIndex;
        } storeRect;
        struct {
            int propertyIndex;
        } storeObject;
        struct {
            int propertyIndex;
            int valueIndex;
        } assignCustomType;
        struct {
            int signalIndex;
            int value;
        } storeSignal;
        struct {
            int signal;
        } assignSignalObject;
        struct {
            int count;
            int endLine;
            int metaObject;
        } createComponent;
        struct {
            int id;
        } fetchAttached;
        struct {
            int deferCount;
        } defer;
    };

    void dump(QmlCompiledData *);
};

QT_END_NAMESPACE

#endif // QMLINSTRUCTION_P_H
