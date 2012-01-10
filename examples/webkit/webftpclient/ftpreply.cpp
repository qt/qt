/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QTextDocument>
#include <QtNetwork>
#include "ftpreply.h"

//! [constructor]
FtpReply::FtpReply(const QUrl &url)
    : QNetworkReply()
{
    ftp = new QFtp(this);
    connect(ftp, SIGNAL(listInfo(QUrlInfo)), this, SLOT(processListInfo(QUrlInfo)));
    connect(ftp, SIGNAL(readyRead()), this, SLOT(processData()));
    connect(ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(processCommand(int, bool)));

    offset = 0;
    units = QStringList() << tr("bytes") << tr("K") << tr("M") << tr("G")
                          << tr("Ti") << tr("Pi") << tr("Ei") << tr("Zi")
                          << tr("Yi");

    setUrl(url);
    ftp->connectToHost(url.host());
}
//! [constructor]

//! [process command]
void FtpReply::processCommand(int, bool err)
{
    if (err) {
        setError(ContentNotFoundError, tr("Unknown command"));
        emit error(ContentNotFoundError);
        return;
    }

    switch (ftp->currentCommand()) {
    case QFtp::ConnectToHost:
        ftp->login();
        break;

    case QFtp::Login:
        ftp->list(url().path());
        break;

    case QFtp::List:
        if (items.size() == 1)
            ftp->get(url().path());
        else
            setListContent();
        break;

    case QFtp::Get:
        setContent();

    default:
        ;
    }
}
//! [process command]

//! [process list info]
void FtpReply::processListInfo(const QUrlInfo &urlInfo)
{
    items.append(urlInfo);
}
//! [process list info]

//! [process data]
void FtpReply::processData()
{
    content += ftp->readAll();
}
//! [process data]

//! [set content]
void FtpReply::setContent()
{
    open(ReadOnly | Unbuffered);
    setHeader(QNetworkRequest::ContentLengthHeader, QVariant(content.size()));
    emit readyRead();
    emit finished();
    ftp->close();
}
//! [set content]

//! [set list content]
void FtpReply::setListContent()
{
    QUrl u = url();
    if (!u.path().endsWith("/"))
        u.setPath(u.path() + "/");

    QString base_url = url().toString();
    QString base_path = u.path();

    open(ReadOnly | Unbuffered);
    QString content(
        "<html>\n"
        "<head>\n"
        "  <title>" + Qt::escape(base_url) + "</title>\n"
        "  <style type=\"text/css\">\n"
        "  th { background-color: #aaaaaa; color: black }\n"
        "  table { border: solid 1px #aaaaaa }\n"
        "  tr.odd { background-color: #dddddd; color: black\n }\n"
        "  tr.even { background-color: white; color: black\n }\n"
        "  </style>\n"
        "</head>\n\n"
        "<body>\n"
        "<h1>" + tr("Listing for %1").arg(base_path) + "</h1>\n\n"
        "<table align=\"center\" cellspacing=\"0\" width=\"90%\">\n"
        "<tr><th>Name</th><th>Size</th></tr>\n");

    QUrl parent = u.resolved(QUrl(".."));

    if (parent.isParentOf(u))

        content += QString("<tr><td><strong><a href=\"" + parent.toString() + "\">"
            + tr("Parent directory") + "</a></strong></td><td></td></tr>\n");

    int i = 0;
    foreach (const QUrlInfo &item, items) {

        QUrl child = u.resolved(QUrl(item.name()));

        if (i == 0)
            content += QString("<tr class=\"odd\">");
        else
            content += QString("<tr class=\"even\">");

        content += QString("<td><a href=\"" + child.toString() + "\">"
                           + Qt::escape(item.name()) + "</a></td>");

        qint64 size = item.size();
        int unit = 0;
        while (size) {
            qint64 new_size = size/1024;
            if (new_size && unit < units.size()) {
                size = new_size;
                unit += 1;
            } else
                break;
        }

        if (item.isFile())
            content += QString("<td>" + QString::number(size) + " "
                               + units[unit] + "</td></tr>\n");
        else
            content += QString("<td></td></tr>\n");

        i = 1 - i;
    }

    content += QString("</table>\n"
                       "</body>\n"
                       "</html>\n");

    this->content = content.toUtf8();

    setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/html; charset=UTF-8"));
    setHeader(QNetworkRequest::ContentLengthHeader, QVariant(this->content.size()));
    emit readyRead();
    emit finished();
    ftp->close();
}
//! [set list content]

// QIODevice methods

//! [abort]
void FtpReply::abort()
{
}
//! [abort]

//! [bytes available]
qint64 FtpReply::bytesAvailable() const
{
    return content.size() - offset;
}
//! [bytes available]

//! [is sequential]
bool FtpReply::isSequential() const
{
    return true;
}
//! [is sequential]

//! [read data]
qint64 FtpReply::readData(char *data, qint64 maxSize)
{
    if (offset < content.size()) {
        qint64 number = qMin(maxSize, content.size() - offset);
        memcpy(data, content.constData() + offset, number);
        offset += number;
        return number;
    } else
        return -1;
}
//! [read data]
