//![0]
import Qt 4.7

Rectangle {
    id: container

    signal clicked
    property string text: "Button"

    color: activePalette.button; smooth: true
    width: buttonLabel.width + 20; height: buttonLabel.height + 6
    border.width: 1; border.color: Qt.darker(activePalette.button); radius: 8;

    gradient: Gradient {
        GradientStop {
            position: 0.0
            color: if (mouseArea.pressed) { activePalette.dark } else { activePalette.light }
        }
        GradientStop { position: 1.0; color: activePalette.button }
    }

    MouseArea { id: mouseArea; anchors.fill: parent; onClicked: container.clicked() }

    Text {
        id: buttonLabel; text: container.text; anchors.centerIn: container; color: activePalette.buttonText
    }
}
//![0]
