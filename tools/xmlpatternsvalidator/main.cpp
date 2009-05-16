/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Patternist project on Trolltech Labs.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "main.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtXmlPatterns/QXmlSchema>
#include <QtXmlPatterns/QXmlSchemaValidator>

QT_USE_NAMESPACE

enum ExecutionMode
{
    InvalidMode,
    SchemaOnlyMode,
    SchemaAndInstanceMode,
    InstanceOnlyMode
};

int main(int argc, char **argv)
{
    const QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QLatin1String("xmlpatternsvalidator"));

    if (argc != 2 && argc != 3) {
        qDebug() << QXmlPatternistCLI::tr("usage: xmlpatternsvalidator (<schema url> | <instance url> <schema url> | <instance url>)");
        return 2;
    }

    // parse command line arguments
    ExecutionMode mode = InvalidMode;

    if (argc == 2) {
        // either it is a schema or instance document

        QString url = QFile::decodeName(argv[1]);
        if (url.toLower().endsWith(QLatin1String(".xsd"))) {
            mode = SchemaOnlyMode;
        } else {
            // as we could validate all types of xml documents, don't check the extension here
            mode = InstanceOnlyMode;
        }
    } else if (argc == 3) {
        mode = SchemaAndInstanceMode;
    }

    // do validation
    QXmlSchema schema;

    if (mode == SchemaOnlyMode) {
        const QString schemaUri = QFile::decodeName(argv[1]);

        schema.load(QUrl(schemaUri));

        if (schema.isValid())
            return 0;
        else
            return 1;
    } else if (mode == SchemaAndInstanceMode) {
        const QString instanceUri = QFile::decodeName(argv[1]);
        const QString schemaUri = QFile::decodeName(argv[2]);

        schema.load(QUrl(schemaUri));

        if (!schema.isValid())
            return 1;

        QXmlSchemaValidator validator(schema);
        if (validator.validate(QUrl(instanceUri)))
            return 0;
        else
            return 1;
    } else if (mode == InstanceOnlyMode) {
        const QString instanceUri = QFile::decodeName(argv[1]);

        QXmlSchemaValidator validator(schema);
        if (validator.validate(QUrl(instanceUri)))
            return 0;
        else
            return 1;
    }

    Q_ASSERT(false);

    return 1;
}
