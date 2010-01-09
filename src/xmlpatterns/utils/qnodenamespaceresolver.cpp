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

#include "qitem_p.h"
#include "qnamepool_p.h"

#include "qnodenamespaceresolver_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

NodeNamespaceResolver::NodeNamespaceResolver(const Item &item) : m_node(item.asNode())
{
    Q_ASSERT(!m_node.isNull());
}

void NodeNamespaceResolver::addBinding(const QXmlName nb)
{
    Q_UNUSED(nb);
    Q_ASSERT_X(false, Q_FUNC_INFO, "Calling this function for this sub-class makes little sense.");
}

QXmlName::NamespaceCode NodeNamespaceResolver::lookupNamespaceURI(const QXmlName::PrefixCode prefix) const
{
    const QXmlName::NamespaceCode ns = m_node.namespaceForPrefix(prefix);

    if(ns == NoBinding)
    {
        if(prefix == StandardPrefixes::empty)
            return StandardNamespaces::empty;
        else
            return NoBinding;
    }
    else
        return ns;
}

NamespaceResolver::Bindings NodeNamespaceResolver::bindings() const
{
    Q_ASSERT_X(false, Q_FUNC_INFO, "Not implemented.");
    return NamespaceResolver::Bindings();
}

QT_END_NAMESPACE
