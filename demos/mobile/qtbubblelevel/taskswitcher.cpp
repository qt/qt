/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#include "taskswitcher.h"

// Task Switcher for maemo
#ifdef Q_WS_MAEMO_5
    #include <QtDBus/QtDBus>
#endif


void TaskSwitcher::minimizeApplication()
{
    //! [0]
    #ifdef Q_WS_MAEMO_5
    // Uses DBus to minimize application in Maemo
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusMessage message =
            QDBusMessage::createSignal("/","com.nokia.hildon_desktop",
                                       "exit_app_view");
    connection.send(message);
    #endif
    //! [0]
}
