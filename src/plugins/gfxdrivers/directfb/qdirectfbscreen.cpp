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
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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

#include "qdirectfbscreen.h"
#include "qdirectfbwindowsurface.h"
#include "qdirectfbpixmap.h"
#include "qdirectfbmouse.h"
#include "qdirectfbkeyboard.h"
#include <QtGui/qwsdisplay_qws.h>
#include <QtGui/qcolor.h>
#include <QtGui/qapplication.h>
#include <QtGui/qwindowsystem_qws.h>
#include <QtGui/private/qgraphicssystem_qws_p.h>
#include <QtGui/private/qwssignalhandler_p.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qvector.h>
#include <QtCore/qrect.h>

class QDirectFBScreenPrivate : public QObject, public QWSGraphicsSystem
{
public:
    QDirectFBScreenPrivate(QDirectFBScreen*);
    ~QDirectFBScreenPrivate();

    void setFlipFlags(const QStringList &args);
    QPixmapData *createPixmapData(QPixmapData::PixelType type) const;

    IDirectFB *dfb;
    IDirectFBSurface *dfbSurface;
    DFBSurfaceFlipFlags flipFlags;
#ifndef QT_NO_DIRECTFB_LAYER
    IDirectFBDisplayLayer *dfbLayer;
#endif
    IDirectFBScreen *dfbScreen;

    QSet<IDirectFBSurface*> allocatedSurfaces;

#ifndef QT_NO_DIRECTFB_MOUSE
    QDirectFBMouseHandler *mouse;
#endif
#ifndef QT_NO_DIRECTFB_KEYBOARD
    QDirectFBKeyboardHandler *keyboard;
#endif
    QDirectFBScreen::DirectFBFlags directFBFlags;
    QImage::Format alphaPixmapFormat;
    QColor backgroundColor;
};

QDirectFBScreenPrivate::QDirectFBScreenPrivate(QDirectFBScreen *screen)
    : QWSGraphicsSystem(screen), dfb(0), dfbSurface(0), flipFlags(DSFLIP_NONE)
#ifndef QT_NO_DIRECTFB_LAYER
    , dfbLayer(0)
#endif
    , dfbScreen(0)
#ifndef QT_NO_DIRECTFB_MOUSE
    , mouse(0)
#endif
#ifndef QT_NO_DIRECTFB_KEYBOARD
    , keyboard(0)
#endif
    , directFBFlags(QDirectFBScreen::NoFlags)
    , alphaPixmapFormat(QImage::Format_Invalid)
{
#ifndef QT_NO_QWS_SIGNALHANDLER
    QWSSignalHandler::instance()->addObject(this);
#endif
}

QDirectFBScreenPrivate::~QDirectFBScreenPrivate()
{
#ifndef QT_NO_DIRECTFB_MOUSE
    delete mouse;
#endif
#ifndef QT_NO_DIRECTFB_KEYBOARD
    delete keyboard;
#endif

    for (QSet<IDirectFBSurface*>::const_iterator it = allocatedSurfaces.begin(); it != allocatedSurfaces.end(); ++it) {
        (*it)->Release(*it);
    }

    if (dfbSurface)
        dfbSurface->Release(dfbSurface);

#ifndef QT_NO_DIRECTFB_LAYER
    if (dfbLayer)
        dfbLayer->Release(dfbLayer);
#endif

    if (dfbScreen)
        dfbScreen->Release(dfbScreen);

    if (dfb)
        dfb->Release(dfb);
}



// creates a preallocated surface with the same format as the image if
// possible.

IDirectFBSurface *QDirectFBScreen::createDFBSurface(const QImage &img, SurfaceCreationOptions options)
{
    if (img.isNull()) // assert?
        return 0;

    if (QDirectFBScreen::getSurfacePixelFormat(img.format()) == DSPF_UNKNOWN) {
        QImage image = img.convertToFormat(img.hasAlphaChannel()
                                           ? d_ptr->alphaPixmapFormat
                                           : pixelFormat());
        IDirectFBSurface *tmp = createDFBSurface(image, false);
        if (!tmp) {
            qWarning("Couldn't create surface createDFBSurface(QImage, bool)");
            return 0;
        }
        IDirectFBSurface *surface = copyDFBSurface(tmp, image.format(), options);
        tmp->Release(tmp);
        return surface;
    }

    IDirectFBSurface *surface = createDFBSurface(QDirectFBScreen::getSurfaceDescription(img), options);
#ifdef QT_NO_DIRECTFB_PREALLOCATED
    if (surface) {
        int bpl;
        uchar *mem = QDirectFBScreen::lockSurface(surface, DSLF_WRITE, &bpl);
        if (mem) {
            const int h = img.height();
            const int w = img.width() * img.depth() / 8;
            for (int i = 0; i < h; ++i) {
                memcpy(mem, img.scanLine(i), w);
                mem += bpl;
            }
            surface->Unlock(surface);
        }
    }
#endif
#ifndef QT_NO_DIRECTFB_PALETTE
    if (img.numColors() != 0 && surface)
        QDirectFBScreen::setSurfaceColorTable(surface, img);
#endif
    return surface;
}

IDirectFBSurface *QDirectFBScreen::copyDFBSurface(IDirectFBSurface *src,
                                                  QImage::Format format,
                                                  SurfaceCreationOptions options)
{
    Q_ASSERT(src);
    QSize size;
    src->GetSize(src, &size.rwidth(), &size.rheight());
    IDirectFBSurface *surface = createDFBSurface(size, format, options);
    DFBSurfacePixelFormat dspf;
    src->GetPixelFormat(src, &dspf);
    DFBSurfaceBlittingFlags flags = QDirectFBScreen::hasAlpha(dspf)
                                    ? DSBLIT_BLEND_ALPHACHANNEL
                                    : DSBLIT_NOFX;
    if (flags & DSBLIT_BLEND_ALPHACHANNEL)
        surface->Clear(surface, 0, 0, 0, 0);

    surface->SetBlittingFlags(surface, flags);
    surface->Blit(surface, src, 0, 0, 0);
#if (Q_DIRECTFB_VERSION >= 0x010000)
    surface->ReleaseSource(surface);
#endif
    return surface;
}

