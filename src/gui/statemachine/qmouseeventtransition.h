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

#ifndef QMOUSEEVENTTRANSITION_H
#define QMOUSEEVENTTRANSITION_H

//Qt
#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "qeventtransition.h"
#else
# include <QtCore/qeventtransition.h>
#endif
QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QMouseEventTransitionPrivate;
class QPainterPath;
class Q_GUI_EXPORT QMouseEventTransition : public QEventTransition
{
    Q_OBJECT
    Q_PROPERTY(Qt::MouseButton button READ button WRITE setButton)
    Q_PROPERTY(Qt::KeyboardModifiers modifiersMask READ modifiersMask WRITE setModifiersMask)
public:
    QMouseEventTransition(QState *sourceState = 0);
    QMouseEventTransition(QObject *object, QEvent::Type type,
                          Qt::MouseButton button, QState *sourceState = 0);
    QMouseEventTransition(QObject *object, QEvent::Type type,
                          Qt::MouseButton button,
                          const QList<QAbstractState*> &targets,
                          QState *sourceState = 0);
    ~QMouseEventTransition();

    Qt::MouseButton button() const;
    void setButton(Qt::MouseButton button);

    Qt::KeyboardModifiers modifiersMask() const;
    void setModifiersMask(Qt::KeyboardModifiers modifiers);

    QPainterPath path() const;
    void setPath(const QPainterPath &path);

protected:
    void onTransition();
    bool eventTest(QEvent *event) const;

private:
    Q_DISABLE_COPY(QMouseEventTransition)
    Q_DECLARE_PRIVATE(QMouseEventTransition)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
