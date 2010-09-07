/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7

Item {
    id: titleBar
    property string untaggedString: "Uploads from everyone"
    property string taggedString: "Recent uploads tagged "

    BorderImage { source: "images/titlebar.sci"; width: parent.width; height: parent.height + 14; y: -7 }

    Item {
        id: container
        width: (parent.width * 2) - 55 ; height: parent.height

        function accept() {
            imageDetails.closed()
            titleBar.state = ""
            background.state = ""
            rssModel.tags = editor.text
        }

        Image {
            id: quitButton
            anchors.left: parent.left//; anchors.leftMargin: 0
            anchors.verticalCenter: parent.verticalCenter
            source: "images/quit.png"
            MouseArea {
                anchors.fill: parent
                onClicked: Qt.quit()
            }
        }

        Text {
            id: categoryText
            anchors {
                left: quitButton.right; right: tagButton.left; leftMargin: 10; rightMargin: 10
                verticalCenter: parent.verticalCenter
            }
            elide: Text.ElideLeft
            text: (rssModel.tags=="" ? untaggedString : taggedString + rssModel.tags)
            font.bold: true; color: "White"; style: Text.Raised; styleColor: "Black"
        }

        Button {
            id: tagButton; x: titleBar.width - 50; width: 45; height: 32; text: "..."
            onClicked: if (titleBar.state == "Tags") container.accept(); else titleBar.state = "Tags"
            anchors.verticalCenter: parent.verticalCenter
        }

        Item {
            id: lineEdit
            y: 4; height: parent.height - 9
            anchors { left: tagButton.right; leftMargin: 5; right: parent.right; rightMargin: 5 }

            BorderImage { source: "images/lineedit.sci"; anchors.fill: parent }

            TextInput {
                id: editor
                anchors {
                    left: parent.left; right: parent.right; leftMargin: 10; rightMargin: 10
                    verticalCenter: parent.verticalCenter
                }
                cursorVisible: true; font.bold: true
                color: "#151515"; selectionColor: "Green"
            }

            Keys.forwardTo: [ (returnKey), (editor)]

            Item {
                id: returnKey
                Keys.onReturnPressed: container.accept()
                Keys.onEnterPressed: container.accept()
                Keys.onEscapePressed: titleBar.state = ""
            }
        }
    }

    states: State {
        name: "Tags"
        PropertyChanges { target: container; x: -tagButton.x + 5 }
        PropertyChanges { target: tagButton; text: "OK" }
        PropertyChanges { target: editor; focus: true }
    }

    transitions: Transition {
        NumberAnimation { properties: "x"; easing.type: Easing.InOutQuad }
    }
}
