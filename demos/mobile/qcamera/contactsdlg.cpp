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

