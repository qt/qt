/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qxlibnativeinterface.h"

#include "qxlibdisplay.h"
#include <QtGui/private/qapplication_p.h>

class QXlibResourceMap : public QMap<QByteArray, QXlibNativeInterface::ResourceType>
{
public:
    QXlibResourceMap()
        :QMap<QByteArray, QXlibNativeInterface::ResourceType>()
    {
        insert("display",QXlibNativeInterface::Display);
        insert("egldisplay",QXlibNativeInterface::EglDisplay);
        insert("connection",QXlibNativeInterface::Connection);
        insert("screen",QXlibNativeInterface::Screen);
        insert("graphicsdevice",QXlibNativeInterface::GraphicsDevice);
        insert("eglcontext",QXlibNativeInterface::EglContext);
    }
};

Q_GLOBAL_STATIC(QXlibResourceMap, qXlibResourceMap)


void * QXlibNativeInterface::nativeResourceForWidget(const QByteArray &resourceString, QWidget *widget)
{
    QByteArray lowerCaseResource = resourceString.toLower();
    ResourceType resource = qXlibResourceMap()->value(lowerCaseResource);
    void *result = 0;
    switch(resource) {
    case Display:
        result = displayForWidget(widget);
        break;
    case EglDisplay:
        result = eglDisplayForWidget(widget);
        break;
    case Connection:
        result = connectionForWidget(widget);
        break;
    case Screen:
        result = reinterpret_cast<void *>(qPlatformScreenForWidget(widget)->xScreenNumber());
        break;
    case GraphicsDevice:
        result = graphicsDeviceForWidget(widget);
        break;
    case EglContext:
        result = eglContextForWidget(widget);
        break;
    default:
        result = 0;
    }
    return result;
}

void * QXlibNativeInterface::displayForWidget(QWidget *widget)
{
    return qPlatformScreenForWidget(widget)->display()->nativeDisplay();
}

void * QXlibNativeInterface::eglDisplayForWidget(QWidget *widget)
{
    Q_UNUSED(widget);
    return 0;
}

void * QXlibNativeInterface::screenForWidget(QWidget *widget)
{
    Q_UNUSED(widget);
    return 0;
}

void * QXlibNativeInterface::graphicsDeviceForWidget(QWidget *widget)
{
    Q_UNUSED(widget);
    return 0;
}

void * QXlibNativeInterface::eglContextForWidget(QWidget *widget)
{
    Q_UNUSED(widget);
    return 0;
}

QXlibScreen * QXlibNativeInterface::qPlatformScreenForWidget(QWidget *widget)
{
    QXlibScreen *screen;
    if (widget) {
        screen = static_cast<QXlibScreen *>(QPlatformScreen::platformScreenForWidget(widget));
    }else {
        screen = static_cast<QXlibScreen *>(QApplicationPrivate::platformIntegration()->screens()[0]);
    }
    return screen;
}
