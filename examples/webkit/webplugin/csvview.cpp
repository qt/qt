/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtNetwork>
#include "csvview.h"

//! [constructor]
CSVView::CSVView(const QString &mimeType, QWidget *parent)
    : QTableView(parent)
{
    this->mimeType = mimeType;

    setEditTriggers(NoEditTriggers);
    setSelectionBehavior(SelectRows);
    setSelectionMode(SingleSelection);
}
//! [constructor]

void CSVView::updateModel()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());

    if (reply->error() != QNetworkReply::NoError)
        return;

    bool hasHeader = false;
    QString charset = "latin1";

    foreach (QString piece, mimeType.split(";")) {
        piece = piece.trimmed();
        if (piece.contains("=")) {
            int index = piece.indexOf("=");
            QString left = piece.left(index).trimmed();
            QString right = piece.mid(index + 1).trimmed();
            if (left == "header")
                hasHeader = (right == "present");
            else if (left == "charset")
                charset = right;
        }
    }

    QTextStream stream(reply);
    stream.setCodec(QTextCodec::codecForName(charset.toLatin1()));

    QStandardItemModel *model = new QStandardItemModel(this);
    QList<QStandardItem *> items;
    bool firstLine = hasHeader;
    bool wasQuote = false;
    bool wasCR = false;
    bool quoted = false;
    QString text;

    while (!stream.atEnd()) {

        QString ch = stream.read(1);

        if (wasQuote) {
            if (ch == "\"") {
                if (quoted) {
                    text += ch;         // quoted "" are inserted as "
                    wasQuote = false;   // no quotes are pending
                } else {
                    quoted = true;      // unquoted "" starts quoting
                    wasQuote = true;    // with a pending quote
                }
                continue;               // process the next character

            } else {
                quoted = !quoted;       // process the pending quote
                wasQuote = false;       // no quotes are pending
            }                           // process the current character

        } else if (wasCR) {
            wasCR = false;

            if (ch == "\n") {           // CR LF represents the end of a row
                if (!text.isEmpty())
                    items.append(new QStandardItem(QString(text)));

                addRow(firstLine, model, items);
                items.clear();
                text = "";
                firstLine = false;
                continue;               // process the next character
            } else
                text += "\r";           // CR on its own is inserted
        }                               // process the current character

        // wasQuote is never true here.
        // wasCR is never true here.

        if (ch == "\"")
            wasQuote = true;            // handle the pending quote later

        else if (ch == ",") {
            if (quoted)
                text += ch;
            else {
                items.append(new QStandardItem(QString(text)));
                text = "";
            }
        }

        else if (ch == "\r") {
            if (!quoted)
                wasCR = true;
            else
                text += ch;
        }

        else if (ch == "\n")
            text += ch;
        else
            text += ch;

    }

    if (items.count() > 0)
        addRow(firstLine, model, items);

    reply->close();

    setModel(model);

    connect(selectionModel(),
            SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(exportRow(const QModelIndex &)));

    resizeColumnsToContents();
    horizontalHeader()->setStretchLastSection(true);
}

void CSVView::addRow(bool firstLine, QStandardItemModel *model,
                     const QList<QStandardItem *> &items)
{
    if (firstLine) {
        for (int j = 0; j < items.count(); ++j)
            model->setHorizontalHeaderItem(j, items[j]);
    } else
        model->appendRow(items);
}

//! [export row]
void CSVView::exportRow(const QModelIndex &current)
{
    QString name = model()->index(current.row(), 0).data().toString();
    QString address = model()->index(current.row(), 1).data().toString();
    QString quantity = model()->index(current.row(), 2).data().toString();

    emit rowSelected(name, address, quantity);
}
//! [export row]
