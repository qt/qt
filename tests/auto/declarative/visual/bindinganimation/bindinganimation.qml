import Qt 4.6

Rect {
    color: "blue"
    width: 320
    height: 240
    id: Page
    Rect {
        id: MyRect
        width: 100
        height: 100
        color: "red"
        x: 10
    }
    states: [
        State {
            name: "hello"
            SetProperties {
                target: MyRect
                x: 100
            }
            SetProperties {
                target: MyMouseRegion
                onClicked: "Page.currentState = ''"
            }
        }
    ]
    transitions: [
        Transition {
            NumberAnimation {
                properties: "x"
            }
        }
    ]
    MouseRegion {
        id: MyMouseRegion
        anchors.fill: parent
        onClicked: { Page.state= 'hello' }
    }
}
