/*
  libconninet - Internet Connectivity support library

  Copyright (C) 2009-2010 Nokia Corporation. All rights reserved.

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

#include <stdlib.h>
#include <string.h>
#include <conn_settings.h>

#include "iapconf.h"

#define QSTRING_TO_CONST_CSTR(str) \
    str.toUtf8().constData()

namespace Maemo {

class IAPConfPrivate {
public:
    ConnSettings *settings;

    ConnSettingsValue *variantToValue(const QVariant &variant);
    QVariant valueToVariant(ConnSettingsValue *value);
};

ConnSettingsValue *IAPConfPrivate::variantToValue(const QVariant &variant)
{
    // Convert variant to ConnSettingsValue
    ConnSettingsValue *value = conn_settings_value_new();
    if (value == 0) {
        qWarning("IAPConf: Unable to create new ConnSettingsValue");
        return 0;
    }

    switch(variant.type()) {
        
    case QVariant::Invalid:
        value->type = CONN_SETTINGS_VALUE_INVALID;
        break;
    
    case QVariant::String: {
        char *valueStr = strdup(QSTRING_TO_CONST_CSTR(variant.toString()));
        value->type = CONN_SETTINGS_VALUE_STRING;
        value->value.string_val = valueStr;
        break;
    }
    
    case QVariant::Int:
        value->type = CONN_SETTINGS_VALUE_INT;
        value->value.int_val = variant.toInt();
        break;
    
    case QMetaType::Float:
    case QVariant::Double:
        value->type = CONN_SETTINGS_VALUE_DOUBLE;
        value->value.double_val = variant.toDouble();
        break;
    
    case QVariant::Bool:
        value->type = CONN_SETTINGS_VALUE_BOOL;
        value->value.bool_val = variant.toBool() ? 1 : 0;
        break;
    
    case QVariant::ByteArray: {
        QByteArray array = variant.toByteArray();
        value->type = CONN_SETTINGS_VALUE_BYTE_ARRAY;
        value->value.byte_array.len = array.size();
        value->value.byte_array.val = (unsigned char *)malloc(array.size());
        memcpy(value->value.byte_array.val, array.constData(), array.size());
        break;
    }
    
    case QVariant::List: {
        QVariantList list = variant.toList();
        ConnSettingsValue **list_val = (ConnSettingsValue **)malloc(
            (list.size() + 1) * sizeof(ConnSettingsValue *));

        for (int idx = 0; idx < list.size(); idx++) {
            list_val[idx] = variantToValue(list.at(idx));
        }
        list_val[list.size()] = 0;

        value->type = CONN_SETTINGS_VALUE_LIST;
        value->value.list_val = list_val;
        break;
    }
    
    default:
        qWarning("IAPConf: Can not handle QVariant of type %d",
                 variant.type());
        conn_settings_value_destroy(value);
        return 0;
    }

    return value;
}

QVariant IAPConfPrivate::valueToVariant(ConnSettingsValue *value)
{
    if (value == 0 || value->type == CONN_SETTINGS_VALUE_INVALID) {
        return QVariant();
    }

    switch(value->type) {
    
    case CONN_SETTINGS_VALUE_BOOL:
        return QVariant(value->value.bool_val ? true : false);
    
    case CONN_SETTINGS_VALUE_STRING:
        return QVariant(QString(value->value.string_val));
    
    case CONN_SETTINGS_VALUE_DOUBLE:
        return QVariant(value->value.double_val);
    
    case CONN_SETTINGS_VALUE_INT:
        return QVariant(value->value.int_val);
    
    case CONN_SETTINGS_VALUE_LIST: {
        // At least with GConf backend connsettings returns byte array as list
        // of ints, first check for that case
        if (value->value.list_val && value->value.list_val[0]) {
            bool canBeConvertedToByteArray = true;
            for (int idx = 0; value->value.list_val[idx]; idx++) {
                ConnSettingsValue *val = value->value.list_val[idx];
                if (val->type != CONN_SETTINGS_VALUE_INT
                     || val->value.int_val > 255
                     || val->value.int_val < 0) {
                    canBeConvertedToByteArray = false;
                    break;
                }
            }

            if (canBeConvertedToByteArray) {
                QByteArray array;
                for (int idx = 0; value->value.list_val[idx]; idx++) {
                    array.append(value->value.list_val[idx]->value.int_val);
                }
                return array;
            }

	    // Create normal list
	    QVariantList list;
	    for (int idx = 0; value->value.list_val[idx]; idx++) {
	      list.append(valueToVariant(value->value.list_val[idx]));
	    }
	    return list;
        }
    }
    
    case CONN_SETTINGS_VALUE_BYTE_ARRAY:
        return QByteArray::fromRawData((char *)value->value.byte_array.val, 
                                       value->value.byte_array.len);
    
    default:
        return QVariant();
    }
}

// Public class implementation

IAPConf::IAPConf(const QString &iap_id)
    : d_ptr(new IAPConfPrivate)
{
    d_ptr->settings = conn_settings_open(CONN_SETTINGS_CONNECTION,
                                         QSTRING_TO_CONST_CSTR(iap_id));
    if (d_ptr->settings == 0) {
        qWarning("IAPConf: Unable to open ConnSettings for %s", 
                 QSTRING_TO_CONST_CSTR(iap_id));
    }
}

IAPConf::~IAPConf()
{
    conn_settings_close(d_ptr->settings);
    delete d_ptr;
}

void IAPConf::setValue(const QString& key, const QVariant& value)
{
    // Invalid value means unsetting the given key
    if (!value.isValid()) {
        int err = conn_settings_unset(d_ptr->settings, 
                                      QSTRING_TO_CONST_CSTR(key));
        if (err != CONN_SETTINGS_E_NO_ERROR) {
            qWarning("IAPConf: unable to unset key %s: %s",
                     QSTRING_TO_CONST_CSTR(key),
                     conn_settings_error_text((ConnSettingsError)err));
        }
        return;
    }

    // Convert value to ConnSettingsValue
    ConnSettingsValue *val = d_ptr->variantToValue(value);
    if (val == 0) return;

    // Set value and handle errors
    int error = conn_settings_set(d_ptr->settings,
                                  QSTRING_TO_CONST_CSTR(key),
                                  val);
    if (error != CONN_SETTINGS_E_NO_ERROR) {
        qWarning("IAPConf: error in setting key %s: %s", 
                 QSTRING_TO_CONST_CSTR(key), 
                 conn_settings_error_text((ConnSettingsError)error));
    }

    // Destroy value
    conn_settings_value_destroy(val);
    return;
}

void IAPConf::set(const QString& key1, const QVariant& value1, 
                  const QString& key2, const QVariant& value2, 
                  const QString& key3, const QVariant& value3, 
                  const QString& key4, const QVariant& value4, 
                  const QString& key5, const QVariant& value5, 
                  const QString& key6, const QVariant& value6, 
                  const QString& key7, const QVariant& value7, 
                  const QString& key8, const QVariant& value8, 
                  const QString& key9, const QVariant& value9,
                  const QString& key10, const QVariant& value10)
{
    if (!key1.isEmpty()) setValue(key1, value1);
    if (!key2.isEmpty()) setValue(key2, value2);
    if (!key3.isEmpty()) setValue(key3, value3);
    if (!key4.isEmpty()) setValue(key4, value4);
    if (!key5.isEmpty()) setValue(key5, value5);
    if (!key6.isEmpty()) setValue(key6, value6);
    if (!key7.isEmpty()) setValue(key7, value7);
    if (!key8.isEmpty()) setValue(key8, value8);
    if (!key9.isEmpty()) setValue(key9, value9);
    if (!key10.isEmpty()) setValue(key10, value10);
}

QVariant IAPConf::value(const QString& key) const
{
    ConnSettingsValue *val = conn_settings_get(d_ptr->settings,
                                               QSTRING_TO_CONST_CSTR(key));

    QVariant variant = d_ptr->valueToVariant(val);
    conn_settings_value_destroy(val);
    return variant;
}

void IAPConf::clear(const char *default_path)
{
    Q_UNUSED(default_path); // default path is unused

    int error = conn_settings_remove(d_ptr->settings);
    if (error != CONN_SETTINGS_E_NO_ERROR) {
        qWarning("IAPConf: Error when removing IAP: %s",
                 conn_settings_error_text((ConnSettingsError)error));
    }
}

void IAPConf::clearAll()
{
    ConnSettings *settings = conn_settings_open(CONN_SETTINGS_CONNECTION,
                                                NULL);
    conn_settings_remove(settings);
    conn_settings_close(settings);
}


void IAPConf::getAll(QList<QString> &all_iaps, bool return_path)
{
    Q_UNUSED(return_path); // We don't use return path currently

    // Go through all available connections and add them to the list
    char **ids = conn_settings_list_ids(CONN_SETTINGS_CONNECTION);
    if (ids == 0) {
        // No ids found - nothing to do
        return;
    }

    for (int idx = 0; ids[idx]; idx++) {
        all_iaps.append(QString(ids[idx]));
        free(ids[idx]);
    }
    free(ids);
}


} // namespace Maemo
