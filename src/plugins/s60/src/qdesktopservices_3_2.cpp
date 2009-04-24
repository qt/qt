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

#include <private/qcore_symbian_p.h>
#include <cdirectorylocalizer.h>     // CDirectoryLocalizer
#include <qstring.h>
#include <qdir.h>

EXPORT_C QString localizedDirectoryName(QString& rawPath)
{
    QString ret;

    TRAPD(err,
        CDirectoryLocalizer* localizer = CDirectoryLocalizer::NewL();
        CleanupStack::PushL(localizer);
        localizer->SetFullPath(qt_QString2TPtrC(QDir::toNativeSeparators(rawPath)));
        if(localizer->IsLocalized()){
            TPtrC locName(localizer->LocalizedName());
            ret = qt_TDesC2QStringL(locName);
        }
        CleanupStack::PopAndDestroy(localizer);
    )

    if (err != KErrNone)
        ret = QString();

    return ret;
}

