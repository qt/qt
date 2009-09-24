import Qt 4.6

Rectangle {
    color: "blue"
    width: 320
    height: 240
    id: Page
    Rectangle {
        id: MyRectangle
        width: 100
        height: 100
        color: "red"
        x: 10
    }
    states: [
        State {
            name: "hello"
            PropertyChanges {
                target: MyRectangle
                x: 100
            }
            PropertyChanges {
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
