import Qt 4.7

Item {
    id: progressbar

    property int minimum: 0
    property int maximum: 100
    property int value: 0
    property alias color: gradient1.color
    property alias secondColor: gradient2.color

    width: 250; height: 23
    clip: true

    BorderImage {
        source: "background.png"
        width: parent.width; height: parent.height
        border { left: 4; top: 4; right: 4; bottom: 4 }
    }

    Rectangle {
        id: highlight

        property int widthDest: ((progressbar.width * (value - minimum)) / (maximum - minimum) - 6)

        width: highlight.widthDest
        Behavior on width { SmoothedAnimation { velocity: 1200 } }

        anchors { left: parent.left; top: parent.top; bottom: parent.bottom; leftMargin: 3; topMargin: 3; bottomMargin: 3 }
        radius: 1
        gradient: Gradient {
            GradientStop { id: gradient1; position: 0.0 }
            GradientStop { id: gradient2; position: 1.0 }
        }

    }
    Text {
        anchors { right: highlight.right; rightMargin: 6; verticalCenter: parent.verticalCenter }
        color: "white"
        font.bold: true
        text: Math.floor((value - minimum) / (maximum - minimum) * 100) + '%'
    }
}
