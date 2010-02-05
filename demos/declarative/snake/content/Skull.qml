import Qt 4.6

Image {
    property bool spawned: false
    property int row;
    property int column;
    property int verticalMovement;
    property int horizontalMovement;

    x: margin + column * gridSize + 2
    y: margin + row * gridSize - 3
    x: Behavior { NumberAnimation { duration: spawned ? halfbeatInterval : 0} }
    y: Behavior { NumberAnimation { duration: spawned ? halfbeatInterval : 0 } }

    opacity: spawned ? 1 : 0
    opacity: Behavior { NumberAnimation { duration: 200 } }

    source: "pics/skull.png"
    width: 24
    height: 40
}
