/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the FOO module of the Qt Toolkit.
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


#include "qdbusunixfiledescriptor.h"
#include <QSharedData>

#ifdef Q_OS_UNIX
# include <private/qcore_unix_p.h>
#endif

class QDBusUnixFileDescriptorPrivate : public QSharedData {
public:
    QDBusUnixFileDescriptorPrivate() : fd(-1) { }
    QDBusUnixFileDescriptorPrivate(const QDBusUnixFileDescriptorPrivate &other)
        : QSharedData(other), fd(-1)
    {  }
    ~QDBusUnixFileDescriptorPrivate();

    QAtomicInt fd;
};

template<> inline
QExplicitlySharedDataPointer<QDBusUnixFileDescriptorPrivate>::~QExplicitlySharedDataPointer()
{ if (d && !d->ref.deref()) delete d; }

QDBusUnixFileDescriptor::QDBusUnixFileDescriptor()
    : d(0)
{
}

QDBusUnixFileDescriptor::QDBusUnixFileDescriptor(int fileDescriptor)
    : d(0)
{
    if (fileDescriptor != -1)
        setFileDescriptor(fileDescriptor);
}

QDBusUnixFileDescriptor::QDBusUnixFileDescriptor(const QDBusUnixFileDescriptor &other)
    : d(other.d)
{
}

QDBusUnixFileDescriptor &QDBusUnixFileDescriptor::operator=(const QDBusUnixFileDescriptor &other)
{
    if (this != &other)
        d.operator=(other.d);
    return *this;
}

QDBusUnixFileDescriptor::~QDBusUnixFileDescriptor()
{
}

bool QDBusUnixFileDescriptor::isValid() const
{
    return d ? d->fd != -1 : false;
}

bool QDBusUnixFileDescriptor::isShared() const
{
    return d && d->ref == 1;
}

int QDBusUnixFileDescriptor::fileDescriptor() const
{
    return d ? d->fd.operator int() : -1;
}

// actual implementation
#ifdef Q_OS_UNIX

bool QDBusUnixFileDescriptor::isSupported()
{
    return true;
}

void QDBusUnixFileDescriptor::setFileDescriptor(int fileDescriptor)
{
    if (fileDescriptor != -1)
        giveFileDescriptor(qt_safe_dup(fileDescriptor));
}

void QDBusUnixFileDescriptor::giveFileDescriptor(int fileDescriptor)
{
    // if we are the sole ref, d remains unchanged
    // if detaching happens, d->fd will be -1
    if (d)
        d.detach();
    else
        d = new QDBusUnixFileDescriptorPrivate;

    if (d->fd != -1)
        qt_safe_close(d->fd);

    if (fileDescriptor != -1)
        d->fd = fileDescriptor;
}

int QDBusUnixFileDescriptor::takeFileDescriptor()
{
    if (!d)
        return -1;

    return d->fd.fetchAndStoreRelaxed(-1);
}

QDBusUnixFileDescriptorPrivate::~QDBusUnixFileDescriptorPrivate()
{
    if (fd != -1)
        qt_safe_close(fd);
}

#else
bool QDBusUnixFileDescriptor::isSupported()
{
    return false;
}

void QDBusUnixFileDescriptor::setFileDescriptor(int)
{
}

void QDBusUnixFileDescriptor::giveFileDescriptor(int)
{
}

int QDBusUnixFileDescriptor::takeFileDescriptor()
{
    return -1;
}

QDBusUnixFileDescriptorPrivate::~QDBusUnixFileDescriptorPrivate()
{
}

#endif