IDirectFBSurface *QDirectFBScreen::createDFBSurface(const QSize &size,
                                                    QImage::Format format,
                                                    SurfaceCreationOptions options)
{
    DFBSurfaceDescription desc;
    memset(&desc, 0, sizeof(DFBSurfaceDescription));
    desc.flags |= DSDESC_WIDTH|DSDESC_HEIGHT;
    if (!QDirectFBScreen::initSurfaceDescriptionPixelFormat(&desc, format))
        return 0;
    desc.width = size.width();
    desc.height = size.height();
    return createDFBSurface(desc, options);
}

IDirectFBSurface *QDirectFBScreen::createDFBSurface(DFBSurfaceDescription desc, SurfaceCreationOptions options)
{
    DFBResult result = DFB_OK;
    IDirectFBSurface *newSurface = 0;

    if (!d_ptr->dfb) {
        qWarning("QDirectFBScreen::createDFBSurface() - not connected");
        return 0;
    }

    if (d_ptr->directFBFlags & VideoOnly
        && !(desc.flags & DSDESC_PREALLOCATED)
        && (!(desc.flags & DSDESC_CAPS) || !(desc.caps & DSCAPS_SYSTEMONLY))) {
        // Add the video only capability. This means the surface will be created in video ram
        if (!(desc.flags & DSDESC_CAPS)) {
            desc.caps = DSCAPS_VIDEOONLY;
            desc.flags |= DSDESC_CAPS;
        } else {
            desc.caps |= DSCAPS_VIDEOONLY;
        }
        result = d_ptr->dfb->CreateSurface(d_ptr->dfb, &desc, &newSurface);
        if (result != DFB_OK
#ifdef QT_NO_DEBUG
            && (desc.flags & DSDESC_CAPS) && (desc.caps & DSCAPS_PRIMARY)
#endif
            ) {
            qWarning("QDirectFBScreen::createDFBSurface() Failed to create surface in video memory!\n"
                     "   Flags %0x Caps %0x width %d height %d pixelformat %0x %d preallocated %p %d\n%s",
                     desc.flags, desc.caps, desc.width, desc.height,
                     desc.pixelformat, DFB_PIXELFORMAT_INDEX(desc.pixelformat),
                     desc.preallocated[0].data, desc.preallocated[0].pitch,
                     DirectFBErrorString(result));
        }
        desc.caps &= ~DSCAPS_VIDEOONLY;
    }

    if (d_ptr->directFBFlags & SystemOnly)
        desc.caps |= DSCAPS_SYSTEMONLY;

    if (!newSurface)
        result = d_ptr->dfb->CreateSurface(d_ptr->dfb, &desc, &newSurface);

    if (result != DFB_OK) {
        qWarning("QDirectFBScreen::createDFBSurface() Failed!\n"
                 "   Flags %0x Caps %0x width %d height %d pixelformat %0x %d preallocated %p %d\n%s",
                 desc.flags, desc.caps, desc.width, desc.height,
                 desc.pixelformat, DFB_PIXELFORMAT_INDEX(desc.pixelformat),
                 desc.preallocated[0].data, desc.preallocated[0].pitch,
                 DirectFBErrorString(result));
        return 0;
    }

    Q_ASSERT(newSurface);

    if (options & TrackSurface) {
        d_ptr->allocatedSurfaces.insert(newSurface);

        //qDebug("Created a new DirectFB surface at %p. New count = %d",
        //        newSurface, d_ptr->allocatedSurfaces.count());
    }

    return newSurface;
}

IDirectFBSurface *QDirectFBScreen::copyToDFBSurface(const QImage &img,
                                                    QImage::Format pixmapFormat,
                                                    SurfaceCreationOptions options)
{
    QImage image = img;
    if (QDirectFBScreen::getSurfacePixelFormat(image.format()) == DSPF_UNKNOWN
#ifdef QT_NO_DIRECTFB_PREALLOCATED
        || image.format() != pixmapFormat
#endif
#ifdef QT_NO_DIRECTFB_PALETTE
        || image.numColors() != 0
#endif
        ) {
        image = image.convertToFormat(pixmapFormat);
    }

    IDirectFBSurface *dfbSurface = createDFBSurface(image.size(), pixmapFormat, options);
    if (!dfbSurface) {
        qWarning("QDirectFBScreen::copyToDFBSurface() Couldn't create surface");
        return 0;
    }

#ifndef QT_NO_DIRECTFB_PREALLOCATED
    IDirectFBSurface *imgSurface = createDFBSurface(image, DontTrackSurface);
    if (!imgSurface) {
        qWarning("QDirectFBScreen::copyToDFBSurface()");
        QDirectFBScreen::releaseDFBSurface(dfbSurface);
        return 0;
    }

    Q_ASSERT(imgSurface);
    DFBSurfaceBlittingFlags flags = img.hasAlphaChannel()
                                    ? DSBLIT_BLEND_ALPHACHANNEL
                                    : DSBLIT_NOFX;
    if (flags & DSBLIT_BLEND_ALPHACHANNEL)
        dfbSurface->Clear(dfbSurface, 0, 0, 0, 0);

    dfbSurface->SetBlittingFlags(dfbSurface, flags);
    DFBResult result = dfbSurface->Blit(dfbSurface, imgSurface, 0, 0, 0);
    if (result != DFB_OK)
        DirectFBError("QDirectFBScreen::copyToDFBSurface()", result);
    imgSurface->Release(imgSurface);
#if (Q_DIRECTFB_VERSION >= 0x010000)
    dfbSurface->ReleaseSource(dfbSurface);
#endif
#else // QT_NO_DIRECTFB_PREALLOCATED
    Q_ASSERT(image.format() == pixmapFormat);
    int bpl;
    uchar *mem = QDirectFBScreen::lockSurface(dfbSurface, DSLF_WRITE, &bpl);
    if (mem) {
        const int h = image.height();
        const int w = image.width() * image.depth() / 8;
        for (int i=0; i<h; ++i) {
            memcpy(mem, image.scanLine(i), w);
            mem += bpl;
        }
        dfbSurface->Unlock(dfbSurface);
    }
#endif
    return dfbSurface;
}

void QDirectFBScreen::releaseDFBSurface(IDirectFBSurface *surface)
{
    Q_ASSERT(QDirectFBScreen::instance());
    Q_ASSERT(surface);
    surface->Release(surface);
    if (!d_ptr->allocatedSurfaces.remove(surface))
        qWarning("QDirectFBScreen::releaseDFBSurface() - %p not in list", surface);

    //qDebug("Released surface at %p. New count = %d", surface, d_ptr->allocatedSurfaces.count());
}

QDirectFBScreen::DirectFBFlags QDirectFBScreen::directFBFlags() const
{
    return d_ptr->directFBFlags;
}

