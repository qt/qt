/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt QML Debugger of the Qt Toolkit.
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
#include <QtGui/qapplication.h>

#include "qmldebugger.h"

QT_USE_NAMESPACE

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    app.setApplicationName("QtQmlDebugger");
    app.setOrganizationName("Nokia");
    app.setOrganizationDomain("nokia.com");    

    QStringList args = app.arguments();

    QmlDebugger win;
    if (args.contains("--engine"))
        win.showEngineTab();

    for (int i=0; i<args.count(); i++) {
        if (!args[i].contains(':'))
            continue;
        QStringList hostAndPort = args[i].split(':');
        bool ok = false;
        quint16 port = hostAndPort.value(1).toInt(&ok);
        if (ok) {
            qWarning() << "qmldebugger connecting to"
                    << hostAndPort[0] << port << "...";
            win.setHost(hostAndPort[0]);
            win.setPort(port);
            win.connectToHost();
            break;
        }
    }

    win.show();

    return app.exec();
}
