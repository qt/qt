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

#ifndef DIRECTSHOWVIDEORENDERERCONTROL_H
#define DIRECTSHOWVIDEORENDERERCONTROL_H

#include <QtMediaServices/qvideorenderercontrol.h>

#include <dshow.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class DirectShowEventLoop;
class VideoSurfaceFilter;

class DirectShowVideoRendererControl : public QVideoRendererControl
{
    Q_OBJECT
public:
    DirectShowVideoRendererControl(DirectShowEventLoop *loop, QObject *parent = 0);
    ~DirectShowVideoRendererControl();

    QAbstractVideoSurface *surface() const;
    void setSurface(QAbstractVideoSurface *surface);

    IBaseFilter *filter();

Q_SIGNALS:
    void filterChanged();

private:
    DirectShowEventLoop *m_loop;
    QAbstractVideoSurface *m_surface;
    VideoSurfaceFilter *m_filter;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
