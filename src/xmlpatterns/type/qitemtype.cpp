/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

#include <QtGlobal>

#include "qcommonsequencetypes_p.h"

#include "qitemtype_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ItemType::~ItemType()
{
}

const ItemType &ItemType::operator|(const ItemType &other) const
{
    const ItemType *ca = this;

    if(other == *CommonSequenceTypes::None)
        return *ca;

    if(*ca == *CommonSequenceTypes::Empty)
        return other;
    else if(other == *CommonSequenceTypes::Empty)
        return *ca;

    do
    {
        const ItemType *cb = &other;
        do
        {
            if(*ca == *cb)
                return *ca;

            cb = cb->xdtSuperType().data();
        }
        while(cb);

        ca = ca->xdtSuperType().data();
    }
    while(ca);

    Q_ASSERT_X(false, Q_FUNC_INFO, "We should never reach this line.");
    return *this;
}

ItemType::Category ItemType::itemTypeCategory() const
{
    return Other;
}

bool ItemType::operator==(const ItemType &other) const
{
    return this == &other;
}

ItemType::InstanceOf ItemType::instanceOf() const
{
    return ClassOther;
}

QT_END_NAMESPACE
