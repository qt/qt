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

#ifndef QGRAPHICSSYSTEM_VNC_H
#define QGRAPHICSSYSTEM_VNC_H

#include "qvnccursor.h"
#include "../fb_base/fb_base.h"
#include <QPlatformIntegration>
#include "qgenericunixfontdatabase.h"

QT_BEGIN_NAMESPACE

class QVNCServer;
class QVNCDirtyMap;

class QVNCScreenPrivate;

class QVNCScreen : public QFbScreen
{
    Q_OBJECT
public:
    QVNCScreen(QRect screenSize, int screenId);

    int linestep() const { return image() ? image()->bytesPerLine() : 0; }
    uchar *base() const { return image() ? image()->bits() : 0; }
    QVNCDirtyMap *dirtyMap();

public:
    QVNCScreenPrivate *d_ptr;

private:
    QVNCServer *server;
    QRegion doRedraw();
    friend class QVNCIntegration;
};

class QVNCIntegrationPrivate;


class QVNCIntegration : public QPlatformIntegration
{
public:
    QVNCIntegration(const QStringList& paramList);

    bool hasCapability(QPlatformIntegration::Capability cap) const;
    QPixmapData *createPixmapData(QPixmapData::PixelType type) const;
    QPlatformWindow *createPlatformWindow(QWidget *widget, WId winId) const;
    QWindowSurface *createWindowSurface(QWidget *widget, WId winId) const;

    QPixmap grabWindow(WId window, int x, int y, int width, int height) const;

    QList<QPlatformScreen *> screens() const { return mScreens; }

    bool isVirtualDesktop() { return virtualDesktop; }
    void moveToScreen(QWidget *window, int screen);

    QPlatformFontDatabase *fontDatabase() const;

private:
    QVNCScreen *mPrimaryScreen;
    QList<QPlatformScreen *> mScreens;
    bool virtualDesktop;
    QPlatformFontDatabase *fontDb;
};



QT_END_NAMESPACE

#endif //QGRAPHICSSYSTEM_VNC_H

