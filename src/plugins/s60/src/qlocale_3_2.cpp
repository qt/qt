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

#include <e32std.h>
#include <e32base.h>

EXPORT_C TPtrC defaultGetLongDateFormatSpec(TExtendedLocale& locale)
{
    return locale.GetLongDateFormatSpec();
}

EXPORT_C TPtrC defaultGetShortDateFormatSpec(TExtendedLocale& locale)
{
    return locale.GetShortDateFormatSpec();
}

EXPORT_C TPtrC defaultGetTimeFormatSpec(TExtendedLocale& locale)
{
    return locale.GetTimeFormatSpec();
}

EXPORT_C void defaultFormatL(TTime& time, TDes& des, const TDesC& format, const TLocale& locale)
{
    time.FormatL(des, format, locale);
}
