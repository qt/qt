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

#include "businesscardhandling.h"
#include <QFile>

BusinessCardHandling::BusinessCardHandling(QObject *parent) :
    QObject(parent)
{
}

BusinessCardHandling::~BusinessCardHandling()
{
    delete m_contactManager;
}

void BusinessCardHandling::createContactManager()
{
#if defined Q_WS_MAEMO_5
    m_contactManager = new QContactManager("maemo5");
#elif defined Q_OS_SYMBIAN
    m_contactManager = new QContactManager("symbian");
#endif

    // Use default
    if (!m_contactManager) {
        m_contactManager = new QContactManager();
    }
}

void BusinessCardHandling::storeAvatarToContact(QString phoneNumber, QString filename,
    QPixmap pixmap)
{
    // Create QContactManager
    if (!m_contactManager) {
        createContactManager();
    }

    // Search contacts and save avatar
    QContact contact;
    if (findContact(phoneNumber, contact)) {
        saveAvatar(filename, pixmap, contact);
    }
}

bool BusinessCardHandling::findContact(const QString phoneNumber, QContact& c)
{
    // Create QContactManager
    if (!m_contactManager) {
        createContactManager();
    }

    QContact contact;
    QContactDetailFilter phoneFilter;
    phoneFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName,
        QContactPhoneNumber::FieldNumber);

#if defined Q_WS_MAEMO_5
    // Workaround for Maemo bug http://bugreports.qt.nokia.com/browse/QTMOBILITY-437
    phoneFilter.setValue(phoneNumber.right(7));
    phoneFilter.setMatchFlags(QContactFilter::MatchContains);
#else
    phoneFilter.setValue(phoneNumber);
    phoneFilter.setMatchFlags(QContactFilter::MatchPhoneNumber);
#endif

    // Find contacts
    QList<QContact> matchingContacts = m_contactManager->contacts(phoneFilter);
    if (matchingContacts.size() > 0) {
        contact = matchingContacts.at(0);
        c = contact;
        return true;
    } else {
        return false;
    }
}

void BusinessCardHandling::saveAvatar(const QString filename, QPixmap p, QContact& contact)
{

    // Path to store avatar picture
    QString path;
#ifdef Q_OS_SYMBIAN
    path.append("c:/System/");
#endif
    path.append(filename);

    // Remove same file if exists
    QFile file;
    if (file.exists(path))
        file.remove(path);

    // Save pixmap into file
    bool saveRet = p.save(path);

    if (saveRet) {
        // Create avatar
        QContactAvatar contactAvatar;
        contactAvatar.setImageUrl(QUrl(path));
        bool saveAvatar = contact.saveDetail(&contactAvatar);

        // Save contact
        if (saveAvatar)
            m_contactManager->saveContact(&contact);

        // NOTE: Do not remove picture, system needs it for showing avatar
        // Remove picture file
        //bool removeRet = file.remove(path);
    }
}

