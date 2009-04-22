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

#ifndef QBASICKEYEVENTTRANSITION_P_H
#define QBASICKEYEVENTTRANSITION_P_H

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

// Qt
#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "qabstracttransition.h"
#else
# include <QtCore/qabstracttransition.h>
#endif
#include <QtGui/qevent.h>

QT_BEGIN_NAMESPACE

class QBasicKeyEventTransitionPrivate;
class Q_AUTOTEST_EXPORT QBasicKeyEventTransition : public QAbstractTransition
{
    Q_OBJECT
public:
    QBasicKeyEventTransition(QState *sourceState = 0);
    QBasicKeyEventTransition(QEvent::Type type, int key, QState *sourceState = 0);
    QBasicKeyEventTransition(QEvent::Type type, int key,
                             Qt::KeyboardModifiers modifiers, QState *sourceState = 0);
    ~QBasicKeyEventTransition();

    QEvent::Type eventType() const;
    void setEventType(QEvent::Type type);

    int key() const;
    void setKey(int key);

    Qt::KeyboardModifiers modifiers() const;
    void setModifiers(Qt::KeyboardModifiers modifiers);

protected:
    bool eventTest(QEvent *event) const;
    void onTransition();

private:
    Q_DISABLE_COPY(QBasicKeyEventTransition)
    Q_DECLARE_PRIVATE(QBasicKeyEventTransition)
};

QT_END_NAMESPACE

#endif
