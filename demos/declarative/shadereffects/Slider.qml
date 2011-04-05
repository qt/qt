import QtQuick 2.0

Item {
    property real value: bar.x / (foo.width - bar.width)
    Item {
        id: foo
        width: parent.width - 4
        height: 6
        anchors.centerIn: parent

        Rectangle {
            height: parent.height
            anchors.left: parent.left
            anchors.right: bar.horizontalCenter
            color: "blue"
            radius: 3
        }
        Rectangle {
            height: parent.height
            anchors.left: bar.horizontalCenter
            anchors.right: parent.right
            color: "gray"
            radius: 3
        }
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            radius: 3
            border.width: 2
            border.color: "black"
        }

        Rectangle {
            id: bar
            y: -7
            width: 20
            height: 20
            radius: 15
            color: "white"
            border.width: 2
            border.color: "black"
            MouseArea {
                anchors.fill: parent
                drag.target: parent
                drag.axis: Drag.XAxis
                drag.minimumX: 0
                drag.maximumX: foo.width - parent.width
            }
        }
    }
}

