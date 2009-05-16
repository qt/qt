/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Patternist project on Trolltech Labs.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "main.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtXmlPatterns/QXmlSchema>
#include <QtXmlPatterns/QXmlSchemaValidator>

QT_USE_NAMESPACE

int main(int argc, char **argv)
{
    enum ExitCode
    {
        Valid = 0,
        Invalid,
        ParseError
    };

    enum ExecutionMode
    {
        InvalidMode,
        SchemaOnlyMode,
        SchemaAndInstanceMode,
        InstanceOnlyMode
    };

    const QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QLatin1String("xmlpatternsvalidator"));

    if (argc != 2 && argc != 3) {
        qDebug() << QXmlPatternistCLI::tr("usage: xmlpatternsvalidator (<schema url> | <instance url> <schema url> | <instance url>)");
        return ParseError;
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
            return Valid;
        else
            return Invalid;
    } else if (mode == SchemaAndInstanceMode) {
        const QString instanceUri = QFile::decodeName(argv[1]);
        const QString schemaUri = QFile::decodeName(argv[2]);

        schema.load(QUrl(schemaUri));

        if (!schema.isValid())
            return Invalid;

        QXmlSchemaValidator validator(schema);
        if (validator.validate(QUrl(instanceUri)))
            return Valid;
        else
            return Invalid;
    } else if (mode == InstanceOnlyMode) {
        const QString instanceUri = QFile::decodeName(argv[1]);

        QXmlSchemaValidator validator(schema);
        if (validator.validate(QUrl(instanceUri)))
            return Valid;
        else
            return Invalid;
    }

    Q_ASSERT(false);

    return Invalid;
}
