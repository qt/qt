import Qt 4.6

Rectangle {
    id: imagePanel
    width: 1024
    height: 768
    color: "black"

    property int maximumColumn: 4
    property int selectedItemRow: 0
    property int selectedItemColumn: 0

    Script { source: "create.js" }

    onSelectedItemColumnChanged: if (selectedItemColumn == maximumColumn) createNewBlock();

    property int imageWidth: 200
    property int imageHeight: 200

    property int selectedX: selectedItemColumn * imageWidth
    property int selectedY: selectedItemRow * imageHeight

    Item {
        anchors.centerIn: parent
        Row {
            id: layout
            property real targetX: -(selectedX + imageWidth / 2)

            property real targetDeform: 0
            property bool slowDeform: true

            property real deform: 0
            deform: SpringFollow {
                id: deformFollow; source: layout.targetDeform; velocity: layout.slowDeform?0.1:2
                onSyncChanged: if(inSync) { layout.slowDeform = true; layout.targetDeform = 0; }
            }

            ImageBatch { offset: 0; ref: imagePanel }

            x: SpringFollow { source: layout.targetX; velocity: 1000 }
            y: SpringFollow { source: -(selectedY + imageHeight / 2); velocity: 500 }
        }

        transform: Rotation {
            axis.y: 1; axis.z: 0
            angle: layout.deform * -100
        }
    }

    Script {
        function left() {
            if (selectedItemColumn <= 0) return;
            selectedItemColumn -= 1;
            layout.slowDeform = false;
            layout.targetDeform = Math.max(Math.min(layout.deform - 0.1, -0.1), -0.4);
        }
        function right() {
            selectedItemColumn += 1;
            layout.slowDeform = false;
            layout.targetDeform = Math.min(Math.max(layout.deform + 0.1, 0.1), 0.4);
        }
    }

    focus: true
    Keys.onLeftPressed: "left()"
    Keys.onRightPressed: "right()"
    Keys.onUpPressed: "if (selectedItemRow > 0) selectedItemRow = selectedItemRow - 1"
    Keys.onDownPressed: "if (selectedItemRow < 3) selectedItemRow = selectedItemRow + 1"
}
