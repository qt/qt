/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#include "qxsdsimpletype_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

QString XsdSimpleType::displayName(const NamePool::Ptr &np) const
{
    return np->displayName(name(np));
}

void XsdSimpleType::setWxsSuperType(const SchemaType::Ptr &type)
{
    m_superType = type;
}

SchemaType::Ptr XsdSimpleType::wxsSuperType() const
{
    return m_superType;
}

void XsdSimpleType::setContext(const NamedSchemaComponent::Ptr &component)
{
    m_context = component;
}

NamedSchemaComponent::Ptr XsdSimpleType::context() const
{
    return m_context;
}

void XsdSimpleType::setPrimitiveType(const AnySimpleType::Ptr &type)
{
    m_primitiveType = type;
}

AnySimpleType::Ptr XsdSimpleType::primitiveType() const
{
    return m_primitiveType;
}

void XsdSimpleType::setItemType(const AnySimpleType::Ptr &type)
{
    m_itemType = type;
}

AnySimpleType::Ptr XsdSimpleType::itemType() const
{
    return m_itemType;
}

void XsdSimpleType::setMemberTypes(const AnySimpleType::List &types)
{
    m_memberTypes = types;
}

AnySimpleType::List XsdSimpleType::memberTypes() const
{
    return m_memberTypes;
}

void XsdSimpleType::setFacets(const XsdFacet::Hash &facets)
{
    m_facets = facets;
}

XsdFacet::Hash XsdSimpleType::facets() const
{
    return m_facets;
}

void XsdSimpleType::setCategory(TypeCategory category)
{
    m_typeCategory = category;
}

XsdSimpleType::TypeCategory XsdSimpleType::category() const
{
    return m_typeCategory;
}

void XsdSimpleType::setDerivationMethod(DerivationMethod method)
{
    m_derivationMethod = method;
}

XsdSimpleType::DerivationMethod XsdSimpleType::derivationMethod() const
{
    return m_derivationMethod;
}

bool XsdSimpleType::isDefinedBySchema() const
{
    return true;
}

QT_END_NAMESPACE
