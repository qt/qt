import Qt 4.6
import "content"

Rectangle {
    color: "#545454"
    width: 300; height: 300

    // Dial with a slider to adjust it
    Dial { id: dial; anchors.centerIn: parent; value: slider.x *100 / (container.width - 34) }

    Rectangle {
        id: container
        anchors.bottom: parent.bottom; anchors.bottomMargin: 10
        anchors.left: parent.left; anchors.leftMargin: 20
        anchors.right: parent.right; anchors.rightMargin: 20; height: 16
        gradient: Gradient {
            GradientStop { position: 0.0; color: "gray" }
            GradientStop { position: 1.0; color: "white" }
        }
        radius: 8; opacity: 0.7; smooth: true
        Rectangle {
            id: slider
            x: 1; y: 1; width: 30; height: 14
            radius: 6; smooth: true
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#424242" }
                GradientStop { position: 1.0; color: "black" }
            }
            MouseRegion {
                anchors.fill: parent
                drag.target: parent; drag.axis: "XAxis"; drag.minimumX: 2; drag.maximumX: container.width - 32
            }
        }
    }
}
