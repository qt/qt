import Qt 4.6

Rectangle {
    width: 200; height: 200
    Rectangle {
        id: MyRect
        objectName: "MyRect"
        width: 50; height: 50
        color: "green";
        anchors.left: parent.left
        anchors.leftMargin: 5
    }
    states: State {
        name: "right"
        AnchorChanges {
            target: MyRect;
            reset: "left"
            right: parent.right
        }
    }
}
