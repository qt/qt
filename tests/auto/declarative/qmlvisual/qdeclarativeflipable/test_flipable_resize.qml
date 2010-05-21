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
Item { //realWindow
    width: 370
    height: 480
    Item {
        id: window
        NumberAnimation on width{ from:320; to:370; duration: 500 }
        NumberAnimation on height{ from:480; to:320; duration: 500 }
        Flipable {
            id: flipable
            x: 0
            y: window.height / 3.0 - 40
            width: parent.width
            height: parent.height
            transform: Rotation {
                id: transform
                origin.x: window.width / 2.0
                origin.y: 0
                origin.z: 0
                axis.x: 0
                axis.y: 1
                axis.z: 0
                angle: 0;
            }
            front: Rectangle{
                width: parent.width
                height: 80
                color: "blue"
            }back: Rectangle{
                width: parent.width
                height: 80
                color: "red"
            }
        }
        Flipable {
            id: flipableBack
            x: 0
            y: 2.0 * window.height / 3.0 - 40
            width: parent.width
            height: parent.height
            transform: Rotation {
                id: transformBack
                origin.x: window.width / 2.0
                origin.y: 0
                origin.z: 0
                axis.x: 0
                axis.y: 1
                axis.z: 0
                angle: 180;
            }
            front: Rectangle{
                width: parent.width
                height: 80
                color: "blue"
            }back: Rectangle{
                width: parent.width
                height: 80
                color: "red"
            }
        }
    }
}
