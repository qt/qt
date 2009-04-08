/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdirectfbscreen.h"
#include "qdirectfbsurface.h"
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
    QRegion prevExpose;

    QSet<IDirectFBSurface*> allocatedSurfaces;

#ifndef QT_NO_DIRECTFB_MOUSE
    QDirectFBMouseHandler *mouse;
#endif
#ifndef QT_NO_DIRECTFB_KEYBOARD
    QDirectFBKeyboardHandler *keyboard;
#endif
    bool videoonly;
    QImage::Format alphaPixmapFormat;
};

QDirectFBScreenPrivate::QDirectFBScreenPrivate(QDirectFBScreen* screen)
    : QWSGraphicsSystem(screen), dfb(0), dfbSurface(0), flipFlags(DSFLIP_BLIT)
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
    , videoonly(false)
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

    foreach (IDirectFBSurface* surf, allocatedSurfaces)
        surf->Release(surf);
    allocatedSurfaces.clear();

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

IDirectFBSurface* QDirectFBScreen::createDFBSurface(const QImage &img, SurfaceCreationOptions options)
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

    DFBSurfaceDescription desc = QDirectFBScreen::getSurfaceDescription(img);
    IDirectFBSurface *surface = createDFBSurface(&desc, options);
#ifdef QT_NO_DIRECTFB_PREALLOCATED
    if (surface) {
        char *mem;
        int bpl;
        surface->Lock(surface, DSLF_WRITE, (void**)&mem, &bpl);
        const int h = img.height();
        for (int i = 0; i < h; ++i) {
            memcpy(mem, img.scanLine(i), bpl);
            mem += bpl;
        }
        surface->Unlock(surface);
    }
#endif
#ifndef QT_NO_DIRECTFB_PALETTE
    if (img.numColors() != 0)
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
    surface->SetBlittingFlags(surface, DSBLIT_NOFX);
    surface->Blit(surface, src, 0, 0, 0);
    surface->ReleaseSource(surface); // ??? Is this always right?
    return surface;
}

IDirectFBSurface *QDirectFBScreen::createDFBSurface(const QSize &size,
                                                    QImage::Format format,
                                                    SurfaceCreationOptions options)
{
    DFBSurfaceDescription desc;
    desc.flags = DFBSurfaceDescriptionFlags(DSDESC_WIDTH|DSDESC_HEIGHT);
    if (!QDirectFBScreen::initSurfaceDescriptionPixelFormat(&desc, format))
        return 0;
    desc.width = size.width();
    desc.height = size.height();
    return createDFBSurface(&desc, options);
}


