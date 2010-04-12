import Qt 4.6
import Qt.widgets 4.6

QGraphicsWidget {
    layout: QGraphicsLinearLayout {
        orientation: Qt.Horizontal
        QGraphicsWidget {
            layout: QGraphicsLinearLayout {
                spacing: 10; orientation: Qt.Vertical
                LayoutItem {
                    QGraphicsLinearLayout.stretchFactor: 1
                    objectName: "left"
                    minimumSize: "100x100"
                    maximumSize: "300x300"
                    preferredSize: "100x100"
                    Rectangle { objectName: "yellowRect"; color: "yellow"; anchors.fill: parent }
                }
                LayoutItem {
                    QGraphicsLinearLayout.stretchFactor: 10
                    objectName: "left"
                    minimumSize: "100x100"
                    maximumSize: "300x300"
                    preferredSize: "100x100"
                    Rectangle { objectName: "yellowRect"; color: "blue"; anchors.fill: parent }
                }
            }
        }
        QGraphicsWidget {
            layout: QGraphicsLinearLayout {
                spacing: 10; orientation: Qt.Vertical
                LayoutItem {
                    objectName: "left"
                    minimumSize: "100x100"
                    maximumSize: "300x300"
                    preferredSize: "100x100"
                    Rectangle { objectName: "yellowRect"; color: "red"; anchors.fill: parent }
                }
                LayoutItem {
                    objectName: "left"
                    minimumSize: "100x100"
                    maximumSize: "300x300"
                    preferredSize: "100x100"
                    Rectangle { objectName: "yellowRect"; color: "green"; anchors.fill: parent }
                }
            }
        }
    }
}

