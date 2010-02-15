import Qt 4.6

TabWidget {
    id: tabs
    width: 200
    height: 200
    current: 2
    Rectangle {
        property string title: "Red"
        color: "red"
        anchors.fill: parent
        Text { anchors.centerIn: parent; text: "<div align=center>Roses are red"; font.pixelSize: 24
            wrap: true; width: parent.width-20 }
    }
    Rectangle {
        property string title: "Green"
        color: "green"
        anchors.fill: parent
        Text { anchors.centerIn: parent; text: "<div align=center>Flower stems are green"; font.pixelSize: 24;
            wrap: true; width: parent.width-20 }
    }
    Rectangle {
        property string title: "Blue"
        color: "blue"
        anchors.fill: parent
        Text { anchors.centerIn: parent; text: "<div align=center>Violets are blue"; color: "white"; font.pixelSize: 24
            wrap: true; width: parent.width-20 }
    }
}
