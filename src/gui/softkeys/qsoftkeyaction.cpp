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

#include "qsoftkeyaction.h"

/*!
    \class QSoftKeyAction
    \brief The QSoftKeyAction class defines a special kind of QAction that may also be displayed in a QSoftKeyBar.
*/

class QSoftKeyActionPrivate
{
public:
    QSoftKeyActionPrivate(QSoftKeyAction::StandardRole role = QSoftKeyAction::Custom)
    {
        this->role = role;
    }

    QSoftKeyAction::StandardRole role;
    QString roleName;
    int nativePosition;
    int qtContextKey;
};

/*!
    \enum QSoftKeyAction::StandardRole
    This enum defines the standard role for a QSoftKeyAction.

    \value Options
    \value Select
    \value Back
    \value Next
    \value Previous
    \value Ok
    \value Cancel
    \value Edit
    \value View
    \value BackSpace
    \value EndEdit
    \value RevertEdit
    \value Deselect
    \value Finish
    \value Custom
*/

QSoftKeyAction::QSoftKeyAction(QObject *parent)
    : QAction(parent)
{
    d = new QSoftKeyActionPrivate();
}

QSoftKeyAction::QSoftKeyAction(QSoftKeyAction::StandardRole role, QObject *parent)
    : QAction(parent)
{
    d = new QSoftKeyActionPrivate(role);
}

QSoftKeyAction::QSoftKeyAction(const QString &text, QObject* parent)
    : QAction(text, parent)
{
    d = new QSoftKeyActionPrivate();
}

QSoftKeyAction::QSoftKeyAction(QSoftKeyAction::StandardRole role, const QString &text, QObject* parent)
    : QAction(text, parent)
{
    d = new QSoftKeyActionPrivate(role);
}

QSoftKeyAction::QSoftKeyAction(const QIcon &icon, const QString &text, QObject* parent)
    : QAction(icon, text, parent)
{
    d = new QSoftKeyActionPrivate();
}

QSoftKeyAction::QSoftKeyAction(QSoftKeyAction::StandardRole role, const QIcon &icon, const QString &text, QObject* parent)
    : QAction(icon, text, parent)
{
    d = new QSoftKeyActionPrivate(role);
}

QSoftKeyAction::~QSoftKeyAction()
{
    delete d;
}

/*!
    Returns the standard role associated with this action, or Custom
    if the role is defined by roleName().

    \sa setRole(), roleName()
*/
QSoftKeyAction::StandardRole QSoftKeyAction::role() const
{
    return d->role;
}

/*!
    Returns the custom role name if role() is Custom, or an empty
    string otherwise.

    \sa role(), setRole()
*/
QString QSoftKeyAction::roleName() const
{
    return d->roleName;
}

/*!
    Sets the standard role associated with this action to \a role,
    and also sets roleName() to the empty string.

    \sa role(), roleName()
*/
void QSoftKeyAction::setRole(QSoftKeyAction::StandardRole role)
{
    d->role = role;
    d->roleName = QString();
    emit changed();
}

/*!
    Sets the custom roleName() associated with this action to \a role,
    and also sets role() to Custom.

    \sa role(), roleName()
*/
void QSoftKeyAction::setRole(const QString& role)
{
    d->role = QSoftKeyAction::Custom;
    d->roleName = role;
    emit changed();
}

int QSoftKeyAction::nativePosition()
{
    return d->nativePosition;
}

void QSoftKeyAction::setNativePosition(int position)
{
    d->nativePosition = position;
}

int QSoftKeyAction::qtContextKey()
{
    return d->qtContextKey;
}

void QSoftKeyAction::setQtContextKey(int key)
{
    d->qtContextKey = key;
}
