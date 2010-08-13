/*
  libconninet - Internet Connectivity support library

  Copyright (C) 2010 Nokia Corporation. All rights reserved.

  Contact: Jukka Rissanen <jukka.rissanen@nokia.com>

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

#ifndef PROXYCONF_H
#define PROXYCONF_H

#include <QString>
#include <QNetworkProxy>

namespace Maemo {

class ProxyConfPrivate;
class ProxyConf {
private:
    ProxyConfPrivate *d_ptr;

public:
    ProxyConf();
    virtual ~ProxyConf();

    QList<QNetworkProxy> flush(const QNetworkProxyQuery &query = QNetworkProxyQuery());  // read the proxies from db

    /* Note that for each update() call there should be corresponding
     * clear() call because the ProxyConf class implements a reference
     * counting mechanism. The factory is removed only when there is
     * no one using the factory any more.
     */
    static void update(void);          // this builds QNetworkProxy factory
    static void clear(void);           // this removes QNetworkProxy factory
};

} // namespace Maemo

#endif
