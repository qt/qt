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
    width: 800; height: 240; color: "gray"

    Rectangle {
        id: rect
        width: 50; height: 20; y: 30; color: "black"
        SequentialAnimation on x {
            loops: Animation.Infinite
            NumberAnimation { from: 50; to: 700; duration: 2000 }
            NumberAnimation { from: 700; to: 50; duration: 2000 }
        }
    }

    Rectangle {
        width: 50; height: 20; y: 60; color: "red"
        SmoothedFollow on x { to: rect.x; velocity: 400; enabled: true }
    }

    Rectangle {
        width: 50; height: 20; y: 90; color: "yellow"
        SmoothedFollow on x { to: rect.x; velocity: 300; reversingMode: SmoothedAnimation.Immediate; enabled: true }
    }

    Rectangle {
        width: 50; height: 20; y: 120; color: "green"
        SmoothedFollow on x { to: rect.x; reversingMode: SmoothedAnimation.Sync; enabled: true }
    }

    Rectangle {
        width: 50; height: 20; y: 150; color: "purple"
        SmoothedFollow on x { to: rect.x; maximumEasingTime: 200; enabled: true }
    }

    Rectangle {
        width: 50; height: 20; y: 180; color: "blue"
        SmoothedFollow on x { to: rect.x; duration: 300; enabled: true }
    }
}
