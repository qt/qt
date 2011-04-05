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

#ifndef QDECLARATIVEV4PROGRAM_P_H
#define QDECLARATIVEV4PROGRAM_P_H

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

#include "qdeclarativev4instruction_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

struct QDeclarativeV4Program {
    quint32 bindings;
    quint32 dataLength;
    quint32 signalTableOffset;
    quint32 exceptionDataOffset;
    quint16 subscriptions;
    quint16 identifiers;
    quint16 instructionCount;

    struct BindingReference {
        quint32 binding;
        quint32 blockMask;
    };
    
    struct BindingReferenceList {
        quint32 count;
        BindingReference bindings[];
    };

    inline const char *data() const;
    inline const char *instructions() const;
    inline BindingReferenceList *signalTable(int signalIndex) const;
};

enum QDeclarativeRegisterType { 
    UndefinedType,
    QObjectStarType,
    QRealType,
    IntType,
    BoolType,

    PODValueType,

    FirstCleanupType, 
    QStringType = FirstCleanupType,
    QUrlType,
    QVariantType,
};

const char *QDeclarativeV4Program::data() const 
{ 
    return ((const char *)this) + sizeof(QDeclarativeV4Program); 
}

const char *QDeclarativeV4Program::instructions() const
{ 
    return (const char *)(data() + dataLength);
}

QDeclarativeV4Program::BindingReferenceList *QDeclarativeV4Program::signalTable(int signalIndex) const 
{ 
    quint32 *signalTable = (quint32 *)(data() + signalTableOffset);
    return (BindingReferenceList *)(signalTable + signalTable[signalIndex]);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDECLARATIVEV4PROGRAM_P_H

