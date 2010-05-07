import Qt 4.7

Rectangle {
    id: window
    width: 600; height: 460; color: "#232323"

    ListModel {
        id: easingTypes
        ListElement { name: "Easing.Linear"; type: Easing.Linear; ballColor: "DarkRed" }
        ListElement { name: "Easing.InQuad"; type: Easing.InQuad; ballColor: "IndianRed" }
        ListElement { name: "Easing.OutQuad"; type: Easing.OutQuad; ballColor: "Salmon" }
        ListElement { name: "Easing.InOutQuad"; type: Easing.InOutQuad; ballColor: "Tomato" }
        ListElement { name: "Easing.OutInQuad"; type: Easing.OutInQuad; ballColor: "DarkOrange" }
        ListElement { name: "Easing.InCubic"; type: Easing.InCubic; ballColor: "Gold" }
        ListElement { name: "Easing.OutCubic"; type: Easing.OutCubic; ballColor: "Yellow" }
        ListElement { name: "Easing.InOutCubic"; type: Easing.InOutCubic; ballColor: "PeachPuff" }
        ListElement { name: "Easing.OutInCubic"; type: Easing.OutInCubic; ballColor: "Thistle" }
        ListElement { name: "Easing.InQuart"; type: Easing.InQuart; ballColor: "Orchid" }
        ListElement { name: "Easing.OutQuart"; type: Easing.OutQuart; ballColor: "Purple" }
        ListElement { name: "Easing.InOutQuart"; type: Easing.InOutQuart; ballColor: "SlateBlue" }
        ListElement { name: "Easing.OutInQuart"; type: Easing.OutInQuart; ballColor: "Chartreuse" }
        ListElement { name: "Easing.InQuint"; type: Easing.InQuint; ballColor: "LimeGreen" }
        ListElement { name: "Easing.OutQuint"; type: Easing.OutQuint; ballColor: "SeaGreen" }
        ListElement { name: "Easing.InOutQuint"; type: Easing.InOutQuint; ballColor: "DarkGreen" }
        ListElement { name: "Easing.OutInQuint"; type: Easing.OutInQuint; ballColor: "Olive" }
        ListElement { name: "Easing.InSine"; type: Easing.InSine; ballColor: "DarkSeaGreen" }
        ListElement { name: "Easing.OutSine"; type: Easing.OutSine; ballColor: "Teal" }
        ListElement { name: "Easing.InOutSine"; type: Easing.InOutSine; ballColor: "Turquoise" }
        ListElement { name: "Easing.OutInSine"; type: Easing.OutInSine; ballColor: "SteelBlue" }
        ListElement { name: "Easing.InExpo"; type: Easing.InExpo; ballColor: "SkyBlue" }
        ListElement { name: "Easing.OutExpo"; type: Easing.OutExpo; ballColor: "RoyalBlue" }
        ListElement { name: "Easing.InOutExpo"; type: Easing.InOutExpo; ballColor: "MediumBlue" }
        ListElement { name: "Easing.OutInExpo"; type: Easing.OutInExpo; ballColor: "MidnightBlue" }
        ListElement { name: "Easing.InCirc"; type: Easing.InCirc; ballColor: "CornSilk" }
        ListElement { name: "Easing.OutCirc"; type: Easing.OutCirc; ballColor: "Bisque" }
        ListElement { name: "Easing.InOutCirc"; type: Easing.InOutCirc; ballColor: "RosyBrown" }
        ListElement { name: "Easing.OutInCirc"; type: Easing.OutInCirc; ballColor: "SandyBrown" }
        ListElement { name: "Easing.InElastic"; type: Easing.InElastic; ballColor: "DarkGoldenRod" }
        ListElement { name: "Easing.InElastic"; type: Easing.OutElastic; ballColor: "Chocolate" }
        ListElement { name: "Easing.InOutElastic"; type: Easing.InOutElastic; ballColor: "SaddleBrown" }
        ListElement { name: "Easing.OutInElastic"; type: Easing.OutInElastic; ballColor: "Brown" }
        ListElement { name: "Easing.InBack"; type: Easing.InBack; ballColor: "Maroon" }
        ListElement { name: "Easing.OutBack"; type: Easing.OutBack; ballColor: "LavenderBlush" }
        ListElement { name: "Easing.InOutBack"; type: Easing.InOutBack; ballColor: "MistyRose" }
        ListElement { name: "Easing.OutInBack"; type: Easing.OutInBack; ballColor: "Gainsboro" }
        ListElement { name: "Easing.OutBounce"; type: Easing.OutBounce; ballColor: "Silver" }
        ListElement { name: "Easing.InBounce"; type: Easing.InBounce; ballColor: "DimGray" }
        ListElement { name: "Easing.InOutBounce"; type: Easing.InOutBounce; ballColor: "SlateGray" }
        ListElement { name: "Easing.OutInBounce"; type: Easing.OutInBounce; ballColor: "DarkSlateGray" }
    }

    Component {
        id: delegate

        Item {
            height: 42; width: window.width

            Text { text: name; anchors.centerIn: parent; color: "White" }

            Rectangle {
                id: slot1; color: "#121212"; x: 30; height: 32; width: 32
                border.color: "#343434"; border.width: 1; radius: 8
                anchors.verticalCenter: parent.verticalCenter
            }

            Rectangle {
                id: slot2; color: "#121212"; x: window.width - 62; height: 32; width: 32
                border.color: "#343434"; border.width: 1; radius: 8
                anchors.verticalCenter: parent.verticalCenter
            }

            Rectangle {
                id: rect; x: 30; color: "#454545"
                border.color: "White"; border.width: 2
                height: 32; width: 32; radius: 8
                anchors.verticalCenter: parent.verticalCenter

                MouseArea {
                    onClicked: if (rect.state == '') rect.state = "right"; else rect.state = ''
                    anchors.fill: parent
                }

                states : State {
                    name: "right"
                    PropertyChanges { target: rect; x: window.width - 62; color: ballColor }
                }

                transitions: Transition {
                    NumberAnimation { properties: "x"; easing.type: type; duration: 1000 }
                    ColorAnimation { properties: "color"; easing.type: type; duration: 1000 }
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
