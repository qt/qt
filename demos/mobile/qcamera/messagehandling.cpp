/**
 * Copyright (c) 2011 Nokia Corporation. All rights reserved.
 *
 * Nokia and Nokia Connecting People are registered trademarks of Nokia
 * Corporation. Oracle and Java are registered trademarks of Oracle and/or its
 * affiliates. Other product and company names mentioned herein may be
 * trademarks or trade names of their respective owners.
 *
 *
 * Subject to the conditions below, you may, without charge:
 *
 *  *  Use, copy, modify and/or merge copies of this software and associated
 *     documentation files (the "Software")
 *
 *  *  Publish, distribute, sub-licence and/or sell new software derived from
 *     or incorporating the Software.
 *
 *
 *
 * This file, unmodified, shall be included with all copies or substantial
 * portions of the Software that are distributed in source code form.
 *
 * The Software cannot constitute the primary value of any new software derived
 * from or incorporating the Software.
 *
 * Any person dealing with the Software shall not misrepresent the source of
 * the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
