#include <QtGui>
#include "filereader.h"


FileReader::FileReader(QWidget *parent)
    : QWidget(parent)
{
    textEdit = new QTextEdit;

    taxFileButton = new QPushButton("Tax File");
    accountFileButton = new QPushButton("Accounts File");
    reportFileButton = new QPushButton("Report File");

//! [0]
    signalMapper = new QSignalMapper(this);
    signalMapper->setMapping(taxFileButton, QString("taxfile.txt"));
    signalMapper->setMapping(accountFileButton, QString("accountsfile.txt"));
    signalMapper->setMapping(reportFileButton, QString("reportfile.txt"));

    connect(taxFileButton, SIGNAL(clicked()),
        signalMapper, SLOT (map()));
    connect(accountFileButton, SIGNAL(clicked()),
        signalMapper, SLOT (map()));
    connect(reportFileButton, SIGNAL(clicked()),
        signalMapper, SLOT (map()));
//! [0]

//! [1]
    connect(signalMapper, SIGNAL(mapped(const QString &)),
        this, SLOT(readFile(const QString &)));
//! [1]

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(taxFileButton);
    buttonLayout->addWidget(accountFileButton);
    buttonLayout->addWidget(reportFileButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(textEdit);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void FileReader::readFile(const QString &filename)
{
    QFile file(filename);
    
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
        return;
    }


    QTextStream in(&file);
    textEdit->setPlainText(in.readAll());
}

