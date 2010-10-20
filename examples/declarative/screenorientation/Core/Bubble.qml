/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

import QtQuick 1.0

Rectangle {
    property bool rising: false
    property bool verticalRise: true
    property real xAttractor: 0
    property real yAttractor: 0

    width: 5 + 10*Math.random()
    height: width
    radius: Math.floor(width/2)-1
    property real amountOfGray: Math.random()
    color: Qt.rgba(amountOfGray,amountOfGray,amountOfGray,1)

    y: (rising && verticalRise) ? yAttractor : Math.random()*(main.inPortrait ? main.baseHeight : main.baseWidth)
    x: (rising && !verticalRise) ? xAttractor : Math.random()*(main.inPortrait ? main.baseWidth : main.baseHeight)
    Behavior on x {
        id: xBehavior
        SmoothedAnimation { 
            velocity: 100+Math.random()*100 
        } 
    }
    Behavior on y { 
        id: yBehavior
        SmoothedAnimation { 
            velocity: 100+Math.random()*100 
        } 
    }
    Timer {
       interval: 80+Math.random()*40 
        repeat: true
        running: true
        onTriggered: {
            if (rising) {
                if (x > main.width || x < 0) {
                    xBehavior.enabled = false;
                    rising = false;
                    xBehavior.enabled = true;
                    rising = true;
                }
                if (y > main.height || y < 0) {
                    yBehavior.enabled = false;
                    rising = false;
                    yBehavior.enabled = true;
                    rising = true;
                }  
            }
        }
    }
}
