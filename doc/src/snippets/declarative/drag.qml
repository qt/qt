import Qt 4.7

//! [0]
Rectangle {
    id: blurtest; width: 600; height: 200; color: "white"
    Image {
        id: pic; source: "qtlogo-64.png"; anchors.verticalCenter: parent.verticalCenter
        opacity: (600.0-pic.x) / 600;
        MouseArea {
            anchors.fill: parent
            drag.target: pic
            drag.axis: "XAxis"
            drag.minimumX: 0
            drag.maximumX: blurtest.width-pic.width
        }
    }
}
//! [0]
