import Qt 4.6

Rectangle {
    color: "white"
    width: 240
    height: 320
    Rectangle { id: MasterRect; objectName: "MasterRect"; x: 26; width: 96; height: 20; color: "red" }
    Rectangle {
        id: Rect1; objectName: "Rect1"
        y: 20; width: 10; height: 10
        anchors.left: MasterRect.left
    }
    Rectangle {
        id: Rect2; objectName: "Rect2"
        y: 20; width: 10; height: 10
        anchors.left: MasterRect.right
    }
    Rectangle {
        id: Rect3; objectName: "Rect3"
        y: 20; width: 10; height: 10
        anchors.left: MasterRect.horizontalCenter
    }
    Rectangle {
        id: Rect4; objectName: "Rect4"
        y: 30; width: 10; height: 10
        anchors.right: MasterRect.left
    }
    Rectangle {
        id: Rect5; objectName: "Rect5"
        y: 30; width: 10; height: 10
        anchors.right: MasterRect.right
    }
    Rectangle {
        id: Rect6; objectName: "Rect6"
        y: 30; width: 10; height: 10
        anchors.right: MasterRect.horizontalCenter
    }
    Rectangle {
        id: Rect7; objectName: "Rect7"
        y: 50; width: 10; height: 10
        anchors.left: parent.left
    }
    Rectangle {
        id: Rect8; objectName: "Rect8"
        y: 50; width: 10; height: 10
        anchors.left: parent.right
    }
    Rectangle {
        id: Rect9; objectName: "Rect9"
        y: 50; width: 10; height: 10
        anchors.left: parent.horizontalCenter
    }
    Rectangle {
        id: Rect10; objectName: "Rect10"
        y: 60; width: 10; height: 10
        anchors.right: parent.left
    }
    Rectangle {
        id: Rect11; objectName: "Rect11"
        y: 60; width: 10; height: 10
        anchors.right: parent.right
    }
    Rectangle {
        id: Rect12; objectName: "Rect12"
        y: 60; width: 10; height: 10
        anchors.right: parent.horizontalCenter
    }
    Rectangle {
        id: Rect13; objectName: "Rect13"
        x: 200; width: 10; height: 10
        anchors.top: MasterRect.bottom
    }
    Rectangle {
        id: Rect14; objectName: "Rect14"
        width: 10; height: 10; color: "steelblue"
        anchors.verticalCenter: parent.verticalCenter
    }
    Rectangle {
        id: Rect15; objectName: "Rect15"
        y: 200; height: 10
        anchors.left: MasterRect.left
        anchors.right: MasterRect.right
    }
    Rectangle {
        id: Rect16; objectName: "Rect16"
        y: 220; height: 10
        anchors.left: MasterRect.left
        anchors.horizontalCenter: MasterRect.right
    }
    Rectangle {
        id: Rect17; objectName: "Rect17"
        y: 240; height: 10
        anchors.right: MasterRect.right
        anchors.horizontalCenter: MasterRect.left
    }
    Rectangle {
        id: Rect18; objectName: "Rect18"
        x: 180; width: 10
        anchors.top: MasterRect.bottom
        anchors.bottom: Rect12.top
    }
    Rectangle {
        id: Rect19; objectName: "Rect19"
        y: 70; width: 10; height: 10
        anchors.horizontalCenter: parent.horizontalCenter
    }
    Rectangle {
        id: Rect20; objectName: "Rect20"
        y: 70; width: 10; height: 10
        anchors.horizontalCenter: parent.right
    }
    Rectangle {
        id: Rect21; objectName: "Rect21"
        y: 70; width: 10; height: 10
        anchors.horizontalCenter: parent.left
    }
}
