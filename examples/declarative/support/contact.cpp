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

#include "contact.h"
#include "qmltypes.h"

QML_DEFINE_TYPE(0,0,0,0,Contact,Contact);
Contact::Contact() : QObject(0)
{
    m_firstName = "John";
    m_lastName = "Smith";
    m_portraitFile = "contact.png";
    m_company = "Trollkia";
    m_emails << "smith@trollkia.com" << "john45@gmail.com";

    m_numbers << new PhoneNumber;
    m_numbers << new PhoneNumber;
    m_numbers << new PhoneNumber;
    
    m_numbers.at(0)->setType(PhoneNumber::HomePhone);
    m_numbers.at(0)->setNumber("35412451");

    m_numbers.at(1)->setType(PhoneNumber::BusinessPhone);
    m_numbers.at(1)->setNumber("33424994");
    
    m_numbers.at(2)->setType(PhoneNumber::MobilePhone);
    m_numbers.at(2)->setNumber("0424655137");

    m_addresses << new Address;
    m_addresses << new Address;
    m_addresses << new Address;
    m_addresses.at(0)->setNumber(13);
    m_addresses.at(0)->setStreet("Blackhill Cr");
    m_addresses.at(0)->setCountry("Australia");
    m_addresses.at(1)->setNumber(116);
    m_addresses.at(1)->setStreet("Sandankerveien");
    m_addresses.at(1)->setCountry("Norway");
    m_addresses.at(2)->setNumber(92);
    m_addresses.at(2)->setStreet("Elizibeth St");
    m_addresses.at(2)->setCountry("Australia");
}

void Contact::addNumber(PhoneNumber *newNumber)
{
    
    m_numbers << newNumber;    
    emit numbersChanged();
}

void Contact::addAddress(Address *newAddress)
{
    m_addresses << newAddress;
    emit addressesChanged();
}

void Contact::addEmail(QString &newEmail)
{

    m_emails << newEmail;
    emit emailsChanged();
}

QML_DEFINE_TYPE(0,0,0,0,Address,Address);
Address::Address()
: _number(0)
{
}

QML_DEFINE_TYPE(0,0,0,0,PhoneNumber, PhoneNumber);
PhoneNumber::PhoneNumber()
: _type(HomePhone)
{
}
