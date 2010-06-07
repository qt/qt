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
import "TwitterCore" 1.0 as Twitter

Item {
    id: screen; width: 320; height: 480
    property bool userView : false
    property variant tmpStr
    function setMode(m){
        screen.userView = m;
        if(m == false){
            rssModel.tags='my timeline';
            rssModel.reload();
            toolBar.button2Label = "View others";
        } else {
            toolBar.button2Label = "Return home";
        }
    }
    function setUser(str){hack.running = true; tmpStr = str}
    function reallySetUser(){rssModel.tags = tmpStr;}

    //Workaround for bug 260266
    Timer{ interval: 1; running: false; repeat: false; onTriggered: screen.reallySetUser(); id:hack }

    //TODO: better way to return to the auth screen
    Keys.onEscapePressed: rssModel.authName=''
    Rectangle {
        id: background
        anchors.fill: parent; color: "#343434";

        Image { source: "TwitterCore/images/stripes.png"; fillMode: Image.Tile; anchors.fill: parent; opacity: 0.3 }

        MouseArea {
            anchors.fill: parent
            onClicked: screen.focus = false;
        }

        Twitter.RssModel { id: rssModel }
        Twitter.Loading { anchors.centerIn: parent; visible: rssModel.status==XmlListModel.Loading && state!='unauthed'}
        Text {
            width: 180
            text: "Could not access twitter using this screen name and password pair.";
            color: "#cccccc"; style: Text.Raised; styleColor: "black"; wrapMode: Text.WordWrap
            visible: rssModel.status==XmlListModel.Error; anchors.centerIn: parent
        }

        Item {
            id: views
            x: 2; width: parent.width - 4
            y:60 //Below the title bars
            height: 380

            Twitter.AuthView{
                id: authView
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width; height: parent.height-60;
                x: -(screen.width * 1.5)
            }

            Twitter.FatDelegate { id: fatDelegate }
            ListView {
                id: mainView; model: rssModel.model; delegate: fatDelegate;
                width: parent.width; height: parent.height; x: 0; cacheBuffer: 100;
            }
        }

        Twitter.MultiTitleBar { id: titleBar; width: parent.width }
        Twitter.ToolBar { id: toolBar; height: 40;
            //anchors.bottom: parent.bottom;
            //TODO: Use anchor changes instead of hard coding
            y: screen.height - 40
            width: parent.width; opacity: 0.9
            button1Label: "Update"
            button2Label: "View others"
            onButton1Clicked: rssModel.reload();
            onButton2Clicked:
            {
                if(screen.userView == true){
                    screen.setMode(false);
                }else{
                    rssModel.tags='';
                    screen.setMode(true);
                }
            }
        }

        states: [
            State {
                name: "unauthed"; when: rssModel.authName==""
                PropertyChanges { target: authView; x: 0 }
                PropertyChanges { target: mainView; x: -(parent.width * 1.5) }
                PropertyChanges { target: titleBar; y: -80 }
                PropertyChanges { target: toolBar; y: screen.height }
            }
        ]
        transitions: [
            Transition { NumberAnimation { properties: "x,y"; duration: 500; easing.type: Easing.InOutQuad } }
        ]
    }
}
