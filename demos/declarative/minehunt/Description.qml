import Qt 4.6

Item {
    id: page
    height: myText.height + 20
    property var text
    MouseArea {
        anchors.fill: parent
        drag.target: page
        drag.axis: "XandYAxis"
        drag.minimumX: 0
        drag.maximumX: 1000
        drag.minimumY: 0
        drag.maximumY: 1000
    }
    Rectangle {
        radius: 10
        anchors.fill: parent
        color: "lightsteelblue"
    }
    Item {
        x: 10
        y: 10
        width: parent.width - 20
        height: parent.height - 20
        Text {
            id: myText
            text: page.text
            width: parent.width
            clip: true
            wrap: true
        }
    }
}
