Rect {
    id: mainrect
    width: 200; height: 200
    state: "first"
    states: [
        State {
            name: "first"
            SetProperty {
                target: mainrect
                property: "color"
                value: "red"
            }
        },
        State {
            name: "second"
            SetProperty {
                target: mainrect
                property: "color"
                value: "blue"
            }
        }
    ]
    transitions: [
        Transition {
            fromState: "first"
            toState: "second"
            reversible: true
            SequentialAnimation {
                ColorAnimation {
                    duration: 2000
                    target: mainrect
                    property: "color"
                }
            }
        }
    ]
    MouseRegion {
        anchors.fill: parent
        onClicked: { mainrect.state = 'second' }
    }
}
