import Qt 4.6

Rect {
    id: ImagePanel
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
        HorizontalPositioner {
            id: MyLayout
            property real targetX: -(selectedX + imageWidth / 2)

            property real targetDeform: 0
            property bool slowDeform: true

            property real deform: 0
            deform: Follow {
                id: "DeformFollow"; source: MyLayout.targetDeform; velocity: MyLayout.slowDeform?0.1:2
                onSyncChanged: if(inSync) { MyLayout.slowDeform = true; MyLayout.targetDeform = 0; }
            }
                
            ImageBatch { offset: 0; ref: ImagePanel }

            x: Follow { source: MyLayout.targetX; velocity: 1000 }
            y: Follow { source: -(selectedY + imageHeight / 2); velocity: 500 }
        }

        transform: Rotation {
            axis.y: 1; axis.z: 0
            angle: MyLayout.deform * -100
        }
    }

    Script {
        function left() {
            if (selectedItemColumn <= 0) return;
            selectedItemColumn -= 1;
            MyLayout.slowDeform = false;
            MyLayout.targetDeform = Math.max(Math.min(MyLayout.deform - 0.1, -0.1), -0.4);
        }
        function right() {
            selectedItemColumn += 1;
            MyLayout.slowDeform = false;
            MyLayout.targetDeform = Math.min(Math.max(MyLayout.deform + 0.1, 0.1), 0.4);
        }
    }

    focus: true
    Keys.onLeftPressed: "left()"
    Keys.onRightPressed: "right()"
    Keys.onUpPressed: "if (selectedItemRow > 0) selectedItemRow = selectedItemRow - 1"
    Keys.onDownPressed: "if (selectedItemRow < 3) selectedItemRow = selectedItemRow + 1"
}
