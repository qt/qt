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

#include <QtCore>

void parseHtmlFile(QTextStream &out, const QString &fileName) {
    QFile file(fileName);

    out << "Analysis of HTML file: " << fileName << endl;

    if (!file.open(QIODevice::ReadOnly)) {
        out << "  Couldn't open the file." << endl << endl << endl;
        return;
    }

//! [0]
    QXmlStreamReader reader(&file);
//! [0]

//! [1]
    int paragraphCount = 0;
    QStringList links;
    QString title;
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            if (reader.name() == "title")
                title = reader.readElementText();
            else if(reader.name() == "a")
                links.append(reader.attributes().value("href").toString());
            else if(reader.name() == "p")
                ++paragraphCount;
        }
    }
//! [1]

//! [2]
    if (reader.hasError()) {
        out << "  The HTML file isn't well-formed: " << reader.errorString()
            << endl << endl << endl;
        return;
    }
//! [2]

    out << "  Title: \"" << title << "\"" << endl
        << "  Number of paragraphs: " << paragraphCount << endl
        << "  Number of links: " << links.size() << endl
        << "  Showing first few links:" << endl;

    while(links.size() > 5)
        links.removeLast();

    foreach(QString link, links)
        out << "    " << link << endl;
    out << endl << endl;
}

int main(int argc, char **argv)
{
    // intialize QtCore application
    QCoreApplication app(argc, argv);

    // get a list of all html files in the current directory
    QStringList filter;
    filter << "*.htm";
    filter << "*.html";
    QStringList htmlFiles = QDir::current().entryList(filter, QDir::Files);

    QTextStream out(stdout);

    if (htmlFiles.isEmpty()) {
        out << "No html files available.";
        return 1;
    }

    // parse each html file and write the result to file/stream
    foreach(QString file, htmlFiles)
        parseHtmlFile(out, file);

    return 0;
}
