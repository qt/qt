/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_EMBEDDED_LICENSE$
**
****************************************************************************/

#ifndef QWINDOWSURFACE_S60_P_H
#define QWINDOWSURFACE_S60_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qglobal.h>
#include "private/qwindowsurface_p.h"

QT_BEGIN_NAMESPACE

struct QS60WindowSurfacePrivate;
class CFbsBitmap;

class QS60WindowSurface : public QWindowSurface
{
public:
    QS60WindowSurface(QWidget *widget);
    ~QS60WindowSurface();

    QPaintDevice *paintDevice();
    void flush(QWidget *widget, const QRegion &region, const QPoint &offset);
    bool scroll(const QRegion &area, int dx, int dy);

    void beginPaint(const QRegion &);
    void endPaint(const QRegion &);

    void setGeometry(const QRect &rect);

    static void lockBitmapHeap();
    static void unlockBitmapHeap();

    CFbsBitmap *symbianBitmap() const;
    
private:
    void updatePaintDeviceOnBitmap();

private:
    QS60WindowSurfacePrivate* d_ptr;

};

QT_END_NAMESPACE

#endif // QWINDOWSURFACE_S60_P_H
