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

/*
 * NOTE: This file is included by qxsduserschematype_p.h
 * if you need some includes, put them in qxsduserschematype_p.h (outside of the namespace)
 */

template<typename TSuperClass>
void XsdUserSchemaType<TSuperClass>::setName(const QXmlName &name)
{
    m_name = name;
}

template<typename TSuperClass>
QXmlName XsdUserSchemaType<TSuperClass>::name(const NamePool::Ptr&) const
{
    return m_name;
}

template<typename TSuperClass>
QString XsdUserSchemaType<TSuperClass>::displayName(const NamePool::Ptr &np) const
{
    return np->displayName(m_name);
}

template<typename TSuperClass>
void XsdUserSchemaType<TSuperClass>::setDerivationConstraints(const SchemaType::DerivationConstraints &constraints)
{
    m_derivationConstraints = constraints;
}

template<typename TSuperClass>
SchemaType::DerivationConstraints XsdUserSchemaType<TSuperClass>::derivationConstraints() const
{
    return m_derivationConstraints;
}
