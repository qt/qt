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

EXPORT_C void defaultFormatL(TTime&, TDes& des, const TDesC&, const TLocale&)
{
    des.Zero();
}

EXPORT_C TPtrC defaultGetTimeFormatSpec(TExtendedLocale&)
{
    return TPtrC(KNullDesC);
}

EXPORT_C TPtrC defaultGetLongDateFormatSpec(TExtendedLocale&)
{
    return TPtrC(KNullDesC);
}

EXPORT_C TPtrC defaultGetShortDateFormatSpec(TExtendedLocale&)
{
    return TPtrC(KNullDesC);
}
