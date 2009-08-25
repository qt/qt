import Qt 4.6

Rectangle {
    id: Container

    signal clicked
    property string text: "Button"

    color: activePalette.button; smooth: true
    width: txtItem.width + 20; height: txtItem.height + 6
    border.width: 1; border.color: activePalette.darker(activePalette.button); radius: 8;

    gradient: Gradient {
        GradientStop {
            id: topGrad; position: 0.0
            color: if (mr.pressed) { activePalette.dark } else { activePalette.light } }
        GradientStop { position: 1.0; color: activePalette.button }
    }

    MouseRegion { id: mr; anchors.fill: parent; onClicked: Container.clicked() }

    Text {
        id: txtItem; text: Container.text; anchors.centerIn: Container; color: activePalette.buttonText
    }
}
