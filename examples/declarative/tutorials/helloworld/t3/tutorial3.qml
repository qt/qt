Rect {
    id: Page
    width: 480
    height: 200
    color: "LightGrey"
    Text {
        id: HelloText
        text: "Hello world!"
        font.size: 24
        font.bold: true
        y: 30
        anchors.horizontalCenter: Page.horizontalCenter
        states: [
            State {
                name: "down"
                when: MouseRegion.pressed == true
                SetProperties {
                    target: HelloText
                    y: 160
                    color: "red"
                }
            }
        ]
        transitions: [
            Transition {
                fromState: "*"
                toState: "down"
                reversible: true
                ParallelAnimation {
                    NumberAnimation {
                        properties: "y"
                        duration: 500
                        easing: "easeOutBounce"
                    }
                    ColorAnimation { duration: 500 }
                }
            }
        ]
    }
    MouseRegion { id: MouseRegion; anchors.fill: HelloText }
    GridLayout {
        id: ColorPicker
        x: 0
        anchors.bottom: Page.bottom
        width: 120; height: 50
        rows: 2; columns: 3
        Cell { color: "#ff0000" }
        Cell { color: "#00ff00" }
        Cell { color: "#0000ff" }
        Cell { color: "#ffff00" }
        Cell { color: "#00ffff" }
        Cell { color: "#ff00ff" }
    }
}
