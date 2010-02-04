import Qt 4.6

Rectangle {
    id: container
    width: 200; height: 200
    Rectangle {
        id: myRect
        objectName: "myRect"
        color: "green";
        anchors.left: parent.left
        anchors.right: rightGuideline.left
        anchors.top: topGuideline.top
        anchors.bottom: container.bottom
    }
    Item { objectName: "leftGuideline"; id: leftGuideline; x: 10 }
    Item { id: rightGuideline; x: 150 }
    Item { id: topGuideline; y: 10 }
    Item { objectName: "bottomGuideline"; id: bottomGuideline; y: 150 }
    states: State {
        name: "reanchored"
        AnchorChanges {
            target: myRect;
            left: leftGuideline.left
            right: container.right
            top: container.top
            bottom: bottomGuideline.bottom
        }
    }
}
