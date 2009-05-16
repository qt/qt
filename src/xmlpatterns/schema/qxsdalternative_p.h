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

#ifndef Patternist_XsdAlternative_H
#define Patternist_XsdAlternative_H

#include "qnamedschemacomponent_p.h"
#include "qschematype_p.h"
#include "qxsdannotated_p.h"
#include "qxsdxpathexpression_p.h"

#include <QtCore/QList>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Represents a XSD alternative object.
     *
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    class XsdAlternative : public NamedSchemaComponent, public XsdAnnotated
    {
        public:
            typedef QExplicitlySharedDataPointer<XsdAlternative> Ptr;
            typedef QList<XsdAlternative::Ptr> List;

            /**
             * Sets the xpath @p test of the alternative.
             *
             * @see <a href="http://www.w3.org/TR/xmlschema11-1/#tac-test">Test Definition</a>
             */
            void setTest(const XsdXPathExpression::Ptr &test);

            /**
             * Returns the xpath test of the alternative.
             */
            XsdXPathExpression::Ptr test() const;

            /**
             * Sets the @p type of the alternative.
             *
             * @see <a href="http://www.w3.org/TR/xmlschema11-1/#tac-type_definition">Type Definition</a>
             */
            void setType(const SchemaType::Ptr &type);

            /**
             * Returns the type of the alternative.
             */
            SchemaType::Ptr type() const;

        private:
            XsdXPathExpression::Ptr m_test;
            SchemaType::Ptr         m_type;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
