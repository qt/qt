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

#ifndef QXMLSCHEMA_H
#define QXMLSCHEMA_H

#include <QtCore/QSharedDataPointer>
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
class QXmlSchemaPrivate;

class Q_XMLPATTERNS_EXPORT QXmlSchema
{
    friend class QXmlSchemaValidatorPrivate;

    public:
        QXmlSchema();
        QXmlSchema(const QXmlSchema &other);
        ~QXmlSchema();

        bool load(const QUrl &source);
        bool load(QIODevice *source, const QUrl &documentUri = QUrl());
        bool load(const QByteArray &data, const QUrl &documentUri = QUrl());

        bool isValid() const;

        QXmlNamePool namePool() const;
        QUrl documentUri() const;

        void setMessageHandler(QAbstractMessageHandler *handler);
        QAbstractMessageHandler *messageHandler() const;

        void setUriResolver(const QAbstractUriResolver *resolver);
        const QAbstractUriResolver *uriResolver() const;

        void setNetworkAccessManager(QNetworkAccessManager *networkmanager);
        QNetworkAccessManager *networkAccessManager() const;

    private:
        QSharedDataPointer<QXmlSchemaPrivate> d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
