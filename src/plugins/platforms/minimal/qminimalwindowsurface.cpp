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


#include "qminimalwindowsurface.h"
#include <QtCore/qdebug.h>
#include <QtGui/private/qapplication_p.h>

QT_BEGIN_NAMESPACE

QMinimalWindowSurface::QMinimalWindowSurface(QWidget *window)
    : QWindowSurface(window)
{
    //qDebug() << "QMinimalWindowSurface::QMinimalWindowSurface:" << (long)this;
}

QMinimalWindowSurface::~QMinimalWindowSurface()
{
}

QPaintDevice *QMinimalWindowSurface::paintDevice()
{
    //qDebug() << "QMinimalWindowSurface::paintDevice";
    return &mImage;
}

void QMinimalWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(region);
    Q_UNUSED(offset);

    static int c = 0;
    QString filename = QString("output%1.png").arg(c++, 4, 10, QLatin1Char('0'));
    qDebug() << "QMinimalWindowSurface::flush() saving contents to" << filename.toLocal8Bit().constData();
    mImage.save(filename);
}

void QMinimalWindowSurface::resize(const QSize &size)
{
    //qDebug() << "QMinimalWindowSurface::setGeometry:" << (long)this << rect;
    QWindowSurface::resize(size);
    QImage::Format format = QApplicationPrivate::platformIntegration()->screens().first()->format();
    if (mImage.size() != size)
        mImage = QImage(size, format);
}

QT_END_NAMESPACE
