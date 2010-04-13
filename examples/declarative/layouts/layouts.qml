import Qt 4.7
import Qt.widgets 4.7

Item {
    id: resizable

    width: 400
    height: 400

    QGraphicsWidget {
        size.width: parent.width
        size.height: parent.height

        layout: QGraphicsLinearLayout {
            LayoutItem {
                minimumSize: "100x100"
                maximumSize: "300x300"
                preferredSize: "100x100"
                Rectangle { color: "yellow"; anchors.fill: parent }
            }
            LayoutItem {
                minimumSize: "100x100"
                maximumSize: "400x400"
                preferredSize: "200x200"
                Rectangle { color: "green"; anchors.fill: parent }
            }
        }
    }
}
