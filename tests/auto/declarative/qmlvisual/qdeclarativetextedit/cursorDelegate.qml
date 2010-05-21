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
    resources: [
        Component { id: cursorA
            Item { id: cPage;
                x: Behavior { NumberAnimation { } }
                y: Behavior { NumberAnimation { } }
                height: Behavior { NumberAnimation { duration: 200 } }
                Rectangle { id: cRectangle; color: "black"; y: 1; width: 1; height: parent.height-2;
                    Rectangle { id:top; color: "black"; width: 3; height: 1; x: -1; y:0}
                    Rectangle { id:bottom; color: "black"; width: 3; height: 1; x: -1; anchors.bottom: parent.bottom;}
                    opacity: 1
                    opacity: SequentialAnimation { running: cPage.parent.focus == true; loops: Animation.Infinite;
                                NumberAnimation { properties: "opacity"; to: 1; duration: 500; easing.type: "InQuad"}
                                NumberAnimation { properties: "opacity"; to: 0; duration: 500; easing.type: "OutQuad"}
                             }
                }
                width: 1;
            }
        }
    ]
    width: 400
    height: 200
    color: "white"
    TextEdit { id: mainText
        text: "Hello World"
        cursorDelegate: cursorA
        focus: true
        font.pixelSize: 28
        selectionColor: "lightsteelblue"
        selectedTextColor: "deeppink"
        color: "forestgreen"
        anchors.centerIn: parent
    }
}