IDirectFB *QDirectFBScreen::dfb()
{
    return d_ptr->dfb;
}

IDirectFBSurface *QDirectFBScreen::dfbSurface()
{
    return d_ptr->dfbSurface;
}

#ifndef QT_NO_DIRECTFB_LAYER
IDirectFBDisplayLayer *QDirectFBScreen::dfbDisplayLayer()
{
    return d_ptr->dfbLayer;
}
#endif

DFBSurfacePixelFormat QDirectFBScreen::getSurfacePixelFormat(QImage::Format format)
{
    switch (format) {
#ifndef QT_NO_DIRECTFB_PALETTE
    case QImage::Format_Indexed8:
        return DSPF_LUT8;
#endif
    case QImage::Format_RGB888:
        return DSPF_RGB24;
    case QImage::Format_ARGB4444_Premultiplied:
        return DSPF_ARGB4444;
#if (Q_DIRECTFB_VERSION >= 0x010100)
    case QImage::Format_RGB444:
        return DSPF_RGB444;
    case QImage::Format_RGB555:
        return DSPF_RGB555;
#endif
    case QImage::Format_RGB16:
        return DSPF_RGB16;
#if (Q_DIRECTFB_VERSION >= 0x010000)
    case QImage::Format_ARGB6666_Premultiplied:
        return DSPF_ARGB6666;
    case QImage::Format_RGB666:
        return DSPF_RGB18;
#endif
    case QImage::Format_RGB32:
        return DSPF_RGB32;
    case QImage::Format_ARGB32_Premultiplied:
    case QImage::Format_ARGB32:
        return DSPF_ARGB;
    default:
        return DSPF_UNKNOWN;
    };
}

QImage::Format QDirectFBScreen::getImageFormat(IDirectFBSurface *surface)
{
    DFBSurfacePixelFormat format;
    surface->GetPixelFormat(surface, &format);

    switch (format) {
    case DSPF_LUT8:
        return QImage::Format_Indexed8;
    case DSPF_RGB24:
        return QImage::Format_RGB888;
    case DSPF_ARGB4444:
        return QImage::Format_ARGB4444_Premultiplied;
#if (Q_DIRECTFB_VERSION >= 0x010100)
    case DSPF_RGB444:
        return QImage::Format_RGB444;
    case DSPF_RGB555:
#endif
    case DSPF_ARGB1555:
        return QImage::Format_RGB555;
    case DSPF_RGB16:
        return QImage::Format_RGB16;
#if (Q_DIRECTFB_VERSION >= 0x010000)
    case DSPF_ARGB6666:
        return QImage::Format_ARGB6666_Premultiplied;
    case DSPF_RGB18:
        return QImage::Format_RGB666;
#endif
    case DSPF_RGB32:
        return QImage::Format_RGB32;
    case DSPF_ARGB: {
        DFBSurfaceCapabilities caps;
        const DFBResult result = surface->GetCapabilities(surface, &caps);
        Q_ASSERT(result == DFB_OK);
        Q_UNUSED(result);
        return (caps & DSCAPS_PREMULTIPLIED
                ? QImage::Format_ARGB32_Premultiplied
                : QImage::Format_ARGB32); }
    default:
        break;
    }
    return QImage::Format_Invalid;
}

DFBSurfaceDescription QDirectFBScreen::getSurfaceDescription(const QImage &image)
{
    DFBSurfaceDescription description;
    memset(&description, 0, sizeof(DFBSurfaceDescription));

    const DFBSurfacePixelFormat format = getSurfacePixelFormat(image.format());

    if (format == DSPF_UNKNOWN || image.isNull()) {
        description.flags = DSDESC_NONE;
        return description;
    }

    description.flags = DSDESC_WIDTH|DSDESC_HEIGHT|DSDESC_PIXELFORMAT;
    QDirectFBScreen::initSurfaceDescriptionPixelFormat(&description, image.format());
    description.width = image.width();
    description.height = image.height();
#ifndef QT_NO_DIRECTFB_PREALLOCATED
    description.flags |= DSDESC_PREALLOCATED;
    description.preallocated[0].data = (void*)(image.bits());
    description.preallocated[0].pitch = image.bytesPerLine();
    description.preallocated[1].data = 0;
    description.preallocated[1].pitch = 0;
#endif

    if (QDirectFBScreen::isPremultiplied(image.format()))
        description.caps = DSCAPS_PREMULTIPLIED;

    return description;
}

DFBSurfaceDescription QDirectFBScreen::getSurfaceDescription(const uint *buffer,
                                                             int length)
{
    DFBSurfaceDescription description;
    memset(&description, 0, sizeof(DFBSurfaceDescription));

    description.flags = DSDESC_CAPS|DSDESC_WIDTH|DSDESC_HEIGHT|DSDESC_PIXELFORMAT|DSDESC_PREALLOCATED;
    description.caps = DSCAPS_PREMULTIPLIED;
    description.width = length;
    description.height = 1;
    description.pixelformat = DSPF_ARGB;
    description.preallocated[0].data = (void*)buffer;
    description.preallocated[0].pitch = length * sizeof(uint);
    description.preallocated[1].data = 0;
    description.preallocated[1].pitch = 0;
return description;
}

#ifndef QT_NO_DIRECTFB_PALETTE
void QDirectFBScreen::setSurfaceColorTable(IDirectFBSurface *surface,
                                           const QImage &image)
{
    if (!surface)
        return;

    const int numColors = image.numColors();
    if (numColors == 0)
        return;

    QVarLengthArray<DFBColor, 256> colors(numColors);
    for (int i = 0; i < numColors; ++i) {
        QRgb c = image.color(i);
        colors[i].a = qAlpha(c);
        colors[i].r = qRed(c);
        colors[i].g = qGreen(c);
        colors[i].b = qBlue(c);
    }

    IDirectFBPalette *palette;
    DFBResult result;
    result = surface->GetPalette(surface, &palette);
    if (result != DFB_OK) {
        DirectFBError("QDirectFBScreen::setSurfaceColorTable GetPalette",
                      result);
        return;
    }
    result = palette->SetEntries(palette, colors.data(), numColors, 0);
    if (result != DFB_OK) {
        DirectFBError("QDirectFBScreen::setSurfaceColorTable SetEntries",
                      result);
    }
    palette->Release(palette);
}

#endif // QT_NO_DIRECTFB_PALETTE

