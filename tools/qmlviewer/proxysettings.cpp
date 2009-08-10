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


#include <QIntValidator>
#include <QSettings>

#include "proxysettings.h"

ProxySettings::ProxySettings (QWidget * parent)
        : QDialog (parent), Ui::ProxySettings()
{
    setupUi (this);

    proxyServerEdit->setInputMask ("000.000.000.000;_");
    QIntValidator *validator = new QIntValidator (0, 9999, this);
    proxyPortEdit->setValidator (validator);

    QSettings settings;
    proxyCheckBox->setChecked (settings.value ("http_proxy/use", 0).toBool ());
    proxyServerEdit->insert (settings.value ("http_proxy/hostname", "").toString ());
    proxyPortEdit->insert (settings.value ("http_proxy/port", "80").toString ());
    usernameEdit->insert (settings.value ("http_proxy/username", "").toString ());
    passwordEdit->insert (settings.value ("http_proxy/password", "").toString ());
}

ProxySettings::~ProxySettings()
{
}

void ProxySettings::accept ()
{
    QSettings settings;

    settings.setValue ("http_proxy/use", proxyCheckBox->isChecked ());
    settings.setValue ("http_proxy/hostname", proxyServerEdit->text ());
    settings.setValue ("http_proxy/port", proxyPortEdit->text ());
    settings.setValue ("http_proxy/username", usernameEdit->text ());
    settings.setValue ("http_proxy/password", passwordEdit->text ());

    QDialog::accept ();
}

QNetworkProxy ProxySettings::httpProxy ()
{
    QSettings settings;
    QNetworkProxy proxy;

    bool proxyInUse = settings.value ("http_proxy/use", 0).toBool ();
    if (proxyInUse) {
        proxy.setType (QNetworkProxy::HttpProxy);
        proxy.setHostName (settings.value ("http_proxy/hostname", "").toString ());// "192.168.220.5"
        proxy.setPort (settings.value ("http_proxy/port", 80).toInt ());  // 8080
        proxy.setUser (settings.value ("http_proxy/username", "").toString ());
        proxy.setPassword (settings.value ("http_proxy/password", "").toString ());
        //QNetworkProxy::setApplicationProxy (proxy);
    }
    else {
        proxy.setType (QNetworkProxy::NoProxy);
    }
    return proxy;
}

bool ProxySettings::httpProxyInUse()
{
    QSettings settings;
    return settings.value ("http_proxy/use", 0).toBool ();
}
