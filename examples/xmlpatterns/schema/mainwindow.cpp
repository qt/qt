/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtXmlPatterns>

#include "mainwindow.h"
#include "xmlsyntaxhighlighter.h"

class MessageHandler : public QAbstractMessageHandler
{
    public:
        MessageHandler()
            : QAbstractMessageHandler(0)
        {
        }

        QString statusMessage() const
        {
            return m_description;
        }

        int line() const
        {
            return m_sourceLocation.line();
        }

        int column() const
        {
            return m_sourceLocation.column();
        }

    protected:
        virtual void handleMessage(QtMsgType type, const QString &description, const QUrl &identifier, const QSourceLocation &sourceLocation)
        {
            Q_UNUSED(type);
            Q_UNUSED(identifier);

            m_messageType = type;
            m_description = description;
            m_sourceLocation = sourceLocation;
        }

    private:
        QtMsgType m_messageType;
        QString m_description;
        QSourceLocation m_sourceLocation;
};

MainWindow::MainWindow()
{
    setupUi(this);

    new XmlSyntaxHighlighter(schemaView->document());
    new XmlSyntaxHighlighter(instanceEdit->document());

    schemaSelection->addItem(tr("Contact Schema"));
    schemaSelection->addItem(tr("Recipe Schema"));
    schemaSelection->addItem(tr("Order Schema"));

    instanceSelection->addItem(tr("Valid Contact Instance"));
    instanceSelection->addItem(tr("Invalid Contact Instance"));

    connect(schemaSelection, SIGNAL(currentIndexChanged(int)), SLOT(schemaSelected(int)));
    connect(instanceSelection, SIGNAL(currentIndexChanged(int)), SLOT(instanceSelected(int)));
    connect(validateButton, SIGNAL(clicked()), SLOT(validate()));
    connect(instanceEdit, SIGNAL(textChanged()), SLOT(textChanged()));

    validationStatus->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    schemaSelected(0);
    instanceSelected(0);
}

void MainWindow::schemaSelected(int index)
{
    instanceSelection->clear();
    if (index == 0) {
        instanceSelection->addItem(tr("Valid Contact Instance"));
        instanceSelection->addItem(tr("Invalid Contact Instance"));
    } else if (index == 1) {
        instanceSelection->addItem(tr("Valid Recipe Instance"));
        instanceSelection->addItem(tr("Invalid Recipe Instance"));
    } else if (index == 2) {
        instanceSelection->addItem(tr("Valid Order Instance"));
        instanceSelection->addItem(tr("Invalid Order Instance"));
    }
    textChanged();

    QFile schemaFile(QString(":/schema_%1.xsd").arg(index));
    schemaFile.open(QIODevice::ReadOnly);
    const QString schemaText(QString::fromLatin1(schemaFile.readAll()));
    schemaView->setPlainText(schemaText);

    validate();
}

void MainWindow::instanceSelected(int index)
{
    QFile instanceFile(QString(":/instance_%1.xml").arg((2*schemaSelection->currentIndex()) + index));
    instanceFile.open(QIODevice::ReadOnly);
    const QString instanceText(QString::fromLatin1(instanceFile.readAll()));
    instanceEdit->setPlainText(instanceText);

    validate();
}

void MainWindow::validate()
{
    const QByteArray schemaData = schemaView->toPlainText().toLatin1();
    const QByteArray instanceData = instanceEdit->toPlainText().toLatin1();

    MessageHandler messageHandler;

    QXmlSchema schema;
    schema.setMessageHandler(&messageHandler);

    schema.load(schemaData, QUrl("http://dummySchemaUrl/"));

    bool errorOccurred = false;
    if (!schema.isValid()) {
        errorOccurred = true;
    } else {
        QXmlSchemaValidator validator(schema);
        if (!validator.validate(instanceData, QUrl("http://dummyInstanceUrl")))
            errorOccurred = true;
    }

    if (errorOccurred) {
        validationStatus->setText(messageHandler.statusMessage());
        moveCursor(messageHandler.line(), messageHandler.column());
    } else {
        validationStatus->setText(tr("validation successful"));
    }

    QString styleSheet = QString("QLabel {background: %1; padding: 3px}").arg(errorOccurred ? QColor(Qt::red).lighter(160).name() : QColor(Qt::green).lighter(160).name());
    validationStatus->setStyleSheet(styleSheet);
}

void MainWindow::textChanged()
{
    instanceEdit->setExtraSelections(QList<QTextEdit::ExtraSelection>());
}

void MainWindow::moveCursor(int line, int column)
{
    instanceEdit->moveCursor(QTextCursor::Start);
    for (int i = 1; i < line; ++i)
        instanceEdit->moveCursor(QTextCursor::Down);

    for (int i = 1; i < column; ++i)
        instanceEdit->moveCursor(QTextCursor::Right);

    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;

    const QColor lineColor = QColor(Qt::red).lighter(160);
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = instanceEdit->textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);

    instanceEdit->setExtraSelections(extraSelections);

    instanceEdit->setFocus();
}
