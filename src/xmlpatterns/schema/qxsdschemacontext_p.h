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

#ifndef Patternist_XsdSchemaContext_H
#define Patternist_XsdSchemaContext_H

#include "qnamedschemacomponent_p.h"
#include "qreportcontext_p.h"
#include "qschematypefactory_p.h"
#include "qxsdschematoken_p.h"
#include "qxsdschema_p.h"
#include "qxsdschemachecker_p.h"
#include "qxsdschemaresolver_p.h"

#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtXmlPatterns/QAbstractMessageHandler>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A context for schema parsing and validation.
     *
     * This class provides the infrastructure for error reporting and
     * network access. Additionally it stores objects that are used by
     * both, the parser and the validator.
     *
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    class XsdSchemaContext : public ReportContext
    {
        public:
            /**
             * A smart pointer wrapping XsdSchemaContext instances.
             */
            typedef QExplicitlySharedDataPointer<XsdSchemaContext> Ptr;

            /**
             * Creates a new schema context object.
             *
             * @param namePool The name pool all names belong to.
             */
            XsdSchemaContext(const NamePool::Ptr &namePool);

            /**
             * Returns the name pool of the schema context.
             */
            virtual NamePool::Ptr namePool() const;

            /**
             * Sets the base URI for the main schema.
             *
             * The main schema is the one that includes resp. imports
             * all the other schema files.
             */
            virtual void setBaseURI(const QUrl &uri);

            /**
             * Returns the base URI of the main schema.
             */
            virtual QUrl baseURI() const;

            /**
             * Sets the network access manager that should be used
             * to access referenced schema definitions.
             */
            void setNetworkAccessManager(QNetworkAccessManager *accessManager);

            /**
             * Returns the network access manager that is used to
             * access referenced schema definitions.
             */
            virtual QNetworkAccessManager* networkAccessManager() const;

            /**
             * Sets the message @p handler used by the context for error reporting.
             */
            void setMessageHandler(QAbstractMessageHandler *handler);

            /**
             * Returns the message handler used by the context for
             * error reporting.
             */
            virtual QAbstractMessageHandler* messageHandler() const;

            /**
             * Always returns an empty source location.
             */
            virtual QSourceLocation locationFor(const SourceLocationReflection *const reflection) const;

            /**
             * Sets the uri @p resolver that is used for resolving URIs in the
             * schema parser.
             */
            void setUriResolver(QAbstractUriResolver *resolver);

            /**
             * Returns the uri resolver that is used for resolving URIs in the
             * schema parser.
             */
            virtual const QAbstractUriResolver* uriResolver() const;

            /**
             * Returns the list of facets for the given simple @p type.
             */
            XsdFacet::Hash facetsForType(const AnySimpleType::Ptr &type) const;

            /**
             * Returns a schema type factory that contains some predefined schema types.
             */
            SchemaTypeFactory::Ptr schemaTypeFactory() const;

            /**
             * The following variables should not be accessed directly.
             */
            mutable SchemaTypeFactory::Ptr                 m_schemaTypeFactory;
            mutable QHash<SchemaType::Ptr, XsdFacet::Hash> m_builtinTypesFacetList;

        private:
            QHash<SchemaType::Ptr, XsdFacet::Hash> setupBuiltinTypesFacetList() const;

            NamePool::Ptr                                 m_namePool;
            QNetworkAccessManager*                        m_networkAccessManager;
            QUrl                                          m_baseURI;
            QAbstractUriResolver*                         m_uriResolver;
            QAbstractMessageHandler*                      m_messageHandler;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
