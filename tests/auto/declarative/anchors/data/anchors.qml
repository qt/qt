import Qt 4.6

Rect {
    color: "white"
    width: 240
    height: 320
    Rect { id: MasterRect; x: 26; width: 96; height: 20; color: "red" }
    Rect {
        id: Rect1
        y: 20; width: 10; height: 10
        anchors.left: MasterRect.left
    }
    Rect {
        id: Rect2
        y: 20; width: 10; height: 10
        anchors.left: MasterRect.right
    }
    Rect {
        id: Rect3
        y: 20; width: 10; height: 10
        anchors.left: MasterRect.horizontalCenter
    }
    Rect {
        id: Rect4
        y: 30; width: 10; height: 10
        anchors.right: MasterRect.left
    }
    Rect {
        id: Rect5
        y: 30; width: 10; height: 10
        anchors.right: MasterRect.right
    }
    Rect {
        id: Rect6
        y: 30; width: 10; height: 10
        anchors.right: MasterRect.horizontalCenter
    }
    Rect {
        id: Rect7
        y: 50; width: 10; height: 10
        anchors.left: parent.left
    }
    Rect {
        id: Rect8
        y: 50; width: 10; height: 10
        anchors.left: parent.right
    }
    Rect {
        id: Rect9
        y: 50; width: 10; height: 10
        anchors.left: parent.horizontalCenter
    }
    Rect {
        id: Rect10
        y: 60; width: 10; height: 10
        anchors.right: parent.left
    }
    Rect {
        id: Rect11
        y: 60; width: 10; height: 10
        anchors.right: parent.right
    }
    Rect {
        id: Rect12
        y: 60; width: 10; height: 10
        anchors.right: parent.horizontalCenter
    }
    Rect {
        id: Rect13
        x: 200; width: 10; height: 10
        anchors.top: MasterRect.bottom
    }
    Rect {
        id: Rect14
        width: 10; height: 10; color: "steelblue"
        anchors.verticalCenter: parent.verticalCenter
    }
    Rect {
        id: Rect15
        y: 200; height: 10
        anchors.left: MasterRect.left
        anchors.right: MasterRect.right
    }
    Rect {
        id: Rect16
        y: 220; height: 10
        anchors.left: MasterRect.left
        anchors.horizontalCenter: MasterRect.right
    }
    Rect {
        id: Rect17
        y: 240; height: 10
        anchors.right: MasterRect.right
        anchors.horizontalCenter: MasterRect.left
    }
    Rect {
        id: Rect18
        x: 180; width: 10
        anchors.top: MasterRect.bottom
        anchors.bottom: Rect12.top
    }
    Rect {
        id: Rect19
        y: 70; width: 10; height: 10
        anchors.horizontalCenter: parent.horizontalCenter
    }
    Rect {
        id: Rect20
        y: 70; width: 10; height: 10
        anchors.horizontalCenter: parent.right
    }
    Rect {
        id: Rect21
        y: 70; width: 10; height: 10
        anchors.horizontalCenter: parent.left
    }
}
