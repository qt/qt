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

#ifndef _CONTACTMODEL_H_
#define _CONTACTMODEL_H_

#include <qlistmodelinterface.h>
#include "contact.h"

class ContactModel : public QListModelInterface
{
    Q_OBJECT
public:
    ContactModel(QObject *parent = 0);
    ~ContactModel();

    enum Roles {
        PortraitRole,
        FirstNameRole,
        LastNameRole,
        CompanyRole,
        EmailsRole,
        AddressesRole,
        NumbersRole
    };

    int count() const;

    QHash<int,QVariant> data(int index, const QList<int> &roles) const;
    QList<int> roles() const;


    QString toString(int role) const;

    void deleteContact(int index);
    int insertContact(Contact *contact); 

    int isAfter(QString &name1, QString &name2) const;
    int findIndex(QString &searchName) const;

private:
    QList<Contact*> contactList;
};

#endif