IDirectFBSurface* QDirectFBScreen::createDFBSurface(const DFBSurfaceDescription *desc, SurfaceCreationOptions options)
{
    DFBResult result;
    IDirectFBSurface* newSurface = 0;

    if (!d_ptr->dfb) {
        qWarning("QDirectFBScreen::createDFBSurface() - not connected");
        return 0;
    }

    if (d_ptr->videoonly && !(desc->flags & DSDESC_PREALLOCATED)) {
        // Add the video only capability. This means the surface will be created in video ram
        DFBSurfaceDescription voDesc = *desc;
        voDesc.caps = DFBSurfaceCapabilities(voDesc.caps | DSCAPS_VIDEOONLY);
        voDesc.flags = DFBSurfaceDescriptionFlags(voDesc.flags | DSDESC_CAPS);
        result = d_ptr->dfb->CreateSurface(d_ptr->dfb, &voDesc, &newSurface);
    }

    if (!newSurface)
        result = d_ptr->dfb->CreateSurface(d_ptr->dfb, desc, &newSurface);

    if (result != DFB_OK) {
        qWarning("QDirectFBScreen::createDFBSurface() Failed!\n"
                 "   Flags %0x Caps %0x width %d height %d pixelformat %0x %d preallocated %p %d\n%s",
                 desc->flags, desc->caps, desc->width, desc->height,
                 desc->pixelformat, DFB_PIXELFORMAT_INDEX(desc->pixelformat),
                 desc->preallocated[0].data, desc->preallocated[0].pitch,
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
        ) {
        image = image.convertToFormat(pixmapFormat);
    }


    IDirectFBSurface *dfbSurface = createDFBSurface(img.size(), pixmapFormat, options);
    if (!dfbSurface) {
        qWarning("QDirectFBPixmapData::fromImage() Couldn't create surface");
        return 0;
    }

#ifndef QT_NO_DIRECTFB_PREALLOCATED
    IDirectFBSurface *imgSurface = createDFBSurface(img, DontTrackSurface);
    if (!imgSurface) {
        qWarning("QDirectFBPixmapData::fromImage()");
        QDirectFBScreen::releaseDFBSurface(dfbSurface);
        return 0;
    }
    Q_ASSERT(imgSurface);
    DFBResult result;
    dfbSurface->SetBlittingFlags(dfbSurface, DSBLIT_NOFX);
    result = dfbSurface->Blit(dfbSurface, imgSurface, 0, 0, 0);
    if (result != DFB_OK)
        DirectFBError("QDirectFBPixmapData::fromImage()", result);
    dfbSurface->ReleaseSource(dfbSurface);
    imgSurface->Release(imgSurface);
#else // QT_NO_DIRECTFB_PREALLOCATED
    Q_ASSERT(image.format() == pixmapFormat);
    char *mem;
    int bpl;
    dfbSurface->Lock(dfbSurface, DSLF_WRITE, (void**)&mem, &bpl);
    const int w = image.width() * image.depth() / 8;
    for (int i = 0; i < image.height(); ++i) {
        memcpy(mem, image.scanLine(i), w);
        mem += bpl;
    }
    dfbSurface->Unlock(dfbSurface);
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

bool QDirectFBScreen::preferVideoOnly() const
{
    return d_ptr->videoonly;
}

IDirectFB* QDirectFBScreen::dfb()
{
    return d_ptr->dfb;
}

IDirectFBSurface* QDirectFBScreen::dfbSurface()
{
    return d_ptr->dfbSurface;
}

#ifndef QT_NO_DIRECTFB_LAYER
IDirectFBDisplayLayer* QDirectFBScreen::dfbDisplayLayer()
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
    case QImage::Format_ARGB6666_Premultiplied:
        return DSPF_ARGB6666;
    case QImage::Format_RGB666:
        return DSPF_RGB18;
    case QImage::Format_RGB32:
        return DSPF_RGB32;
    case QImage::Format_ARGB32_Premultiplied:
    case QImage::Format_ARGB32:
        return DSPF_ARGB;
    default:
        return DSPF_UNKNOWN;
    };
}

static inline  bool isPremultiplied(IDirectFBSurface *surface)
{
    Q_ASSERT(surface);
    DFBSurfaceCapabilities caps;
    const DFBResult result = surface->GetCapabilities(surface, &caps);
    Q_ASSERT(result == DFB_OK);
    Q_UNUSED(result);
    return caps & DSCAPS_PREMULTIPLIED;
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
    case DSPF_ARGB6666:
        return QImage::Format_ARGB6666_Premultiplied;
    case DSPF_RGB18:
        return QImage::Format_RGB666;
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

    const DFBSurfacePixelFormat format = getSurfacePixelFormat(image.format());

    if (format == DSPF_UNKNOWN || image.isNull()) {
        description.flags = DFBSurfaceDescriptionFlags(0);
        return description;
    }

    description.flags = DFBSurfaceDescriptionFlags(DSDESC_WIDTH
                                                   | DSDESC_HEIGHT
#ifndef QT_NO_DIRECTFB_PREALLOCATED
                                                   | DSDESC_PREALLOCATED
#endif
                                                   | DSDESC_PIXELFORMAT);
    QDirectFBScreen::initSurfaceDescriptionPixelFormat(&description, image.format());
    description.width = image.width();
    description.height = image.height();
#ifndef QT_NO_DIRECTFB_PREALLOCATED
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

    description.flags = DFBSurfaceDescriptionFlags(DSDESC_CAPS
                                                   | DSDESC_WIDTH
                                                   | DSDESC_HEIGHT
                                                   | DSDESC_PIXELFORMAT
                                                   | DSDESC_PREALLOCATED);
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
    ~QDirectFBScreenCursor();

    void set(const QImage &image, int hotx, int hoty);
    void move(int x, int y);
    void show();
    void hide();

private:
    IDirectFBDisplayLayer *layer;
    bool implicitHide;
};

