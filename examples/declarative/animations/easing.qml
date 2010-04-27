import Qt 4.7

Rectangle {
    id: window
    width: 600; height: 460; color: "#232323"

    ListModel {
        id: easingTypes
        ListElement { type: "Linear"; ballColor: "DarkRed" }
        ListElement { type: "InQuad"; ballColor: "IndianRed" }
        ListElement { type: "OutQuad"; ballColor: "Salmon" }
        ListElement { type: "InOutQuad"; ballColor: "Tomato" }
        ListElement { type: "OutInQuad"; ballColor: "DarkOrange" }
        ListElement { type: "InCubic"; ballColor: "Gold" }
        ListElement { type: "OutCubic"; ballColor: "Yellow" }
        ListElement { type: "InOutCubic"; ballColor: "PeachPuff" }
        ListElement { type: "OutInCubic"; ballColor: "Thistle" }
        ListElement { type: "InQuart"; ballColor: "Orchid" }
        ListElement { type: "OutQuart"; ballColor: "Purple" }
        ListElement { type: "InOutQuart"; ballColor: "SlateBlue" }
        ListElement { type: "OutInQuart"; ballColor: "Chartreuse" }
        ListElement { type: "InQuint"; ballColor: "LimeGreen" }
        ListElement { type: "OutQuint"; ballColor: "SeaGreen" }
        ListElement { type: "InOutQuint"; ballColor: "DarkGreen" }
        ListElement { type: "OutInQuint"; ballColor: "Olive" }
        ListElement { type: "InSine"; ballColor: "DarkSeaGreen" }
        ListElement { type: "OutSine"; ballColor: "Teal" }
        ListElement { type: "InOutSine"; ballColor: "Turquoise" }
        ListElement { type: "OutInSine"; ballColor: "SteelBlue" }
        ListElement { type: "InExpo"; ballColor: "SkyBlue" }
        ListElement { type: "OutExpo"; ballColor: "RoyalBlue" }
        ListElement { type: "InOutExpo"; ballColor: "MediumBlue" }
        ListElement { type: "OutInExpo"; ballColor: "MidnightBlue" }
        ListElement { type: "InCirc"; ballColor: "CornSilk" }
        ListElement { type: "OutCirc"; ballColor: "Bisque" }
        ListElement { type: "InOutCirc"; ballColor: "RosyBrown" }
        ListElement { type: "OutInCirc"; ballColor: "SandyBrown" }
        ListElement { type: "InElastic"; ballColor: "DarkGoldenRod" }
        ListElement { type: "OutElastic"; ballColor: "Chocolate" }
        ListElement { type: "InOutElastic"; ballColor: "SaddleBrown" }
        ListElement { type: "OutInElastic"; ballColor: "Brown" }
        ListElement { type: "InBack"; ballColor: "Maroon" }
        ListElement { type: "OutBack"; ballColor: "LavenderBlush" }
        ListElement { type: "InOutBack"; ballColor: "MistyRose" }
        ListElement { type: "OutInBack"; ballColor: "Gainsboro" }
        ListElement { type: "OutBounce"; ballColor: "Silver" }
        ListElement { type: "InBounce"; ballColor: "DimGray" }
        ListElement { type: "InOutBounce"; ballColor: "SlateGray" }
        ListElement { type: "OutInBounce"; ballColor: "DarkSlateGray" }
    }

    Component {
        id: delegate

        Item {
            height: 42; width: window.width
            Text { text: type; anchors.centerIn: parent; color: "White" }
            Rectangle {
                id: slot1; color: "#121212"; x: 30; height: 32; width: 32
                border.color: "#343434"; border.width: 1; radius: 8; anchors.verticalCenter: parent.verticalCenter
            }
            Rectangle {
                id: slot2; color: "#121212"; x: window.width - 62; height: 32; width: 32
                border.color: "#343434"; border.width: 1; radius: 8; anchors.verticalCenter: parent.verticalCenter
            }
            Rectangle {
                id: rect; x: 30; color: "#454545"
                border.color: "White"; border.width: 2
                height: 32; width: 32; radius: 8; anchors.verticalCenter: parent.verticalCenter

                MouseArea {
                    onClicked: if (rect.state == '') rect.state = "right"; else rect.state = ''
                    anchors.fill: parent
                }

                states : State {
                    name: "right"
                    PropertyChanges { target: rect; x: window.width - 62; color: ballColor }
                }

                transitions: Transition {
                    // ParallelAnimation {
                        NumberAnimation { properties: "x"; easing.type: type; duration: 1000 }
                        ColorAnimation { properties: "color"; easing.type: type; duration: 1000 }
                    // }
                }
            }
        }
    }

    Flickable {
        anchors.fill: parent; contentHeight: layout.height
        Column {
            id: layout
            anchors.left: parent.left; anchors.right: parent.right
            Repeater { model: easingTypes; delegate: delegate }
        }
    }
}
