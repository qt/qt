import Qt 4.7

Image {
    property bool spawned: false
    property int row;
    property int column;
    property int verticalMovement;
    property int horizontalMovement;

    x: margin + column * gridSize + 2
    y: margin + row * gridSize - 3
    Behavior on x { NumberAnimation { duration: spawned ? halfbeatInterval : 0} }
    Behavior on y { NumberAnimation { duration: spawned ? halfbeatInterval : 0 } }

    opacity: spawned ? 1 : 0
    Behavior on opacity { NumberAnimation { duration: 200 } }

    source: "pics/skull.png"
    width: 24
    height: 40
}
