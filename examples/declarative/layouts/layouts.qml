import Qt 4.6

Item { id: resizable
    width:400; height:400;
    GraphicsObjectContainer{
        anchors.fill:parent
        QGraphicsWidget{
            geometry:{ "0,0," + parent.width.toString() + "x" + parent.height.toString(); }
            layout: QGraphicsLinearLayout{   
                LayoutItem{
                    minimumSize: "100x100"
                    maximumSize: "300x300"
                    preferredSize: "100x100"
                    Rectangle { color: "yellow"; anchors.fill: parent }
                }
                LayoutItem{
                    minimumSize: "100x100"
                    maximumSize: "400x400"
                    preferredSize: "200x200"
                    Rectangle { color: "green"; anchors.fill: parent }
                }
            }
        }
    }
}
