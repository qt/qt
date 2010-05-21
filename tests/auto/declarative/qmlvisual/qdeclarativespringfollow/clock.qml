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
    id: clock
    color: "gray"
    width: 200; height: 200

    property variant hours: 10
    property variant minutes: 28
    property variant seconds: 0

    Timer {
        interval: 1000; running: true; repeat: true; triggeredOnStart: true
        onTriggered: seconds++
    }

    Image { id: background; source: "content/clock.png" }

    Image {
        x: 92.5; y: 27
        source: "content/hour.png"
        smooth: true
        transform: Rotation {
            id: hourRotation
            origin.x: 7.5; origin.y: 73; angle: 0
            SpringFollow on angle {
                spring: 2; damping: 0.2; modulus: 360
                to: (clock.hours * 30) + (clock.minutes * 0.5)
            }
        }
    }

    Image {
        x: 93.5; y: 17
        source: "content/minute.png"
        smooth: true
        transform: Rotation {
            id: minuteRotation
            origin.x: 6.5; origin.y: 83; angle: 0
            SpringFollow on angle {
                spring: 2; damping: 0.2; modulus: 360
                to: clock.minutes * 6
            }
        }
    }

    Image {
        x: 97.5; y: 20
        source: "content/second.png"
        smooth: true
        transform: Rotation {
            id: secondRotation
            origin.x: 2.5; origin.y: 80; angle: 0
            SpringFollow on angle {
                spring: 5; damping: 0.25; modulus: 360
                to: clock.seconds * 6
            }
        }
    }

    Image {
        anchors.centerIn: background; source: "content/center.png"
    }
}
