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

#ifndef CONTACT_H
#define CONTACT_H

#include <qml.h>
#include <QtCore>

class Address : public QObject
{
    Q_OBJECT
public:
    Address();

    Q_PROPERTY(int number READ number WRITE setNumber NOTIFY changed);
    Q_PROPERTY(QString street READ street WRITE setStreet NOTIFY changed);
    Q_PROPERTY(QString country READ country WRITE setCountry NOTIFY changed);

    int number() const { return _number; }
    void setNumber(int n) { _number = n; emit changed(); }

    QString street() const { return _street; }
    void setStreet(const QString &s) { _street = s; emit changed(); }

    QString country() const { return _country; }
    void setCountry(const QString &c) { _country = c; emit changed(); }

signals:
    void changed();

private:
    int _number;
    QString _street;
    QString _country;
};
QML_DECLARE_TYPE(Address);

class PhoneNumber : public QObject
{
    Q_OBJECT
    Q_ENUMS(PhoneType)
public:
    PhoneNumber();

    enum PhoneType {
        HomePhone,
        BusinessPhone,
        MobilePhone
    };

    Q_PROPERTY(QString number READ number WRITE setNumber NOTIFY changed);
    Q_PROPERTY(PhoneType type READ type WRITE setType NOTIFY changed);

    QString number() const { return _number; }
    void setNumber(QString n) { _number = n; emit changed(); }

    PhoneType type() const { return _type; }
    void setType(PhoneType type) { _type = type; emit changed(); }

signals:
    void changed();

private:
    QString _number;
    PhoneType _type;
};
QML_DECLARE_TYPE(PhoneNumber);

class Contact : public QObject
{
    Q_OBJECT
public:
    Contact();

    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName NOTIFY nameChanged);
    QString firstName() const { return m_firstName; }

    Q_PROPERTY(QString lastName READ lastName WRITE setLastName NOTIFY nameChanged);
    QString lastName() const { return m_lastName; }

    Q_PROPERTY(QString portraitFile READ portraitFile WRITE setPortraitFile NOTIFY portraitChanged);
    QString portraitFile() const { return m_portraitFile; }

    Q_PROPERTY(QString company READ company WRITE setCompany NOTIFY companyChanged);
    QString company() const { return m_company; }

    Q_PROPERTY(QStringList emails READ emails WRITE setEmails NOTIFY emailsChanged);
    QStringList emails() const { return m_emails; }

    Q_PROPERTY(QList<Address *>* addresses READ addresses);
    QList<Address *>* addresses() { return &m_addresses; }

    Q_PROPERTY(QList<PhoneNumber *>* numbers READ numbers);
    QList<PhoneNumber *>* numbers() { return &m_numbers; }


    void addEmail(QString&);
    void addAddress(Address*);
    void addNumber(PhoneNumber*);

public slots:
    void setFirstName(const QString &name) { m_firstName = name; emit nameChanged(); }
    void setLastName(const QString &name) { m_lastName = name; emit nameChanged(); }
    void setPortraitFile(const QString &portraitFile) { m_portraitFile = portraitFile; emit portraitChanged(); }
    void setCompany(const QString &company) { m_company = company; emit companyChanged(); }
    void setEmails(const QStringList &emails) { m_emails = emails; emit emailsChanged(); }

signals:
    void nameChanged();
    void portraitChanged();
    void companyChanged();
    void emailsChanged();
    void numbersChanged();
    void addressesChanged();

private:
    QString m_firstName;
    QString m_lastName;
    QString m_portraitFile;

    QString m_company;

    QList<Address *> m_addresses;
    QList<PhoneNumber *>m_numbers;
    QStringList m_emails;
};
QML_DECLARE_TYPE(Contact);

#endif
