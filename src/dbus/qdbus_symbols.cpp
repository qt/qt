/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtDBus module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qglobal.h>
#include <QtCore/qlibrary.h>
#include <QtCore/qmutex.h>
#include <private/qmutexpool_p.h>

#ifndef QT_NO_DBUS

QT_BEGIN_NAMESPACE

void *qdbus_resolve_me(const char *name);

#if !defined QT_LINKED_LIBDBUS

static QLibrary *qdbus_libdbus = 0;

void qdbus_unloadLibDBus()
{
    delete qdbus_libdbus;
    qdbus_libdbus = 0;
}

bool qdbus_loadLibDBus()
{
    static volatile bool triedToLoadLibrary = false;
#ifndef QT_NO_THREAD
    QMutexLocker locker(QMutexPool::globalInstanceGet((void *)&qdbus_resolve_me));
#endif
    QLibrary *&lib = qdbus_libdbus;
    if (triedToLoadLibrary)
        return lib && lib->isLoaded();

    lib = new QLibrary;
    triedToLoadLibrary = true;

    static int majorversions[] = { 3, 2, -1 };
    lib->unload();
    lib->setFileName(QLatin1String("dbus-1"));
    lib->setLoadHints(QLibrary::ImprovedSearchHeuristics);
    for (uint i = 0; i < sizeof(majorversions) / sizeof(majorversions[0]); ++i) {
        lib->setFileNameAndVersion(lib->fileName(), majorversions[i]);
        if (lib->load() && lib->resolve("dbus_connection_open_private"))
            return true;

        lib->unload();
    }

    delete lib;
    lib = 0;
    return false;
}

void *qdbus_resolve_conditionally(const char *name)
{
    if (qdbus_loadLibDBus())
        return qdbus_libdbus->resolve(name);
    return 0;
}

void *qdbus_resolve_me(const char *name)
{
    void *ptr = 0;
    if (!qdbus_loadLibDBus())
        qFatal("Cannot find libdbus-1 in your system to resolve symbol '%s'.", name);

    ptr = qdbus_libdbus->resolve(name);
    if (!ptr)
        qFatal("Cannot resolve '%s' in your libdbus-1.", name);

    return ptr;
}

Q_DESTRUCTOR_FUNCTION(qdbus_unloadLibDBus)

#endif // QT_LINKED_LIBDBUS

QT_END_NAMESPACE

#endif // QT_NO_DBUS
