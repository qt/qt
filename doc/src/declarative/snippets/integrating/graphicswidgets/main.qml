import Qt 4.7
import Qt.widgets 4.7

Rectangle {
    width: 200
    height: 200

    RedSquare { 
        id: square
        width: 80
        height: 80
    }

    BlueCircle {
        anchors.left: square.right
        width: 80
        height: 80
    }

    QGraphicsWidget {
        anchors.top: square.bottom
        size.width: 80
        size.height: 80
        layout: QGraphicsLinearLayout {
            LayoutItem {
                preferredSize: "100x100"
                Rectangle { color: "yellow"; anchors.fill: parent }
            }
        }
    }
}

