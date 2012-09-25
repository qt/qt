/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "virtualserialdevice.h"
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>

namespace SymbianUtils {

bool VirtualSerialDevice::isSequential() const
{
    return true;
}

VirtualSerialDevice::VirtualSerialDevice(const QString &aPortName, QObject *parent) :
    QIODevice(parent), portName(aPortName), lock(QMutex::NonRecursive), emittingBytesWritten(false), waiterForBytesWritten(NULL)
{
    platInit();
}

const QString& VirtualSerialDevice::getPortName() const
{
    return portName;
}

void VirtualSerialDevice::close()
{
    if (isOpen()) {
        QMutexLocker locker(&lock);
        delete waiterForBytesWritten;
        waiterForBytesWritten = NULL;
        QIODevice::close();
        platClose();
    }
}

void VirtualSerialDevice::emitBytesWrittenIfNeeded(QMutexLocker& locker, qint64 len)
{
    if (waiterForBytesWritten) {
        waiterForBytesWritten->wakeAll();
    }
    if (!emittingBytesWritten) {
        emittingBytesWritten = true;
        locker.unlock();
        emit bytesWritten(len);
        locker.relock();
        emittingBytesWritten = false;
    }
}

} // namespace SymbianUtils
