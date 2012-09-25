/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "eglnullwswindowsurface.h"
#include "eglnullwsscreenplugin.h"

#include <QGLWidget>

static const QWSWindowSurface::SurfaceFlags Flags
    = QWSWindowSurface::RegionReserved | QWSWindowSurface::RegionReserved;

EGLNullWSWindowSurface::EGLNullWSWindowSurface(QWidget *w)
    :
    QWSGLWindowSurface(w),
    widget(w)
{
    setSurfaceFlags(Flags);
}

EGLNullWSWindowSurface::EGLNullWSWindowSurface()
    : widget(0)
{
    setSurfaceFlags(Flags);
}

EGLNullWSWindowSurface::~EGLNullWSWindowSurface() {}

QString EGLNullWSWindowSurface::key() const
{
    return QLatin1String(PluginName);
}

QPaintDevice *EGLNullWSWindowSurface::paintDevice()
{
    return widget;
}

bool EGLNullWSWindowSurface::isValid() const
{
    return qobject_cast<QGLWidget *>(window());
}

QImage EGLNullWSWindowSurface::image() const
{
    return QImage();
}