#if !defined(QT_NO_DIRECTFB_LAYER) && !defined(QT_NO_QWS_CURSOR)
class Q_GUI_EXPORT QDirectFBScreenCursor : public QScreenCursor
{
public:
    QDirectFBScreenCursor();
    virtual void set(const QImage &image, int hotx, int hoty);
    virtual void move(int x, int y);
    virtual void show();
    virtual void hide();
private:
    IDirectFBDisplayLayer *layer;
};

QDirectFBScreenCursor::QDirectFBScreenCursor()
{
    IDirectFB *fb = QDirectFBScreen::instance()->dfb();
    if (!fb)
        qFatal("QDirectFBScreenCursor: DirectFB not initialized");

    layer = QDirectFBScreen::instance()->dfbDisplayLayer();
    Q_ASSERT(layer);

    enable = false;
    hwaccel = true;
    supportsAlpha = true;
}

void QDirectFBScreenCursor::move(int x, int y)
{
    layer->WarpCursor(layer, x, y);
}

void QDirectFBScreenCursor::hide()
{
    if (enable) {
        enable = false;
        DFBResult result;
        result = layer->SetCooperativeLevel(layer, DLSCL_ADMINISTRATIVE);
        if (result != DFB_OK) {
            DirectFBError("QDirectFBScreenCursor::hide: "
                          "Unable to set cooperative level", result);
        }
        result = layer->SetCursorOpacity(layer, 0);
        if (result != DFB_OK) {
            DirectFBError("QDirectFBScreenCursor::hide: "
                          "Unable to set cursor opacity", result);
        }
        result = layer->SetCooperativeLevel(layer, DLSCL_SHARED);
        if (result != DFB_OK) {
            DirectFBError("QDirectFBScreenCursor::hide: "
                          "Unable to set cooperative level", result);
        }
    }
}

void QDirectFBScreenCursor::show()
{
    if (!enable) {
        enable = true;
        DFBResult result;
        result = layer->SetCooperativeLevel(layer, DLSCL_ADMINISTRATIVE);
        if (result != DFB_OK) {
            DirectFBError("QDirectFBScreenCursor::show: "
                          "Unable to set cooperative level", result);
        }
        result = layer->SetCursorOpacity(layer, 255);
        if (result != DFB_OK) {
            DirectFBError("QDirectFBScreenCursor::show: "
                          "Unable to set cursor shape", result);
        }
        result = layer->SetCooperativeLevel(layer, DLSCL_SHARED);
        if (result != DFB_OK) {
            DirectFBError("QDirectFBScreenCursor::show: "
                          "Unable to set cooperative level", result);
        }
    }
}

void QDirectFBScreenCursor::set(const QImage &image, int hotx, int hoty)
{
    QDirectFBScreen *screen = QDirectFBScreen::instance();
    if (!screen)
        return;

    if (image.isNull()) {
        cursor = QImage();
        hide();
    } else {
        cursor = image.convertToFormat(screen->alphaPixmapFormat());
        size = cursor.size();
        hotspot = QPoint(hotx, hoty);
        IDirectFBSurface *surface = screen->createDFBSurface(cursor, QDirectFBScreen::DontTrackSurface);
        if (!surface) {
            qWarning("QDirectFBScreenCursor::set: Unable to create surface");
            return;
        }
        DFBResult result = layer->SetCooperativeLevel(layer, DLSCL_ADMINISTRATIVE);
        if (result != DFB_OK) {
            DirectFBError("QDirectFBScreenCursor::show: "
                          "Unable to set cooperative level", result);
        }
        result = layer->SetCursorShape(layer, surface, hotx, hoty);
        if (result != DFB_OK) {
            DirectFBError("QDirectFBScreenCursor::show: "
                          "Unable to set cursor shape", result);
        }
        surface->Release(surface);
        result = layer->SetCooperativeLevel(layer, DLSCL_SHARED);
        if (result != DFB_OK) {
            DirectFBError("QDirectFBScreenCursor::show: "
                          "Unable to set cooperative level", result);
        }
        show();
    }

}
#endif // QT_NO_DIRECTFB_LAYER

QDirectFBScreen::QDirectFBScreen(int display_id)
    : QScreen(display_id, DirectFBClass), d_ptr(new QDirectFBScreenPrivate(this))
{
}

QDirectFBScreen::~QDirectFBScreen()
{
    delete d_ptr;
}

int QDirectFBScreen::depth(DFBSurfacePixelFormat format)
{
    switch (format) {
    case DSPF_A1:
        return 1;
    case DSPF_A8:
    case DSPF_RGB332:
    case DSPF_LUT8:
    case DSPF_ALUT44:
        return 8;
    case DSPF_I420:
    case DSPF_YV12:
    case DSPF_NV12:
    case DSPF_NV21:
#if (Q_DIRECTFB_VERSION >= 0x010100)
    case DSPF_RGB444:
#endif
        return 12;
#if (Q_DIRECTFB_VERSION >= 0x010100)
    case DSPF_RGB555:
        return 15;
#endif
    case DSPF_ARGB1555:
    case DSPF_RGB16:
    case DSPF_YUY2:
    case DSPF_UYVY:
    case DSPF_NV16:
    case DSPF_ARGB2554:
    case DSPF_ARGB4444:
        return 16;
    case DSPF_RGB24:
        return 24;
    case DSPF_RGB32:
    case DSPF_ARGB:
    case DSPF_AiRGB:
        return 32;
    case DSPF_UNKNOWN:
    default:
        return 0;
    };
    return 0;
}

void QDirectFBScreenPrivate::setFlipFlags(const QStringList &args)
{
    QRegExp flipRegexp(QLatin1String("^flip=([\\w,]*)$"));
    int index = args.indexOf(flipRegexp);
    if (index >= 0) {
        const QStringList flips = flipRegexp.cap(1).split(QLatin1Char(','),
                                                          QString::SkipEmptyParts);
        flipFlags = DSFLIP_NONE;
        foreach(const QString &flip, flips) {
            if (flip == QLatin1String("wait"))
                flipFlags |= DSFLIP_WAIT;
            else if (flip == QLatin1String("blit"))
                flipFlags |= DSFLIP_BLIT;
            else if (flip == QLatin1String("onsync"))
                flipFlags |= DSFLIP_ONSYNC;
            else if (flip == QLatin1String("pipeline"))
                flipFlags |= DSFLIP_PIPELINE;
            else
                qWarning("QDirectFBScreen: Unknown flip argument: %s",
                         qPrintable(flip));
        }
    } else {
        flipFlags = DSFLIP_BLIT;
    }
}

