/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include <qglobal.h>
#include <qstring.h>

EXPORT_C QString localizedDirectoryName(QString&)
{
    qWarning("QDesktopServices::displayName() not implemented for this platform version");
    return QString();
}

