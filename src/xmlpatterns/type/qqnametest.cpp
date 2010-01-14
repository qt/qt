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

#include <QHash>

#include "qbuiltintypes_p.h"
#include "qitem_p.h"
#include "qitem_p.h"

#include "qqnametest_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

QNameTest::QNameTest(const ItemType::Ptr &primaryType,
                     const QXmlName qName) : AbstractNodeTest(primaryType)
                                        , m_qName(qName)
{
    Q_ASSERT(!qName.isNull());
}

ItemType::Ptr QNameTest::create(const ItemType::Ptr &primaryType, const QXmlName qName)
{
    Q_ASSERT(!qName.isNull());
    Q_ASSERT(primaryType);

    return ItemType::Ptr(new QNameTest(primaryType, qName));
}

bool QNameTest::itemMatches(const Item &item) const
{
    Q_ASSERT(item.isNode());
    return m_primaryType->itemMatches(item) &&
           item.asNode().name() == m_qName;
}

QString QNameTest::displayName(const NamePool::Ptr &np) const
{
    QString displayOther(m_primaryType->displayName(np));

    return displayOther.insert(displayOther.size() - 1, np->displayName(m_qName));
}

ItemType::InstanceOf QNameTest::instanceOf() const
{
    return ClassQNameTest;
}

bool QNameTest::operator==(const ItemType &other) const
{
    return other.instanceOf() == ClassQNameTest &&
           static_cast<const QNameTest &>(other).m_qName == m_qName;
}

PatternPriority QNameTest::patternPriority() const
{
    return 0;
}

QT_END_NAMESPACE
