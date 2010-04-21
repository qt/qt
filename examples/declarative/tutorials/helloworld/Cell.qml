//![0]
import Qt 4.7

//![1]
Item {
    id: container
//![4]
    property alias cellColor: rectangle.color
//![4]
//![5]
    signal clicked(color cellColor)
//![5]

    width: 40; height: 25
//![1]

//![2]
    Rectangle {
        id: rectangle
        border.color: "white"
        anchors.fill: parent
    }
//![2]

//![3]
    MouseArea {
        anchors.fill: parent
        onClicked: container.clicked(container.cellColor)
    }
//![3]
}
//![0]
