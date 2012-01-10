/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0

Item {
    id: titleBar
    property string untaggedString: "Uploads from everyone"
    property string taggedString: "Recent uploads tagged "

    BorderImage { source: "images/titlebar.sci"; width: parent.width; height: parent.height + 14; y: -7 }

    Item {
        id: container
        width: (parent.width * 2) - 55 ; height: parent.height

        function accept() {
            titleBar.state = ""
            background.state = ""
            rssModel.tags = editor.text
        }

        Item {
            id:imageBox
            x: 6; width: 0; height: 50; smooth: true
            anchors.verticalCenter: parent.verticalCenter

            UserModel { user: rssModel.from; id: userModel }
            Component {
                id: imgDelegate;
                Item {
                    id:imageitem
                    visible:true
                    Loading { width:48; height:48; visible: realImage.status != Image.Ready }
                    Image { id: realImage; source: image; width:48; height:48; opacity:0; }
                    states:
                        State {
                        name: "loaded"
                        when:  (realImage.status == Image.Ready)
                        PropertyChanges { target: realImage; opacity:1 }
                    }
                    transitions: Transition {
                        NumberAnimation { target: realImage; property: "opacity"; duration: 200 }
                    }
                }
            }
            ListView { id:view; model: userModel.model; x:1; y:1; delegate: imgDelegate }
            states:
            State {
                when: !userModel.user==""
                PropertyChanges { target: imageBox; width: 50; }
            }
            transitions:
            Transition {
                NumberAnimation { target: imageBox; property: "width"; duration: 200 }
            }
        }

        Image {
            id: quitButton
            x: 5
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
                left: quitButton.right; right: parent.right; leftMargin: 10; rightMargin: 10
                verticalCenter: parent.verticalCenter
            }
            elide: Text.ElideLeft
            text: (rssModel.from=="" ? untaggedString : taggedString + rssModel.from)
            font.bold: true; color: "White"; style: Text.Raised; styleColor: "Black"
            font.pixelSize: 12
        }
    }

    states: State {
        name: "Tags"
        PropertyChanges { target: container; x: -tagButton.x + 5 }
        PropertyChanges { target: editor; focus: true }
    }

    transitions: Transition {
        NumberAnimation { properties: "x"; easing.type: Easing.InOutQuad }
    }
}
