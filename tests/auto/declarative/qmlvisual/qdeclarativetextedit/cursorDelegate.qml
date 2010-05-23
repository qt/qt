import Qt 4.7
    Rectangle {
    resources: [
        Component { id: cursorA
            Item { id: cPage;
                x: Behavior { NumberAnimation { } }
                y: Behavior { NumberAnimation { } }
                height: Behavior { NumberAnimation { duration: 200 } }
                Rectangle { id: cRectangle; color: "black"; y: 1; width: 1; height: parent.height-2;
                    Rectangle { id:top; color: "black"; width: 3; height: 1; x: -1; y:0}
                    Rectangle { id:bottom; color: "black"; width: 3; height: 1; x: -1; anchors.bottom: parent.bottom;}
                    opacity: 1
                    opacity: SequentialAnimation { running: cPage.parent.focus == true; loops: Animation.Infinite;
                                NumberAnimation { properties: "opacity"; to: 1; duration: 500; easing.type: "InQuad"}
                                NumberAnimation { properties: "opacity"; to: 0; duration: 500; easing.type: "OutQuad"}
                             }
                }
                width: 1;
            }
        }
    ]
    width: 400
    height: 200
    color: "white"
    TextEdit { id: mainText
        text: "Hello World"
        cursorDelegate: cursorA
        focus: true
        font.pixelSize: 28
        selectionColor: "lightsteelblue"
        selectedTextColor: "deeppink"
        color: "forestgreen"
        anchors.centerIn: parent
    }
}
