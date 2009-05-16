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

#ifndef Patternist_XsdXPathExpression_H
#define Patternist_XsdXPathExpression_H

#include "qanyuri_p.h"
#include "qnamedschemacomponent_p.h"
#include "qxsdannotated_p.h"

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
     * @see <a href="http://www.w3.org/TR/xmlschema11-1/#x">XPathExpression Definition</a>
     */
    class XsdXPathExpression : public NamedSchemaComponent, public XsdAnnotated
    {
        public:
            typedef QExplicitlySharedDataPointer<XsdXPathExpression> Ptr;
            typedef QList<XsdXPathExpression::Ptr> List;

            /**
             * Sets the list of namespace @p bindings of the XPath expression.
             *
             * @see <a href="http://www.w3.org/TR/xmlschema11-1/#x-namespace_bindings">Namespace Bindings Definition</a>
             *
             * @note We can't use a QSet<QXmlName> here, as the hash method does not take the prefix
             *       in account, so we loose entries.
             */
            void setNamespaceBindings(const QList<QXmlName> &bindings);

            /**
             * Returns the list of namespace bindings of the XPath expression.
             */
            QList<QXmlName> namespaceBindings() const;

            /**
             * Sets the default namespace of the XPath expression.
             *
             * @see <a href="http://www.w3.org/TR/xmlschema11-1/#x-default_namespace">Default Namespace Definition</a>
             */
            void setDefaultNamespace(const AnyURI::Ptr &defaultNamespace);

            /**
             * Returns the default namespace of the XPath expression.
             */
            AnyURI::Ptr defaultNamespace() const;

            /**
             * Sets the base @p uri of the XPath expression.
             *
             * @see <a href="http://www.w3.org/TR/xmlschema11-1/#x-base_URI">Base URI Definition</a>
             */
            void setBaseURI(const AnyURI::Ptr &uri);

            /**
             * Returns the base uri of the XPath expression.
             */
            AnyURI::Ptr baseURI() const;

            /**
             * Sets the @p expression string of the XPath expression.
             *
             * @see <a href="http://www.w3.org/TR/xmlschema11-1/#x-expression">Expression Definition</a>
             */
            void setExpression(const QString &expression);

            /**
             * Returns the expression string of the XPath expression.
             */
            QString expression() const;

        private:
            QList<QXmlName> m_namespaceBindings;
            AnyURI::Ptr     m_defaultNamespace;
            AnyURI::Ptr     m_baseURI;
            QString         m_expression;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
