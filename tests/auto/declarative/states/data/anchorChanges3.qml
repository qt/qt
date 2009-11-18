import Qt 4.6

Rectangle {
    id: Container
    width: 200; height: 200
    Rectangle {
        id: MyRect
        objectName: "MyRect"
        color: "green";
        anchors.left: parent.left
        anchors.right: RightGuideline.left
        anchors.top: TopGuideline.top
        anchors.bottom: Container.bottom
    }
    Item { objectName: "LeftGuideline"; id: LeftGuideline; x: 10 }
    Item { id: RightGuideline; x: 150 }
    Item { id: TopGuideline; y: 10 }
    Item { objectName: "BottomGuideline"; id: BottomGuideline; y: 150 }
    states: State {
        name: "reanchored"
        AnchorChanges {
            target: MyRect;
            left: LeftGuideline.left
            right: Container.right
            top: Container.top
            bottom: BottomGuideline.bottom
        }
    }
}
