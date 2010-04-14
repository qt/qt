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

#ifndef QT7MOVIERENDERER_H
#define QT7MOVIERENDERER_H

#include "qt7backend.h"

#include <QtCore/qobject.h>
#include <QtCore/qmutex.h>

#include <qvideorenderercontrol.h>
#include <qmediaplayer.h>

#include <QtGui/qmacdefines_mac.h>
#include "qt7videooutputcontrol.h"

#include <QuartzCore/CVOpenGLTexture.h>
#include <QuickTime/QuickTime.h>


QT_BEGIN_HEADER

class QGLContext;

QT_BEGIN_NAMESPACE

class QCvDisplayLink;
class QT7PlayerSession;
class QT7PlayerService;

class QT7MovieRenderer : public QT7VideoRendererControl
{
Q_OBJECT
public:
    QT7MovieRenderer(QObject *parent = 0);
    virtual ~QT7MovieRenderer();

    void setMovie(void *movie);
    void updateNaturalSize(const QSize &newSize);

    QAbstractVideoSurface *surface() const;
    void setSurface(QAbstractVideoSurface *surface);

    QSize nativeSize() const;

private slots:
    void updateVideoFrame(const CVTimeStamp &ts);
    
private:
    void setupVideoOutput();
    bool createPixelBufferVisualContext();
    bool createGLVisualContext();

    void *m_movie;

    QMutex m_mutex;

    QCvDisplayLink *m_displayLink;
#ifdef QUICKTIME_C_API_AVAILABLE
    QTVisualContextRef	m_visualContext;
    bool m_usingGLContext;
    const QGLContext *m_currentGLContext;
    QSize m_pixelBufferContextGeometry;
#endif
    QAbstractVideoSurface *m_surface;
    QSize m_nativeSize;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
