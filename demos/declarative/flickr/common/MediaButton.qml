import Qt 4.6

Item {
    id: Container

    signal clicked

    property string text

    Image {
        id: ButtonImage
        source: "pics/button.png"
    }
    Image {
        id: Pressed
        source: "pics/button-pressed.png"
        opacity: 0
    }
    MouseRegion {
        id: MyMouseRegion
        anchors.fill: ButtonImage
        onClicked: { Container.clicked(); }
    }
    Text {
        font.bold: true
        color: "white"
        anchors.centerIn: ButtonImage
        text: Container.text
    }
    width: ButtonImage.width
    states: [
        State {
            name: "Pressed"
            when: MyMouseRegion.pressed == true
            PropertyChanges {
                target: Pressed
                opacity: 1
            }
        }
    ]
}
