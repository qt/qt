import Qt 4.6

Item {
    id: Page
    height: MyText.height + 20
    property var text
    MouseRegion {
        anchors.fill: parent
        drag.target: Page
        drag.axis: "XandYAxis"
        drag.minimumX: 0
        drag.maximumX: 1000
        drag.minimumY: 0
        drag.maximumY: 1000
    }
    Rect {
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
            id: MyText
            text: Page.text
            width: parent.width
            clip: true
            wrap: true
        }
    }
}
