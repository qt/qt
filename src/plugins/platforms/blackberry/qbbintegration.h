/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QBBINTEGRATION_H
#define QBBINTEGRATION_H

#include <QtGui/QPlatformIntegration>

#include <screen/screen.h>

QT_BEGIN_NAMESPACE

class QBBScreenEventThread;
class QBBNavigatorEventHandler;
class QBBNavigatorEventNotifier;
class QBBLocaleThread;
class QBBAbstractVirtualKeyboard;
class QBBScreen;
class QBBScreenEventHandler;
class QBBNativeInterface;
class QBBBpsEventFilter;
class QBBButtonEventNotifier;

class QBBIntegration : public QPlatformIntegration
{
public:
    QBBIntegration();
    virtual ~QBBIntegration();

    virtual bool hasCapability(QPlatformIntegration::Capability cap) const;

    virtual QPixmapData *createPixmapData(QPixmapData::PixelType type) const;
    virtual QPlatformWindow *createPlatformWindow(QWidget *widget, WId winId) const;
    virtual QWindowSurface *createWindowSurface(QWidget *widget, WId winId) const;
    virtual QPlatformNativeInterface *nativeInterface() const;

    virtual QList<QPlatformScreen *> screens() const;
    virtual void moveToScreen(QWidget *window, int screen);
    virtual void setCursorPos(int x, int y);

    virtual QPlatformFontDatabase *fontDatabase() const { return mFontDb; }

#ifndef QT_NO_CLIPBOARD
    virtual QPlatformClipboard *clipboard() const;
#endif

    bool paintUsingOpenGL() const { return mPaintUsingOpenGL; }

    QBBScreen *screenForWindow(screen_window_t window) const;

    QBBScreen *screenForNative(screen_display_t screen) const;

    void createDisplay(screen_display_t display, int screenIndex);
    void removeDisplay(QBBScreen *screen);
private:
    QBBScreen *primaryDisplay() const;
    void createDisplays();
    void destroyDisplays();

    screen_context_t mContext;
    QBBScreenEventThread *mScreenEventThread;
    QBBNavigatorEventHandler *mNavigatorEventHandler;
    QBBNavigatorEventNotifier *mNavigatorEventNotifier;
    QBBButtonEventNotifier *mButtonsNotifier;
    QBBLocaleThread *mLocaleThread;
    QPlatformFontDatabase *mFontDb;
    QList<QPlatformScreen*> mScreens;
    QBBScreenEventHandler *mScreenEventHandler;
    bool mPaintUsingOpenGL;
    QBBAbstractVirtualKeyboard *mVirtualKeyboard;
    QBBNativeInterface *mNativeInterface;
    QBBBpsEventFilter *mBpsEventFilter;
};

QT_END_NAMESPACE

#endif // QBBINTEGRATION_H
