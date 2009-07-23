import Qt 4.6

Item {
    id: button
    width: 30
    height: 30
    property var icon: ""
    signal clicked
    Rect {
        id: buttonRect
        anchors.fill: parent
        color: "lightgreen"
        radius: 5
        Image {
            id: iconImage
            source: button.icon
            anchors.horizontalCenter: buttonRect.horizontalCenter
            anchors.verticalCenter: buttonRect.verticalCenter
        }
        MouseRegion {
            id: buttonMouseRegion
            anchors.fill: buttonRect
            onClicked: { button.clicked() }
        }
        states: [
            State {
                name: "pressed"
                when: buttonMouseRegion.pressed == true
                SetProperties {
                    target: buttonRect
                    color: "green"
                }
            }
        ]
        transitions: [
            Transition {
                fromState: "*"
                toState: "pressed"
                ColorAnimation {
                    property: "color"
                    duration: 200
                }
            },
            Transition {
                fromState: "pressed"
                toState: "*"
                ColorAnimation {
                    property: "color"
                    duration: 1000
                }
            }
        ]
    }
    opacity: Behavior {
        NumberAnimation {
            property: "opacity"
            duration: 250
        }
    }
}
