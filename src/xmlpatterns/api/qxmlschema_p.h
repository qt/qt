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

#ifndef QXMLSCHEMA_P_H
#define QXMLSCHEMA_P_H

#include "qabstractmessagehandler.h"
#include "qabstracturiresolver.h"
#include "qautoptr_p.h"
#include "qcoloringmessagehandler_p.h"
#include "qreferencecountedvalue_p.h"

#include "qxsdschemacontext_p.h"
#include "qxsdschemaparser_p.h"
#include "qxsdschemaparsercontext_p.h"

#include <QtCore/QSharedData>
#include <QtNetwork/QNetworkAccessManager>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QXmlSchemaPrivate : public QSharedData
{
    public:
        QXmlSchemaPrivate(const QXmlNamePool &namePool);
        QXmlSchemaPrivate(const QPatternist::XsdSchemaContext::Ptr &schemaContext);
        QXmlSchemaPrivate(const QXmlSchemaPrivate &other);

        void load(const QUrl &source, const QString &targetNamespace);
        void load(QIODevice *source, const QUrl &documentUri, const QString &targetNamespace);
        void load(const QByteArray &data, const QUrl &documentUri, const QString &targetNamespace);
        bool isValid() const;
        QXmlNamePool namePool() const;
        QUrl documentUri() const;
        void setMessageHandler(QAbstractMessageHandler *handler);
        QAbstractMessageHandler *messageHandler() const;
        void setUriResolver(QAbstractUriResolver *resolver);
        QAbstractUriResolver *uriResolver() const;
        void setNetworkAccessManager(QNetworkAccessManager *networkmanager);
        QNetworkAccessManager *networkAccessManager() const;

        QXmlNamePool                                                     m_namePool;
        QAbstractMessageHandler*                                         m_userMessageHandler;
        QAbstractUriResolver*                                            m_uriResolver;
        QNetworkAccessManager*                                           m_userNetworkAccessManager;
        QPatternist::ReferenceCountedValue<QAbstractMessageHandler>::Ptr m_messageHandler;
        QPatternist::ReferenceCountedValue<QNetworkAccessManager>::Ptr   m_networkAccessManager;

        QPatternist::XsdSchemaContext::Ptr                               m_schemaContext;
        QPatternist::XsdSchemaParserContext::Ptr                         m_schemaParserContext;
        bool                                                             m_schemaIsValid;
        QUrl                                                             m_documentUri;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
