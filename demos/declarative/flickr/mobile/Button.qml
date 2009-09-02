import Qt 4.6

Item {
    id: Container

    signal clicked

    property string text

    BorderImage {
        id: ButtonImage
        source: "images/toolbutton.sci"
        width: Container.width; height: Container.height
    }
    BorderImage {
        id: Pressed
        opacity: 0
        source: "images/toolbutton.sci"
        width: Container.width; height: Container.height
    }
    MouseRegion {
        id: MyMouseRegion
        anchors.fill: ButtonImage
        onClicked: { Container.clicked(); }
    }
    Text {
        color: "white"
        anchors.centerIn: ButtonImage; font.bold: true
        text: Container.text; style: "Raised"; styleColor: "black"
    }
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
