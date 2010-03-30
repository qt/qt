import Qt 4.6

Rectangle {
    id: container

    signal clicked
    property string text: "Button"
    property bool down: false
    property string mainCol: "lightgray"
    property string darkCol: "darkgray"
    property string lightCol: "white"

    color: mainCol; smooth: true
    width: txtItem.width + 20; height: txtItem.height + 6
    border.width: 1; border.color: Qt.darker(mainCol); radius: 8;

    gradient: Gradient {
        GradientStop {
            id: topGrad; position: 0.0
            color: if (container.down) { darkCol } else { lightCol } }
        GradientStop { position: 1.0; color: mainCol }
    }

    MouseArea { id: mr; anchors.fill: parent; onClicked: container.clicked() }

    Text {
        id: txtItem; text: container.text;
        anchors.centerIn: container
        color: "blue"
        styleColor: "white"
        style: Text.Outline
        font.pixelSize: 14
        font.bold: true
    }
}
