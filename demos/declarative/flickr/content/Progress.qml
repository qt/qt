Item {
    id: Progress;

    property var progress: 0

    Rect {
        id: Container; anchors.fill: parent; gradientColor: "#66000000";
        pen.color: "white"; pen.width: 1;  color: "#66343434"; radius: height/2 - 2
    }

    Rect {
        id: Fill
        y: 2; height: parent.height-4;
        x: 2; width: Math.max(parent.width * progress - 4, 0);
        opacity: width < 1 ? 0 : 1
        color: "lightsteelblue"; gradientColor: "steelblue"; radius: height/2 - 2
    }

    Text {
        text: Math.round(progress * 100) + "%"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        color: "white"; font.bold: true
    }
}
