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

#ifndef PROXYSETTINGS_H
#define PROXYSETTINGS_H

#include <QDialog>
#include <QNetworkProxy>
#include "ui_proxysettings.h"

/**
*/
class ProxySettings : public QDialog, public Ui::ProxySettings
{

Q_OBJECT

public:
    ProxySettings(QWidget * parent = 0);

    ~ProxySettings();

    static QNetworkProxy httpProxy ();
    static bool httpProxyInUse ();

public slots:
    virtual void accept ();
};

#endif // PROXYSETTINGS_H
