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
            SetProperty {
                target: MyRect
                property: "x"
                binding: 100
            }
            SetProperty {
                target: MyMouseRegion
                property: "onClicked"
                value: "Page.currentState = ''"
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
