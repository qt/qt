import Qt 4.6

Rectangle{
    width:400
    height:440
    color: "white"
    Rectangle{
        id: draggable
        width:40; height:40; color: "lightsteelblue"
            y:20
        MouseArea{
            anchors.fill: parent
            drag.target: draggable
            drag.axis: "XandYAxis"
            drag.minimumX: 0
            drag.maximumX: 360
            drag.minimumY: 20
            drag.maximumY: 400
        }
    }
}
