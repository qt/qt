/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QPIXMAPDATA_S60_P_H
#define QPIXMAPDATA_S60_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtGui/private/qpixmap_raster_p.h>

QT_BEGIN_NAMESPACE

class CFbsBitmap;
class CFbsBitmapDevice;
class CBitmapContext;

class QSymbianBitmapDataAccess;

class QSymbianFbsHeapLock
{
public:
    
    enum LockAction {
        Unlock
    };
    
    explicit QSymbianFbsHeapLock(LockAction a);
    ~QSymbianFbsHeapLock();
    void relock();	
    
private:
    
    LockAction action;
    bool wasLocked;
};

class QS60PixmapData : public QRasterPixmapData
{
public:
    QS60PixmapData(PixelType type);
    ~QS60PixmapData();

    void resize(int width, int height);
    void fromImage(const QImage &image, Qt::ImageConversionFlags flags);
    void copy(const QPixmapData *data, const QRect &rect);
    bool scroll(int dx, int dy, const QRect &rect);

    int metric(QPaintDevice::PaintDeviceMetric metric) const;
    void fill(const QColor &color);
    void setMask(const QBitmap &mask);
    void setAlphaChannel(const QPixmap &alphaChannel);
    QImage toImage() const;
    QPaintEngine* paintEngine() const;

    void beginDataAccess();
    void endDataAccess(bool readOnly=false) const;

private:
	void release();
	void fromSymbianBitmap(CFbsBitmap* bitmap);
	bool initSymbianBitmapContext();

    QSymbianBitmapDataAccess *symbianBitmapDataAccess;
    
    CFbsBitmap *cfbsBitmap;
    CFbsBitmapDevice *bitmapDevice;
    CBitmapContext *bitmapContext;
    QPaintEngine *pengine;
    uchar* bytes;

    friend class QPixmap;
    friend class QS60WindowSurface;
    friend class QS60PaintEngine;
    friend class QS60Data;
};

QT_END_NAMESPACE

#endif // QPIXMAPDATA_S60_P_H

