import Qt 4.6

Item {
    id: container

    signal clicked

    property string text

    Image {
        id: buttonImage
        source: "pics/button.png"
    }
    Image {
        id: pressed
        source: "pics/button-pressed.png"
        opacity: 0
    }
    MouseRegion {
        id: mouseRegion
        anchors.fill: buttonImage
        onClicked: { container.clicked(); }
    }
    Text {
        font.bold: true
        color: "white"
        anchors.centerIn: buttonImage
        text: container.text
    }
    width: buttonImage.width
    states: [
        State {
            name: "Pressed"
            when: mouseRegion.pressed == true
            PropertyChanges {
                target: pressed
                opacity: 1
            }
        }
    ]
}
