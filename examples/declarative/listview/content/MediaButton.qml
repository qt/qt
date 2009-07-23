import Qt 4.6

Item {
    property var text
    signal clicked

    id: Container
    Image {
        id: Image
        source: "pics/button.png"
    }
    Image {
        id: Pressed
        source: "pics/button-pressed.png"
        opacity: 0
    }
    MouseRegion {
        id: MouseRegion
        anchors.fill: Image
        onClicked: { Container.clicked(); }
    }
    Text {
        font.bold: true
        color: "white"
        anchors.centeredIn: Image
        text: Container.text
    }
    width: Image.width
    states: [
        State {
            name: "Pressed"
            when: MouseRegion.pressed == true
            SetProperties {
                target: Pressed
                opacity: 1
            }
        }
    ]
}
