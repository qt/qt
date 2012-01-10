/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui/QImageIOHandler>
#include <QtCore/QDebug>

#ifndef QT_NO_IMAGEFORMATPLUGIN

#ifdef QT_NO_IMAGEFORMAT_TGA
#undef QT_NO_IMAGEFORMAT_TGA
#endif

#include "qtgahandler.h"

QT_BEGIN_NAMESPACE

class QTgaPlugin : public QImageIOPlugin
{
public:
    Capabilities capabilities(QIODevice * device, const QByteArray & format) const;
    QImageIOHandler * create(QIODevice * device, const QByteArray & format = QByteArray()) const;
    QStringList keys() const;
};

QImageIOPlugin::Capabilities QTgaPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (format == "tga")
        return Capabilities(CanRead);
    if (!format.isEmpty())
        return 0;
    if (!device->isOpen())
        return 0;

    Capabilities cap;
    if (device->isReadable() && QTgaHandler::canRead(device))
        cap |= CanRead;
    return cap;
}

QImageIOHandler* QTgaPlugin::create(QIODevice *device, const QByteArray &format) const
{
    QImageIOHandler *tgaHandler = new QTgaHandler();
    tgaHandler->setDevice(device);
    tgaHandler->setFormat(format);
    return tgaHandler;
}

QStringList QTgaPlugin::keys() const
{
    return QStringList() << QLatin1String("tga");
}

Q_EXPORT_PLUGIN2(qtga, QTgaPlugin)

QT_END_NAMESPACE

#endif /* QT_NO_IMAGEFORMATPLUGIN */
