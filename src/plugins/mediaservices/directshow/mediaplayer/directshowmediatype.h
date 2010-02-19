/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef DIRECTSHOWMEDIATYPE_H
#define DIRECTSHOWMEDIATYPE_H

#include <QtMultimedia/qvideosurfaceformat.h>

#include <dshow.h>
#include <dvdmedia.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class DirectShowMediaType : public AM_MEDIA_TYPE
{
public:
    DirectShowMediaType() { memset(this, 0, sizeof(DirectShowMediaType)); }
    DirectShowMediaType(const AM_MEDIA_TYPE &type) { copy(this, type); }
    DirectShowMediaType(const DirectShowMediaType &other) { copy(this, other); }
    DirectShowMediaType &operator =(const AM_MEDIA_TYPE &type) {
        freeData(this); copy(this, type); return *this; }
    DirectShowMediaType &operator =(const DirectShowMediaType &other) {
        freeData(this); copy(this, other); return *this; }
    ~DirectShowMediaType() { freeData(this); }

    void clear() { freeData(this); memset(this, 0, sizeof(DirectShowMediaType)); }

    static void copy(AM_MEDIA_TYPE *target, const AM_MEDIA_TYPE &source);
    static void freeData(AM_MEDIA_TYPE *type);
    static void deleteType(AM_MEDIA_TYPE *type);

    static GUID convertPixelFormat(QVideoFrame::PixelFormat format);
    static QVideoSurfaceFormat formatFromType(const AM_MEDIA_TYPE &type);

    static int bytesPerLine(const QVideoSurfaceFormat &format);
};


QT_END_NAMESPACE

QT_END_HEADER

#endif
