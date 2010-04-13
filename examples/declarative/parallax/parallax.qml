import Qt 4.7
import "../clocks/content"
import "qml"

Rectangle {
    id: root

    width: 320; height: 480

    ParallaxView {
        id: parallax
        anchors.fill: parent
        background: "pics/background.jpg"

        Item {
            property url icon: "pics/yast-wol.png"
            width: 320; height: 480
            Clock { anchors.centerIn: parent }
        }

        Item {
            property url icon: "pics/home-page.svg"
            width: 320; height: 480
            Smiley { }
        }

        Item {
            property url icon: "pics/yast-joystick.png"
            width: 320; height: 480

            Loader {
                anchors { top: parent.top; topMargin: 10; horizontalCenter: parent.horizontalCenter }
                width: 300; height: 400
                clip: true; resizeMode: Loader.SizeItemToLoader
                source: "../../../demos/declarative/samegame/samegame.qml"
            }
        }

        currentIndex: root.currentIndex
    }
}