QDirectFBScreenCursor::QDirectFBScreenCursor()
{
    IDirectFB *fb = QDirectFBScreen::instance()->dfb();
    if (!fb)
        qFatal("QDirectFBScreenCursor: DirectFB not initialized");

    layer = QDirectFBScreen::instance()->dfbDisplayLayer();

    if (layer)
        layer->SetCooperativeLevel(layer, DLSCL_SHARED); // XXX: hw: remove?
    else
        qFatal("QDirectFBScreenCursor: Unable to get primary display layer!");

    enable = true;
    hwaccel = true;
    implicitHide = false;
    supportsAlpha = true;

    set(QImage(), 0, 0);
}

QDirectFBScreenCursor::~QDirectFBScreenCursor()
{
}

void QDirectFBScreenCursor::show()
{
    DFBResult result;
    result = layer->SetCooperativeLevel(layer, DLSCL_ADMINISTRATIVE);
    if (result != DFB_OK) {
        DirectFBError("QDirectFBScreenCursor::show: "
                      "Unable to set cooperative level", result);
    }
    result = layer->EnableCursor(layer, 1);
    if (result != DFB_OK) {
        DirectFBError("QDirectFBScreenCursor::show: "
                      "Unable to enable cursor", result);
    }
    result = layer->SetCooperativeLevel(layer, DLSCL_SHARED);
    if (result != DFB_OK) {
        DirectFBError("QDirectFBScreenCursor::show: "
                      "Unable to reset cooperative level", result);
    }
    implicitHide = false;
}

void QDirectFBScreenCursor::hide()
{
    DFBResult result;
    result = layer->SetCooperativeLevel(layer, DLSCL_ADMINISTRATIVE);
    if (result != DFB_OK) {
        DirectFBError("QDirectFBScreenCursor::hide: "
                      "Unable to set cooperative level", result);
    }
    result = layer->EnableCursor(layer, 0);
    if (result != DFB_OK) {
        DirectFBError("QDirectFBScreenCursor::hide: "
                      "Unable to disable cursor", result);
    }
    result = layer->SetCooperativeLevel(layer, DLSCL_SHARED);
    if (result != DFB_OK) {
        DirectFBError("QDirectFBScreenCursor::hide: "
                      "Unable to reset cooperative level", result);
    }
    implicitHide = true;
}

void QDirectFBScreenCursor::move(int x, int y)
{
    layer->WarpCursor(layer, x, y);
}

