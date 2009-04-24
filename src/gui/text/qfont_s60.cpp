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

#include "qfont.h"
#include "qt_s60_p.h"
#include <private/qwindowsurface_s60_p.h>
#include "qmutex.h"

QT_BEGIN_NAMESPACE

#if 1
#ifdef QT_NO_FREETYPE
Q_GLOBAL_STATIC(QMutex, lastResortFamilyMutex);
#endif // QT_NO_FREETYPE

QString QFont::lastResortFamily() const
{
#ifdef QT_NO_FREETYPE
    QMutexLocker locker(lastResortFamilyMutex());
    static QString family;
    if (family.isEmpty()) {
        QS60WindowSurface::unlockBitmapHeap();
        CFont *font;
        const TInt err = S60->screenDevice()->GetNearestFontInTwips(font, TFontSpec());
        Q_ASSERT(err == KErrNone);
        const TFontSpec spec = font->FontSpecInTwips();
        family = QString((const QChar *)spec.iTypeface.iName.Ptr(), spec.iTypeface.iName.Length());
        S60->screenDevice()->ReleaseFont(font);
        QS60WindowSurface::lockBitmapHeap();
    }
    return family;
#else
    // For the FreeType case we just hard code the face name, since otherwise on
    // East Asian systems we may get a name for a stroke based (non-ttf) font.

    // TODO: Get the type face name in a proper way

    const bool isJapaneseOrChineseSystem =
        User::Language() == ELangJapanese || User::Language() == ELangPrcChinese;

    return QLatin1String(isJapaneseOrChineseSystem?"Heisei Kaku Gothic S60":"Series 60 Sans");
#endif // QT_NO_FREETYPE
}
#else // 0
QString QFont::lastResortFamily() const
{
    return QLatin1String("Series 60 Sans");
}
#endif // 0

QString QFont::defaultFamily() const
{
    return lastResortFamily();
}

QT_END_NAMESPACE
