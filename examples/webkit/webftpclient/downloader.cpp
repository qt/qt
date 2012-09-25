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

#include <QFileDialog>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "downloader.h"

Downloader::Downloader(QWidget *parentWidget, QNetworkAccessManager *manager)
    : QObject(parentWidget), manager(manager), parentWidget(parentWidget)
{
}

QString Downloader::chooseSaveFile(const QUrl &url)
{
    QString fileName = url.path().split("/").last();
    if (!path.isEmpty())
        fileName = QDir(path).filePath(fileName);

    return QFileDialog::getSaveFileName(parentWidget, tr("Save File"), fileName);
}

void Downloader::startDownload(const QNetworkRequest &request)
{
    downloads[request.url().toString()] = chooseSaveFile(request.url());

    QNetworkReply *reply = manager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(finishDownload()));
}

void Downloader::saveFile(QNetworkReply *reply)
{
    QString newPath = downloads[reply->url().toString()];

    if (newPath.isEmpty())
        newPath = chooseSaveFile(reply->url());

    if (!newPath.isEmpty()) {
        QFile file(newPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
            path = QDir(newPath).dirName();
            QMessageBox::information(parentWidget, tr("Download Completed"),
                                     tr("Saved '%1'.").arg(newPath));
        } else
            QMessageBox::warning(parentWidget, tr("Download Failed"),
                                 tr("Failed to save the file."));
    }
}

void Downloader::finishDownload()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    saveFile(reply);
    downloads.remove(reply->url().toString());
    reply->deleteLater();
}