QPixmapData *QDirectFBScreenPrivate::createPixmapData(QPixmapData::PixelType type) const
{
    if (type == QPixmapData::BitmapType)
        return QWSGraphicsSystem::createPixmapData(type);

    return new QDirectFBPixmapData(type);
}

#ifdef QT_NO_DEBUG
struct FlagDescription;
static const FlagDescription *accelerationDescriptions = 0;
static const FlagDescription *blitDescriptions = 0;
static const FlagDescription *drawDescriptions = 0;
#else
struct FlagDescription {
    const char *name;
    uint flag;
};

static const FlagDescription accelerationDescriptions[] = {
    { " DFXL_NONE ", DFXL_NONE },
    { " DFXL_FILLRECTANGLE", DFXL_FILLRECTANGLE },
    { " DFXL_DRAWRECTANGLE", DFXL_DRAWRECTANGLE },
    { " DFXL_DRAWLINE", DFXL_DRAWLINE },
    { " DFXL_FILLTRIANGLE", DFXL_FILLTRIANGLE },
    { " DFXL_BLIT", DFXL_BLIT },
    { " DFXL_STRETCHBLIT", DFXL_STRETCHBLIT },
    { " DFXL_TEXTRIANGLES", DFXL_TEXTRIANGLES },
    { " DFXL_DRAWSTRING", DFXL_DRAWSTRING },
    { 0, 0 }
};

static const FlagDescription blitDescriptions[] = {
    { " DSBLIT_NOFX", DSBLIT_NOFX },
    { " DSBLIT_BLEND_ALPHACHANNEL", DSBLIT_BLEND_ALPHACHANNEL },
    { " DSBLIT_BLEND_COLORALPHA", DSBLIT_BLEND_COLORALPHA },
    { " DSBLIT_COLORIZE", DSBLIT_COLORIZE },
    { " DSBLIT_SRC_COLORKEY", DSBLIT_SRC_COLORKEY },
    { " DSBLIT_DST_COLORKEY", DSBLIT_DST_COLORKEY },
    { " DSBLIT_SRC_PREMULTIPLY", DSBLIT_SRC_PREMULTIPLY },
    { " DSBLIT_DST_PREMULTIPLY", DSBLIT_DST_PREMULTIPLY },
    { " DSBLIT_DEMULTIPLY", DSBLIT_DEMULTIPLY },
    { " DSBLIT_DEINTERLACE", DSBLIT_DEINTERLACE },
#if (Q_DIRECTFB_VERSION >= 0x000923)
    { " DSBLIT_SRC_PREMULTCOLOR", DSBLIT_SRC_PREMULTCOLOR },
    { " DSBLIT_XOR", DSBLIT_XOR },
#endif
#if (Q_DIRECTFB_VERSION >= 0x010000)
    { " DSBLIT_INDEX_TRANSLATION", DSBLIT_INDEX_TRANSLATION },
#endif
    { 0, 0 }
};

static const FlagDescription drawDescriptions[] = {
    { " DSDRAW_NOFX", DSDRAW_NOFX },
    { " DSDRAW_BLEND", DSDRAW_BLEND },
    { " DSDRAW_DST_COLORKEY", DSDRAW_DST_COLORKEY },
    { " DSDRAW_SRC_PREMULTIPLY", DSDRAW_SRC_PREMULTIPLY },
    { " DSDRAW_DST_PREMULTIPLY", DSDRAW_DST_PREMULTIPLY },
    { " DSDRAW_DEMULTIPLY", DSDRAW_DEMULTIPLY },
    { " DSDRAW_XOR", DSDRAW_XOR },
    { 0, 0 }
};
#endif



#if (Q_DIRECTFB_VERSION >= 0x000923)
static const QByteArray flagDescriptions(uint mask, const FlagDescription *flags)
{
#ifdef QT_NO_DEBUG
    Q_UNUSED(mask);
    Q_UNUSED(flags);
    return QByteArray("");
#else
    if (!mask)
        return flags[0].name;

    QStringList list;
    for (int i=1; flags[i].name; ++i) {
        if (mask & flags[i].flag) {
            list.append(QString::fromLatin1(flags[i].name));
        }
    }
    Q_ASSERT(!list.isEmpty());
    return (QLatin1Char(' ') + list.join(QLatin1String("|"))).toLatin1();
#endif
}
static void printDirectFBInfo(IDirectFB *fb, IDirectFBSurface *primarySurface)
{
    DFBResult result;
    DFBGraphicsDeviceDescription dev;

    result = fb->GetDeviceDescription(fb, &dev);
    if (result != DFB_OK) {
        DirectFBError("Error reading graphics device description", result);
        return;
    }

    DFBSurfacePixelFormat pixelFormat;
    primarySurface->GetPixelFormat(primarySurface, &pixelFormat);

    qDebug("Device: %s (%s), Driver: %s v%i.%i (%s) Pixelformat: %d (%d)\n"
           "acceleration: 0x%x%s\nblit: 0x%x%s\ndraw: 0x%0x%s\nvideo: %iKB\n",
           dev.name, dev.vendor, dev.driver.name, dev.driver.major,
           dev.driver.minor, dev.driver.vendor, DFB_PIXELFORMAT_INDEX(pixelFormat),
           QDirectFBScreen::getImageFormat(primarySurface), dev.acceleration_mask,
           ::flagDescriptions(dev.acceleration_mask, accelerationDescriptions).constData(),
           dev.blitting_flags, ::flagDescriptions(dev.blitting_flags, blitDescriptions).constData(),
           dev.drawing_flags, ::flagDescriptions(dev.drawing_flags, drawDescriptions).constData(),
           (dev.video_memory >> 10));
}
#endif

static inline bool setIntOption(const QStringList &arguments, const QString &variable, int *value)
{
    Q_ASSERT(value);
    QRegExp rx(QString("%1=?(\\d+)").arg(variable));
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    if (arguments.indexOf(rx) != -1) {
        *value = rx.cap(1).toInt();
        return true;
    }
    return false;
}

