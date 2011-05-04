/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qsgcontextplugin_p.h"
#include <private/qsgcontext_p.h>
#include <QtGui/qapplication.h>
#include <QtCore/private/qfactoryloader_p.h>
#include <QtCore/qlibraryinfo.h>

QT_BEGIN_NAMESPACE

QSGContextPlugin::QSGContextPlugin(QObject *parent)
    : QObject(parent)
{
}

QSGContextPlugin::~QSGContextPlugin()
{
}

#if !defined (QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
    (QSGContextFactoryInterface_iid, QLatin1String("/scenegraph")))
#endif

/*!
    \fn QSGContext *QSGContext::createDefaultContext()

    Creates a default scene graph context for the current hardware.
    This may load a device-specific plugin.
*/
QSGContext *QSGContext::createDefaultContext()
{
    const QStringList args = QApplication::arguments();
    QString device;
    for (int index = 0; index < args.count(); ++index) {
        if (args.at(index).startsWith(QLatin1String("--device="))) {
            device = args.at(index).mid(9);
            break;
        }
    }
    if (device.isEmpty())
        device = QString::fromLocal8Bit(qgetenv("QMLSCENE_DEVICE"));
    if (device.isEmpty())
        return new QSGContext();

#if !defined (QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
    if (QSGContextFactoryInterface *factory
            = qobject_cast<QSGContextFactoryInterface*>
                (loader()->instance(device))) {
        QSGContext *context = factory->create(device);
        if (context)
            return context;
    }
#ifndef QT_NO_DEBUG
    qWarning("Could not create scene graph context for device '%s'"
             " - check that plugins are installed correctly in %s",
             qPrintable(device),
             qPrintable(QLibraryInfo::location(QLibraryInfo::PluginsPath)));
#endif
#endif // QT_NO_LIBRARY || QT_NO_SETTINGS

    return new QSGContext();
}

QT_END_NAMESPACE
