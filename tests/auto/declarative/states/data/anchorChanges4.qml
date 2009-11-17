import Qt 4.6

Rectangle {
    width: 200; height: 200
    Rectangle {
        id: MyRect
        objectName: "MyRect"
        color: "green";
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
    Item { objectName: "LeftGuideline"; id: LeftGuideline; x: 10 }
    Item { objectName: "BottomGuideline"; id: BottomGuideline; y: 150 }
    states: State {
        name: "reanchored"
        AnchorChanges {
            target: MyRect;
            horizontalCenter: BottomGuideline.horizontalCenter
            verticalCenter: LeftGuideline.verticalCenter
        }
    }
}
