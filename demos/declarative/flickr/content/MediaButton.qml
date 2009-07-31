import Qt 4.6

Item {
    id: Container

    signal clicked

    property string text

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
        anchors.centerIn: Image
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
