import Qt 4.6

Item {
    id: Progress;

    property var progress: 0

    Rect {
        id: Container; anchors.fill: parent
        pen.color: "white"; pen.width: 0; radius: height/2 - 2
        gradient: Gradient {
            GradientStop { position: 0; color: "#66343434" }
            GradientStop { position: 1.0; color: "#66000000" }
        }
    }

    Rect {
        id: Fill
        y: 2; height: parent.height-4;
        x: 2; width: Math.max(parent.width * progress - 4, 0);
        opacity: width < 1 ? 0 : 1
        gradient: Gradient {
            GradientStop { position: 0; color: "lightsteelblue" }
            GradientStop { position: 1.0; color: "steelblue" }
        }
        radius: height/2 - 2
    }

    Text {
        text: Math.round(progress * 100) + "%"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        color: "white"; font.bold: true
    }
}
