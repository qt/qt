/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

// #define QBBSCREEN_DEBUG

#include "qbbscreen.h"
#include "qbbvirtualkeyboard.h"
#include "qbbrootwindow.h"
#include "qbbwindow.h"

#include <QUuid>
#include <QDebug>

#include <errno.h>
#include <unistd.h>

QT_BEGIN_NAMESPACE

#define MAGIC_ZORDER_FOR_NO_NAV     10

QList<QPlatformScreen *> QBBScreen::sScreens;
QList<QBBWindow*> QBBScreen::sChildren;

QBBScreen::QBBScreen(screen_context_t context, screen_display_t display, bool primary)
    : mContext(context),
      mDisplay(display),
      mPosted(false),
      mUsingOpenGL(false),
      mPrimaryDisplay(primary)
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::QBBScreen";
#endif

    // cache initial orientation of this display
    // TODO: use ORIENTATION environment variable?
    errno = 0;
    int result = screen_get_display_property_iv(mDisplay, SCREEN_PROPERTY_ROTATION, &mStartRotation);
    if (result != 0) {
        qFatal("QBBScreen: failed to query display rotation, errno=%d", errno);
    }

    mCurrentRotation = mStartRotation;

    // cache size of this display in pixels
    // TODO: use WIDTH and HEIGHT environment variables?
    errno = 0;
    int val[2];
    result = screen_get_display_property_iv(mDisplay, SCREEN_PROPERTY_SIZE, val);
    if (result != 0) {
        qFatal("QBBScreen: failed to query display size, errno=%d", errno);
    }

    mCurrentGeometry = mStartGeometry = QRect(0, 0, val[0], val[1]);

    // cache size of this display in millimeters
    errno = 0;
    result = screen_get_display_property_iv(mDisplay, SCREEN_PROPERTY_PHYSICAL_SIZE, val);
    if (result != 0) {
        qFatal("QBBScreen: failed to query display physical size, errno=%d", errno);
    }

    // Peg the DPI to 96 (for now) so fonts are a reasonable size. We'll want to match
    // everything with a QStyle later, and at that point the physical size can be used
    // instead.
    {
        static const int dpi = 96;
        int width = mCurrentGeometry.width() / dpi * qreal(25.4) ;
        int height = mCurrentGeometry.height() / dpi * qreal(25.4) ;

        mCurrentPhysicalSize = mStartPhysicalSize = QSize(width,height);
    }

    // We only create the root window if we are not the primary display.
    if (mPrimaryDisplay)
        mRootWindow = QSharedPointer<QBBRootWindow>(new QBBRootWindow(this));
}

QBBScreen::~QBBScreen()
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::~QBBScreen";
#endif
}

/* static */
void QBBScreen::createDisplays(screen_context_t context)
{
    // get number of displays
    errno = 0;
    int displayCount;
    int result = screen_get_context_property_iv(context, SCREEN_PROPERTY_DISPLAY_COUNT, &displayCount);
    if (result != 0) {
        qFatal("QBBScreen: failed to query display count, errno=%d", errno);
    }

    // get all displays
    errno = 0;
    screen_display_t *displays = (screen_display_t *)alloca(sizeof(screen_display_t) * displayCount);
    result = screen_get_context_property_pv(context, SCREEN_PROPERTY_DISPLAYS, (void **)displays);
    if (result != 0) {
        qFatal("QBBScreen: failed to query displays, errno=%d", errno);
    }

    for (int i=0; i<displayCount; i++) {
#if defined(QBBSCREEN_DEBUG)
        qDebug() << "QBBScreen::Creating screen for display " << i;
#endif
        QBBScreen *screen = new QBBScreen(context, displays[i], i==0);
        sScreens.push_back(screen);
    }
}

/* static */
void QBBScreen::destroyDisplays()
{
    while (sScreens.length()) {
        delete sScreens.front();
        sScreens.pop_front();
    }

    // We're not managing the child windows anymore so we need to clear the list.
    sChildren.clear();
}

/* static */
int QBBScreen::defaultDepth()
{
    static int defaultDepth = 0;
    if (defaultDepth == 0) {
        // check if display depth was specified in environment variable;
        // use default value if no valid value found
        defaultDepth = qgetenv("QBB_DISPLAY_DEPTH").toInt();
        if (defaultDepth != 16 && defaultDepth != 32) {
            defaultDepth = 32;
        }
    }
    return defaultDepth;
}

