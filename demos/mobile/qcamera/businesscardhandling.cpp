/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
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
    // Workaround for Maemo bug http://bugreports.qt-project.org/browse/QTMOBILITY-437
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

