/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef HYBRIDSURFACE_H
#define HYBRIDSURFACE_H

#include <private/qglwindowsurface_qws_p.h>
#include <private/qglpaintdevice_qws_p.h>
#include <GLES/egl.h>
#include <vanilla/eglVanilla.h>
#include <private/qwssharedmemory_p.h>

class HybridPaintDevice;
class HybridSurfacePrivate;
class QWSLock;

class HybridSurface : public QWSGLWindowSurface
{
public:
    HybridSurface();
    HybridSurface(QWidget *w, EGLDisplay display);
    ~HybridSurface();

    void beginPaint(const QRegion &region);
    bool lock(int timeout);
    void unlock();

    bool isValid() const;
    void setGeometry(const QRect &rect, const QRegion &mask);
    QString key() const { return QLatin1String("hybrid"); }

    QByteArray permanentState() const;
    void setPermanentState(const QByteArray &state);

    QImage image() const;
    QPaintDevice *paintDevice();
    QPoint painterOffset() const;

private:
    QWSSharedMemory mem;
    QImage img;
    QWSLock *memlock;
    EGLDisplay display;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;
    QWSGLPaintDevice *pdevice;

    VanillaPixmap vanillaPix;
};

#endif // HYBRIDSURFACE_H
