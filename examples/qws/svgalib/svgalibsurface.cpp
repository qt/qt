/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "svgalibsurface.h"
#include "svgalibpaintdevice.h"

#include <vgagl.h>

SvgalibSurface::SvgalibSurface() : QWSWindowSurface(), pdevice(0)
{
    setSurfaceFlags(Opaque);
}

SvgalibSurface::SvgalibSurface(QWidget *w)
    : QWSWindowSurface(w)
{
    setSurfaceFlags(Opaque);
    pdevice = new SvgalibPaintDevice(w);
}

SvgalibSurface::~SvgalibSurface()
{
    delete pdevice;
}

void SvgalibSurface::setGeometry(const QRect &rect)
{
    QWSWindowSurface::setGeometry(rect);
}

QPoint SvgalibSurface::painterOffset() const
{
    return geometry().topLeft() + QWSWindowSurface::painterOffset();
}

//! [0]
bool SvgalibSurface::scroll(const QRegion &region, int dx, int dy)
{
    const QVector<QRect> rects = region.rects();
    for (int i = 0; i < rects.size(); ++i) {
        const QRect r = rects.at(i);
        gl_copybox(r.left(), r.top(), r.width(), r.height(),
                   r.left() + dx, r.top() + dy);
    }

    return true;
}
//! [0]

