import Qt 4.7

Item {
    property variant progress: 0

    Rectangle {
        anchors.fill: parent; smooth: true
        border.color: "white"; border.width: 0; radius: height/2 - 2
        gradient: Gradient {
            GradientStop { position: 0; color: "#66343434" }
            GradientStop { position: 1.0; color: "#66000000" }
        }
    }

    Rectangle {
        y: 2; height: parent.height-4;
        x: 2; width: Math.max(parent.width * progress - 4, 0);
        opacity: width < 1 ? 0 : 1; smooth: true
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
