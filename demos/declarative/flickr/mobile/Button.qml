import Qt 4.6

Item {
    id: Container

    signal clicked

    property string text

    BorderImage {
        id: Image
        source: "images/toolbutton2.sci"
        width: Container.width; height: Container.height
    }
    BorderImage {
        id: Pressed
        opacity: 0
        source: "images/toolbutton2.sci"
        width: Container.width; height: Container.height
    }
    MouseRegion {
        id: MouseRegion
        anchors.fill: Image
        onClicked: { Container.clicked(); }
    }
    Text {
        color: "white"
        anchors.centerIn: Image; font.bold: true
        text: Container.text; style: "Raised"; styleColor: "black"
    }
    states: [
        State {
            name: "Pressed"
            when: MouseRegion.pressed == true
            PropertyChanges {
                target: Pressed
                opacity: 1
            }
        }
    ]
}
