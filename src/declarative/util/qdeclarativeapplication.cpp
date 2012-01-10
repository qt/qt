/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativeapplication_p.h"
#include <private/qobject_p.h>
#include <QtGui/QApplication>

QT_BEGIN_NAMESPACE

class QDeclarativeApplicationPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeApplication)
public:
    QDeclarativeApplicationPrivate() : active(QApplication::activeWindow() != 0),
    layoutDirection(QApplication::layoutDirection()) {}
    bool active;
    Qt::LayoutDirection layoutDirection;
};

/*
    This object and its properties are documented as part of the Qt object,
    in qdeclarativengine.cpp
*/

QDeclarativeApplication::QDeclarativeApplication(QObject *parent) : QObject(*new QDeclarativeApplicationPrivate(), parent)
{
    if (qApp)
        qApp->installEventFilter(this);
}

QDeclarativeApplication::~QDeclarativeApplication()
{
}

bool QDeclarativeApplication::active() const
{
    Q_D(const QDeclarativeApplication);
    return d->active;
}

Qt::LayoutDirection QDeclarativeApplication::layoutDirection() const
{
    Q_D(const QDeclarativeApplication);
    return d->layoutDirection;
}

bool QDeclarativeApplication::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)
    Q_D(QDeclarativeApplication);
    if (event->type() == QEvent::ApplicationActivate
     || event->type() == QEvent::ApplicationDeactivate) {
        bool active = d->active;
        if (event->type() == QEvent::ApplicationActivate)
            active  = true;
        else if (event->type() == QEvent::ApplicationDeactivate)
            active  = false;

        if (d->active != active) {
            d->active = active;
            emit activeChanged();
        }
    }
    if (event->type() == QEvent::LayoutDirectionChange) {
        Qt::LayoutDirection direction = QApplication::layoutDirection();
        if (d->layoutDirection != direction) {
            d->layoutDirection = direction;
            emit layoutDirectionChanged();
        }
    }
    return false;
}

QT_END_NAMESPACE
