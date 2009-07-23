import Qt 4.6

Rect {
    width: 360; height: 80
    color: "white"
//! [0]
    HorizontalLayout {
        margin: 10
        spacing: 10
        Image { source: "pics/qt.png" }
        Image {
            source: "pics/qt.png"
            transform: Rotation3D { axis.startX: 30; axis.endX: 30; axis.endY: 60; angle: 18 }
        }
        Image {
            source: "pics/qt.png"
            transform: Rotation3D { axis.startX: 30; axis.endX: 30; axis.endY: 60; angle: 36 }
        }
        Image {
            source: "pics/qt.png"
            transform: Rotation3D { axis.startX: 30; axis.endX: 30; axis.endY: 60; angle: 54 }
        }
        Image {
            source: "pics/qt.png"
            transform: Rotation3D { axis.startX: 30; axis.endX: 30; axis.endY: 60; angle: 72 }
        }
    }
//! [0]
}
