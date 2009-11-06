import Qt 4.6

Rectangle {
    id: window
    width: 600; height: 460; color: "#232323"

    ListModel {
        id: easingTypes
        ListElement { type: "easeLinear"; ballColor: "DarkRed" }
        ListElement { type: "easeInQuad"; ballColor: "IndianRed" }
        ListElement { type: "easeOutQuad"; ballColor: "Salmon" }
        ListElement { type: "easeInOutQuad"; ballColor: "Tomato" }
        ListElement { type: "easeOutInQuad"; ballColor: "DarkOrange" }
        ListElement { type: "easeInCubic"; ballColor: "Gold" }
        ListElement { type: "easeOutCubic"; ballColor: "Yellow" }
        ListElement { type: "easeInOutCubic"; ballColor: "PeachPuff" }
        ListElement { type: "easeOutInCubic"; ballColor: "Thistle" }
        ListElement { type: "easeInQuart"; ballColor: "Orchid" }
        ListElement { type: "easeOutQuart"; ballColor: "Purple" }
        ListElement { type: "easeInOutQuart"; ballColor: "SlateBlue" }
        ListElement { type: "easeOutInQuart"; ballColor: "Chartreuse" }
        ListElement { type: "easeInQuint"; ballColor: "LimeGreen" }
        ListElement { type: "easeOutQuint"; ballColor: "SeaGreen" }
        ListElement { type: "easeInOutQuint"; ballColor: "DarkGreen" }
        ListElement { type: "easeOutInQuint"; ballColor: "Olive" }
        ListElement { type: "easeInSine"; ballColor: "DarkSeaGreen" }
        ListElement { type: "easeOutSine"; ballColor: "Teal" }
        ListElement { type: "easeInOutSine"; ballColor: "Turquoise" }
        ListElement { type: "easeOutInSine"; ballColor: "SteelBlue" }
        ListElement { type: "easeInExpo"; ballColor: "SkyBlue" }
        ListElement { type: "easeOutExpo"; ballColor: "RoyalBlue" }
        ListElement { type: "easeInOutExpo"; ballColor: "MediumBlue" }
        ListElement { type: "easeOutInExpo"; ballColor: "MidnightBlue" }
        ListElement { type: "easeInCirc"; ballColor: "CornSilk" }
        ListElement { type: "easeOutCirc"; ballColor: "Bisque" }
        ListElement { type: "easeInOutCirc"; ballColor: "RosyBrown" }
        ListElement { type: "easeOutInCirc"; ballColor: "SandyBrown" }
        ListElement { type: "easeInElastic"; ballColor: "DarkGoldenRod" }
        ListElement { type: "easeOutElastic"; ballColor: "Chocolate" }
        ListElement { type: "easeInOutElastic"; ballColor: "SaddleBrown" }
        ListElement { type: "easeOutInElastic"; ballColor: "Brown" }
        ListElement { type: "easeInBack"; ballColor: "Maroon" }
        ListElement { type: "easeOutBack"; ballColor: "LavenderBlush" }
        ListElement { type: "easeInOutBack"; ballColor: "MistyRose" }
        ListElement { type: "easeOutInBack"; ballColor: "Gainsboro" }
        ListElement { type: "easeOutBounce"; ballColor: "Silver" }
        ListElement { type: "easeInBounce"; ballColor: "DimGray" }
        ListElement { type: "easeInOutBounce"; ballColor: "SlateGray" }
        ListElement { type: "easeOutInBounce"; ballColor: "DarkSlateGray" }
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

                MouseRegion {
                    onClicked: if (rect.state == '') rect.state = "right"; else rect.state = ''
                    anchors.fill: parent
                }

                states : State {
                    name: "right"
                    PropertyChanges { target: rect; x: window.width - 62; color: ballColor }
                }

                transitions: Transition {
                    ParallelAnimation {
                        NumberAnimation { matchProperties: "x"; easing: type; duration: 1000 }
                        ColorAnimation { matchProperties: "color"; easing: type; duration: 1000 }
                    }
                }
            }
        }
    }

    Flickable {
        anchors.fill: parent; viewportHeight: layout.height
        Column {
            id: layout
            anchors.left: parent.left; anchors.right: parent.right
            Repeater { model: easingTypes; delegate: delegate }
        }
    }
}
