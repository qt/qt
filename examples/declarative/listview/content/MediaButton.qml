import Qt 4.6

Item {
    property var text
    signal clicked

    id: Container
    Image {
        id: Normal
        source: "pics/button.png"
    }
    Image {
        id: Pressed
        source: "pics/button-pressed.png"
        opacity: 0
    }
    MouseRegion {
        id: ClickRegion
        anchors.fill: Normal
        onClicked: { Container.clicked(); }
    }
    Text {
        font.bold: true
        color: "white"
        anchors.centerIn: Normal
        text: Container.text
    }
    width: Normal.width
    states: [
        State {
            name: "Pressed"
            when: ClickRegion.pressed == true
            PropertyChanges {
                target: Pressed
                opacity: 1
            }
        }
    ]
}