bool QDirectFBScreen::connect(const QString &displaySpec)
{
    DFBResult result = DFB_OK;

    {   // pass command line arguments to DirectFB
        const QStringList args = QCoreApplication::arguments();
        int argc = args.size();
        char **argv = new char*[argc];

        for (int i = 0; i < argc; ++i)
            argv[i] = qstrdup(args.at(i).toLocal8Bit().constData());

        result = DirectFBInit(&argc, &argv);
        if (result != DFB_OK) {
            DirectFBError("QDirectFBScreen: error initializing DirectFB",
                          result);
        }
        delete[] argv;
    }

    const QStringList displayArgs = displaySpec.split(QLatin1Char(':'),
                                                      QString::SkipEmptyParts);

    d_ptr->setFlipFlags(displayArgs);

    result = DirectFBCreate(&d_ptr->dfb);
    if (result != DFB_OK) {
        DirectFBError("QDirectFBScreen: error creating DirectFB interface",
                      result);
        return false;
    }

    if (displayArgs.contains(QLatin1String("videoonly"), Qt::CaseInsensitive))
        d_ptr->directFBFlags |= VideoOnly;

    if (displayArgs.contains(QLatin1String("systemonly"), Qt::CaseInsensitive)) {
        if (d_ptr->directFBFlags & VideoOnly) {
            qWarning("QDirectFBScreen: error. videoonly and systemonly are mutually exclusive");
        } else {
            d_ptr->directFBFlags |= SystemOnly;
        }
    }

    if (displayArgs.contains(QLatin1String("boundingrectflip"), Qt::CaseInsensitive)) {
        d_ptr->directFBFlags |= BoundingRectFlip;
    }

#ifdef QT_DIRECTFB_IMAGECACHE
    int imageCacheSize = 4 * 1024 * 1024; // 4 MB
    ::setIntOption(displayArgs, QLatin1String("imagecachesize"), &imageCacheSize);
    QDirectFBPaintEngine::initImageCache(imageCacheSize);
#endif

    if (displayArgs.contains(QLatin1String("ignoresystemclip"), Qt::CaseInsensitive))
        d_ptr->directFBFlags |= IgnoreSystemClip;

#ifndef QT_NO_DIRECTFB_WM
    if (displayArgs.contains(QLatin1String("fullscreen")))
#endif
        d_ptr->dfb->SetCooperativeLevel(d_ptr->dfb, DFSCL_FULLSCREEN);

    DFBSurfaceDescription description;
    memset(&description, 0, sizeof(DFBSurfaceDescription));

    description.flags = DSDESC_CAPS;
    if (::setIntOption(displayArgs, QLatin1String("width"), &description.width))
        description.flags |= DSDESC_WIDTH;
    if (::setIntOption(displayArgs, QLatin1String("height"), &description.height))
        description.flags |= DSDESC_HEIGHT;

    description.caps = DSCAPS_PRIMARY|DSCAPS_DOUBLE;
    struct {
        const char *name;
        const DFBSurfaceCapabilities cap;
    } const capabilities[] = {
        { "static_alloc", DSCAPS_STATIC_ALLOC },
        { "triplebuffer", DSCAPS_TRIPLE },
        { "interlaced", DSCAPS_INTERLACED },
        { "separated", DSCAPS_SEPARATED },
//        { "depthbuffer", DSCAPS_DEPTH }, // only makes sense with TextureTriangles which are not supported
        { 0, DSCAPS_NONE }
    };
    for (int i=0; capabilities[i].name; ++i) {
        if (displayArgs.contains(QString::fromLatin1(capabilities[i].name), Qt::CaseInsensitive))
            description.caps |= capabilities[i].cap;
    }

    if (displayArgs.contains(QLatin1String("forcepremultiplied"), Qt::CaseInsensitive)) {
        description.caps |= DSCAPS_PREMULTIPLIED;
    }

    // We don't track the primary surface as it's released in disconnect
    d_ptr->dfbSurface = createDFBSurface(description, DontTrackSurface);
    if (!d_ptr->dfbSurface) {
        DirectFBError("QDirectFBScreen: error creating primary surface",
                      result);
        return false;
    }

    // Work out what format we're going to use for surfaces with an alpha channel
    d_ptr->alphaPixmapFormat = QDirectFBScreen::getImageFormat(d_ptr->dfbSurface);
    setPixelFormat(d_ptr->alphaPixmapFormat);
    switch (d_ptr->alphaPixmapFormat) {
    case QImage::Format_RGB666:
        d_ptr->alphaPixmapFormat = QImage::Format_ARGB6666_Premultiplied;
        break;
    case QImage::Format_RGB444:
        d_ptr->alphaPixmapFormat = QImage::Format_ARGB4444_Premultiplied;
        break;
    case QImage::Format_RGB32:
        qWarning("QDirectFBScreen::connect(). Qt/DirectFB does not work with the RGB32 pixelformat. "
                 "We recommmend using ARGB instead");
        return false;
    case QImage::Format_Indexed8:
        qWarning("QDirectFBScreen::connect(). Qt/DirectFB does not work with the LUT8  pixelformat.");
        return false;
    case QImage::NImageFormats:
    case QImage::Format_Invalid:
    case QImage::Format_Mono:
    case QImage::Format_MonoLSB:
    case QImage::Format_RGB888:
    case QImage::Format_RGB16:
    case QImage::Format_RGB555:
        d_ptr->alphaPixmapFormat = QImage::Format_ARGB32_Premultiplied;
        break;
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
    case QImage::Format_ARGB4444_Premultiplied:
    case QImage::Format_ARGB8555_Premultiplied:
    case QImage::Format_ARGB8565_Premultiplied:
    case QImage::Format_ARGB6666_Premultiplied:
        // works already
        break;
    }
    d_ptr->dfbSurface->GetSize(d_ptr->dfbSurface, &w, &h);

    data = 0;
    lstep = 0;
    size = 0;
    dw = w;
    dh = h;

    DFBSurfacePixelFormat format;
    result = d_ptr->dfbSurface->GetPixelFormat(d_ptr->dfbSurface, &format);
    if (result == DFB_OK)
        QScreen::d = depth(format);
    else
        DirectFBError("QDirectFBScreen: error getting surface format", result);

    setPixelFormat(getImageFormat(d_ptr->dfbSurface));

    physWidth = physHeight = -1;
    ::setIntOption(displayArgs, QLatin1String("mmWidth"), &physWidth);
    ::setIntOption(displayArgs, QLatin1String("mmHeight"), &physHeight);
    const int dpi = 72;
    if (physWidth < 0)
        physWidth = qRound(dw * 25.4 / dpi);
    if (physHeight < 0)
        physHeight = qRound(dh * 25.4 / dpi);

#ifndef QT_NO_DIRECTFB_LAYER
    result = d_ptr->dfb->GetDisplayLayer(d_ptr->dfb, DLID_PRIMARY,
                                         &d_ptr->dfbLayer);
    if (result != DFB_OK) {
        DirectFBError("QDirectFBScreen::connect: "
                      "Unable to get primary display layer!", result);
        return false;
    }
    result = d_ptr->dfbLayer->GetScreen(d_ptr->dfbLayer, &d_ptr->dfbScreen);
#else
    result = d_ptr->dfb->GetScreen(d_ptr->dfb, 0, &d_ptr->dfbScreen);
#endif
    if (result != DFB_OK) {
        DirectFBError("QDirectFBScreen::connect: "
                      "Unable to get screen!", result);
        return false;
    }

    setGraphicsSystem(d_ptr);

#if (Q_DIRECTFB_VERSION >= 0x000923)
    if (displayArgs.contains(QLatin1String("debug"), Qt::CaseInsensitive))
        printDirectFBInfo(d_ptr->dfb, d_ptr->dfbSurface);
#endif

    QRegExp backgroundColorRegExp("bgcolor=?(.+)");
    backgroundColorRegExp.setCaseSensitivity(Qt::CaseInsensitive);
    if (displayArgs.indexOf(backgroundColorRegExp) != -1) {
        d_ptr->backgroundColor.setNamedColor(backgroundColorRegExp.cap(1));
    }
    if (!d_ptr->backgroundColor.isValid())
        d_ptr->backgroundColor = Qt::green;

    return true;
}

