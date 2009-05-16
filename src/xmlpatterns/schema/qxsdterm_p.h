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

#ifndef Patternist_XsdTerm_H
#define Patternist_XsdTerm_H

#include "qnamedschemacomponent_p.h"
#include "qxsdannotated_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A base class for all particles of a model group.
     *
     * This class is the base class for all particles of a model group
     * as the <em>element</em>, <em>group</em> or <em>any</em> tag, it is not supposed to
     * be instantiated directly.
     *
     * @see <a href="http://www.w3.org/Submission/2004/SUBM-xmlschema-api-20040309/xml-schema-api.html#Interface-XSTerm">XML Schema API reference</a>
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    class XsdTerm : public NamedSchemaComponent, public XsdAnnotated
    {
        public:
            typedef QExplicitlySharedDataPointer<XsdTerm> Ptr;

            /**
             * Returns @c true if the term is an element, @c false otherwise.
             */
            virtual bool isElement() const;

            /**
             * Returns @c true if the term is a model group (group tag), @c false otherwise.
             */
            virtual bool isModelGroup() const;

            /**
             * Returns @c true if the term is a wildcard (any tag), @c false otherwise.
             */
            virtual bool isWildcard() const;

            /**
             * Returns @c true if the term is a reference, @c false otherwise.
             *
             * @note The reference term is only used internally as helper during type resolving.
             */
            virtual bool isReference() const;

        protected:
            /**
             * This constructor only exists to ensure this class is subclassed.
             */
            inline XsdTerm() {};
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
