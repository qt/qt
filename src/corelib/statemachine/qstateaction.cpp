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

#include "qstateaction.h"
#include "qstateaction_p.h"
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qmetaobject.h>

QT_BEGIN_NAMESPACE

QStateActionPrivate::QStateActionPrivate()
{
    when = ExecuteOnEntry;
}

QStateActionPrivate::~QStateActionPrivate()
{
}

QStateActionPrivate *QStateActionPrivate::get(QStateAction *q)
{
    return q->d_func();
}

void QStateActionPrivate::callExecute()
{
    Q_Q(QStateAction);
    q->execute();
}

/*!
  \class QStateAction

  \brief The QStateAction class is the base class of QState actions.

  \ingroup statemachine

  A state action is added to a state by calling QState::addEntryAction() or
  QState::addExitAction(). QStateAction is part of \l{The State Machine
  Framework}.

  \section1 Subclassing

  Subclasses must implement the execute() function.
*/

/*!
  Constructs a new QStateAction object with the given \a parent.
*/
QStateAction::QStateAction(QObject *parent)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        *new QStateActionPrivate,
#endif
        parent)
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(new QStateActionPrivate)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
}

/*!
  \internal
*/
QStateAction::QStateAction(QStateActionPrivate &dd, QObject *parent)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        dd,
#endif
        parent)
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(&dd)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
}

/*!
  Destroys this QStateAction object.
*/
QStateAction::~QStateAction()
{
#ifdef QT_STATEMACHINE_SOLUTION
    delete d_ptr;
#endif
}

/*!
  \fn QStateAction::execute()

  Executes this action.
*/

/*!
  \reimp
*/
bool QStateAction::event(QEvent *e)
{
    return QObject::event(e);
}

QStateSetPropertyActionPrivate *QStateSetPropertyActionPrivate::get(QStateSetPropertyAction *q)
{
    return q->d_func();
}

/*!
  \class QStateSetPropertyAction

  \brief The QStateSetPropertyAction class provides a set property action for QObjects.

  \ingroup statemachine

  The QStateSetPropertyAction class provides an action that sets a property of
  a QObject to a pre-defined value when a QState is entered or exited.
  QStateSetPropertyAction is part of \l{The State Machine Framework}.

  Typically you don't construct QStateSetPropertyAction objects directly, but
  rather call the QState::setPropertyOnEntry() function or the
  QState::setPropertyOnExit() function.
*/

/*!
    \property QStateSetPropertyAction::target

    \brief the object for which this action sets a property
*/

/*!
    \property QStateSetPropertyAction::propertyName

    \brief the name of the property set by this action
*/

/*!
    \property QStateSetPropertyAction::value

    \brief the value set by this action
*/

/*!
  Constructs a new QStateSetPropertyAction object for the property named \a
  propertyName of the given \a target object, with the given \a value, and
  with the given \a parent.
*/
QStateSetPropertyAction::QStateSetPropertyAction(
    QObject *target, const QByteArray &propertyName,
    const QVariant &value, QObject *parent)
    : QStateAction(*new QStateSetPropertyActionPrivate, parent)
{
    Q_D(QStateSetPropertyAction);
    d->target = target;
    d->propertyName = propertyName;
    d->value = value;
}

/*!
  Constructs a new QStateSetPropertyAction object with the given \a parent.
*/
QStateSetPropertyAction::QStateSetPropertyAction(QObject *parent)
    : QStateAction(*new QStateSetPropertyActionPrivate, parent)
{
    Q_D(QStateSetPropertyAction);
    d->target = 0;
}

/*!
  Destroys this QStateAbstractSetPropertyAction object.
*/
QStateSetPropertyAction::~QStateSetPropertyAction()
{
}

/*!
  Returns the object for which this action sets a property.
*/
QObject *QStateSetPropertyAction::targetObject() const
{
    Q_D(const QStateSetPropertyAction);
    return d->target;
}

/*!
  Sets the object for which this action sets a property.
*/
void QStateSetPropertyAction::setTargetObject(QObject *target)
{
    Q_D(QStateSetPropertyAction);
    d->target = target;
}

/*!
  Returns the name of the property set by this action.
*/
QByteArray QStateSetPropertyAction::propertyName() const
{
    Q_D(const QStateSetPropertyAction);
    return d->propertyName;
}

/*!
  Sets the name of the property set by this action.
*/
void QStateSetPropertyAction::setPropertyName(const QByteArray &propertyName)
{
    Q_D(QStateSetPropertyAction);
    d->propertyName = propertyName;
}

/*!
  Returns the value set by this action.
*/
QVariant QStateSetPropertyAction::value() const
{
    Q_D(const QStateSetPropertyAction);
    return d->value;
}

/*!
  Sets the value set by this action.
*/
void QStateSetPropertyAction::setValue(const QVariant &value)
{
    Q_D(QStateSetPropertyAction);
    d->value = value;
}

/*!
  \reimp
*/
void QStateSetPropertyAction::execute()
{
    Q_D(QStateSetPropertyAction);
    if (!d->target)
        return;
    d->target->setProperty(d->propertyName, d->value);
}

/*!
  \reimp
*/
bool QStateSetPropertyAction::event(QEvent *e)
{
    return QStateAction::event(e);
}

QStateInvokeMethodActionPrivate *QStateInvokeMethodActionPrivate::get(QStateInvokeMethodAction *q)
{
    return q->d_func();
}

