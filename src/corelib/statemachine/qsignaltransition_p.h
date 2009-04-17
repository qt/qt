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

#ifndef QSIGNALTRANSITION_P_H
#define QSIGNALTRANSITION_P_H

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

#include "qtransition_p.h"

QT_BEGIN_NAMESPACE

class QSignalTransition;
class QSignalTransitionPrivate : public QTransitionPrivate
{
    Q_DECLARE_PUBLIC(QSignalTransition)
public:
    QSignalTransitionPrivate();

    static QSignalTransitionPrivate *get(QSignalTransition *q);

    void invalidate();

    QObject *sender;
    QByteArray signal;
    int signalIndex;
};

QT_END_NAMESPACE

#endif
