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

#ifndef Patternist_XsdWildcard_H
#define Patternist_XsdWildcard_H

#include "qxsdterm_p.h"

#include <QtCore/QSet>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Represents a XSD wildcard object.
     *
     * This class represents the <em>wildcard</em> object of a XML schema as described
     * <a href="http://www.w3.org/TR/xmlschema11-1/#Wildcards">here</a>.
     *
     * It contains information from either an <em>any</em> object or an <em>anyAttribute</em> object.
     *
     * @see <a href="http://www.w3.org/Submission/2004/SUBM-xmlschema-api-20040309/xml-schema-api.html#Interface-XSWildcard">XML Schema API reference</a>
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    class XsdWildcard : public XsdTerm
    {
        public:
            typedef QExplicitlySharedDataPointer<XsdWildcard> Ptr;

            /**
             * Defines the absent namespace that is used in wildcards.
             */
            static QString absentNamespace();

            /**
             * Describes the <a href="http://www.w3.org/TR/xmlschema11-1/#w-namespace_constraint">namespace constraint</a> of the wildcard.
             */
            class NamespaceConstraint : public QSharedData
            {
                public:
                    typedef QExplicitlySharedDataPointer<NamespaceConstraint> Ptr;

                    /**
                     * Describes the variety of the namespace constraint.
                     *
                     * @see <a href="http://www.w3.org/TR/xmlschema11-1/#nc-variety">Variety Definition</a>
                     */
                    enum Variety
                    {
                        Any,         ///< Any namespace is allowed.
                        Enumeration, ///< Namespaces in the namespaces set are allowed.
                        Not          ///< Namespaces in the namespaces set are not allowed.
                    };

                    /**
                     * Sets the @p variety of the namespace constraint.
                     *
                     * @see <a href="http://www.w3.org/TR/xmlschema11-1/#nc-variety">Variety Definition</a>
                     */
                    void setVariety(Variety variety);

                    /**
                     * Returns the variety of the namespace constraint.
                     */
                    Variety variety() const;

                    /**
                     * Sets the set of @p namespaces of the namespace constraint.
                     */
                    void setNamespaces(const QSet<QString> &namespaces);

                    /**
                     * Returns the set of namespaces of the namespace constraint.
                     */
                    QSet<QString> namespaces() const;

                    /**
                     * Sets the set of disallowed @p names of the namespace constraint.
                     */
                    void setDisallowedNames(const QSet<QString> &names);

                    /**
                     * Returns the set of disallowed names of the namespace constraint.
                     */
                    QSet<QString> disallowedNames() const;

                private:
                    Variety       m_variety;
                    QSet<QString> m_namespaces;
                    QSet<QString> m_disallowedNames;
            };

            /**
             * Describes the <a href="http://www.w3.org/TR/xmlschema11-1/#w-process_contents">type of content processing</a> of the wildcard.
             */
            enum ProcessContents
            {
                Strict,      ///< There must be a top-level declaration for the item available, or the item must have an xsi:type, and the item must be valid as appropriate.
                Lax,         ///< If the item has a uniquely determined declaration available, it must be valid with respect to that definition.
                Skip         ///< No constraints at all: the item must simply be well-formed XML.
            };

            /**
             * Creates a new wildcard object.
             */
            XsdWildcard();

            /**
             * Returns always @c true, used to avoid dynamic casts.
             */
            virtual bool isWildcard() const;

            /**
             * Sets the namespace @p constraint of the wildcard.
             *
             * @see <a href="http://www.w3.org/TR/xmlschema11-1/#w-namespace_constraint">Namespace Constraint Definition</a>
             */
            void setNamespaceConstraint(const NamespaceConstraint::Ptr &constraint);

            /**
             * Returns the namespace constraint of the wildcard.
             */
            NamespaceConstraint::Ptr namespaceConstraint() const;

            /**
             * Sets the process @p contents of the wildcard.
             *
             * @see <a href="http://www.w3.org/TR/xmlschema11-1/#w-process_contents">Process Contents Definition</a>
             */
            void setProcessContents(ProcessContents contents);

            /**
             * Returns the process contents of the wildcard.
             */
            ProcessContents processContents() const;

        private:
            NamespaceConstraint::Ptr m_namespaceConstraint;
            ProcessContents          m_processContents;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
