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
    id: root

    width: 800
    height: 600

    property alias font: myText.font

    property int myPixelSize: 12
    property int myPixelSize2: 24
    
    Text {
        id: other
        font.pixelSize: 6
    }

    Text {
        id: myText

        text: "Hello world!"
        font.pixelSize: myPixelSize
    }

    states: State {
        name: "Swapped"
        PropertyChanges {
            target: myText
            font.pixelSize: myPixelSize2
        }
    }

    function toggle() {
        if (root.state == "") root.state = "Swapped"; else root.state = "";
    }

    MouseArea {
        anchors.fill: parent
        onClicked: { if (root.state == "") root.state = "Swapped"; else root.state = "";}
    }
}
