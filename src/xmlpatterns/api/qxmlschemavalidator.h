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

#ifndef QXMLSCHEMAVALIDATOR_H
#define QXMLSCHEMAVALIDATOR_H

#include <QtCore/QUrl>
#include <QtXmlPatterns/QXmlNamePool>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(XmlPatterns)

class QAbstractMessageHandler;
class QAbstractUriResolver;
class QIODevice;
class QNetworkAccessManager;
class QUrl;
class QXmlNamePool;
class QXmlSchema;
class QXmlSchemaValidatorPrivate;

class Q_XMLPATTERNS_EXPORT QXmlSchemaValidator
{
    public:
        QXmlSchemaValidator();
        QXmlSchemaValidator(const QXmlSchema &schema);
        ~QXmlSchemaValidator();

        void setSchema(const QXmlSchema &schema);

        bool validate(const QUrl &source) const;
        bool validate(QIODevice *source, const QUrl &documentUri = QUrl()) const;
        bool validate(const QByteArray &data, const QUrl &documentUri = QUrl()) const;

        QXmlNamePool namePool() const;
        QXmlSchema schema() const;

        void setMessageHandler(QAbstractMessageHandler *handler);
        QAbstractMessageHandler *messageHandler() const;

        void setUriResolver(const QAbstractUriResolver *resolver);
        const QAbstractUriResolver *uriResolver() const;

        void setNetworkAccessManager(QNetworkAccessManager *networkmanager);
        QNetworkAccessManager *networkAccessManager() const;

    private:
        QXmlSchemaValidatorPrivate* const d;

        Q_DISABLE_COPY(QXmlSchemaValidator)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
