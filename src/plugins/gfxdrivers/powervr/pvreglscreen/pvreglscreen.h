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

#ifndef PVREGLSCREEN_H
#define PVREGLSCREEN_H

#include <QScreen>
#include <QGLScreen>
#include "pvrqwsdrawable.h"

class PvrEglScreen;

class PvrEglScreenSurfaceFunctions : public QGLScreenSurfaceFunctions
{
public:
    PvrEglScreenSurfaceFunctions(PvrEglScreen *s, int screenNum)
        : screen(s), displayId(screenNum) {}

    bool createNativeWindow(QWidget *widget, EGLNativeWindowType *native);

private:
    PvrEglScreen *screen;
    int displayId;
};

class PvrEglScreen : public QGLScreen
{
public:
    PvrEglScreen(int displayId);
    ~PvrEglScreen();

    bool initDevice();
    bool connect(const QString &displaySpec);
    void disconnect();
    void shutdownDevice();
    void setMode(int, int, int) {}

    void blit(const QImage &img, const QPoint &topLeft, const QRegion &region);
    void solidFill(const QColor &color, const QRegion &region);

    bool chooseContext(QGLContext *context, const QGLContext *shareContext);
    bool hasOpenGL();

    QWSWindowSurface* createSurface(QWidget *widget) const;
    QWSWindowSurface* createSurface(const QString &key) const;

    int transformation() const;

private:
    void sync();
    void openTty();
    void closeTty();

    int fd;
    int ttyfd, oldKdMode;
    QString ttyDevice;
    bool doGraphicsMode;
    mutable const QScreen *parent;
};

#endif