void QDirectFBScreen::disconnect()
{
    d_ptr->dfbSurface->Release(d_ptr->dfbSurface);
    d_ptr->dfbSurface = 0;

    foreach (IDirectFBSurface *surf, d_ptr->allocatedSurfaces)
        surf->Release(surf);
    d_ptr->allocatedSurfaces.clear();

#ifndef QT_NO_DIRECTFB_LAYER
    d_ptr->dfbLayer->Release(d_ptr->dfbLayer);
    d_ptr->dfbLayer = 0;
#endif

    d_ptr->dfbScreen->Release(d_ptr->dfbScreen);
    d_ptr->dfbScreen = 0;

    d_ptr->dfb->Release(d_ptr->dfb);
    d_ptr->dfb = 0;
}

bool QDirectFBScreen::initDevice()
{
#ifndef QT_NO_DIRECTFB_MOUSE
    if (qgetenv("QWS_MOUSE_PROTO").isEmpty()) {
        QWSServer::instance()->setDefaultMouse("None");
        d_ptr->mouse = new QDirectFBMouseHandler;
    }
#endif
#ifndef QT_NO_DIRECTFB_KEYBOARD
    if (qgetenv("QWS_KEYBOARD").isEmpty()) {
        QWSServer::instance()->setDefaultKeyboard("None");
        d_ptr->keyboard = new QDirectFBKeyboardHandler(QString());
    }
#endif

#ifndef QT_NO_QWS_CURSOR
#if defined QT_NO_DIRECTFB_WM || defined QT_NO_DIRECTFB_LAYER
    QScreenCursor::initSoftwareCursor();
#else
    qt_screencursor = new QDirectFBScreenCursor;
#endif
#endif
    return true;
}

void QDirectFBScreen::shutdownDevice()
{
#ifndef QT_NO_DIRECTFB_MOUSE
    delete d_ptr->mouse;
    d_ptr->mouse = 0;
#endif
#ifndef QT_NO_DIRECTFB_KEYBOARD
    delete d_ptr->keyboard;
    d_ptr->keyboard = 0;
#endif

#ifndef QT_NO_QWS_CURSOR
    delete qt_screencursor;
    qt_screencursor = 0;
#endif
}

void QDirectFBScreen::setMode(int width, int height, int depth)
{
    d_ptr->dfb->SetVideoMode(d_ptr->dfb, width, height, depth);
}

void QDirectFBScreen::blank(bool on)
{
    d_ptr->dfbScreen->SetPowerMode(d_ptr->dfbScreen,
                                   (on ? DSPM_ON : DSPM_SUSPEND));
}

QWSWindowSurface *QDirectFBScreen::createSurface(QWidget *widget) const
{
#ifdef QT_NO_DIRECTFB_WM
    if (QApplication::type() == QApplication::GuiServer) {
        return new QDirectFBWindowSurface(d_ptr->flipFlags, const_cast<QDirectFBScreen*>(this), widget);
    } else {
        return QScreen::createSurface(widget);
    }
#else
    return new QDirectFBWindowSurface(d_ptr->flipFlags, const_cast<QDirectFBScreen*>(this), widget);
#endif
}

QWSWindowSurface *QDirectFBScreen::createSurface(const QString &key) const
{
    if (key == QLatin1String("directfb")) {
        return new QDirectFBWindowSurface(d_ptr->flipFlags, const_cast<QDirectFBScreen*>(this));
    }
    return QScreen::createSurface(key);
}

// Normally, when using DirectFB to compose the windows (I.e. when
// QT_NO_DIRECTFB_WM isn't set), exposeRegion will simply return. If
// QT_NO_DIRECTFB_WM is set, exposeRegion will compose only non-directFB
// window surfaces. Normal, directFB surfaces are handled by DirectFB.
static inline bool needExposeRegion()
{
#ifdef QT_NO_DIRECTFB_WM
    return true;
#endif
#ifdef QT_NO_DIRECTFB_LAYER
#ifndef QT_NO_QWS_CURSOR
    return true;
#endif
#endif
    return false;
}

