import Qt 4.7

Rectangle {
    id: container

    property string text: "Button"
    property bool down: false
    property string mainCol: "lightgray"
    property string darkCol: "darkgray"
    property string lightCol: "white"

    width: txtItem.width + 20; height: txtItem.height + 6
    border { width: 1; color: Qt.darker(mainCol) } 
    radius: 8;
    color: mainCol
    smooth: true

    gradient: Gradient {
        GradientStop {
            id: topGrad; position: 0.0
            color: if (container.down) { darkCol } else { lightCol }
        }
        GradientStop { position: 1.0; color: mainCol }
    }

    signal clicked

    MouseArea { id: mr; anchors.fill: parent; onClicked: container.clicked() }

    Text {
        id: txtItem

        anchors.centerIn: container
        text: container.text;
        color: "blue"
        style: Text.Outline; styleColor: "white"
        font.pixelSize: 14; font.bold: true
    }
}
