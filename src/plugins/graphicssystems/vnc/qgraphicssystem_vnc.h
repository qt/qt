/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
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

#ifndef QGRAPHICSSYSTEM_VNC_H
#define QGRAPHICSSYSTEM_VNC_H

#include <QtGui/private/qgraphicssystem_p.h>

QT_BEGIN_NAMESPACE

class QVNCServer;
class QVNCDirtyMap;


class QVNCGraphicsSystemScreenPrivate;

class QVNCGraphicsSystemScreen : public QGraphicsSystemScreen
{
public:
    QVNCGraphicsSystemScreen();
    ~QVNCGraphicsSystemScreen();

    QRect geometry() const { return mGeometry; }
    int depth() const { return mDepth; }
    QImage::Format format() const { return mFormat; }
    QSize physicalSize() const { return mPhysicalSize; }
    int linestep() const { return mScreenImage ? mScreenImage->bytesPerLine() : 0; }
    uchar *base() const { return mScreenImage ? mScreenImage->bits() : 0; }
    QVNCDirtyMap *dirtyMap();

    void setDirty(const QRect &rect);

public:
    QRect mGeometry;
    int mDepth;
    QImage::Format mFormat;
    QSize mPhysicalSize;
    QImage *mScreenImage;
    QVNCServer *server;

    QVNCGraphicsSystemScreenPrivate *d_ptr;
};

class QVNCGraphicsSystemPrivate;


class QVNCGraphicsSystem : public QGraphicsSystem
{
public:
    QVNCGraphicsSystem();

    QPixmapData *createPixmapData(QPixmapData::PixelType type) const;
    QWindowSurface *createWindowSurface(QWidget *widget) const;

    QList<QGraphicsSystemScreen *> screens() const { return mScreens; }

private:
    QVNCGraphicsSystemScreen *mPrimaryScreen;
    QList<QGraphicsSystemScreen *> mScreens;
};



QT_END_NAMESPACE

#endif //QGRAPHICSSYSTEM_VNC_H

