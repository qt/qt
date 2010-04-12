import Qt 4.7

Rectangle {
    width: 240; height: 200; color: 'white'
//! [0]
//! [1]
    Component {
        id: delegate
        Item {
            id: wrapper
            width: 80; height: 80
            Column {
                Image { anchors.horizontalCenter: name.horizontalCenter; width: 64; height: 64; source: icon }
                Text { text: name; font.pointSize: 16}
            }
        }
    }
//! [1]
//! [2]
    PathView {
        anchors.fill: parent; model: MenuModel; delegate: delegate
        path: Path {
            startX: 120; startY: 100
            PathQuad { x: 120; y: 25; controlX: 260; controlY: 75 }
            PathQuad { x: 120; y: 100; controlX: -20; controlY: 75 }
        }
    }
//! [2]
//! [0]
}
