import Qt 4.7

Rectangle {
    id: root
    color: "#ffffff"
    width: 320; height: 240
    property bool mr1_pressed: false
    property bool mr1_released: false
    property bool mr1_canceled: false
    property bool mr2_pressed: false
    property bool mr2_released: false
    property bool mr2_canceled: false

    MouseArea {
        id: mouseRegion1
        anchors.fill: parent
        onPressed: {console.log("press111"); root.mr1_pressed = true}
        onReleased: {console.log("release111"); root.mr1_released = true}
        onCanceled: {console.log("ungrab1111"); root.mr1_canceled = true}
    }
    MouseArea {
        id: mouseRegion2
        width: 120; height: 120
        onPressed: {console.log("press222"); root.mr2_pressed = true; mouse.accepted = false}
        onReleased: {console.log("release2222"); root.mr2_released = true}
        onCanceled: {console.log("ungrab2222"); root.mr2_canceled = true}
    }
}
