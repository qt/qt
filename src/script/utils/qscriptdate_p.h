/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTDATE_P_H
#define QSCRIPTDATE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qdatetime.h>

#ifndef QT_NO_SCRIPT

QT_BEGIN_NAMESPACE

typedef double qsreal;

namespace QScript
{
    qsreal FromDateTime(const QDateTime &dt);
    QDateTime ToDateTime(qsreal t, Qt::TimeSpec spec);
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
