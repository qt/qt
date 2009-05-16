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

#include "qxsdidchelper_p.h"

#include "qderivedstring_p.h"
#include "qxsdschemahelper_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

FieldNode::FieldNode()
{
}

FieldNode::FieldNode(const QXmlItem &item, const QString &data, const SchemaType::Ptr &type)
    : m_item(item)
    , m_data(data)
    , m_type(type)
{
}

bool FieldNode::isEmpty() const
{
    return m_item.isNull();
}

bool FieldNode::isEqualTo(const FieldNode &other, const NamePool::Ptr &namePool, const ReportContext::Ptr &context, const SourceLocationReflection *const reflection) const
{
    if (m_type != other.m_type)
        return false;

    const DerivedString<TypeString>::Ptr string = DerivedString<TypeString>::fromLexical(namePool, m_data);
    const DerivedString<TypeString>::Ptr otherString = DerivedString<TypeString>::fromLexical(namePool, other.m_data);

    return XsdSchemaHelper::constructAndCompare(string, AtomicComparator::OperatorEqual, otherString, m_type, context, reflection);
}

QXmlItem FieldNode::item() const
{
    return m_item;
}

TargetNode::TargetNode(const QXmlItem &item)
    : m_item(item)
{
}

QXmlItem TargetNode::item() const
{
    return m_item;
}

QVector<QXmlItem> TargetNode::fieldItems() const
{
    QVector<QXmlItem> items;

    for (int i = 0; i < m_fields.count(); ++i)
        items.append(m_fields.at(i).item());

    return items;
}

int TargetNode::emptyFieldsCount() const
{
    int counter = 0;
    for (int i = 0; i < m_fields.count(); ++i) {
        if (m_fields.at(i).isEmpty())
            ++counter;
    }

    return counter;
}

bool TargetNode::fieldsAreEqual(const TargetNode &other, const NamePool::Ptr &namePool, const ReportContext::Ptr &context, const SourceLocationReflection *const reflection) const
{
    if (m_fields.count() != other.m_fields.count())
        return false;

    for (int i = 0; i < m_fields.count(); ++i) {
        if (!m_fields.at(i).isEqualTo(other.m_fields.at(i), namePool, context, reflection))
            return false;
    }

    return true;
}

void TargetNode::addField(const QXmlItem &item, const QString &data, const SchemaType::Ptr &type)
{
    m_fields.append(FieldNode(item, data, type));
}

bool TargetNode::operator==(const TargetNode &other) const
{
    return (m_item.toNodeModelIndex() == other.m_item.toNodeModelIndex());
}

QT_END_NAMESPACE
