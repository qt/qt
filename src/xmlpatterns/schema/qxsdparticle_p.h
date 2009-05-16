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

#ifndef Patternist_XsdParticle_H
#define Patternist_XsdParticle_H

#include "qnamedschemacomponent_p.h"
#include "qxsdterm_p.h"

#include <QtCore/QList>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Represents a XSD particle object.
     *
     * This class represents the <em>particle</em> object of a XML schema as described
     * <a href="http://www.w3.org/TR/xmlschema11-1/#cParticles">here</a>.
     *
     * It contains information about the number of occurrence and a reference to
     * either an <em>element</em> object, a <em>group</em> object or an <em>any</em> object.
     *
     * @see <a href="http://www.w3.org/Submission/2004/SUBM-xmlschema-api-20040309/xml-schema-api.html#Interface-XSParticle">XML Schema API reference</a>
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    class XsdParticle : public NamedSchemaComponent
    {
        public:
            typedef QExplicitlySharedDataPointer<XsdParticle> Ptr;
            typedef QList<XsdParticle::Ptr> List;

            /**
             * Creates a new particle object.
             */
            XsdParticle();

            /**
             * Sets the minimum @p occurrence of the particle.
             *
             * @see <a href="http://www.w3.org/TR/xmlschema11-1/#p-min_occurs">Minimum Occurrence Definition</a>
             */
            void setMinimumOccurs(unsigned int occurrence);

            /**
             * Returns the minimum occurrence of the particle.
             */
            unsigned int minimumOccurs() const;

            /**
             * Sets the maximum @p occurrence of the particle.
             *
             * @see <a href="http://www.w3.org/TR/xmlschema11-1/#p-max_occurs">Maximum Occurrence Definition</a>
             */
            void setMaximumOccurs(unsigned int occurrence);

            /**
             * Returns the maximum occurrence of the particle.
             *
             * @note This value has only a meaning if maximumOccursUnbounded is @c false.
             */
            unsigned int maximumOccurs() const;

            /**
             * Sets whether the maximum occurrence of the particle is unbounded.
             *
             * @see <a href="http://www.w3.org/TR/xmlschema11-1/#p-max_occurs">Maximum Occurrence Definition</a>
             */
            void setMaximumOccursUnbounded(bool unbounded);

            /**
             * Returns whether the maximum occurrence of the particle is unbounded.
             */
            bool maximumOccursUnbounded() const;

            /**
             * Sets the @p term of the particle.
             *
             * The term can be an element, a model group or an element wildcard.
             *
             * @see <a href="http://www.w3.org/TR/xmlschema11-1/#p-term">Term Definition</a>
             */
            void setTerm(const XsdTerm::Ptr &term);

            /**
             * Returns the term of the particle.
             */
            XsdTerm::Ptr term() const;

        private:
            unsigned int m_minimumOccurs;
            unsigned int m_maximumOccurs;
            bool         m_maximumOccursUnbounded;
            XsdTerm::Ptr m_term;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
