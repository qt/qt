/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SYMBIANDEVICEMANAGER_H
#define SYMBIANDEVICEMANAGER_H

#include "symbianutils_global.h"

#include <QtCore/QObject>
#include <QtCore/QExplicitlySharedDataPointer>

QT_BEGIN_NAMESPACE
class QDebug;
class QTextStream;
QT_END_NAMESPACE

namespace SymbianUtils {

struct SymbianDeviceManagerPrivate;
class SymbianDeviceData;

enum DeviceCommunicationType {
    SerialPortCommunication = 0,
    BlueToothCommunication = 1
};

// SymbianDevice, explicitly shared.
class SYMBIANUTILS_EXPORT SymbianDevice {
    explicit SymbianDevice(SymbianDeviceData *data);
    friend class SymbianDeviceManager;
public:
    SymbianDevice();
    SymbianDevice(const SymbianDevice &rhs);
    SymbianDevice &operator=(const SymbianDevice &rhs);
    ~SymbianDevice();
    int compare(const SymbianDevice &rhs) const;

    DeviceCommunicationType type() const;
    bool isNull() const;
    QString portName() const;
    QString friendlyName() const;

    // Windows only.
    QString deviceDesc() const;
    QString manufacturer() const;

    void format(QTextStream &str) const;
    QString toString() const;

private:
    QExplicitlySharedDataPointer<SymbianDeviceData> m_data;
};

QDebug operator<<(QDebug d, const SymbianDevice &);

inline bool operator==(const SymbianDevice &d1, const SymbianDevice &d2)
    { return d1.compare(d2) == 0; }
inline bool operator!=(const SymbianDevice &d1, const SymbianDevice &d2)
    { return d1.compare(d2) != 0; }
inline bool operator<(const SymbianDevice &d1, const SymbianDevice &d2)
    { return d1.compare(d2) < 0; }

/* SymbianDeviceManager: Singleton that maintains a list of Symbian devices.
 * and emits change signals.
 * On Windows, the update slot must be connected to a signal
 * emitted from an event handler listening for WM_DEVICECHANGE. */
class SYMBIANUTILS_EXPORT SymbianDeviceManager : public QObject
{
    Q_OBJECT
public:
    typedef QList<SymbianDevice> SymbianDeviceList;

    static const char *linuxBlueToothDeviceRootC;

    // Do not use this constructor, it is just public for Q_GLOBAL_STATIC
    explicit SymbianDeviceManager(QObject *parent = 0);
    virtual ~SymbianDeviceManager();

    // Singleton access.
    static SymbianDeviceManager *instance();

    SymbianDeviceList devices() const;
    QString toString() const;

    QString friendlyNameForPort(const QString &port) const;

public slots:
    void update();

signals:
    void deviceRemoved(const SymbianDevice &d);
    void deviceAdded(const SymbianDevice &d);
    void updated();

private:
    void update(bool emitSignals);
    SymbianDeviceList serialPorts() const;
    SymbianDeviceList blueToothDevices() const;

    SymbianDeviceManagerPrivate *d;
};

QDebug operator<<(QDebug d, const SymbianDeviceManager &);

} // namespace SymbianUtils

#endif // SYMBIANDEVICEMANAGER_H
