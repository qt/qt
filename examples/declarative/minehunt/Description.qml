import Qt 4.6

Item {
    id: Page
    height: MyText.height + 20
    property var text
    MouseRegion {
        anchors.fill: parent
        drag.target: Page
        drag.axis: "xy"
        drag.xmin: 0
        drag.xmax: 1000
        drag.ymin: 0
        drag.ymax: 1000
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
