/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <QtCore/qdebug.h>

#include <private/qt_x11_p.h>
#include <QtGui/qx11info_x11.h>
#include <private/qpixmapdata_p.h>
#include <private/qpixmap_x11_p.h>

#include <QtGui/qpaintdevice.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qwidget.h>

#include "qegl_p.h"
#include "qeglcontext_p.h"

QT_BEGIN_NAMESPACE


EGLNativeDisplayType QEgl::nativeDisplay()
{
    Display *xdpy = QX11Info::display();
    if (!xdpy) {
        qWarning("QEglContext::getDisplay(): X11 display is not open");
        return EGLNativeDisplayType(EGL_DEFAULT_DISPLAY);
    }
    return EGLNativeDisplayType(xdpy);
}

static int countBits(unsigned long mask)
{
    int count = 0;
    while (mask != 0) {
        if (mask & 1)
            ++count;
        mask >>= 1;
    }
    return count;
}

// Set the pixel format parameters from the visual in "xinfo".
void QEglProperties::setVisualFormat(const QX11Info *xinfo)
{
    if (!xinfo)
        return;
    Visual *visual = (Visual*)xinfo->visual();
    if (!visual)
        return;
    if (visual->c_class != TrueColor && visual->c_class != DirectColor)
        return;
    setValue(EGL_RED_SIZE, countBits(visual->red_mask));
    setValue(EGL_GREEN_SIZE, countBits(visual->green_mask));
    setValue(EGL_BLUE_SIZE, countBits(visual->blue_mask));

    EGLint alphaBits = 0;
#if !defined(QT_NO_XRENDER)
    XRenderPictFormat *format;
    format = XRenderFindVisualFormat(xinfo->display(), visual);
    if (format && (format->type == PictTypeDirect) && format->direct.alphaMask) {
        alphaBits = countBits(format->direct.alphaMask);
        qDebug("QEglProperties::setVisualFormat() - visual's alphaMask is %d", alphaBits);
    }
#endif
    setValue(EGL_ALPHA_SIZE, alphaBits);
}

extern const QX11Info *qt_x11Info(const QPaintDevice *pd);

// Set pixel format and other properties based on a paint device.
void QEglProperties::setPaintDeviceFormat(QPaintDevice *dev)
{
    if (!dev)
        return;
    if (dev->devType() == QInternal::Image)
        setPixelFormat(static_cast<QImage *>(dev)->format());
    else
        setVisualFormat(qt_x11Info(dev));
}


