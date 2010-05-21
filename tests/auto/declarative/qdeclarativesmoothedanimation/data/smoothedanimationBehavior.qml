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

import Qt 4.7

Rectangle {
     width: 400; height: 400; color: "blue"

    Rectangle {
        id: rect1
        color: "red"
        width: 60; height: 60;
        x: 100; y: 100;
        SmoothedAnimation on x { to: 200; velocity: 500 }
        SmoothedAnimation on y { to: 200; velocity: 500 }
    }

     Rectangle {
         objectName: "theRect"
         color: "green"
         width: 60; height: 60;
         x: rect1.x; y: rect1.y;
         // id are needed for SmoothedAnimation in order to avoid deferred creation
         Behavior on x { SmoothedAnimation { id: anim1; objectName: "easeX"; velocity: 400 } }
         Behavior on y { SmoothedAnimation { id: anim2; objectName: "easeY"; velocity: 400 } }
     }
 }
