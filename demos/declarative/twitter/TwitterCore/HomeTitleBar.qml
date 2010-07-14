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

Item {
    id: titleBar

    signal update()
    onYChanged: state="" //When switching titlebars

    BorderImage { source: "images/titlebar.sci"; width: parent.width; height: parent.height + 14; y: -7 }
    Item {
        id: container
        width: (parent.width * 2) - 55 ; height: parent.height

        function accept() {
            if(rssModel.authName == '' || rssModel.authPass == '')
                return false;//Can't login like that

            var postData = "status=" + editor.text;
            var postman = new XMLHttpRequest();
            postman.open("POST", "http://twitter.com/statuses/update.xml", true, rssModel.authName,  rssModel.authPass);
            postman.onreadystatechange = function() {
                if (postman.readyState == postman.DONE) {
                    titleBar.update();
                }
            }
            postman.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
            postman.send(postData);

            editor.text = ""
            titleBar.state = ""
        }

        Rectangle {
            x: 6; width: 50; height: 50; color: "white"; smooth: true
            anchors.verticalCenter: parent.verticalCenter

            UserModel { user: rssModel.authName; id: userModel }
            Component { id: imgDelegate;
                Item {
                    Loading { width:48; height:48; visible: realImage.status != Image.Ready }
                    Image { source: image; width:48; height:48; id: realImage }
                }
            }
            ListView { model: userModel.model; x:1; y:1; delegate: imgDelegate }
        }

        Text {
            id: categoryText
            anchors.left: parent.left; anchors.right: tagButton.left
            anchors.leftMargin: 58; anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideLeft
            text: "Timeline for " + rssModel.authName
            font.pixelSize: 12; font.bold: true; color: "white"; style: Text.Raised; styleColor: "black"
        }

        Button {
            id: tagButton; x: titleBar.width - 90; width: 85; height: 32; text: "New Post..."
            anchors.verticalCenter: parent.verticalCenter;
            onClicked: if (titleBar.state == "Posting") container.accept(); else titleBar.state = "Posting"
        }

        Text {
            id: charsLeftText; anchors.horizontalCenter: tagButton.horizontalCenter;
            anchors.top: tagButton.bottom; anchors.topMargin: 2
            text: {140 - editor.text.length;} visible: titleBar.state == "Posting"
            font.pointSize: 10; font.bold: true; color: "white"; style: Text.Raised; styleColor: "black"
        }
        Item {
            id: txtEdit;
            anchors.left: tagButton.right; anchors.leftMargin: 5; y: 4
            anchors.right: parent.right; anchors.rightMargin: 40; height: parent.height - 9
            BorderImage { source: "images/lineedit.sci"; anchors.fill: parent }

            Binding {//TODO: Can this be a function, which also resets the cursor? And flashes?
                when: editor.text.length > 140
                target: editor
                property: "text"
                value: editor.text.slice(0,140)
            }
            TextEdit {
                id: editor
                anchors.left: parent.left;
                anchors.leftMargin: 8;
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 4;
                cursorVisible: true; font.bold: true
                width: parent.width - 12
                height: parent.height - 8
                font.pixelSize: 12
                wrapMode: TextEdit.Wrap
                color: "#151515"; selectionColor: "green"
            }
            Keys.forwardTo: [(returnKey), (editor)]
            Item {
                id: returnKey
                Keys.onReturnPressed: container.accept()
                Keys.onEnterPressed: container.accept()
                Keys.onEscapePressed: titleBar.state = ""
            }
        }
    }
    states: [
        State {
            name: "Posting"
            PropertyChanges { target: container; x: -tagButton.x + 5 }
            PropertyChanges { target: titleBar; height: 80 }
            PropertyChanges { target: tagButton; text: "OK" }
            PropertyChanges { target: tagButton; width: 28 }
            PropertyChanges { target: tagButton; height: 24 }
            PropertyChanges { target: editor; focus: true }
        }
    ]
    transitions: [
        Transition {
            from: "*"; to: "*"
            NumberAnimation { properties: "x,y,width,height"; easing.type: Easing.InOutQuad }
        }
    ]
}
