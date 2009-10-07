import Qt 4.6

Item {
    property var text
    signal clicked

    id: container
    Image {
        id: normal
        source: "pics/button.png"
    }
    Image {
        id: pressed
        source: "pics/button-pressed.png"
        opacity: 0
    }
    MouseRegion {
        id: clickRegion
        anchors.fill: normal
        onClicked: { container.clicked(); }
    }
    Text {
        font.bold: true
        color: "white"
        anchors.centerIn: normal
        text: container.text
    }
    width: normal.width

    states: State {
        name: "Pressed"
        when: clickRegion.pressed == true
        PropertyChanges { target: pressed; opacity: 1 }
    }
}
