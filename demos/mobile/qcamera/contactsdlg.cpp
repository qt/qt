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

#include "contactsdlg.h"
#include <QHBoxLayout>
#include <QApplication>
#include <QAction>
#include <QTimer>
#include <QPushButton>

ContactsDialog::ContactsDialog(QWidget *parent) :
    QDialog(parent)
{
    QHBoxLayout* l = new QHBoxLayout;
    m_listWidget = new QListWidget(this);
    l->addWidget(m_listWidget);

    QVBoxLayout* lv = new QVBoxLayout;

    QPushButton* backBtn = new QPushButton("Back",this);
    QObject::connect(backBtn, SIGNAL(pressed()), this, SLOT(close()));
    backBtn->setFixedWidth(100);
    lv->addWidget(backBtn);
    lv->setAlignment(backBtn,Qt::AlignTop);

    QPushButton* okBtn = new QPushButton("Ok",this);
    QObject::connect(okBtn, SIGNAL(pressed()), this, SLOT(selectContact()));
    okBtn->setFixedWidth(100);
    lv->addWidget(okBtn);
    lv->setAlignment(okBtn,Qt::AlignBottom);

    l->addLayout(lv);

    setLayout(l);

    // Remove context menu from the all widgets
#ifdef Q_OS_SYMBIAN
    QWidgetList widgets = QApplication::allWidgets();
    QWidget* w = 0;
    foreach (w,widgets)
        {
            w->setContextMenuPolicy(Qt::NoContextMenu);
        }
#endif

    // Create QContactManager and search contacts
    createContactManager();
    searchContact();

    showFullScreen();
}

ContactsDialog::~ContactsDialog()
{
    delete m_contactManager;
}

void ContactsDialog::createContactManager()
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

void ContactsDialog::searchContact()
{
    m_listWidget->clear();

    // Sort contacts by lastname
    QContactSortOrder sort;
    sort.setDirection(Qt::AscendingOrder);
    sort.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLastName);

    // Build QListWidget from the contact list
    QList<QContactLocalId> contactIds = m_contactManager->contactIds(sort);
    QContact currContact;
    foreach (const QContactLocalId& id, contactIds)
        {
            QListWidgetItem *currItem = new QListWidgetItem;
            currContact = m_contactManager->contact(id);
            QContactDisplayLabel dl = currContact.detail(QContactDisplayLabel::DefinitionName);
            currItem->setData(Qt::DisplayRole, dl.label());
            currItem->setData(Qt::UserRole, currContact.localId()); // also store the id of the contact
            m_listWidget->addItem(currItem);
        }

    if (m_listWidget->count()>0) {
        m_listWidget->setCurrentRow(0);
    }
}

void ContactsDialog::selectContact()
{
    QList<QListWidgetItem*> items = m_listWidget->selectedItems();
    if (!items.isEmpty()) {
        itemClicked(items.first());
    }
}

void ContactsDialog::itemClicked(QListWidgetItem *item)
{
    QVariant data = item->data(Qt::UserRole);
    QContactLocalId id = data.toInt();
    QContact contact = m_contactManager->contact(id);
    QContactPhoneNumber cpn = contact.detail<QContactPhoneNumber> ();

    // Emit contact phonenumber
    if (!cpn.isEmpty()) {
        emit contactSelected(cpn.number());
        // Close dialog
        close();
    }
}

