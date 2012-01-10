/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

Rectangle {
    id: editor
    color: "lightGrey"
    width: 640; height: 480

    Rectangle {
        color: "white"
        anchors.fill: parent
        anchors.margins: 20

        BorderImage {
            id: startHandle
            source: "pics/startHandle.sci"
            opacity: 0.0
            width: 10
            x: edit.positionToRectangle(edit.selectionStart).x - flick.contentX-width
            y: edit.positionToRectangle(edit.selectionStart).y - flick.contentY
            height: edit.positionToRectangle(edit.selectionStart).height
        }

        BorderImage {
            id: endHandle
            source: "pics/endHandle.sci"
            opacity: 0.0
            width: 10
            x: edit.positionToRectangle(edit.selectionEnd).x - flick.contentX
            y: edit.positionToRectangle(edit.selectionEnd).y - flick.contentY
            height: edit.positionToRectangle(edit.selectionEnd).height
        }

        Flickable {
            id: flick

            anchors.fill: parent
            contentWidth: edit.paintedWidth
            contentHeight: edit.paintedHeight
            interactive: true
            clip: true

            function ensureVisible(r) {
                if (contentX >= r.x)
                    contentX = r.x;
                else if (contentX+width <= r.x+r.width)
                    contentX = r.x+r.width-width;
                if (contentY >= r.y)
                    contentY = r.y;
                else if (contentY+height <= r.y+r.height)
                    contentY = r.y+r.height-height;
            }

            TextEdit {
                id: edit
                width: flick.width
                height: flick.height
                focus: true
                wrapMode: TextEdit.Wrap

                onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)

                text: "<h1>Text Selection</h1>"
                    +"<p>This example is a whacky text selection mechanisms, showing how these can be implemented in the TextEdit element, to cater for whatever style is appropriate for the target platform."
                    +"<p><b>Press-and-hold</b> to select a word, then drag the selection handles."
                    +"<p><b>Drag outside the selection</b> to scroll the text."
                    +"<p><b>Click inside the selection</b> to cut/copy/paste/cancel selection."
                    +"<p>It's too whacky to let you paste if there is no current selection."

                MouseArea {
                    property string drag: ""
                    property int pressPos

                    x: -startHandle.width
                    y: 0
                    width: parent.width+startHandle.width+endHandle.width
                    height: parent.height

                    onPressAndHold: {
                        if (editor.state == "") {
                            edit.cursorPosition = edit.positionAt(mouse.x+x,mouse.y+y);
                            edit.selectWord();
                            editor.state = "selection"
                        }
                    }

                    onClicked: {
                        if (editor.state == "") {
                            edit.cursorPosition = edit.positionAt(mouse.x+x,mouse.y+y);
                            if (!edit.focus)
                                edit.focus = true;
                            edit.openSoftwareInputPanel();
                        }
                    }

                    function hitHandle(h,x,y) { 
                        return x>=h.x+flick.contentX && x<h.x+flick.contentX+h.width && y>=h.y+flick.contentY && y<h.y+flick.contentY+h.height 
                    }

                    onPressed: {
                        if (editor.state == "selection") {
                            if (hitHandle(startHandle,mouse.x+x,mouse.y+y)) {
                                drag = "start"
                                flick.interactive = false
                            } else if (hitHandle(endHandle,mouse.x+x,mouse.y+y)) {
                                drag = "end"
                                flick.interactive = false
                            } else {
                                var pos = edit.positionAt(mouse.x+x,mouse.y+y);
                                if (pos >= edit.selectionStart && pos <= edit.selectionEnd) {
                                    drag = "selection"
                                    flick.interactive = false
                                } else {
                                    drag = ""
                                    flick.interactive = true
                                }
                            }
                        }
                    }

                    onReleased: {
                        if (editor.state == "selection") {
                            if (drag == "selection") {
                                editor.state = "menu"
                            }
                            drag = ""
                        }
                        flick.interactive = true
                    }

                    onPositionChanged: {
                        if (editor.state == "selection" && drag != "") {
                            if (drag == "start") {
                                var pos = edit.positionAt(mouse.x+x+startHandle.width/2,mouse.y+y);
                                var e = edit.selectionEnd;
                                if (e < pos)
                                    e = pos;
                                edit.select(pos,e);
                            } else if (drag == "end") {
                                var pos = edit.positionAt(mouse.x+x-endHandle.width/2,mouse.y+y);
                                var s = edit.selectionStart;
                                if (s > pos)
                                    s = pos;
                                edit.select(s,pos);
                            }
                        }
                    }
                }
            }
        }

        Item {
            id: menu
            opacity: 0.0
            width: 100
            height: 120
            anchors.centerIn: parent

            Rectangle {
                border.width: 1
                border.color: "darkBlue"
                radius: 15
                color: "#806080FF"
                anchors.fill: parent
            }

            Column {
                anchors.centerIn: parent
                spacing: 8

                Rectangle {
                    border.width: 1
                    border.color: "darkBlue"
                    color: "#ff7090FF"
                    width: 60
                    height: 16

                    Text { anchors.centerIn: parent; text: "Cut" }

                    MouseArea { 
                        anchors.fill: parent
                        onClicked: { edit.cut(); editor.state = "" } 
                    }
                }

                Rectangle {
                    border.width: 1
                    border.color: "darkBlue"
                    color: "#ff7090FF"
                    width: 60
                    height: 16

                    Text { anchors.centerIn: parent; text: "Copy" }

                    MouseArea { 
                        anchors.fill: parent
                        onClicked: { edit.copy(); editor.state = "selection" } 
                    }
                }

                Rectangle {
                    border.width: 1
                    border.color: "darkBlue"
                    color: "#ff7090FF"
                    width: 60
                    height: 16

                    Text { anchors.centerIn: parent; text: "Paste" }

                    MouseArea { 
                        anchors.fill: parent
                        onClicked: { edit.paste(); edit.cursorPosition = edit.selectionEnd; editor.state = "" } 
                    }
                }

                Rectangle {
                    border.width: 1
                    border.color: "darkBlue"
                    color: "#ff7090FF"
                    width: 60
                    height: 16

                    Text { anchors.centerIn: parent; text: "Deselect" }

                    MouseArea { 
                        anchors.fill: parent
                        onClicked: { 
                            edit.cursorPosition = edit.selectionEnd;
                            edit.select(edit.cursorPosition, edit.cursorPosition);
                            editor.state = "" 
                        } 
                    }
                }
            }
        }
    }

    states: [
        State {
            name: "selection"
            PropertyChanges { target: startHandle; opacity: 1.0 }
            PropertyChanges { target: endHandle; opacity: 1.0 }
        },
        State {
            name: "menu"
            PropertyChanges { target: startHandle; opacity: 0.5 }
            PropertyChanges { target: endHandle; opacity: 0.5 }
            PropertyChanges { target: menu; opacity: 1.0 }
        }
    ]
}
