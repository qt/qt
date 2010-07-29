/*
  libconninet - Internet Connectivity support library

  Copyright (C) 2009 Nokia Corporation. All rights reserved.

  Contact: Aapo Makela <aapo.makela@nokia.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  version 2.1 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef IAPCONF_H
#define IAPCONF_H

#include <QString>
#include <QVariant>

namespace Maemo {

class IAPConfPrivate;
class IAPConf {
public:
    IAPConf(const QString &iap_id);
    virtual ~IAPConf();

    /**
        Convenience method for setting multiple IAP values with one call.
    */
    void set(const QString& key1, const QVariant& value1, 
             const QString& key2 = "", const QVariant& value2 = QVariant(), 
             const QString& key3 = "", const QVariant& value3 = QVariant(), 
             const QString& key4 = "", const QVariant& value4 = QVariant(), 
             const QString& key5 = "", const QVariant& value5 = QVariant(), 
             const QString& key6 = "", const QVariant& value6 = QVariant(), 
             const QString& key7 = "", const QVariant& value7 = QVariant(), 
             const QString& key8 = "", const QVariant& value8 = QVariant(), 
             const QString& key9 = "", const QVariant& value9 = QVariant(),
             const QString& key10 = "", const QVariant& value10 = QVariant());

    /**
        Set one IAP value.
    */
    void setValue(const QString& key, const QVariant& value);

    /**
        Get one IAP value.
    */
    QVariant value(const QString& key) const;

    /**
        Clear this IAP from GConf
    */
    void clear(const char *default_path=0);

    /**
        Clear all IAP specific information from GConf (including all IAPs).
        DO NOT USE THIS FUNCTION IN ANYWHERE ELSE EXCEPT IN TEST CODE!
    */
    void clearAll();

    /**
        Return all the IAPs found in the system. If return_path is true,
	then do not strip the IAP path away.
    */
    static void getAll(QList<QString> &all_iaps, bool return_path=false);

private:
    IAPConfPrivate *d_ptr;
};

} // namespace Maemo

#endif