VisualID QEgl::getCompatibleVisualId(EGLConfig config)
{
    VisualID    visualId = 0;
    EGLint      eglValue = 0;

    EGLint configRedSize = 0;
    eglGetConfigAttrib(display(), config, EGL_RED_SIZE, &configRedSize);

    EGLint configGreenSize = 0;
    eglGetConfigAttrib(display(), config, EGL_GREEN_SIZE, &configGreenSize);

    EGLint configBlueSize = 0;
    eglGetConfigAttrib(display(), config, EGL_BLUE_SIZE, &configBlueSize);

    EGLint configAlphaSize = 0;
    eglGetConfigAttrib(display(), config, EGL_ALPHA_SIZE, &configAlphaSize);

    eglGetConfigAttrib(display(), config, EGL_BUFFER_SIZE, &eglValue);
    int configBitDepth = eglValue;

    // See if EGL provided a valid VisualID:
    eglGetConfigAttrib(display(), config, EGL_NATIVE_VISUAL_ID, &eglValue);
    visualId = (VisualID)eglValue;
    if (visualId) {
        // EGL has suggested a visual id, so get the rest of the visual info for that id:
        XVisualInfo visualInfoTemplate;
        memset(&visualInfoTemplate, 0, sizeof(XVisualInfo));
        visualInfoTemplate.visualid = visualId;

        XVisualInfo *chosenVisualInfo;
        int matchingCount = 0;
        chosenVisualInfo = XGetVisualInfo(X11->display, VisualIDMask, &visualInfoTemplate, &matchingCount);
        if (chosenVisualInfo) {
            if (configBitDepth == chosenVisualInfo->depth) {
#if !defined(QT_NO_XRENDER)
                // If we have XRender, actually check the visual supplied by EGL is ARGB
                if (configAlphaSize > 0) {
                    XRenderPictFormat *format;
                    format = XRenderFindVisualFormat(X11->display, chosenVisualInfo->visual);
                    if (!format || (format->type != PictTypeDirect) || (!format->direct.alphaMask)) {
                        qWarning("Warning: EGL suggested using X visual ID %d for config %d, but this is not ARGB",
                                 (int)visualId, (int)config);
                        visualId = 0;
                    }
                }
#endif
            } else {
                qWarning("Warning: EGL suggested using X visual ID %d (%d bpp) for config %d (%d bpp), but the depths do not match!",
                         (int)visualId, chosenVisualInfo->depth, (int)config, configBitDepth);
                visualId = 0;
            }
        }
        XFree(chosenVisualInfo);
    }

    if (visualId) {
#ifdef QT_DEBUG_X11_VISUAL_SELECTION
        if (configHasAlpha)
            qDebug("Using ARGB Visual ID %d provided by EGL for config %d", (int)visualId, (int)config);
        else
            qDebug("Using Opaque Visual ID %d provided by EGL for config %d", (int)visualId, (int)config);
#endif
        return visualId;
    }


    // If EGL didn't give us a valid visual ID, try XRender
#if !defined(QT_NO_XRENDER)
    if (!visualId) {
        XVisualInfo visualInfoTemplate;
        memset(&visualInfoTemplate, 0, sizeof(XVisualInfo));

        visualInfoTemplate.depth = configBitDepth;
        visualInfoTemplate.c_class = TrueColor;

        XVisualInfo *matchingVisuals;
        int matchingCount = 0;
        matchingVisuals = XGetVisualInfo(X11->display,
                                         VisualDepthMask|VisualClassMask,
                                         &visualInfoTemplate,
                                         &matchingCount);

        for (int i = 0; i < matchingCount; ++i) {
            XRenderPictFormat *format;
            format = XRenderFindVisualFormat(X11->display, matchingVisuals[i].visual);

            // Check the format for the visual matches the EGL config
            if ( (countBits(format->direct.redMask) == configRedSize) &&
                 (countBits(format->direct.greenMask) == configGreenSize) &&
                 (countBits(format->direct.blueMask) == configBlueSize) &&
                 (countBits(format->direct.alphaMask) == configAlphaSize) )
            {
                visualId = matchingVisuals[i].visualid;
                break;
            }
        }
        if (matchingVisuals)
            XFree(matchingVisuals);

    }
    if (visualId) {
# ifdef QT_DEBUG_X11_VISUAL_SELECTION
        if (configHasAlpha)
            qDebug("Using ARGB Visual ID %d provided by XRender for EGL config %d", (int)visualId, (int)config);
        else
            qDebug("Using Opaque Visual ID %d provided by XRender for EGL config %d", (int)visualId, (int)config);
# endif // QT_DEBUG_X11_VISUAL_SELECTION
        return visualId;
    }
#endif //!defined(QT_NO_XRENDER)


    // Finally, if XRender also failed to find a visual (or isn't present), try to
    // use XGetVisualInfo and only use the bit depth to match on:
    if (!visualId) {
        XVisualInfo visualInfoTemplate;
        memset(&visualInfoTemplate, 0, sizeof(XVisualInfo));

        visualInfoTemplate.depth = configBitDepth;

        XVisualInfo *matchingVisuals;
        int matchingCount = 0;
        matchingVisuals = XGetVisualInfo(X11->display,
                                         VisualDepthMask,
                                         &visualInfoTemplate,
                                         &matchingCount);
        if (matchingVisuals) {
            visualId = matchingVisuals[0].visualid;
            XFree(matchingVisuals);
        }
    }

    if (visualId) {
#ifdef QT_DEBUG_X11_VISUAL_SELECTION
        qDebug("Using Visual ID %d provided by XGetVisualInfo for EGL config %d", (int)visualId, (int)config);
#endif
        return visualId;
    }

    qWarning("Unable to find an X11 visual which matches EGL config %d", (int)config);
    return (VisualID)0;
}


QT_END_NAMESPACE