void QBBScreen::ensureDisplayCreated()
{
    if (!mRootWindow)
        mRootWindow = QSharedPointer<QBBRootWindow>(new QBBRootWindow(this));
}

QRect QBBScreen::availableGeometry() const
{
    // available geometry = total geometry - keyboard
    int keyboardHeight = QBBVirtualKeyboard::instance().getHeight();
    return QRect(mCurrentGeometry.x(), mCurrentGeometry.y(),
                 mCurrentGeometry.width(), mCurrentGeometry.height() - keyboardHeight);
}

/*!
    Check if the supplied angles are perpendicular to each other.
*/
static bool isOrthogonal(int angle1, int angle2)
{
    return ((angle1 - angle2) % 180) != 0;
}

void QBBScreen::setRotation(int rotation)
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::setRotation, o=" << rotation;
#endif

    // check if rotation changed
    if (mCurrentRotation != rotation) {
        // update rotation of root window
        if (mRootWindow)
            mRootWindow->setRotation(rotation);

        // swap dimensions if we've rotated 90 or 270 from initial orientation
        if (isOrthogonal(mStartRotation, rotation)) {
            mCurrentGeometry = QRect(0, 0, mStartGeometry.height(), mStartGeometry.width());
            mCurrentPhysicalSize = QSize(mStartPhysicalSize.height(), mStartPhysicalSize.width());
        } else {
            mCurrentGeometry = QRect(0, 0, mStartGeometry.width(), mStartGeometry.height());
            mCurrentPhysicalSize = mStartPhysicalSize;
        }

        // resize app window if we've rotated 90 or 270 from previous orientation
        if (isOrthogonal(mCurrentRotation, rotation)) {

#if defined(QBBSCREEN_DEBUG)
            qDebug() << "QBBScreen::setRotation - resize, s=" << mCurrentGeometry.size();
#endif
            if (mRootWindow)
                mRootWindow->resize(mCurrentGeometry.size());
        } else {
            // TODO: find one global place to flush display updates
#if defined(QBBSCREEN_DEBUG)
            qDebug() << "QBBScreen::setRotation - flush";
#endif
            // force immediate display update if no geometry changes required
            if (mRootWindow)
                mRootWindow->flush();
        }

        // save new rotation
        mCurrentRotation = rotation;
    }
}

void QBBScreen::addWindow(QBBWindow* window)
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::addWindow=" << window;
#endif

    if (sChildren.contains(window))
        return;

    sChildren.push_back(window);
    QBBScreen::updateHierarchy();
}

void QBBScreen::removeWindow(QBBWindow* window)
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::removeWindow=" << window;
#endif

    sChildren.removeAll(window);
    QBBScreen::updateHierarchy();
}

void QBBScreen::raiseWindow(QBBWindow* window)
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::raise window=" << window;
#endif

    removeWindow(window);
    sChildren.push_back(window);
    QBBScreen::updateHierarchy();
}

void QBBScreen::lowerWindow(QBBWindow* window)
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::lower window=" << window;
#endif

    removeWindow(window);
    sChildren.push_front(window);
    QBBScreen::updateHierarchy();
}

void QBBScreen::updateHierarchy()
{
#if defined(QBBSCREEN_DEBUG)
    qDebug() << "QBBScreen::updateHierarchy";
#endif

    QList<QBBWindow*>::iterator it;
    QList<QPlatformScreen *>::iterator sit;
    QMap<QPlatformScreen *, int> map;
    int topZorder = 1; // root window is z-order 0, all "top" level windows are "above" it

    for (sit = sScreens.begin(); sit != sScreens.end(); sit++)
        map[*sit] = 0;

    for (it = sChildren.begin(); it != sChildren.end(); it++) {
        (*it)->updateZorder(topZorder);
        map[static_cast<QBBScreen*>((*it)->screen())] = 1;
    }

    // After a hierarchy update, we need to force a flush on all screens.
    // Right now, all screens share a context.
    screen_flush_context( primaryDisplay()->mContext, 0 );
}

void QBBScreen::onWindowPost(QBBWindow* window)
{
    Q_UNUSED(window)

    // post app window (so navigator will show it) after first child window
    // has posted; this only needs to happen once as the app window's content
    // never changes
    if (!mPosted && mRootWindow) {
        mRootWindow->post();
        mPosted = true;
    }
}

QT_END_NAMESPACE
