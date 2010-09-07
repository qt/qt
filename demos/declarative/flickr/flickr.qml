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
import "common" as Common
import "mobile" as Mobile

Item {
    id: screen; width: 320; height: 480
    property bool inListView : false

    Rectangle {
        id: background
        anchors.fill: parent; color: "#343434";

        Image { source: "mobile/images/stripes.png"; fillMode: Image.Tile; anchors.fill: parent; opacity: 0.3 }

        Common.RssModel { id: rssModel }

        Item {
            id: views
            x: 2; width: parent.width - 4
            anchors.top: titleBar.bottom; anchors.bottom: toolBar.top

            Mobile.GridDelegate { id: gridDelegate }
            GridView {
                x: (width/4-79)/2; y: x
                id: photoGridView; model: rssModel; delegate: gridDelegate; cacheBuffer: 100
                cellWidth: (parent.width-2)/4; cellHeight: cellWidth; width: parent.width; height: parent.height - 1; z: 6
            }

            Mobile.ListDelegate { id: listDelegate }
            ListView {
                id: photoListView; model: rssModel; delegate: listDelegate; z: 6
                width: parent.width; height: parent.height; x: -(parent.width * 1.5); cacheBuffer: 100;
            }
            states: State {
                name: "ListView"; when: screen.inListView == true
                PropertyChanges { target: photoListView; x: 0 }
                PropertyChanges { target: photoGridView; x: -(parent.width * 1.5) }
            }

            transitions: Transition {
                NumberAnimation { properties: "x"; duration: 500; easing.type: Easing.InOutQuad }
            }
        }

        Mobile.ImageDetails { id: imageDetails; width: parent.width; anchors.left: views.right; height: parent.height; z:1 }
        Mobile.TitleBar { id: titleBar; z: 5; width: parent.width; height: 40; opacity: 0.9 }

        Mobile.ToolBar {
            id: toolBar; z: 5
            height: 40; anchors.bottom: parent.bottom; width: parent.width; opacity: 0.9
            button1Label: "Update"; button2Label: "View mode"
            onButton1Clicked: rssModel.reload()
            onButton2Clicked: if (screen.inListView == true) screen.inListView = false; else screen.inListView = true
        }

        Connections {
            target: imageDetails
            onClosed: {
                if (background.state == "DetailedView") {
                    background.state = '';
                    imageDetails.photoUrl = "";
                }
            }
        }

        states: State {
            name: "DetailedView"
            PropertyChanges { target: views; x: -parent.width }
            PropertyChanges { target: toolBar; button1Label: "More..." }
            PropertyChanges {
                target: toolBar
                onButton1Clicked: if (imageDetails.state=='') imageDetails.state='Back'; else imageDetails.state=''
            }
            PropertyChanges { target: toolBar; button2Label: "Back" }
            PropertyChanges { target: toolBar; onButton2Clicked: imageDetails.closed() }
        }

        transitions: Transition {
            NumberAnimation { properties: "x"; duration: 500; easing.type: Easing.InOutQuad }
        }
    }
}
