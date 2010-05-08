import Qt 4.7
import GraphicsLayouts 4.7

Item {
    id: resizable

    width: 800
    height: 400

    QGraphicsWidget {
        size.width: parent.width/2
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
    QGraphicsWidget {
        x: parent.width/2
        size.width: parent.width/2
        size.height: parent.height

        layout: QGraphicsGridLayout {
            LayoutItem {
                QGraphicsGridLayout.row: 0
                QGraphicsGridLayout.column: 0
                minimumSize: "100x100"
                maximumSize: "300x300"
                preferredSize: "100x100"
                Rectangle { color: "red"; anchors.fill: parent }
            }
            LayoutItem {
                QGraphicsGridLayout.row: 1
                QGraphicsGridLayout.column: 0
                minimumSize: "100x100"
                maximumSize: "200x200"
                preferredSize: "100x100"
                Rectangle { color: "orange"; anchors.fill: parent }
            }
            LayoutItem {
                QGraphicsGridLayout.row: 2
                QGraphicsGridLayout.column: 0
                minimumSize: "100x100"
                maximumSize: "300x300"
                preferredSize: "200x200"
                Rectangle { color: "yellow"; anchors.fill: parent }
            }
            LayoutItem {
                QGraphicsGridLayout.row: 0
                QGraphicsGridLayout.column: 1
                minimumSize: "100x100"
                maximumSize: "200x200"
                preferredSize: "200x200"
                Rectangle { color: "green"; anchors.fill: parent }
            }
            LayoutItem {
                QGraphicsGridLayout.row: 1
                QGraphicsGridLayout.column: 1
                minimumSize: "100x100"
                maximumSize: "400x400"
                preferredSize: "200x200"
                Rectangle { color: "blue"; anchors.fill: parent }
            }
        }
    }
}
