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

#include "qpaintdevice.h"
#include "qpainter.h"
#include "qwidget.h"
#include "qbitmap.h"
#include "qapplication.h"
#include <private/qapplication_p.h>
#include "qprinter.h"

QT_BEGIN_NAMESPACE

QPaintDevice::QPaintDevice()
{
    painters = 0;
}


QPaintDevice::~QPaintDevice()
{
    if (paintingActive())
        qWarning("QPaintDevice: Cannot destroy paint device that is being "
                  "painted.  Be sure to QPainter::end() painters!");
}

int QPaintDevice::metric(PaintDeviceMetric) const
{
    qWarning("QPaintDevice::metrics: Device has no metric information");
    return 0;
}


QT_END_NAMESPACE
