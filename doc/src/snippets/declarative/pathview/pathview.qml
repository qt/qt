import Qt 4.6

Rectangle {
    width: 240; height: 200; color: 'white'
//! [0]
//! [1]
    Component {
        id: Delegate
        Item {
            id: Wrapper
            width: 80; height: 80
            VerticalLayout {
                Image { anchors.horizontalCenter: Name.horizontalCenter; width: 64; height: 64; source: icon }
                Text { id: Name; text: name; font.pointSize: 16}
            }
        }
    }
//! [1]
//! [2]
    PathView {
        anchors.fill: parent; model: MenuModel; delegate: Delegate
        path: Path {
            startX: 120; startY: 100
            PathQuad { x: 120; y: 25; controlX: 260; controlY: 75 }
            PathQuad { x: 120; y: 100; controlX: -20; controlY: 75 }
        }
    }
//! [2]
//! [0]
}
