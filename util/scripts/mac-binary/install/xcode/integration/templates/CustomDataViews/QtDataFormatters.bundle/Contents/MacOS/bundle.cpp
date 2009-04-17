/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************///xcode
extern "C"  {
#include "DataFormatterPlugin.h"
    _pbxgdb_plugin_function_list *_pbxgdb_plugin_functions = 0;
}

//qt stuff
#include <QtCore/QtCore>

static int serialId(int id)
{
#if 0
    static int serialRet = 0xF00F00F0;
    return serialRet--;
#else
    return id;
#endif
}

static void *doAllocate(int id, int size)
{
    if(_pbxgdb_plugin_functions && _pbxgdb_plugin_functions->allocate)
        return (*_pbxgdb_plugin_functions->allocate)(id, size);
    return malloc(size);
}

static char *doSprintf(int id, const char *format, va_list ap)
{
    if(_pbxgdb_plugin_functions && _pbxgdb_plugin_functions->vmessage)
        return (*_pbxgdb_plugin_functions->vmessage)(id, format, ap);
    return "Unable to sprintf!";
}

static char *doSprintf(int id, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    char *ret = doSprintf(id, format, ap);
    va_end(ap);
    return ret;
}

char *
Qt_QPointSummary(QPoint &point, int id)
{
    return doSprintf(serialId(id), "(%d,%d)", point.x(), point.y());
}

char *
Qt_QSizeSummary(QSize &size, int id)
{
    return doSprintf(serialId(id), "[%dx%d]", size.width(), size.height());
}

char *
Qt_QStringSummary(QString &string, int id)
{
    return doSprintf(serialId(id), "%s", string.toLatin1().constData());
}

char *
Qt_QDateSummary(QDate &v, int id)
{
    return doSprintf(serialId(id), "%s", v.toString().toLatin1().constData());
}

char *
Qt_QTimeSummary(QTime &v, int id)
{
    return doSprintf(serialId(id), "%s", v.toString().toLatin1().constData());
}

char *
Qt_QDateTimeSummary(QDateTime &v, int id)
{
    return doSprintf(serialId(id), "%s", v.toString().toLatin1().constData());
}

char *
Qt_QVariantSummary(QVariant &v, int id)
{
    return doSprintf(serialId(id), "%s", v.toString().toLatin1().constData());
}

char *
Qt_QRectSummary(QRect &rect, int id)
{
    return doSprintf(serialId(id), "(%d,%d)[%dx%d]", rect.x(), rect.y(),
                     rect.width(), rect.height());
}