void QDirectFBScreenCursor::set(const QImage &image, int hotx, int hoty)
{
    if (image.isNull() && isVisible()) {
        hide();
        implicitHide = true;
    } else if (!image.isNull() && implicitHide) {
        show();
    }
    cursor = image.convertToFormat(QDirectFBScreen::instance()->alphaPixmapFormat());

    if (!image.isNull()) {
        Q_ASSERT(cursor.numColors() == 0);
        size = cursor.size();
        hotspot = QPoint(hotx, hoty);

        DFBSurfaceDescription description;
        description = QDirectFBScreen::getSurfaceDescription(cursor);

        IDirectFBSurface *surface;
        surface = QDirectFBScreen::instance()->createDFBSurface(&description,
                                                                QDirectFBScreen::TrackSurface);
        if (!surface) {
            qWarning("QDirectFBScreenCursor::set: Unable to create surface");
            return;
        }
        DFBResult result = layer->SetCooperativeLevel(layer, DLSCL_ADMINISTRATIVE);
        if (result != DFB_OK) {
            DirectFBError("QDirectFBScreenCursor::set: "
                          "Unable to set cooperative level", result);
        }
        result = layer->SetCursorShape(layer, surface, hotx, hoty);
        if (result != DFB_OK) {
            DirectFBError("QDirectFBScreenCursor::set: Unable to set cursor shape",
                          result);
        }

        result = layer->SetCooperativeLevel(layer, DLSCL_SHARED);
        if (result != DFB_OK) {
            DirectFBError("QDirectFBScreenCursor::set: "
                          "Unable to reset cooperative level", result);
        }

        if (surface)
            QDirectFBScreen::instance()->releaseDFBSurface(surface);
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
    QRegExp flipRegexp(QLatin1String("^flip=([\\w,]+)$"));
    int index = args.indexOf(flipRegexp);
    if (index >= 0) {
        const QStringList flips = flipRegexp.cap(1).split(QLatin1Char(','),
                                                          QString::SkipEmptyParts);
        flipFlags = DSFLIP_NONE;
        foreach (QString flip, flips) {
            if (flip == QLatin1String("wait"))
                flipFlags = DFBSurfaceFlipFlags(flipFlags | DSFLIP_WAIT);
            else if (flip == QLatin1String("blit"))
                flipFlags = DFBSurfaceFlipFlags(flipFlags | DSFLIP_BLIT);
            else if (flip == QLatin1String("onsync"))
                flipFlags = DFBSurfaceFlipFlags(flipFlags | DSFLIP_ONSYNC);
            else if (flip == QLatin1String("pipeline"))
                flipFlags = DFBSurfaceFlipFlags(flipFlags | DSFLIP_PIPELINE);
            else
                qWarning("QDirectFBScreen: Unknown flip argument: %s",
                         qPrintable(flip));
        }
    }
}

QPixmapData* QDirectFBScreenPrivate::createPixmapData(QPixmapData::PixelType type) const
{
    if (type == QPixmapData::BitmapType)
        return QWSGraphicsSystem::createPixmapData(type);

    return new QDirectFBPixmapData(type);
}

static void printDirectFBInfo(IDirectFB *fb)
{
    DFBResult result;
    DFBGraphicsDeviceDescription dev;

    result = fb->GetDeviceDescription(fb, &dev);
    if (result != DFB_OK) {
        DirectFBError("Error reading graphics device description", result);
        return;
    }

    qDebug("Device: %s (%s), Driver: %s v%i.%i (%s)\n"
           "  acceleration: 0x%x, blit: 0x%x, draw: 0x%0x video: %i\n",
           dev.name, dev.vendor, dev.driver.name, dev.driver.major,
           dev.driver.minor, dev.driver.vendor, dev.acceleration_mask,
           dev.blitting_flags, dev.drawing_flags, dev.video_memory);
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

    if (displayArgs.contains(QLatin1String("debug"), Qt::CaseInsensitive))
        printDirectFBInfo(d_ptr->dfb);

    if (displayArgs.contains(QLatin1String("videoonly")))
        d_ptr->videoonly = true;

#ifndef QT_NO_DIRECTFB_WM
    if (displayArgs.contains(QLatin1String("fullscreen")))
#endif
        d_ptr->dfb->SetCooperativeLevel(d_ptr->dfb, DFSCL_FULLSCREEN);

    DFBSurfaceDescription description;
    description.flags = DFBSurfaceDescriptionFlags(DSDESC_CAPS);
    description.caps = DFBSurfaceCapabilities(DSCAPS_PRIMARY
                                              | DSCAPS_DOUBLE
                                              | DSCAPS_STATIC_ALLOC);
    if (displayArgs.contains(QLatin1String("forcepremultiplied"),
                             Qt::CaseInsensitive)) {
        description.caps = DFBSurfaceCapabilities(description.caps
                                                  | DSCAPS_PREMULTIPLIED);
    }

    if (!(d_ptr->flipFlags & DSFLIP_BLIT)) {
        description.caps = DFBSurfaceCapabilities(description.caps
                                                  | DSCAPS_DOUBLE
                                                  | DSCAPS_TRIPLE);
    }


    // We don't track the primary surface as it's released in disconnect
    d_ptr->dfbSurface = createDFBSurface(&description, DontTrackSurface);
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
    case QImage::NImageFormats:
    case QImage::Format_Invalid:
    case QImage::Format_Mono:
    case QImage::Format_MonoLSB:
    case QImage::Format_Indexed8:
    case QImage::Format_RGB32:
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
    QRegExp mmWidthRx(QLatin1String("mmWidth=?(\\d+)"));
    int dimIdxW = displayArgs.indexOf(mmWidthRx);
    if (dimIdxW >= 0) {
        mmWidthRx.exactMatch(displayArgs.at(dimIdxW));
        physWidth = mmWidthRx.cap(1).toInt();
    }
    QRegExp mmHeightRx(QLatin1String("mmHeight=?(\\d+)"));
    int dimIdxH = displayArgs.indexOf(mmHeightRx);
    if (dimIdxH >= 0) {
        mmHeightRx.exactMatch(displayArgs.at(dimIdxH));
        physHeight = mmHeightRx.cap(1).toInt();
    }
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

    return true;
}

void QDirectFBScreen::disconnect()
{
    d_ptr->dfbSurface->Release(d_ptr->dfbSurface);
    d_ptr->dfbSurface = 0;

    foreach (IDirectFBSurface* surf, d_ptr->allocatedSurfaces)
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
    QWSServer *server = QWSServer::instance();
#ifndef QT_NO_DIRECTFB_MOUSE
    if (qgetenv("QWS_MOUSE_PROTO").isEmpty()) {
        server->setDefaultMouse("None");
        d_ptr->mouse = new QDirectFBMouseHandler;
    }
#endif
#ifndef QT_NO_DIRECTFB_KEYBOARD
    if (qgetenv("QWS_KEYBOARD").isEmpty()) {
        server->setDefaultKeyboard("None");
        d_ptr->keyboard = new QDirectFBKeyboardHandler(QString());
    }
#endif

#ifndef QT_NO_QWS_CURSOR
#ifdef QT_NO_DIRECTFB_LAYER
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

QWSWindowSurface* QDirectFBScreen::createSurface(QWidget *widget) const
{
#ifdef QT_NO_DIRECTFB_WM
    if (QApplication::type() == QApplication::GuiServer)
        return new QDirectFBSurface(const_cast<QDirectFBScreen*>(this), widget);
    else
        return QScreen::createSurface(widget);
#else
    return new QDirectFBSurface(const_cast<QDirectFBScreen*>(this), widget);
#endif
}

QWSWindowSurface* QDirectFBScreen::createSurface(const QString &key) const
{
    if (key == QLatin1String("directfb"))
        return new QDirectFBSurface(const_cast<QDirectFBScreen*>(this));
    return QScreen::createSurface(key);
}

void QDirectFBScreen::compose(const QRegion &region)
{
    const QList<QWSWindow*> windows = QWSServer::instance()->clientWindows();

    QRegion blitRegion = region;
    QRegion blendRegion;

    d_ptr->dfbSurface->SetBlittingFlags(d_ptr->dfbSurface, DSBLIT_NOFX);

    // blit opaque region
    for (int i = 0; i < windows.size(); ++i) {
        QWSWindow *win = windows.at(i);
        QWSWindowSurface *surface = win->windowSurface();
        if (!surface)
            continue;

        const QRegion r = win->allocatedRegion() & blitRegion;
        if (r.isEmpty())
            continue;

        blitRegion -= r;

        if (surface->isRegionReserved()) {
            // nothing
        } else if (win->isOpaque()) {
            const QPoint offset = win->requestedRegion().boundingRect().topLeft();

            if (surface->key() == QLatin1String("directfb")) {
                QDirectFBSurface *s = static_cast<QDirectFBSurface*>(surface);
                blit(s->directFBSurface(), offset, r);
            } else {
                blit(surface->image(), offset, r);
            }
        } else {
            blendRegion += r;
        }
        if (blitRegion.isEmpty())
            break;
    }

    { // fill background
        const QRegion fill = blitRegion + blendRegion;
        if (!fill.isEmpty()) {
            const QColor color = QWSServer::instance()->backgroundBrush().color();
            solidFill(color, fill);
            blitRegion = QRegion();
        }
    }

    if (blendRegion.isEmpty())
        return;

    // blend non-opaque region
    for (int i = windows.size() - 1; i >= 0; --i) {
        QWSWindow *win = windows.at(i);
        QWSWindowSurface *surface = win->windowSurface();
        if (!surface)
            continue;

        const QRegion r = win->allocatedRegion() & blendRegion;
        if (r.isEmpty())
            continue;

        DFBSurfaceBlittingFlags flags = DSBLIT_NOFX;
        if (!win->isOpaque()) {
            flags = DFBSurfaceBlittingFlags(flags | DSBLIT_BLEND_ALPHACHANNEL);
            const uint opacity = win->opacity();
            if (opacity < 255) {
                flags = DFBSurfaceBlittingFlags(flags | DSBLIT_BLEND_COLORALPHA);
                d_ptr->dfbSurface->SetColor(d_ptr->dfbSurface, 0xff, 0xff, 0xff, opacity);
            }
        }
        d_ptr->dfbSurface->SetBlittingFlags(d_ptr->dfbSurface, flags);

        const QPoint offset = win->requestedRegion().boundingRect().topLeft();

        if (surface->key() == QLatin1String("directfb")) {
            QDirectFBSurface *s = static_cast<QDirectFBSurface*>(surface);
            blit(s->directFBSurface(), offset, r);
        } else {
            blit(surface->image(), offset, r);
        }
    }
}

// Normally, when using DirectFB to compose the windows (I.e. when
// QT_NO_DIRECTFB_WM isn't set), exposeRegion will simply return. If
// QT_NO_DIRECTFB_WM is set, exposeRegion will compose only non-directFB
// window surfaces. Normal, directFB surfaces are handled by DirectFB.
void QDirectFBScreen::exposeRegion(QRegion r, int changing)
{
    const QList<QWSWindow*> windows = QWSServer::instance()->clientWindows();
    if (changing < 0 || changing >= windows.size())
        return;

#ifndef QT_NO_DIRECTFB_WM
    QWSWindow *win = windows.at(changing);
    QWSWindowSurface *s = win->windowSurface();
    if (s && s->key() == QLatin1String("directfb"))
        return;
#endif

    r &= region();
    if (r.isEmpty())
        return;

    if (d_ptr->flipFlags & DSFLIP_BLIT) {
        const QRect brect = r.boundingRect();
        DFBRegion dfbRegion = { brect.left(), brect.top(),
                                brect.right(), brect.bottom() };
        compose(r);
        d_ptr->dfbSurface->Flip(d_ptr->dfbSurface, &dfbRegion,
                                d_ptr->flipFlags);
    } else {
        compose(r + d_ptr->prevExpose);
        d_ptr->dfbSurface->Flip(d_ptr->dfbSurface, 0, d_ptr->flipFlags);
    }
    d_ptr->prevExpose = r;
}


void QDirectFBScreen::blit(const QImage &img, const QPoint &topLeft,
                           const QRegion &reg)
{
    IDirectFBSurface *src = createDFBSurface(img, QDirectFBScreen::DontTrackSurface);
    if (!src) {
        qWarning("QDirectFBScreen::blit(): Error creating surface");
        return;
    }
    blit(src, topLeft, reg);
    d_ptr->dfbSurface->ReleaseSource(d_ptr->dfbSurface);
    src->Release(src);
}

void QDirectFBScreen::blit(IDirectFBSurface *src, const QPoint &topLeft,
                           const QRegion &region)
{
    const QVector<QRect> rs = region.translated(-offset()).rects();
    const int size = rs.size();
    const QPoint tl = topLeft - offset();

    QVarLengthArray<DFBRectangle> rects(size);
    QVarLengthArray<DFBPoint> points(size);

    int n = 0;
    for (int i = 0; i < size; ++i) {
        const QRect r = rs.at(i);
        if (!r.isValid())
            continue;
        rects[n].x = r.x() - tl.x();
        rects[n].y = r.y() - tl.y();
        rects[n].w = r.width();
        rects[n].h = r.height();
        points[n].x = r.x();
        points[n].y = r.y();
        ++n;
    }

    d_ptr->dfbSurface->BatchBlit(d_ptr->dfbSurface, src, rects.data(),
                                 points.data(), n);
}

void QDirectFBScreen::solidFill(const QColor &color, const QRegion &region)
{
    if (region.isEmpty())
        return;

    const QVector<QRect> rects = region.rects();
    QVarLengthArray<DFBRectangle> dfbRects(rects.size());
    for (int i = 0; i < rects.size(); ++i) {
        const QRect r = rects.at(i);
        dfbRects[i].x = r.x();
        dfbRects[i].y = r.y();
        dfbRects[i].w = r.width();
        dfbRects[i].h = r.height();
    }

    d_ptr->dfbSurface->SetColor(d_ptr->dfbSurface,
                                color.red(), color.green(), color.blue(),
                                color.alpha());
    d_ptr->dfbSurface->FillRectangles(d_ptr->dfbSurface, dfbRects.data(),
                                      dfbRects.size());
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
    description->flags = DFBSurfaceDescriptionFlags(description->flags | DSDESC_PIXELFORMAT);
    description->pixelformat = pixelformat;
    if (QDirectFBScreen::isPremultiplied(format)) {
        if (!(description->flags & DSDESC_CAPS)) {
            description->caps = DSCAPS_PREMULTIPLIED;
            description->flags = DFBSurfaceDescriptionFlags(description->flags | DSDESC_CAPS);
        } else {
            description->caps = DFBSurfaceCapabilities(description->caps | DSCAPS_PREMULTIPLIED);
        }
    }
    return true;
}
