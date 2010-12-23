/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
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

class QDeclarativeApplicationPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeApplication)
public:
    QDeclarativeApplicationPrivate() : active(QApplication::activeWindow() != 0) {}
    bool active;
};

/*!
    \qmlclass Application QDeclarativeApplication
    \since QtQuick 1.1
    \ingroup qml-utility-elements
    \brief The Application element provides access to global application
    state properties shared by many QML components.

    These properties include application activity property \c active.

    \section1 Example Usage

    The following example shows the simplest usage of the Application element.

    \snippet doc/src/snippets/declarative/application.qml document

*/

QDeclarativeApplication::QDeclarativeApplication(QObject *parent) : QObject(*new QDeclarativeApplicationPrivate(), parent)
{
    if (qApp)
        qApp->installEventFilter(this);
}

QDeclarativeApplication::~QDeclarativeApplication()
{
}

/*!
    \qmlproperty bool Application::active

    This property indicates whether the application is the top-most and focused
    application, and user is able to interact with the application. The property
    is false when the application is on the background, device keylock or screen
    saver is active, the screen backlight is turned off, or global system dialog
    is being displayed on top of the application. It can be used for stopping and
    pausing animations, timers, active processing of data to save device battery
    and free device memory and processor load when the application is not active.
    This property is readonly.

*/
bool QDeclarativeApplication::active() const
{
    Q_D(const QDeclarativeApplication);
    return d->active;
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
    return false;
}
