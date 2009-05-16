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

#ifndef Patternist_XsdSchemaHelper_H
#define Patternist_XsdSchemaHelper_H

#include "qcomparisonfactory_p.h"
#include "qschematype_p.h"
#include "qxsdattributegroup_p.h"
#include "qxsdelement_p.h"
#include "qxsdparticle_p.h"
#include "qxsdschemacontext_p.h"
#include "qxsdwildcard_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Contains helper methods that are used by XsdSchemaParser, XsdSchemaResolver and XsdSchemaChecker.
     *
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    class XsdSchemaHelper
    {
        public:
            /**
             * Checks whether the given @p particle is emptiable as defined by the
             * algorithm in the schema spec.
             */
            static bool isParticleEmptiable(const XsdParticle::Ptr &particle);

            /**
             * Checks whether the given @p nameSpace is allowed by the given namespace @p constraint.
             */
            static bool wildcardAllowsNamespaceName(const QString &nameSpace, const XsdWildcard::NamespaceConstraint::Ptr &constraint);

            /**
             * Checks whether the given @p name is allowed by the namespace constraint of the given @p wildcard.
             */
            static bool wildcardAllowsExpandedName(const QXmlName &name, const XsdWildcard::Ptr &wildcard, const NamePool::Ptr &namePool);

            /**
             * Checks whether the @p wildcard is a subset of @p otherWildcard.
             */
            static bool isWildcardSubset(const XsdWildcard::Ptr &wildcard, const XsdWildcard::Ptr &otherWildcard);

            /**
             * Returns the union of the given @p wildcard and @p otherWildcard.
             */
            static XsdWildcard::Ptr wildcardUnion(const XsdWildcard::Ptr &wildcard, const XsdWildcard::Ptr &otherWildcard);

            /**
             * Returns the intersection of the given @p wildcard and @p otherWildcard.
             */
            static XsdWildcard::Ptr wildcardIntersection(const XsdWildcard::Ptr &wildcard, const XsdWildcard::Ptr &otherWildcard);

            /**
             * Returns whether the given @p type is validly substitutable for an @p otherType
             * under the given @p constraints.
             */
            static bool isValidlySubstitutable(const SchemaType::Ptr &type, const SchemaType::Ptr &otherType, const SchemaType::DerivationConstraints &constraints);

            /**
             * Returns whether the simple @p derivedType can be derived from the simple @p baseType
             * under the given @p constraints.
             */
            static bool isSimpleDerivationOk(const SchemaType::Ptr &derivedType, const SchemaType::Ptr baseType, const SchemaType::DerivationConstraints &constraints);

            /**
             * Returns whether the complex @p derivedType can be derived from the complex @p baseType
             * under the given @p constraints.
             */
            static bool isComplexDerivationOk(const SchemaType::Ptr &derivedType, const SchemaType::Ptr baseType, const SchemaType::DerivationConstraints &constraints);

            /**
             * This method takes the two string based operands @p operand1 and @p operand2 and converts them to instances of type @p type.
             * If the conversion fails, @c false is returned, otherwise the instances are compared by the given operator @p op and the
             * result of the comparison is returned.
             */
            static bool constructAndCompare(const DerivedString<TypeString>::Ptr &operand1,
                                            const AtomicComparator::Operator op,
                                            const DerivedString<TypeString>::Ptr &operand2,
                                            const SchemaType::Ptr &type,
                                            const ReportContext::Ptr &context,
                                            const SourceLocationReflection *const sourceLocationReflection);

            /**
             * Returns whether the process content property of the @p derivedWildcard is valid
             * according to the process content property of its @p baseWildcard.
             */
            static bool checkWildcardProcessContents(const XsdWildcard::Ptr &baseWildcard, const XsdWildcard::Ptr &derivedWildcard);

            /**
             * Checks whether @[ member is a member of the substitution group with the given @p head.
             */
            static bool foundSubstitutionGroupTransitive(const XsdElement::Ptr &head, const XsdElement::Ptr &member, QSet<XsdElement::Ptr> &visitedElements);

            /**
             * A helper method that iterates over the type hierarchy from @p memberType up to @p headType and collects all
             * @p derivationSet and @p blockSet constraints that exists on the way there.
             */
            static void foundSubstitutionGroupTypeInheritance(const SchemaType::Ptr &headType, const SchemaType::Ptr &memberType,
                                                              QSet<SchemaType::DerivationMethod> &derivationSet, NamedSchemaComponent::BlockingConstraints &blockSet);

            /**
             * Checks if the @p member is transitive to @p head.
             */
            static bool substitutionGroupOkTransitive(const XsdElement::Ptr &head, const XsdElement::Ptr &member, const NamePool::Ptr &namePool);

            /**
             * Checks if @p derivedAttributeGroup is a valid restriction for @p attributeGroup.
             */
            static bool isValidAttributeGroupRestriction(const XsdAttributeGroup::Ptr &derivedAttributeGroup, const XsdAttributeGroup::Ptr &attributeGroup, const XsdSchemaContext::Ptr &context, QString &errorMsg);

            /**
             * Checks if @p derivedAttributeUses are a valid restriction for @p attributeUses.
             */
            static bool isValidAttributeUsesRestriction(const XsdAttributeUse::List &derivedAttributeUses, const XsdAttributeUse::List &attributeUses,
                                                        const XsdWildcard::Ptr &derivedWildcard, const XsdWildcard::Ptr &wildcard, const XsdSchemaContext::Ptr &context, QString &errorMsg);

            /**
             * Checks if @p derivedAttributeUses are a valid extension for @p attributeUses.
             */
            static bool isValidAttributeUsesExtension(const XsdAttributeUse::List &derivedAttributeUses, const XsdAttributeUse::List &attributeUses,
                                                      const XsdWildcard::Ptr &derivedWildcard, const XsdWildcard::Ptr &wildcard, const XsdSchemaContext::Ptr &context, QString &errorMsg);
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
