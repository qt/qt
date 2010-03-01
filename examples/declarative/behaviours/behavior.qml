import Qt 4.6

Rectangle {
    color: "black"
    width: 400; height: 400

    Rectangle {
        color: "transparent"
        anchors.centerIn: parent
        width: 200; height: 200
        radius: 30
        border.width: 10; border.color: "white";

        SideRect {
            id: leftRect
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.left
            text: "Left"
        }

        SideRect {
            id: rightRect
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.right
            text: "Right"
        }

        SideRect {
            id: topRect
            anchors.verticalCenter: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Top"
        }

        SideRect {
            id: bottomRect
            anchors.verticalCenter: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Bottom"
        }


        Rectangle {
            id: focusRect

            property string text

            color: "red"
            width: 75; height: 50
            radius: 5
            border.width: 10; border.color: "white";
            x: 100-37; y: 100-25
            x: Behavior { NumberAnimation { duration: 300 } }
            y: Behavior { NumberAnimation { duration: 300 } }
            Text {
                id: focusText
                text: focusRect.text;
                text: Behavior {
                    SequentialAnimation {
                        NumberAnimation { target: focusText; property: "opacity"; to: 0; duration: 150 }
                        PropertyAction {}
                        NumberAnimation { target: focusText; property: "opacity"; to: 1; duration: 150 }
                    }
                }
                anchors.centerIn: parent;
                color: "white";
                font.pixelSize: 16
                font.bold: true
            }
        }
    }
}
