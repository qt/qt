import Qt 4.6

Item {
    property var horizontalMode : "Stretch"
    property var verticalMode : "Stretch"
    property string source
    property int minWidth
    property int minHeight
    property int maxWidth
    property int maxHeight
    property int margin

    id: Container
    width: 240; height: 240
    BorderImage {
        x: Container.width / 2 - width / 2
        y: Container.height / 2 - height / 2
        width: SequentialAnimation {
            running: true; repeat: true
            NumberAnimation { from: Container.minWidth; to: Container.maxWidth; duration: 2000; easing: "easeInOutQuad"}
            NumberAnimation { from: Container.maxWidth; to: Container.minWidth; duration: 2000; easing: "easeInOutQuad" }
        }
        height: SequentialAnimation {
            running: true; repeat: true
            NumberAnimation { from: Container.minHeight; to: Container.maxHeight; duration: 2000; easing: "easeInOutQuad"}
            NumberAnimation { from: Container.maxHeight; to: Container.minHeight; duration: 2000; easing: "easeInOutQuad" }
        }
        source: Container.source
        horizontalTileMode: Container.horizontalMode
        verticalTileMode: Container.verticalMode
        border.top: Container.margin
        border.left: Container.margin
        border.bottom: Container.margin
        border.right: Container.margin
    }
}
