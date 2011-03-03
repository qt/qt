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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QPixmap>

// QtMobility API headers
// Messaging
#include <QMessage>
#include <QMessageManager>
#include <QMessageService>

// Location
#include <QGeoPositionInfo>

// QtMobility namespace
QTM_USE_NAMESPACE

class Message: public QObject
{
Q_OBJECT

public:
    Message(QObject *parent = 0);
    ~Message();

    void checkMessages();
    bool sendMMS(QString picturePath, QString phoneNumber);

private:
    void processIncomingMMS(const QMessageId& id);

public slots:
    // QMessageService
    void stateChanged(QMessageService::State s);
    void messagesFound(const QMessageIdList &ids);

    // QMessageManager
    void messageAdded(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);

signals:
    void messageStateChanged(int);
    void messageReceived(QString from, QString filename, QPixmap pixmap);

private:
    QMessageService* m_service;
    QMessageManager* m_manager;
    QMessageManager::NotificationFilterIdSet m_notifFilterSet;
    QMessageId m_messageId;
};

#endif // MESSAGE_H
