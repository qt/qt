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

#ifndef QEVENTTRANSITION_P_H
#define QEVENTTRANSITION_P_H

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

class QEventTransition;
class Q_CORE_EXPORT QEventTransitionPrivate : public QTransitionPrivate
{
    Q_DECLARE_PUBLIC(QEventTransition)
public:
    QEventTransitionPrivate();

    static QEventTransitionPrivate *get(QEventTransition *q);

    void invalidate();

    bool registered;
    QObject *object;
    QEvent::Type eventType;
};

QT_END_NAMESPACE

#endif
