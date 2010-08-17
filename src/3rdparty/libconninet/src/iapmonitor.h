/*
  libconninet - Internet Connectivity support library

  Copyright (C) 2009-2010 Nokia Corporation. All rights reserved.

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

#ifndef IAPMONITOR_H
#define IAPMONITOR_H

#include <QString>

namespace Maemo {

class IAPMonitorPrivate;
class IAPMonitor {
public:
    IAPMonitor();
    ~IAPMonitor();

protected:
    virtual void iapAdded(const QString &id);
    virtual void iapRemoved(const QString &id);

private:
    IAPMonitorPrivate *d_ptr;
    Q_DECLARE_PRIVATE(IAPMonitor);
};

} // namespace Maemo

#endif // IAPMONITOR_H

