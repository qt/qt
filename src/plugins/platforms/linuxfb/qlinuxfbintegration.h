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

#ifndef QGRAPHICSSYSTEM_LINUXFB_H
#define QGRAPHICSSYSTEM_LINUXFB_H

#include <QPlatformIntegration>
#include "../fb_base/fb_base.h"

QT_BEGIN_NAMESPACE

class QLinuxFbScreen : public QFbScreen
{
    Q_OBJECT
public:
    QLinuxFbScreen(uchar * d, int w, int h, int lstep, QImage::Format screenFormat);
    void setGeometry(QRect rect);
    void setFormat(QImage::Format format);

public slots:
    QRegion doRedraw();

private:
    QImage * mFbScreenImage;
    uchar * data;
    int bytesPerLine;

    QPainter *compositePainter;
};

class QLinuxFbIntegrationPrivate;
struct fb_cmap;
struct fb_var_screeninfo;
struct fb_fix_screeninfo;

class QLinuxFbIntegration : public QPlatformIntegration
{
public:
    QLinuxFbIntegration();
    ~QLinuxFbIntegration();

    bool hasCapability(QPlatformIntegration::Capability cap) const;

    QPixmapData *createPixmapData(QPixmapData::PixelType type) const;
    QPlatformWindow *createPlatformWindow(QWidget *widget, WId WinId) const;
    QWindowSurface *createWindowSurface(QWidget *widget, WId WinId) const;

    QList<QPlatformScreen *> screens() const { return mScreens; }

    QPlatformFontDatabase *fontDatabase() const;

private:
    QLinuxFbScreen *mPrimaryScreen;
    QList<QPlatformScreen *> mScreens;
    QLinuxFbIntegrationPrivate *d_ptr;

    enum PixelType { NormalPixel, BGRPixel };

    QRgb screenclut[256];
    int screencols;

    uchar * data;

    QImage::Format screenFormat;
    int w;
    int lstep;
    int h;
    int d;
    PixelType pixeltype;
    bool grayscale;

    int dw;
    int dh;

    int size;               // Screen size
    int mapsize;       // Total mapped memory

    int displayId;

    int physWidth;
    int physHeight;

    bool canaccel;
    int dataoffset;
    int cacheStart;

    bool connect(const QString &displaySpec);
    bool initDevice();
    void setPixelFormat(struct fb_var_screeninfo);
    void createPalette(fb_cmap &cmap, fb_var_screeninfo &vinfo, fb_fix_screeninfo &finfo);
    void blank(bool on);
    QPlatformFontDatabase *fontDb;
};

QT_END_NAMESPACE

#endif
