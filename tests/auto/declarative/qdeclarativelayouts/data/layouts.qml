import Qt 4.6
import Qt.widgets 4.6

Item {
    id: resizable
    width:300
    height:300
    QGraphicsWidget {
        x : resizable.x
        y : resizable.y
        width : resizable.width
        height : resizable.height
        layout: QGraphicsLinearLayout {
            spacing: 0
            LayoutItem {
                objectName: "left"
                minimumSize: "100x100"
                maximumSize: "300x300"
                preferredSize: "100x100"
                Rectangle { objectName: "yellowRect"; color: "yellow"; anchors.fill: parent }
            }
            LayoutItem {
                objectName: "right"
                minimumSize: "100x100"
                maximumSize: "400x400"
                preferredSize: "200x200"
                Rectangle { objectName: "greenRect"; color: "green"; anchors.fill: parent }
            }
        }
    }
}
