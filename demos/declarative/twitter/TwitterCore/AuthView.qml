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
    id: wrapper
    Column {
        anchors.centerIn: parent
        spacing: 20
        Column{
            spacing: 4
            Text {
                text: "Screen name:"
                font.pixelSize: 16; font.bold: true; color: "white"; style: Text.Raised; styleColor: "black"
                horizontalAlignment: Qt.AlignRight
            }
            Item {
                width: 220
                height: 28
                BorderImage { source: "images/lineedit.sci"; anchors.fill: parent }
                TextInput{
                    id: nameIn
                    width: parent.width - 8
                    anchors.centerIn: parent
                    maximumLength:21
                    font.pixelSize: 16;
                    font.bold: true
                    color: "#151515"; selectionColor: "green"
                    KeyNavigation.tab: passIn
                    KeyNavigation.backtab: guest
                    focus: true
                }
            }
        }
        Column{
            spacing: 4
            Text {
                text: "Password:"
                font.pixelSize: 16; font.bold: true; color: "white"; style: Text.Raised; styleColor: "black"
                horizontalAlignment: Qt.AlignRight
            }
            Item {
                width: 220
               height: 28
                BorderImage { source: "images/lineedit.sci"; anchors.fill: parent }
                TextInput{
                    id: passIn
                    width: parent.width - 8
                    anchors.centerIn: parent
                    maximumLength:21
                    echoMode: TextInput.Password
                    font.pixelSize: 16;
                    font.bold: true
                    color: "#151515"; selectionColor: "green"
                    KeyNavigation.tab: login
                    KeyNavigation.backtab: nameIn
                    onAccepted: login.doLogin();
                }
            }
        }
        Row{
            spacing: 10
            Button {
                width: 100
                height: 32
                id: login
                keyUsing: true;
                function doLogin(){
                    rssModel.authName=nameIn.text;
                    rssModel.authPass=passIn.text;
                    rssModel.tags='my timeline';
                    screen.focus = true;
                }
                text: "Log in"
                KeyNavigation.right: guest
                KeyNavigation.tab: guest
                KeyNavigation.backtab: passIn
                Keys.onReturnPressed: login.doLogin();
                Keys.onEnterPressed: login.doLogin();
                Keys.onSelectPressed: login.doLogin();
                Keys.onSpacePressed: login.doLogin();
                onClicked: login.doLogin();
            }
            Button {
                width: 100
                height: 32
                id: guest
                keyUsing: true;
                function doGuest()
                {
                    rssModel.authName='-';
                    screen.focus = true;
                    screen.setMode(true);
                }
                text: "Guest"
                KeyNavigation.left: login
                KeyNavigation.tab: nameIn
                KeyNavigation.backtab: login
                Keys.onReturnPressed: guest.doGuest();
                Keys.onEnterPressed: guest.doGuest();
                Keys.onSelectPressed: guest.doGuest();
                Keys.onSpacePressed: guest.doGuest();
                onClicked: guest.doGuest();
            }
        }
    }
}
