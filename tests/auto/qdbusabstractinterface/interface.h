/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef INTERFACE_H
#define INTERFACE_H

#include <QtCore/QObject>
#include <QtDBus/QDBusArgument>

struct RegisteredType
{
    inline RegisteredType(const QString &str = QString()) : s(str) {}
    inline bool operator==(const RegisteredType &other) const { return s == other.s; }
    QString s;
};
Q_DECLARE_METATYPE(RegisteredType)

inline QDBusArgument &operator<<(QDBusArgument &s, const RegisteredType &data)
{
    s.beginStructure();
    s << data.s;
    s.endStructure();
    return s;
}

inline const QDBusArgument &operator>>(const QDBusArgument &s, RegisteredType &data)
{
    s.beginStructure();
    s >> data.s;
    s.endStructure();
    return s;
}

struct UnregisteredType
{
    QString s;
};
Q_DECLARE_METATYPE(UnregisteredType)

class Interface: public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.trolltech.QtDBus.Pinger")
    Q_PROPERTY(QString stringProp READ stringProp WRITE setStringProp SCRIPTABLE true)
    Q_PROPERTY(RegisteredType complexProp READ complexProp WRITE setComplexProp SCRIPTABLE true)

    friend class tst_QDBusAbstractInterface;
    QString m_stringProp;
    RegisteredType m_complexProp;

public:
    Interface();

    QString stringProp() const { return m_stringProp; }
    void setStringProp(const QString &s) { m_stringProp = s; }
    RegisteredType complexProp() const { return m_complexProp; }
    void setComplexProp(const RegisteredType &r) { m_complexProp = r; }

public slots:
    Q_SCRIPTABLE void voidMethod() {}
    Q_SCRIPTABLE QString stringMethod() { return "Hello, world"; }
    Q_SCRIPTABLE RegisteredType complexMethod() { return RegisteredType("Hello, world"); }
    Q_SCRIPTABLE QString multiOutMethod(int &value) { value = 42; return "Hello, world"; }

signals:
    Q_SCRIPTABLE void voidSignal();
    Q_SCRIPTABLE void stringSignal(const QString &);
    Q_SCRIPTABLE void complexSignal(RegisteredType);
};

#endif // INTERFACE_H
