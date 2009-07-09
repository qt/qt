/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Anomaly project on Qt Labs.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 or 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtCore>
#include <QtGui>
#include <QtWebKit>

#include "BrowserWindow.h"

int main(int argc, char *argv[])
{
#if !defined(Q_WS_S60)
    QApplication::setGraphicsSystem("raster");
#endif

    QApplication app(argc, argv);

    app.setApplicationName("Anomaly");
    app.setApplicationVersion("0.0.0");

    BrowserWindow window;
#ifdef Q_OS_SYMBIAN
    window.showFullScreen();
    QWebSettings::globalSettings()->setObjectCacheCapacities(128*1024, 1024*1024, 1024*1024);
    QWebSettings::globalSettings()->setMaximumPagesInCache(3);
#else
    window.resize(360, 640);
    window.show();
    app.setStyle("windows");
#endif

    return app.exec();
}

