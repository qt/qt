import Qt 4.6
import "content"

Rectangle {
    id: window; color: "#646464"
    width: 640; height: 480

    function turnLeft() {
        image.rotation -= 90
    }
    function turnRight() {
        image.rotation += 90
    }

    Image {
        id: image; source: "content/bg1.jpg"; anchors.centerIn: parent; transformOrigin: Item.Center
        rotation: Behavior { NumberAnimation { easing: "easeOutCubic"; duration: 300 } }
    }

    Button {
        id: leftButton; image: "content/rotate-left.png"
        anchors { left: parent.left; bottom: parent.bottom; leftMargin: 10; bottomMargin: 10 }
    }
    Button {
        id: rightButton; image: "content/rotate-right.png"
        anchors { right: parent.right; bottom: parent.bottom; rightMargin: 10; bottomMargin: 10 }
    }

    Connection { sender: leftButton; signal: "clicked()"; script: window.turnLeft() }
    Connection { sender: rightButton; signal: "clicked()"; script: window.turnRight() }
}
