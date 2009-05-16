/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
****************************************************************************/

#include <QtTest/QtTest>

#ifdef QTEST_XMLPATTERNS

#include <QAbstractMessageHandler>
#include <QAbstractUriResolver>
#include <QtNetwork/QNetworkAccessManager>
#include <QXmlName>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

class DummyMessageHandler : public QAbstractMessageHandler
{
    public:
        DummyMessageHandler(QObject *parent = 0)
            : QAbstractMessageHandler(parent)
        {
        }

    protected:
        virtual void handleMessage(QtMsgType, const QString&, const QUrl&, const QSourceLocation&)
        {
        }
};

class DummyUriResolver : public QAbstractUriResolver
{
    public:
        DummyUriResolver(QObject *parent = 0)
            : QAbstractUriResolver(parent)
        {
        }

        virtual QUrl resolve(const QUrl&, const QUrl&) const
        {
            return QUrl();
        }
};

/*!
 \class tst_QXmlSchemaValidatorValidator
 \internal
 \brief Tests class QXmlSchemaValidator.

 This test is not intended for testing the engine, but the functionality specific
 to the QXmlSchemaValidator class.
 */
class tst_QXmlSchemaValidator : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void defaultConstructor() const;
    void propertyInitialization() const;

    void networkAccessManagerSignature() const;
    void networkAccessManagerDefaultValue() const;
    void networkAccessManager() const;

    void messageHandlerSignature() const;
    void messageHandlerDefaultValue() const;
    void messageHandler() const;

    void uriResolverSignature() const;
    void uriResolverDefaultValue() const;
    void uriResolver() const;
};

void tst_QXmlSchemaValidator::defaultConstructor() const
{
    /* Allocate instance in different orders. */
    {
        QXmlSchema schema;
        QXmlSchemaValidator validator(schema);
    }

    {
        QXmlSchema schema1;
        QXmlSchema schema2;

        QXmlSchemaValidator validator1(schema1);
        QXmlSchemaValidator validator2(schema2);
    }

    {
        QXmlSchema schema;

        QXmlSchemaValidator validator1(schema);
        QXmlSchemaValidator validator2(schema);
    }
}

void tst_QXmlSchemaValidator::propertyInitialization() const
{
    /* Verify that properties set in the schema are used as default values for the validator */
    {
        DummyMessageHandler handler;
        DummyUriResolver resolver;
        QNetworkAccessManager manager;

        QXmlSchema schema;
        schema.setMessageHandler(&handler);
        schema.setUriResolver(&resolver);
        schema.setNetworkAccessManager(&manager);

        QXmlSchemaValidator validator(schema);
        QCOMPARE(validator.messageHandler(), &handler);
        QCOMPARE(validator.uriResolver(), &resolver);
        QCOMPARE(validator.networkAccessManager(), &manager);
    }
}

void tst_QXmlSchemaValidator::networkAccessManagerSignature() const
{
    const QXmlSchema schema;

    /* Const object. */
    const QXmlSchemaValidator validator(schema);

    /* The function should be const. */
    validator.networkAccessManager();
}

void tst_QXmlSchemaValidator::networkAccessManagerDefaultValue() const
{
    /* Test that the default value of network access manager is equal to the one from the schema. */
    {
        const QXmlSchema schema;
        const QXmlSchemaValidator validator(schema);
        QVERIFY(validator.networkAccessManager() == schema.networkAccessManager());
    }

    /* Test that the default value of network access manager is equal to the one from the schema. */
    {
        QXmlSchema schema;

        QNetworkAccessManager manager;
        schema.setNetworkAccessManager(&manager);

        const QXmlSchemaValidator validator(schema);
        QVERIFY(validator.networkAccessManager() == schema.networkAccessManager());
    }
}

