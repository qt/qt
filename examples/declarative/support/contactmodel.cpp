/****************************************************************************
**
** Copyright (C) 1992-$THISYEAR$ $TROLLTECH$. All rights reserved.
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "contactmodel.h"

ContactModel::ContactModel(QObject *parent) : QListModelInterface(parent)
{
    QFile f("../contacts/contacts.txt");
    f.open(QIODevice::ReadOnly);
    QTextStream ts(&f);
    QString text = ts.readLine();
    while(!text.isEmpty()) {
        Contact *c = new Contact;
        QStringList list = text.split(" ");
        c->setFirstName(list[0]);
        c->setLastName(list[1]);
        for (int i = 2; i < list.count(); ++i)
            c->addEmail(list[i]);
        //contactList.append(c);
        insertContact(c);

        text = ts.readLine();
    }
    f.close();
}

ContactModel::~ContactModel()
{
    while (!contactList.isEmpty()) {
        Contact *c = contactList.takeFirst();
        delete c;
    }
}

int ContactModel::count() const
{
    return contactList.count();
}

QHash<int,QVariant> ContactModel::data(int index, const QList<int> &roles) const
{
    QHash<int,QVariant> returnHash;

    for (int i = 0; i < roles.size(); ++i) {
        int role = roles.at(i);
        QVariant info;
        switch(role) {
        case PortraitRole:
            info = "contact.png";
            break;
        case FirstNameRole:
            info = contactList.at(index)->firstName();
            break;
        case LastNameRole:
            info = contactList.at(index)->lastName();
            break;
        case CompanyRole:
            info = contactList.at(index)->company();
            break;
        case EmailsRole:
            info = contactList.at(index)->emails();
            break;
        case AddressesRole:
            //returns QVariant BOOL
            info = QVariant::fromValue(contactList.at(index)->addresses());
            break;
        case NumbersRole:
            info = QVariant::fromValue(contactList.at(index)->numbers());
            break;
        default:
            break;
        }
        returnHash.insert(role, info);
    }

    return returnHash;
}

QString ContactModel::toString(int role) const
{
    switch(role) {
    case PortraitRole:
        return "portrait";
    case FirstNameRole:
        return "firstName";
    case LastNameRole:
        return "lastName";
    case CompanyRole:
        return "company";
    case EmailsRole:
        return "emails";
    case AddressesRole:
        return "addresses";
    case NumbersRole:
        return "numbers";
    default:
        return "";
    }
}

QList<int> ContactModel::roles() const
{
    return QList<int>() << PortraitRole << FirstNameRole << LastNameRole << CompanyRole << EmailsRole << AddressesRole << NumbersRole;
}

void ContactModel::deleteContact(int index)
{
    delete contactList.takeAt(index);
    emit itemsRemoved(index, 1);
}

int ContactModel::insertContact(Contact *contact)
{
    int index = 0;
    QString fullName = contact->lastName();
    index = findIndex(fullName);
    contactList.insert(index, contact);
    emit itemsInserted(index, 1);
    return index;
}


//search - binary search algorithm lastname only

int ContactModel::findIndex(QString &searchName) const
{
    int start = 0;
    int end = contactList.size()-1;
    int middle = 0;
    QString middleString;

    while (start <= end)
    {
        middle = (start+end)/2;
        middleString = contactList.at(middle)->lastName();
        if (isAfter(searchName, middleString) < 0) start = middle+1;
        else if( isAfter(middleString, searchName) < 0) end = middle-1;
        else return middle;
    }
    return start;
}

int ContactModel::isAfter(QString &name1, QString &name2) const
{
    //if c1 is after c2 alphabetically, return positive
    int compString = QString::compare(name1, name2, Qt::CaseInsensitive);
    return -compString;
}
