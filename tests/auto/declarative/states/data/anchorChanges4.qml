import Qt 4.6

Rectangle {
    width: 200; height: 200
    Rectangle {
        id: myRect
        objectName: "myRect"
        color: "green";
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
    Item { objectName: "leftGuideline"; id: leftGuideline; x: 10 }
    Item { objectName: "bottomGuideline"; id: bottomGuideline; y: 150 }
    states: State {
        name: "reanchored"
        AnchorChanges {
            target: myRect;
            horizontalCenter: bottomGuideline.horizontalCenter
            verticalCenter: leftGuideline.verticalCenter
        }
    }
}
