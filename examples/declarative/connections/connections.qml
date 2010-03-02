import Qt 4.6
import "content"

Rectangle {
    id: window; color: "#646464"
    width: 640; height: 480

    property int angle: 0

    Image {
        id: image; source: "content/bg1.jpg"; anchors.centerIn: parent; transformOrigin: Item.Center
	rotation: window.angle
        rotation: Behavior { NumberAnimation { easing.type: "OutCubic"; duration: 300 } }
    }

    Button {
        id: leftButton; image: "content/rotate-left.png"
        anchors { left: parent.left; bottom: parent.bottom; leftMargin: 10; bottomMargin: 10 }
    }
    Button {
        id: rightButton; image: "content/rotate-right.png"
        anchors { right: parent.right; bottom: parent.bottom; rightMargin: 10; bottomMargin: 10 }
    }

    Connections { target: leftButton; onClicked: window.angle -= 90 }
    Connections { target: rightButton; onClicked: window.angle += 90 }
}
