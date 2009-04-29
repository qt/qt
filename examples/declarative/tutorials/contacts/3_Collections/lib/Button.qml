Item {
    id: button
    width: 30
    height: 30
    properties: Property {
        name: "icon"
    }
    signals: Signal {
        name: "clicked"
    }
    Rect {
        id: buttonRect
        anchors.fill: parent
        color: "lightgreen"
        radius: 5
        Image {
            id: iconImage
            src: button.icon
            anchors.horizontalCenter: buttonRect.horizontalCenter
            anchors.verticalCenter: buttonRect.verticalCenter
        }
        MouseRegion {
            id: buttonMouseRegion
            anchors.fill: buttonRect
            onClicked: { button.clicked.emit() }
        }
        states: [
            State {
                name: "pressed"
                when: buttonMouseRegion.pressed == true
                SetProperty {
                    target: buttonRect
                    property: "color"
                    value: "green"
                }
            }
        ]
        transitions: [
            Transition {
                fromState: "*"
                toState: "pressed"
                ColorAnimation {
                    duration: 200
                }
            },
            Transition {
                fromState: "pressed"
                toState: "*"
                ColorAnimation {
                    duration: 1000
                }
            }
        ]
    }
    opacity: Behaviour {
        NumericAnimation {
            property: "opacity"
            duration: 250
        }
    }
}
