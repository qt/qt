/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include "messagehandling.h"
#include <QMessageBox>
#include <QDebug>
#include <QTimerEvent>
#include <QTimer>
#include <QFile>
#include <QPixmap>
#include <QImageReader>

Message::Message(QObject *parent) :
    QObject(parent)
{
    // QMessageService class provides the interface for requesting messaging service operations
    m_service = new QMessageService(this);
    //QObject::connect(m_service, SIGNAL(stateChanged(QMessageService::State)), this, SLOT(stateChanged(QMessageService::State)));
    QObject::connect(m_service, SIGNAL(messagesFound(const QMessageIdList&)), this, SLOT(messagesFound(const QMessageIdList&)));

    // QMessageManager class represents the main interface for storage and
    // retrieval of messages, folders and accounts in the system message store
    m_manager = new QMessageManager(this);

    QObject::connect(m_manager, SIGNAL(messageAdded(const QMessageId&,const QMessageManager::NotificationFilterIdSet&)),
    this, SLOT(messageAdded(const QMessageId&,const QMessageManager::NotificationFilterIdSet&)));

    // Register MMS in inbox (draft in emulator) folder notificationfilter
#ifdef Q_OS_SYMBIAN
#ifdef __WINS__
    m_notifFilterSet.insert(m_manager->registerNotificationFilter(QMessageFilter::byStandardFolder(
        QMessage::DraftsFolder)));
#else
    m_notifFilterSet.insert(m_manager->registerNotificationFilter(QMessageFilter::byStandardFolder(
        QMessage::InboxFolder)));
#endif
#else
    m_notifFilterSet.insert(m_manager->registerNotificationFilter(QMessageFilter::byStandardFolder(
        QMessage::InboxFolder)));
#endif

}

Message::~Message()
{
}

void Message::messageAdded(const QMessageId& id,
    const QMessageManager::NotificationFilterIdSet& matchingFilterIds)
{
    if (matchingFilterIds.contains(m_notifFilterSet)) {
        processIncomingMMS(id);
    }
}


void Message::checkMessages()
{
#ifdef Q_OS_SYMBIAN
#ifdef __WINS__
    QMessageFilter folderFilter(QMessageFilter::byStandardFolder(QMessage::DraftsFolder));
#else
    QMessageFilter folderFilter(QMessageFilter::byStandardFolder(QMessage::InboxFolder));
#endif
#else
    QMessageFilter folderFilter(QMessageFilter::byStandardFolder(QMessage::InboxFolder));
#endif

    m_service->queryMessages(folderFilter);
    // Message::messagesFound() is called if MMS messages found

}

void Message::messagesFound(const QMessageIdList &ids)
{
    foreach (const QMessageId& id, ids) {
            processIncomingMMS(id);
        }
}

void Message::processIncomingMMS(const QMessageId& id)
{
    QMessage message = m_manager->message(id);

    // Handle only MMS messages
    if (message.type()!=QMessage::Mms)
        return;


    QMessageContentContainerIdList attachments = message.attachmentIds();
    if (!attachments.isEmpty()) {
        QMessageContentContainer messageContent = message.find(attachments[0]);
        if (messageContent.isContentAvailable() && messageContent.contentType() == "image") {

            // Create QPixmap from the message image attachment
            QPixmap pixmap;
            pixmap.loadFromData(messageContent.content());

            QString from = message.from().addressee();
            QString filename = messageContent.suggestedFileName();

            // Emit received MMS message info
            emit messageReceived(from, filename, pixmap);
        }
    }
}

bool Message::sendMMS(QString picturePath, QString phoneNumber)
{
    QString tmpFileName = "c:/System/qcamera_mms.jpg";

    // Create temp image for MMS
    // Delete previous temp image
    QFile previousFile(tmpFileName);
    if (previousFile.exists()) {
        previousFile.remove();
    }
    // Create new temp image
    QImageReader reader;
    reader.setFileName(picturePath);
    QSize imageSize = reader.size();
    imageSize.scale(QSize(300,300), Qt::KeepAspectRatio);
    reader.setScaledSize(imageSize);
    QImage image = reader.read();
    image.save(tmpFileName);

    // Use temp mms image
    picturePath = tmpFileName;

    // Send MMS
    QMessage message;
    message.setType(QMessage::Mms);
    message.setParentAccountId(QMessageAccount::defaultAccount(QMessage::Mms));
    message.setTo(QMessageAddress(QMessageAddress::Phone, phoneNumber));

    QStringList paths;
    paths << picturePath;
    message.appendAttachments(paths);

    return m_service->send(message);
}

void Message::stateChanged(QMessageService::State s)
{
    emit messageStateChanged(s);
}