void QDirectFBScreen::exposeRegion(QRegion r, int changing)
{
    if (!needExposeRegion()) {
        return;
    }

    const QList<QWSWindow*> windows = QWSServer::instance()->clientWindows();
    if (changing < 0 || changing >= windows.size())
        return;


    QWSWindow *win = windows.at(changing);
    QWSWindowSurface *s = win->windowSurface();
    r &= region();
    if (r.isEmpty())
        return;

    const QRect brect = r.boundingRect();

    if (!s) {
        solidFill(d_ptr->backgroundColor, r);
    } else {
        const QRect windowGeometry = s->geometry();
        const QRegion outsideWindow = r.subtracted(windowGeometry);
        if (!outsideWindow.isEmpty()) {
            solidFill(d_ptr->backgroundColor, outsideWindow);
        }
        const QRegion insideWindow = r.intersected(windowGeometry);
        if (!insideWindow.isEmpty()) {
            QDirectFBWindowSurface *dfbWindowSurface = (s->key() == QLatin1String("directfb"))
                                                       ? static_cast<QDirectFBWindowSurface*>(s) : 0;
            if (dfbWindowSurface) {
                IDirectFBSurface *surface = dfbWindowSurface->directFBSurface();
                const int n = insideWindow.numRects();
                if (n == 1 || d_ptr->directFBFlags & BoundingRectFlip) {
                    const QRect source = (insideWindow.boundingRect().intersected(windowGeometry)).translated(-windowGeometry.topLeft());
                    const DFBRectangle rect = {
                        source.x(), source.y(), source.width(), source.height()
                    };
                    d_ptr->dfbSurface->Blit(d_ptr->dfbSurface, surface, &rect,
                                            windowGeometry.x() + source.x(),
                                            windowGeometry.y() + source.y());
                } else {
                    const QVector<QRect> rects = insideWindow.rects();
                    QVarLengthArray<DFBRectangle, 16> dfbRectangles(n);
                    QVarLengthArray<DFBPoint, 16> dfbPoints(n);

                    for (int i=0; i<n; ++i) {
                        const QRect source = (rects.at(i).intersected(windowGeometry)).translated(-windowGeometry.topLeft());
                        DFBRectangle &rect = dfbRectangles[i];
                        rect.x = source.x();
                        rect.y = source.y();
                        rect.w = source.width();
                        rect.h = source.height();
                        dfbPoints[i].x = (windowGeometry.x() + source.x());
                        dfbPoints[i].y = (windowGeometry.y() + source.y());
                    }
                    d_ptr->dfbSurface->BatchBlit(d_ptr->dfbSurface, surface, dfbRectangles.constData(),
                                                 dfbPoints.constData(), n);
                }
            }
        }
    }

    if (QScreenCursor *cursor = QScreenCursor::instance()) {
        const QRect cursorRectangle = cursor->boundingRect();
        if (cursor->isVisible() && !cursor->isAccelerated() && cursorRectangle.intersects(brect)) {
            const QImage image = cursor->image();
            IDirectFBSurface *surface = createDFBSurface(image, QDirectFBScreen::DontTrackSurface);
            d_ptr->dfbSurface->SetBlittingFlags(d_ptr->dfbSurface, DSBLIT_BLEND_ALPHACHANNEL);
            d_ptr->dfbSurface->Blit(d_ptr->dfbSurface, surface, 0, cursorRectangle.x(), cursorRectangle.y());
            surface->Release(surface);
#if (Q_DIRECTFB_VERSION >= 0x010000)
            d_ptr->dfbSurface->ReleaseSource(d_ptr->dfbSurface);
#endif
        }
    }
    flipSurface(d_ptr->dfbSurface, d_ptr->flipFlags, r, QPoint());
}

void QDirectFBScreen::solidFill(const QColor &color, const QRegion &region)
{
    if (region.isEmpty())
        return;

    d_ptr->dfbSurface->SetColor(d_ptr->dfbSurface,
                                color.red(), color.green(), color.blue(),
                                color.alpha());
    const int n = region.numRects();
    if (n > 1) {
        const QRect r = region.boundingRect();
        d_ptr->dfbSurface->FillRectangle(d_ptr->dfbSurface, r.x(), r.y(), r.width(), r.height());
    } else {
        const QVector<QRect> rects = region.rects();
        QVarLengthArray<DFBRectangle, 32> rectArray(n);
        for (int i=0; i<n; ++i) {
            const QRect &r = rects.at(i);
            rectArray[i].x = r.x();
            rectArray[i].y = r.y();
            rectArray[i].w = r.width();
            rectArray[i].h = r.height();
        }
        d_ptr->dfbSurface->FillRectangles(d_ptr->dfbSurface, rectArray.constData(), n);
    }
}

void QDirectFBScreen::erase(const QRegion &region)
{
    solidFill(d_ptr->backgroundColor, region);
}

QImage::Format QDirectFBScreen::alphaPixmapFormat() const
{
    return d_ptr->alphaPixmapFormat;
}

bool QDirectFBScreen::initSurfaceDescriptionPixelFormat(DFBSurfaceDescription *description,
                                                        QImage::Format format)
{
    const DFBSurfacePixelFormat pixelformat = QDirectFBScreen::getSurfacePixelFormat(format);
    if (pixelformat == DSPF_UNKNOWN)
        return false;
    description->flags |= DSDESC_PIXELFORMAT;
    description->pixelformat = pixelformat;
    if (QDirectFBScreen::isPremultiplied(format)) {
        if (!(description->flags & DSDESC_CAPS)) {
            description->caps = DSCAPS_PREMULTIPLIED;
            description->flags |= DSDESC_CAPS;
        } else {
            description->caps |= DSCAPS_PREMULTIPLIED;
        }
    }
    return true;
}

uchar *QDirectFBScreen::lockSurface(IDirectFBSurface *surface, uint flags, int *bpl)
{
    void *mem;
    const DFBResult result = surface->Lock(surface, static_cast<DFBSurfaceLockFlags>(flags), static_cast<void**>(&mem), bpl);
    if (result != DFB_OK) {
        DirectFBError("QDirectFBScreen::lockSurface()", result);
    }

    return reinterpret_cast<uchar*>(mem);
}


void QDirectFBScreen::flipSurface(IDirectFBSurface *surface, DFBSurfaceFlipFlags flipFlags,
                                  const QRegion &region, const QPoint &offset)
{
    if (!(flipFlags & DSFLIP_BLIT)) {
        surface->Flip(surface, 0, flipFlags);
    } else {
        if (!(d_ptr->directFBFlags & BoundingRectFlip) && region.numRects() > 1) {
            const QVector<QRect> rects = region.rects();
            const DFBSurfaceFlipFlags nonWaitFlags = flipFlags & ~DSFLIP_WAIT;
            for (int i=0; i<rects.size(); ++i) {
                const QRect &r = rects.at(i);
                const DFBRegion dfbReg = { r.x() + offset.x(), r.y() + offset.y(),
                                           r.x() + r.width() + offset.x(),
                                           r.y() + r.height() + offset.y() };
                surface->Flip(surface, &dfbReg, i + 1 < rects.size() ? nonWaitFlags : flipFlags);
            }
        } else {
            const QRect r = region.boundingRect();
            const DFBRegion dfbReg = { r.x() + offset.x(), r.y() + offset.y(),
                                       r.x() + r.width() + offset.x(),
                                       r.y() + r.height() + offset.y() };
            surface->Flip(surface, &dfbReg, flipFlags);
        }
    }
}


