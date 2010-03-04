import Qt 4.6

Item {
    id: progressbar
    width: 250; height: 23; clip: true

    property int minimum: 0
    property int maximum: 100
    property int value: 0
    property alias color: g1.color
    property alias secondColor: g2.color

    BorderImage {
        source: "background.png"
        width: parent.width; height: parent.height
        border.left: 4; border.top: 4; border.right: 4; border.bottom: 4
    }

    Rectangle {
        property int widthDest: ((progressbar.width * (value - minimum)) / (maximum - minimum) - 6)
        id: highlight; radius: 1
        anchors.left: parent.left; anchors.top: parent.top; anchors.bottom: parent.bottom
        anchors.leftMargin: 3; anchors.topMargin: 3; anchors.bottomMargin: 3
        EaseFollow on width { source: highlight.widthDest; velocity: 1200 }
        gradient: Gradient {
            GradientStop { id: g1; position: 0.0 }
            GradientStop { id: g2; position: 1.0 }
        }
    }
    Text {
        anchors.right: highlight.right; anchors.rightMargin: 6
        color: "white"; font.bold: true
        anchors.verticalCenter: parent.verticalCenter
        text: Math.floor((value - minimum) / (maximum - minimum) * 100) + '%'
    }
}
