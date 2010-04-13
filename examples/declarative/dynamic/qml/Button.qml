import Qt 4.7

Rectangle {
    id: container

    property variant text
    signal clicked

    SystemPalette { id: activePalette }
    height: text.height + 10
    width: text.width + 20
    border.width: 1
    radius: 4; smooth: true
    gradient: Gradient {
        GradientStop { position: 0.0; 
            color: if(!mr.pressed){activePalette.light;}else{activePalette.button;}
        }
        GradientStop { position: 1.0;
            color: if(!mr.pressed){activePalette.button;}else{activePalette.dark;}
        }
    }
    MouseArea { id:mr; anchors.fill: parent; onClicked: container.clicked() }
    Text { id: text; anchors.centerIn:parent; font.pointSize: 10; text: parent.text; color: activePalette.buttonText }
}
