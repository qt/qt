import Qt 4.6

Rect {
    id: Page
    color: "white"
    width: 520; height: 280

    HorizontalPositioner {
        anchors.centerIn: parent
        spacing: 50
//! [0]
        BorderImage {
            width: 180; height: 180
            border.left: 30; border.top: 30
            border.right: 30; border.bottom: 30
            horizontalTileMode: "Stretch"
            verticalTileMode: "Stretch"
            source: "colors.png"
        }

        BorderImage {
            width: 180; height: 180
            border.left: 30; border.top: 30
            border.right: 30; border.bottom: 30
            horizontalTileMode: "Round"
            verticalTileMode: "Round"
            source: "colors.png"
        }
//! [0]
    }
}
