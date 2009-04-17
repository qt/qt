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

#ifndef QBASICMOUSEEVENTTRANSITION_P_H
#define QBASICMOUSEEVENTTRANSITION_P_H

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

#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "qabstracttransition.h"
#else
# include <QtCore/qabstracttransition.h>
#endif

#include <QtGui/qevent.h>

QT_BEGIN_NAMESPACE

class QPainterPath;

class QBasicMouseEventTransitionPrivate;
class Q_AUTOTEST_EXPORT QBasicMouseEventTransition : public QAbstractTransition
{
    Q_OBJECT
public:
    QBasicMouseEventTransition(QState *sourceState = 0);
    QBasicMouseEventTransition(QEvent::Type type, Qt::MouseButton button,
                               QState *sourceState = 0);
    ~QBasicMouseEventTransition();

    QEvent::Type eventType() const;
    void setEventType(QEvent::Type type);

    Qt::MouseButton button() const;
    void setButton(Qt::MouseButton button);

    QPainterPath path() const;
    void setPath(const QPainterPath &path);

protected:
    bool eventTest(QEvent *event) const;
    void onTransition();

private:
    Q_DISABLE_COPY(QBasicMouseEventTransition)
    Q_DECLARE_PRIVATE(QBasicMouseEventTransition)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
