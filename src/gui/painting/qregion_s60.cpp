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

#include "qbitmap.h"
#include "qbuffer.h"
#include "qimage.h"
#include "qpolygon.h"
#include "qregion.h"

QT_BEGIN_NAMESPACE

QRegion::QRegionData QRegion::shared_empty = { Q_BASIC_ATOMIC_INITIALIZER(1), 0 };

QT_END_NAMESPACE
