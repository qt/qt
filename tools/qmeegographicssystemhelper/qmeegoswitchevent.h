/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include <QtCore/QEvent>
#include <QtCore/QString>

//! A custom event representing a graphics system switch.
/*!
 This event is sent two times -- before the actual switch and after the switch.
 The current mode of the event can be detected by looking at the State of the
 event.

 The end-user application can use the event to drop it's own allocated GL resources
 when going to software mode.
*/

class Q_DECL_EXPORT QMeeGoSwitchEvent : public QEvent
{
public:

    //! The state represented by this event.
    enum State {
        WillSwitch,
        DidSwitch
    };

    //! Constructor for the event.
    /*!
     Creates a new event with the given name and the given state.
    */
    QMeeGoSwitchEvent(const QString &graphicsSystemName, State s);

    //! Returns the name of the target graphics system.
    /*!
     Depending on the state, the name represents the system we're about to swtich to,
     or the system we just switched to.
    */
    QString graphicsSystemName() const;

    //! Returns the state represented by this event.
    State state() const;

    //! Returns the event type/number for QMeeGoSwitchEvent.
    /*!
     The type is registered on first access. Use this to detect incoming
     QMeeGoSwitchEvents.
    */
    static QEvent::Type eventNumber();

private:
    QString name;
    State switchState;
};
