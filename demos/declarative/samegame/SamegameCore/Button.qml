import Qt 4.7

Rectangle {
    id: container

    signal clicked
    property string text: "Button"

    color: activePalette.button; smooth: true
    width: txtItem.width + 20; height: txtItem.height + 6
    border.width: 1; border.color: Qt.darker(activePalette.button); radius: 8;

    gradient: Gradient {
        GradientStop {
            id: topGrad; position: 0.0
            color: if (mr.pressed) { activePalette.dark } else { activePalette.light } }
        GradientStop { position: 1.0; color: activePalette.button }
    }

    MouseArea { id: mr; anchors.fill: parent; onClicked: container.clicked() }

    Text {
        id: txtItem; text: container.text; anchors.centerIn: container; color: activePalette.buttonText
    }
}