/*!
  \class QStateInvokeMethodAction

  \brief The QStateInvokeMethodAction class provides an invoke method action for QObjects.

  \ingroup statemachine

  The QStateInvokeMethodAction class provides an action that calls a method of
  a QObject when a QState is entered or exited. QStateInvokeMethodAction is
  part of \l{The State Machine Framework}.

  Typically you don't construct QStateInvokeMethodAction objects directly, but
  rather call the QState::invokeMethodOnEntry() function or the
  QState::invokeMethodOnExit() function.
*/

/*!
    \property QStateInvokeMethodAction::arguments

    \brief the arguments to the method this action invokes
*/

/*!
    \property QStateInvokeMethodAction::methodName

    \brief the name of the method this action invokes
*/

/*!
    \property QStateInvokeMethodAction::target

    \brief the object on which this action invokes a method
*/

/*!
  Constructs a new QStateInvokeMethodAction object for the method named \a
  methodName of the given \a target object, with the given \a parent.
*/
QStateInvokeMethodAction::QStateInvokeMethodAction(
    QObject *target, const QByteArray &methodName, QObject *parent)
    : QStateAction(*new QStateInvokeMethodActionPrivate, parent)
{
    Q_D(QStateInvokeMethodAction);
    d->target = target;
    d->methodName = methodName;
    d->methodIndex = -1;
}

/*!
  Constructs a new QStateInvokeMethodAction object for the method named \a
  methodName of the given \a target object, with the given arguments, \a args,
  and with the given \a parent.
*/
QStateInvokeMethodAction::QStateInvokeMethodAction(
    QObject *target, const QByteArray &methodName,
    const QList<QVariant> &args, QObject *parent)
    : QStateAction(*new QStateInvokeMethodActionPrivate, parent)
{
    Q_D(QStateInvokeMethodAction);
    d->target = target;
    d->methodName = methodName;
    d->methodIndex = -1;
    d->args = args;
}

/*!
  Constructs a new QStateInvokeMethodAction object with the given \a parent.
*/
QStateInvokeMethodAction::QStateInvokeMethodAction(QObject *parent)
    : QStateAction(*new QStateInvokeMethodActionPrivate, parent)
{
    Q_D(QStateInvokeMethodAction);
    d->target = 0;
    d->methodIndex = -1;
}

/*!
  Destroys this QStateInvokeMethodAction object.
*/
QStateInvokeMethodAction::~QStateInvokeMethodAction()
{
}

/*!
  \reimp
*/
void QStateInvokeMethodAction::execute()
{
    Q_D(QStateInvokeMethodAction);
    if (!d->target)
        return;

    if (d->methodIndex == -1) {
        QVarLengthArray<char, 512> sig;
        int len = d->methodName.length();
        if (len <= 0)
            return;
        sig.append(d->methodName, len);
        sig.append('(');

        int paramCount;
        for (paramCount = 0; paramCount < d->args.size() && paramCount < 10; ++paramCount) {
            const char *tn = d->args.at(paramCount).typeName();
            len = qstrlen(tn);
            if (len <= 0)
                break;
            sig.append(tn, len);
            sig.append(',');
        }
        if (paramCount == 0)
            sig.append(')'); // no parameters
        else
            sig[sig.size() - 1] = ')';
        sig.append('\0');

        const QMetaObject *meta = d->target->metaObject();
        int idx = meta->indexOfMethod(sig.constData());
        if (idx < 0) {
            QByteArray norm = QMetaObject::normalizedSignature(sig.constData());
            idx = meta->indexOfMethod(norm.constData());
            if ((idx < 0) || (idx >= meta->methodCount())) {
                qWarning("InvokeMethodAction: unable to find method '%s' of %s(%p)",
                         sig.constData(), meta->className(), d->target);
                return;
            }
        }
        d->methodIndex = idx;
    }

    void *param[11];
    param[0] = 0; // return value
    for (int i = 0; i < 10; ++i)
        param[i+1] = (i < d->args.size()) ? const_cast<void*>(d->args.at(i).constData()) : (void*)0;
    (void)d->target->qt_metacall(QMetaObject::InvokeMetaMethod, d->methodIndex, param);
}

/*!
  Returns the object on which this action invokes a method.
*/
QObject *QStateInvokeMethodAction::targetObject() const
{
    Q_D(const QStateInvokeMethodAction);
    return d->target;
}

/*!
  Sets the object on which this action invokes a method.
*/
void QStateInvokeMethodAction::setTargetObject(QObject *target)
{
    Q_D(QStateInvokeMethodAction);
    d->target = target;
}

/*!
  Returns the name of the method this action will invoke.
*/
QByteArray QStateInvokeMethodAction::methodName() const
{
    Q_D(const QStateInvokeMethodAction);
    return d->methodName;
}

/*!
  Sets the name of the method this action will invoke.
*/
void QStateInvokeMethodAction::setMethodName(const QByteArray &methodName)
{
    Q_D(QStateInvokeMethodAction);
    if (methodName != d->methodName) {
        d->methodName = methodName;
        d->methodIndex = -1;
    }
}

/*!
  Returns the arguments to the method this action will invoke.
*/
QVariantList QStateInvokeMethodAction::arguments() const
{
    Q_D(const QStateInvokeMethodAction);
    return d->args;
}

/*!
  Sets the arguments to the method this action will invoke.
*/
void QStateInvokeMethodAction::setArguments(const QVariantList &arguments)
{
    Q_D(QStateInvokeMethodAction);
    if (d->args != arguments) {
        d->args = arguments;
        d->methodIndex = -1;
    }
}

/*!
  \reimp
*/
bool QStateInvokeMethodAction::event(QEvent *e)
{
    return QStateAction::event(e);
}

QT_END_NAMESPACE
