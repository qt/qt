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

#ifndef Patternist_XsdAttributeGroup_H
#define Patternist_XsdAttributeGroup_H

#include "qxsdannotated_p.h"
#include "qxsdattributeuse_p.h"
#include "qxsdwildcard_p.h"

#include <QtCore/QList>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Represents the XSD attributeGroup object.
     *
     * This class represents the <em>attributeGroup</em> object of a XML schema as described
     * <a href="http://www.w3.org/TR/xmlschema11-1/#cAttribute_Group_Definitions">here</a>.
     *
     * @see <a href="http://www.w3.org/Submission/2004/SUBM-xmlschema-api-20040309/xml-schema-api.html#Interface-XSAttributeGroup">XML Schema API reference</a>
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    class XsdAttributeGroup : public NamedSchemaComponent, public XsdAnnotated
    {
        public:
            typedef QExplicitlySharedDataPointer<XsdAttributeGroup> Ptr;
            typedef QList<XsdAttributeGroup::Ptr> List;

            /**
             * Sets the list of attribute @p uses that are defined in the attribute group.
             *
             * @see <a href="http://www.w3.org/TR/xmlschema11-1/#agd-attribute_uses">Attribute Uses</a>
             */
            void setAttributeUses(const XsdAttributeUse::List &uses);

            /**
             * Adds a new attribute @p use to the attribute group.
             */
            void addAttributeUse(const XsdAttributeUse::Ptr &use);

            /**
             * Returns the list of all attribute uses of the attribute group.
             */
            XsdAttributeUse::List attributeUses() const;

            /**
             * Sets the attribute @p wildcard of the attribute group.
             *
             * @see <a href="http://www.w3.org/TR/xmlschema11-1/#agd-attribute_wildcard">Attribute Wildcard</a>
             */
            void setWildcard(const XsdWildcard::Ptr &wildcard);

            /**
             * Returns the attribute wildcard of the attribute group.
             */
            XsdWildcard::Ptr wildcard() const;

        private:
            XsdAttributeUse::List m_attributeUses;
            XsdWildcard::Ptr      m_wildcard;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
