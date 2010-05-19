import Qt 4.7

Rectangle {
    id: container

    property variant text
    signal clicked

    height: text.height + 10; width: text.width + 20
    border.width: 1
    radius: 4
    smooth: true

    gradient: Gradient {
        GradientStop {
            position: 0.0
            color: !mouseArea.pressed ? activePalette.light : activePalette.button
        }
        GradientStop {
            position: 1.0
            color: !mouseArea.pressed ? activePalette.button : activePalette.dark
        }
    }

    SystemPalette { id: activePalette }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: container.clicked()
    }

    Text {
        id: text
        anchors.centerIn:parent
        font.pointSize: 10
        text: parent.text
        color: activePalette.buttonText
    }
}
