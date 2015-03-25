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

#include <QPainter>
#include <QTcpSocket>
#include <arpa/inet.h>
#include <QBitmap>
#include <QApplication>

#include <QDebug>

#include "qvnccursor.h"
#include "qvncserver.h"
#include "qvncintegration.h"

QT_BEGIN_NAMESPACE

QVNCCursor::QVNCCursor(QVNCServer * srvr, QVNCScreen *scr )
        :QPlatformSoftwareCursor(scr), useVncCursor(false), server(srvr)
{
}

void QVNCCursor::changeCursor(QCursor * widgetCursor, QWidget * widget)
{
    QPlatformSoftwareCursor::changeCursor(widgetCursor, widget);
    if (useVncCursor) {
        server->setDirtyCursor();
    } else {
        setDirty();
    }
}

void QVNCCursor::setCursorMode(bool vnc)
{
    if (vnc) {
        setDirty();
        server->setDirtyCursor();
    } else {
        server->setDirtyCursor();
    }
    useVncCursor = vnc;
}

QRect QVNCCursor::drawCursor(QPainter & painter)
{
    if (useVncCursor)
        return QRect();

    return QPlatformSoftwareCursor::drawCursor(painter);
}

void QVNCCursor::clearClientCursor()
{
    QTcpSocket *socket = server->clientSocket();
    if (!socket) {
        return;
    }
    // FramebufferUpdate header
    {
        const quint16 tmp[6] = { htons(0),
                                 htons(1),
                                 htons(0), htons(0),
                                 htons(0),
                                 htons(0) };
        socket->write((char*)tmp, sizeof(tmp));

        const quint32 encoding = htonl(-239);
        socket->write((char*)(&encoding), sizeof(encoding));
    }
}

void QVNCCursor::sendClientCursor()
{
    if (useVncCursor == false) {
        clearClientCursor();
        return;
    }
    QImage *image = graphic->image();
    if (image->isNull())
        return;
    QTcpSocket *socket = server->clientSocket();
    if (!socket) {
        return;
    }
    // FramebufferUpdate header
    {
        const quint16 tmp[6] = { htons(0),
                                 htons(1),
                                 htons(graphic->hotspot().x()), htons(graphic->hotspot().y()),
                                 htons(image->width()),
                                 htons(image->height()) };
        socket->write((char*)tmp, sizeof(tmp));

        const quint32 encoding = htonl(-239);
        socket->write((char*)(&encoding), sizeof(encoding));
    }

    // write pixels
    //Q_ASSERT(cursor->hasAlphaChannel());
    const QImage img = image->convertToFormat(QImage::Format_RGB32);
    const int n = server->clientBytesPerPixel() * img.width();
    char *buffer = new char[n];
    for (int i = 0; i < img.height(); ++i) {
        server->convertPixels(buffer, (const char*)img.scanLine(i), img.width());
        socket->write(buffer, n);
    }
    delete[] buffer;

    // write mask
    const QImage bitmap = image->createAlphaMask().convertToFormat(QImage::Format_Mono);
    Q_ASSERT(bitmap.depth() == 1);
    Q_ASSERT(bitmap.size() == img.size());
    const int width = (bitmap.width() + 7) / 8;
    for (int i = 0; i < bitmap.height(); ++i)
        socket->write((const char*)bitmap.scanLine(i), width);
}

QT_END_NAMESPACE
