import Qt 4.7

Rectangle {
    width: 600; height: 400
    color: "#343434"

    Rectangle {
        anchors.centerIn: parent
        width: 200; height: 200
        radius: 30
        color: "transparent"
        border.width: 4; border.color: "white"


        SideRect {
            id: leftRect
            anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.left }
            text: "Left"
        }

        SideRect {
            id: rightRect
            anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.right }
            text: "Right"
        }

        SideRect {
            id: topRect
            anchors { verticalCenter: parent.top; horizontalCenter: parent.horizontalCenter }
            text: "Top"
        }

        SideRect {
            id: bottomRect
            anchors { verticalCenter: parent.bottom; horizontalCenter: parent.horizontalCenter }
            text: "Bottom"
        }


        Rectangle {
            id: focusRect

            property string text

            x: 62.5; y: 75; width: 75; height: 50
            radius: 6
            border.width: 4; border.color: "white"
            color: "firebrick"

            // Setting an 'elastic' behavior on the focusRect's x property.
            Behavior on x {
                NumberAnimation { easing.type: "OutElastic"; easing.amplitude: 3.0; easing.period: 2.0; duration: 300 }
            }

            // Setting an 'elastic' behavior on the focusRect's y property.
            Behavior on y {
                NumberAnimation { easing.type: "OutElastic"; easing.amplitude: 3.0; easing.period: 2.0; duration: 300 }
            }

            Text {
                id: focusText
                text: focusRect.text
                anchors.centerIn: parent
                color: "white"
                font.pixelSize: 16; font.bold: true

                // Setting a behavior on the focusText's x property:
                // Set the opacity to 0, set the new text value, then set the opacity back to 1.
                Behavior on text {
                    SequentialAnimation {
                        NumberAnimation { target: focusText; property: "opacity"; to: 0; duration: 150 }
                        PropertyAction { }
                        NumberAnimation { target: focusText; property: "opacity"; to: 1; duration: 150 }
                    }
                }
            }
        }
    }
}
