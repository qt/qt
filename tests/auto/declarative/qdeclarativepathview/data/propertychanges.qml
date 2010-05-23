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
    width: 350; height: 220; color: "white"
    Component {
        id: myDelegate
        Item {
            id: wrapper
            width: 180; height: 40; 
            opacity: PathView.opacity
            Column {
                x: 5; y: 5
                Text { text: '<b>Name:</b> ' + name }
                Text { text: '<b>Number:</b> ' + number }
            }
        }
    }

    PathView {
        preferredHighlightBegin: 0.1
        preferredHighlightEnd: 0.1
        dragMargin: 5.0
        id: pathView
        objectName: "pathView"
        anchors.fill: parent
        model: listModel
        delegate: myDelegate
        focus: true
        path: Path {
            id: myPath
            objectName: "path"
            startX: 220; startY: 200
            PathAttribute { name: "opacity"; value: 1.0; objectName: "pathAttribute"; }
            PathQuad { x: 220; y: 25; controlX: 260; controlY: 75 }
            PathAttribute { name: "opacity"; value: 0.3 }
            PathQuad { x: 220; y: 200; controlX: -20; controlY: 75 }
        }
        Timer {
            interval: 2000; running: true; repeat: true
            onTriggered: {
                if (pathView.path == alternatePath)
                    pathView.path = myPath;
                else
                    pathView.path = alternatePath;
            }
        }
    }

    data:[  
    ListModel {
        id: listModel
        ListElement {
            name: "Bill Smith"
            number: "555 3264"
        }
        ListElement {
            name: "John Brown"
            number: "555 8426"
        }
        ListElement {
            name: "Sam Wise"
            number: "555 0473"
        }
        ListElement {
            name: "Bill Smith"
            number: "555 3264"
        }
        ListElement {
            name: "John Brown"
            number: "555 8426"
        }
        ListElement {
            name: "Sam Wise"
            number: "555 0473"
        }
        ListElement {
            name: "Bill Smith"
            number: "555 3264"
        }
        ListElement {
            name: "John Brown"
            number: "555 8426"
        }
        ListElement {
            name: "Sam Wise"
            number: "555 0473"
        }
    },
    ListModel {
        objectName: "alternateModel"
        ListElement {
            name: "Jack"
            number: "555 8426"
        }
        ListElement {
            name: "Mary"
            number: "555 3264"
        }
    },
    Path {
        id: alternatePath
        objectName: "alternatePath"
        startX: 100; startY: 40
        PathAttribute { name: "opacity"; value: 0.0 }
        PathLine { x: 100; y: 160 }
        PathAttribute { name: "opacity"; value: 0.2 }
        PathLine { x: 300; y: 160 }
        PathAttribute { name: "opacity"; value: 0.0 }
        PathLine { x: 300; y: 40 }
        PathAttribute { name: "opacity"; value: 0.2 }
        PathLine { x: 100; y: 40 }
    }
    ]
}


