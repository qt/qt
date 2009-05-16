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

#ifndef Patternist_XsdAssertion_H
#define Patternist_XsdAssertion_H

#include "qnamedschemacomponent_p.h"
#include "qxsdannotated_p.h"
#include "qxsdxpathexpression_p.h"

#include <QtCore/QList>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Represents a XSD assertion object.
     *
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     * @see <a href="http://www.w3.org/TR/xmlschema11-1/#cAssertions">Assertion Definition</a>
     */
    class XsdAssertion : public NamedSchemaComponent, public XsdAnnotated
    {
        public:
            typedef QExplicitlySharedDataPointer<XsdAssertion> Ptr;
            typedef QList<XsdAssertion::Ptr> List;

            /**
             * Sets the @p test of the assertion.
             *
             * @see <a href="http://www.w3.org/TR/xmlschema11-1/#as-test">Test Definition</a>
             */
            void setTest(const XsdXPathExpression::Ptr &test);

            /**
             * Returns the test of the assertion.
             */
            XsdXPathExpression::Ptr test() const;

        private:
            XsdXPathExpression::Ptr m_test;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
