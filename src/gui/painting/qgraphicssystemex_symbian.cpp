/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgraphicssystemex_symbian_p.h"
#include "private/qwidget_p.h"
#include "private/qbackingstore_p.h"
#include "private/qapplication_p.h"

#include <e32property.h>

QT_BEGIN_NAMESPACE

static bool bcm2727Initialized = false;
static bool bcm2727 = false;

bool QSymbianGraphicsSystemEx::hasBCM2727()
{
    if (bcm2727Initialized)
        return bcm2727;

    const TUid KIvePropertyCat = {0x2726beef};
    enum TIvePropertyChipType {
        EVCBCM2727B1 = 0x00000000,
        EVCBCM2763A0 = 0x04000100,
        EVCBCM2763B0 = 0x04000102,
        EVCBCM2763C0 = 0x04000103,
        EVCBCM2763C1 = 0x04000104,
        EVCBCMUnknown = 0x7fffffff
    };

    TInt chipType = EVCBCMUnknown;
    if (RProperty::Get(KIvePropertyCat, 0, chipType) == KErrNone) {
        if (chipType == EVCBCM2727B1)
            bcm2727 = true;
    }

    bcm2727Initialized = true;

    return bcm2727;
}

void QSymbianGraphicsSystemEx::releaseCachedGpuResources()
{
    // Do nothing here
    // This is implemented in graphics system specific plugin
}

void QSymbianGraphicsSystemEx::releaseAllGpuResources()
{
    releaseCachedGpuResources();

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        if (QTLWExtra *topExtra = qt_widget_private(widget)->maybeTopData())
            topExtra->backingStore.destroy();
    }
}

void QSymbianGraphicsSystemEx::forceToRaster(QWidget *window)
{
    if (window && window->isWindow()) {
        qt_widget_private(window)->createTLExtra();
        if (QTLWExtra *topExtra = qt_widget_private(window)->maybeTopData()) {
            topExtra->forcedToRaster = 1;
            if (topExtra->backingStore.data()) {
                topExtra->backingStore.create(window);
                topExtra->backingStore.registerWidget(window);
            }
        }
    }
}

QT_END_NAMESPACE
