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
    color: "white"
    width: 800
    height: 600

    ListModel {
        id: model
        ListElement { name: "1" }
        ListElement { name: "2" }
        ListElement { name: "3" }
        ListElement { name: "4" }
        ListElement { name: "5" }
        ListElement { name: "6" }
        ListElement { name: "6" }
        ListElement { name: "8" }
        ListElement { name: "9" }
    }

    Component {
        id: verticalDelegate
        FocusScope {
            id: root
            width: 50; height: 50;
            Keys.onDigit9Pressed: console.log("Error - " + name)
            Rectangle {
                focus: true
                Keys.onDigit9Pressed: console.log(name)
                width: 50; height: 50;
                color: root.ListView.isCurrentItem?"red":"green"
                Text { text: name; anchors.centerIn: parent }
            }
        }
    }

    ListView {
        width: 800; height: 50; orientation: "Horizontal"
        focus: true
        model: model
        delegate: verticalDelegate
        preferredHighlightBegin: 100
        preferredHighlightEnd: 100
        highlightRangeMode: "StrictlyEnforceRange"
    }


    Text {
        y: 100; x: 50
        text: "Currently selected element should be red\nPressing \"9\" should print the number of the currently selected item\nBe sure to scroll all the way to the right, pause, and then all the way to the left."
    }
}
