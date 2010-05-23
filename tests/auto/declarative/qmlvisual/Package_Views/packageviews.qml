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
    width: 200
    height: 200
    color: "black"

    VisualDataModel {
        id: model
        model: ListModel {
            ListElement { itemColor: "red" }
            ListElement { itemColor: "green" }
            ListElement { itemColor: "blue" }
            ListElement { itemColor: "orange" }
            ListElement { itemColor: "purple" }
            ListElement { itemColor: "yellow" }
            ListElement { itemColor: "slategrey" }
            ListElement { itemColor: "cyan" }
            ListElement { itemColor: "red" }
            ListElement { itemColor: "green" }
            ListElement { itemColor: "blue" }
            ListElement { itemColor: "orange" }
            ListElement { itemColor: "purple" }
            ListElement { itemColor: "yellow" }
            ListElement { itemColor: "slategrey" }
            ListElement { itemColor: "cyan" }
        }
        delegate: Package {
            Rectangle {
                id: listItem; Package.name: "list"; width:root.width/2; height: 50; color: "transparent"; border.color: "white"
                MouseArea {
                    anchors.fill: parent
                    onClicked: myState.state = myState.state == "list" ? "grid" : "list"
                }
            }
            Rectangle {
                id: gridItem; Package.name: "grid"; width:50; height: 50; color: "transparent"; border.color: "white"
                MouseArea {
                    anchors.fill: parent
                    onClicked: myState.state = myState.state == "list" ? "grid" : "list"
                }
            }
            Rectangle { id: myContent; width:50; height: 50; color: itemColor }

            StateGroup {
                id: myState
                state: "list"
                states: [
                    State {
                        name: "list"
                        ParentChange { target: myContent; parent: listItem }
                        PropertyChanges { target: myContent; x: 0; y: 0; width: listItem.width }
                    },
                    State {
                        name: "grid"
                        ParentChange { target: myContent; parent: gridItem }
                        PropertyChanges { target: myContent; x: 0; y: 0; width: gridItem.width }
                    }
                ]

                transitions: [
                    Transition {
                        from: "*"; to: "*"
                        SequentialAnimation {
                            ParentAnimation{
                                NumberAnimation { properties: "x,y,width"; easing.type: "InOutQuad" }
                            }
                        }
                    }
                ]
            }
        }
    }

    ListView {
        width: parent.width/2
        height: parent.height
        model: model.parts.list
    }

    GridView {
        x: parent.width/2
        width: parent.width/2
        cellWidth: 50
        cellHeight: 50
        height: parent.height
        model: model.parts.grid
    }
}
