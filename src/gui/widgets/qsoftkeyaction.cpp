/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsoftkeyaction.h"
#include <QtGui/private/qaction_p.h>

/*!
    \class QSoftKeyAction
    \brief The QSoftKeyAction class defines a special kind of QAction that may also be displayed in a QSoftKeyBar.
*/

class QSoftKeyActionPrivate : public QActionPrivate
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
    : QAction(*new QSoftKeyActionPrivate(), parent)
{
}

QSoftKeyAction::QSoftKeyAction(StandardRole role, QObject *parent)
    : QAction(*new QSoftKeyActionPrivate(), parent)
{
    Q_D(QSoftKeyAction);
    d->role = role;
}

QSoftKeyAction::QSoftKeyAction(const QString &text, QObject* parent)
    : QAction(*new QSoftKeyActionPrivate(), parent)
{
    setText(text);
}

QSoftKeyAction::QSoftKeyAction(StandardRole role, const QString &text, QObject* parent)
    : QAction(*new QSoftKeyActionPrivate(), parent)
{
    setText(text);
    Q_D(QSoftKeyAction);
    d->role = role;
}

QSoftKeyAction::QSoftKeyAction(const QIcon &icon, const QString &text, QObject* parent)
    : QAction(*new QSoftKeyActionPrivate(), parent)
{
    setIcon(icon);
    setText(text);
}

QSoftKeyAction::QSoftKeyAction(StandardRole role, const QIcon &icon, const QString &text, QObject* parent)
    : QAction(*new QSoftKeyActionPrivate(), parent)
{
    setIcon(icon);
    setText(text);
    Q_D(QSoftKeyAction);
    d->role = role;
}

QSoftKeyAction::~QSoftKeyAction()
{
}

/*!
    Returns the standard role associated with this action, or Custom
    if the role is defined by roleName().

    \sa setRole(), roleName()
*/
QSoftKeyAction::StandardRole QSoftKeyAction::role() const
{
    Q_D(const QSoftKeyAction);
    return d->role;
}

/*!
    Returns the custom role name if role() is Custom, or an empty
    string otherwise.

    \sa role(), setRole()
*/
QString QSoftKeyAction::roleName() const
{
    Q_D(const QSoftKeyAction);
    return d->roleName;
}

/*!
    Sets the standard role associated with this action to \a role,
    and also sets roleName() to the empty string.

    \sa role(), roleName()
*/
void QSoftKeyAction::setRole(StandardRole role)
{
    Q_D(QSoftKeyAction);
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
    Q_D(QSoftKeyAction);
    d->role = Custom;
    d->roleName = role;
    emit changed();
}

int QSoftKeyAction::nativePosition() const
{
    Q_D(const QSoftKeyAction);
    return d->nativePosition;
}

void QSoftKeyAction::setNativePosition(int position)
{
    Q_D(QSoftKeyAction);
    d->nativePosition = position;
}

int QSoftKeyAction::qtContextKey() const
{
    Q_D(const QSoftKeyAction);
    return d->qtContextKey;
}

void QSoftKeyAction::setQtContextKey(int key)
{
    Q_D(QSoftKeyAction);
    d->qtContextKey = key;
}
