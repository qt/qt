import Qt 4.6

//! [0]
Rect {
    id: blurtest; width: 600; height: 200; color: "white"
    Image {
        id: pic; source: "qtlogo-64.png"; anchors.verticalCenter: parent.verticalCenter
        opacity: (600.0-pic.x) / 600;
        MouseRegion {
            anchors.fill: parent
            drag.target: pic
            drag.axis: "x"
            drag.xmin: 0
            drag.xmax: blurtest.width-pic.width
        }
    }
}
//! [0]
