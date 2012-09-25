/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0

Item {
    id: main
    //height and width set by program to fill window
    //below properties are sometimes set from C++
    property url qmlFile: ''
    property bool show: false

    Item{ id:embeddedViewer
        width: parent.width
        height: parent.height
        opacity: 0;
        z: 10
        Loader{
            id: loader
            z: 10
            focus: true //Automatic FocusScope
            clip: true
            source: qmlFile
            anchors.centerIn: parent
            onStatusChanged:{
            if(status == Loader.Null) {
                loader.focus = false;//fixes QTBUG11411, probably because the focusScope needs to gain focus to focus the right child
            }else if(status == Loader.Ready) {
                if(loader.item.width > 640)
                    loader.item.width = 640;
                if(loader.item.height > 480)
                    loader.item.height = 480;
            }}

        }
        Rectangle{ id: frame
            z: 9
            anchors.fill: loader.status == Loader.Ready ? loader : errorTxt
            anchors.margins: -8
            radius: 4
            smooth: true
            border.color: "#88aaaaaa"
            gradient: Gradient{
                GradientStop{ position: 0.0; color: "#14FFFFFF" }
                GradientStop{ position: 1.0; color: "#5AFFFFFF" }
            }
            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
                onClicked: loader.focus=true;/* and don't propagate to the 'exit' area*/
            }

            Rectangle{ id: innerFrame
                anchors.margins: 7
                anchors.bottomMargin: 8
                anchors.rightMargin: 8
                color: "black"
                border.color: "#44000000"
                anchors.fill:parent
            }

        }
        Rectangle{ id: closeButton
            width: 24
            height: 24
            z: 11
            border.color: "#aaaaaaaa"
            gradient: Gradient{
                GradientStop{ position: 0.0; color: "#34FFFFFF" }
                GradientStop{ position: 1.0; color: "#7AFFFFFF" }
            }
            anchors.left: frame.right
            anchors.bottom: frame.top
            anchors.margins: -(2*width/3)
            Text{
                text: 'X'
                font.bold: true
                color: "white"
                font.pixelSize: 12
                anchors.centerIn: parent
            }
            MouseArea{
                anchors.fill: parent
                onClicked: main.show = false;
            }
        }

        Text{
            id: errorTxt
            z: 10
            anchors.centerIn: parent
            color: 'white'
            smooth: true
            visible: loader.status == Loader.Error
            textFormat: Text.RichText
            //Note that if loader is Error, it is because the file was found but there was an error creating the component
            //This means either we have a bug in our demos, or the required modules (which ship with Qt) did not deploy correctly
            text: "The example has failed to load.<br />If you installed all Qt's C++ and QML modules then this is a bug!<br />"
                + 'Report it at <a href="http://bugreports.qt-project.org">http://bugreports.qt-project.org</a>';
            onLinkActivated: Qt.openUrlExternally(link);
        }
    }
    Rectangle{ id: blackout //Maybe use a colorize effect instead?
        z: 8
        anchors.fill: parent
        color: "#000000"
        opacity: 0
    }
    MouseArea{
        z: 8
        enabled: main.show
        hoverEnabled: main.show //To steal focus from the buttons
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        anchors.fill: parent
    }

    states: [
        State {
            name: "show"
            when: show == true
            PropertyChanges {
                target: embeddedViewer
                opacity: 1
            }
            PropertyChanges {
                target: blackout
                opacity: 0.5
            }
        }
    ]
    transitions: [//Should not be too long, because the component has already started running
        Transition { from: ''; to: "show"; reversible: true
            ParallelAnimation{
                NumberAnimation{ properties: "opacity"; easing.type: Easing.InQuad; duration: 500}
                PropertyAction { target: loader; property: "focus"; value: true}//Might be needed to ensure the focus stays with us
            }
        }
    ]
}