void tst_QXmlSchemaValidator::networkAccessManager() const
{
    /* Test that we return the network access manager that was set. */
    {
        QNetworkAccessManager manager;

        const QXmlSchema schema;
        QXmlSchemaValidator validator(schema);

        validator.setNetworkAccessManager(&manager);
        QCOMPARE(validator.networkAccessManager(), &manager);
    }

    /* Test that we return the network access manager that was set, even if the schema changed in between. */
    {
        QNetworkAccessManager manager;

        const QXmlSchema schema;
        QXmlSchemaValidator validator(schema);

        validator.setNetworkAccessManager(&manager);

        const QXmlSchema schema2;
        validator.setSchema(schema2);

        QCOMPARE(validator.networkAccessManager(), &manager);
    }
}

void tst_QXmlSchemaValidator::messageHandlerSignature() const
{
    const QXmlSchema schema;

    /* Const object. */
    const QXmlSchemaValidator validator(schema);

    /* The function should be const. */
    validator.messageHandler();
}

void tst_QXmlSchemaValidator::messageHandlerDefaultValue() const
{
    /* Test that the default value of message handler is equal to the one from the schema. */
    {
        const QXmlSchema schema;
        const QXmlSchemaValidator validator(schema);
        QVERIFY(validator.messageHandler() == schema.messageHandler());
    }

    /* Test that the default value of network access manager is equal to the one from the schema. */
    {
        QXmlSchema schema;

        DummyMessageHandler handler;
        schema.setMessageHandler(&handler);

        const QXmlSchemaValidator validator(schema);
        QVERIFY(validator.messageHandler() == schema.messageHandler());
    }
}

void tst_QXmlSchemaValidator::messageHandler() const
{
    /* Test that we return the message handler that was set. */
    {
        DummyMessageHandler handler;

        const QXmlSchema schema;
        QXmlSchemaValidator validator(schema);

        validator.setMessageHandler(&handler);
        QCOMPARE(validator.messageHandler(), &handler);
    }

    /* Test that we return the message handler that was set, even if the schema changed in between. */
    {
        DummyMessageHandler handler;

        const QXmlSchema schema;
        QXmlSchemaValidator validator(schema);

        validator.setMessageHandler(&handler);

        const QXmlSchema schema2;
        validator.setSchema(schema2);

        QCOMPARE(validator.messageHandler(), &handler);
    }
}

void tst_QXmlSchemaValidator::uriResolverSignature() const
{
    const QXmlSchema schema;

    /* Const object. */
    const QXmlSchemaValidator validator(schema);

    /* The function should be const. */
    validator.uriResolver();
}

void tst_QXmlSchemaValidator::uriResolverDefaultValue() const
{
    /* Test that the default value of uri resolver is equal to the one from the schema. */
    {
        const QXmlSchema schema;
        const QXmlSchemaValidator validator(schema);
        QVERIFY(validator.uriResolver() == schema.uriResolver());
    }

    /* Test that the default value of uri resolver is equal to the one from the schema. */
    {
        QXmlSchema schema;

        DummyUriResolver resolver;
        schema.setUriResolver(&resolver);

        const QXmlSchemaValidator validator(schema);
        QVERIFY(validator.uriResolver() == schema.uriResolver());
    }
}

void tst_QXmlSchemaValidator::uriResolver() const
{
    /* Test that we return the uri resolver that was set. */
    {
        DummyUriResolver resolver;

        const QXmlSchema schema;
        QXmlSchemaValidator validator(schema);

        validator.setUriResolver(&resolver);
        QCOMPARE(validator.uriResolver(), &resolver);
    }

    /* Test that we return the uri resolver that was set, even if the schema changed in between. */
    {
        DummyUriResolver resolver;

        const QXmlSchema schema;
        QXmlSchemaValidator validator(schema);

        validator.setUriResolver(&resolver);

        const QXmlSchema schema2;
        validator.setSchema(schema2);

        QCOMPARE(validator.uriResolver(), &resolver);
    }
}

QTEST_MAIN(tst_QXmlSchemaValidator)

#include "tst_qxmlschemavalidator.moc"
#else //QTEST_PATTERNIST
QTEST_NOOP_MAIN
#endif
