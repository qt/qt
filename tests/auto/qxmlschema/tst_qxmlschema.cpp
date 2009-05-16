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
 \class tst_QXmlSchema
 \internal
 \brief Tests class QXmlSchema.

 This test is not intended for testing the engine, but the functionality specific
 to the QXmlSchema class.
 */
class tst_QXmlSchema : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void defaultConstructor() const;
    void copyConstructor() const;
    void constructorQXmlNamePool() const;

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

void tst_QXmlSchema::defaultConstructor() const
{
    /* Allocate instance in different orders. */
    {
        QXmlSchema schema;
    }

    {
        QXmlSchema schema1;
        QXmlSchema schema2;
    }

    {
        QXmlSchema schema1;
        QXmlSchema schema2;
        QXmlSchema schema3;
    }
}

void tst_QXmlSchema::copyConstructor() const
{
    /* Verify that we can take a const reference, and simply do a copy of a default constructed object. */
    {
        const QXmlSchema schema1;
        QXmlSchema schema2(schema1);
    }

    /* Copy twice. */
    {
        const QXmlSchema schema1;
        QXmlSchema schema2(schema1);
        QXmlSchema schema3(schema2);
    }

    /* Verify that copying default values works. */
    {
        const QXmlSchema schema1;
        const QXmlSchema schema2(schema1);
        QCOMPARE(schema2.messageHandler(), schema1.messageHandler());
        QCOMPARE(schema2.uriResolver(), schema1.uriResolver());
        QCOMPARE(schema2.networkAccessManager(), schema1.networkAccessManager());
        QCOMPARE(schema2.isValid(), schema1.isValid());
    }
}

void tst_QXmlSchema::constructorQXmlNamePool() const
{
    QXmlSchema schema;

    QXmlNamePool np = schema.namePool();

    const QXmlName name(np, QLatin1String("localName"),
                            QLatin1String("http://example.com/"),
                            QLatin1String("prefix"));

    QXmlNamePool np2(schema.namePool());
    QCOMPARE(name.namespaceUri(np2), QString::fromLatin1("http://example.com/"));
    QCOMPARE(name.localName(np2), QString::fromLatin1("localName"));
    QCOMPARE(name.prefix(np2), QString::fromLatin1("prefix"));
}

void tst_QXmlSchema::networkAccessManagerSignature() const
{
    /* Const object. */
    const QXmlSchema schema;

    /* The function should be const. */
    schema.networkAccessManager();
}

void tst_QXmlSchema::networkAccessManagerDefaultValue() const
{
    /* Test that the default value of network access manager is not empty. */
    {
        QXmlSchema schema;
        QVERIFY(schema.networkAccessManager() != static_cast<QNetworkAccessManager*>(0));
    }
}

void tst_QXmlSchema::networkAccessManager() const
{
    /* Test that we return the network manager that was set. */
    {
        QNetworkAccessManager manager;
        QXmlSchema schema;
        schema.setNetworkAccessManager(&manager);
        QCOMPARE(schema.networkAccessManager(), &manager);
    }
}

void tst_QXmlSchema::messageHandlerSignature() const
{
    /* Const object. */
    const QXmlSchema schema;

    /* The function should be const. */
    schema.messageHandler();
}

void tst_QXmlSchema::messageHandlerDefaultValue() const
{
    /* Test that the default value of message handler is not empty. */
    {
        QXmlSchema schema;
        QVERIFY(schema.messageHandler() != static_cast<QAbstractMessageHandler*>(0));
    }
}

void tst_QXmlSchema::messageHandler() const
{
    /* Test that we return the message handler that was set. */
    {
        DummyMessageHandler handler;

        QXmlSchema schema;
        schema.setMessageHandler(&handler);
        QCOMPARE(schema.messageHandler(), &handler);
    }
}

void tst_QXmlSchema::uriResolverSignature() const
{
    /* Const object. */
    const QXmlSchema schema;

    /* The function should be const. */
    schema.uriResolver();
}

void tst_QXmlSchema::uriResolverDefaultValue() const
{
    /* Test that the default value of uri resolver is empty. */
    {
        QXmlSchema schema;
        QVERIFY(schema.uriResolver() == static_cast<QAbstractUriResolver*>(0));
    }
}

void tst_QXmlSchema::uriResolver() const
{
    /* Test that we return the uri resolver that was set. */
    {
        DummyUriResolver resolver;

        QXmlSchema schema;
        schema.setUriResolver(&resolver);
        QCOMPARE(schema.uriResolver(), &resolver);
    }
}

QTEST_MAIN(tst_QXmlSchema)

#include "tst_qxmlschema.moc"
#else //QTEST_PATTERNIST
QTEST_NOOP_MAIN
#endif
