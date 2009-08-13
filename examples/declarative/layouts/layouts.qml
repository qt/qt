import Qt 4.6

QGraphicsView{
QGraphicsScene{
QGraphicsWidget{
    visible: true
    opacity: 1
    geometry: "0,0,400x400"
    layout: QGraphicsLinearLayout{   
        LayoutItem{
            minimumSize: "100x100"
            maximumSize: "300x300"
            preferredSize: "100x100"
            Rect { color: "yellow"; anchors.fill: parent }
        }
        LayoutItem{
            minimumSize: "100x100"
            maximumSize: "400x400"
            preferredSize: "200x200"
            Rect { color: "green"; anchors.fill: parent }
        }
    }
}
}
}
