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

#ifndef QGRAPHICSSYSTEM_QVFB_H
#define QGRAPHICSSYSTEM_QVFB_H

#include <QPlatformScreen>
#include <QPlatformIntegration>

QT_BEGIN_NAMESPACE


class QVFbScreenPrivate;

class QVFbScreen : public QPlatformScreen
{
public:
    QVFbScreen(int id);
    ~QVFbScreen();

    QRect geometry() const;
     int depth() const;
     QImage::Format format() const;
     QSize physicalSize() const;

    QImage *screenImage();

    void setDirty(const QRect &rect);

public:

    QVFbScreenPrivate *d_ptr;
};

class QVFbIntegrationPrivate;


class QVFbIntegration : public QPlatformIntegration
{
public:
    QVFbIntegration(const QStringList &paramList);

    QPixmapData *createPixmapData(QPixmapData::PixelType type) const;
    QPlatformWindow *createPlatformWindow(QWidget *widget, WId winId) const;
    QWindowSurface *createWindowSurface(QWidget *widget, WId winId) const;

    QList<QPlatformScreen *> screens() const { return mScreens; }

    QPlatformFontDatabase *fontDatabase() const;

private:
    QVFbScreen *mPrimaryScreen;
    QList<QPlatformScreen *> mScreens;
    QPlatformFontDatabase *mFontDb;
};



QT_END_NAMESPACE


#endif
