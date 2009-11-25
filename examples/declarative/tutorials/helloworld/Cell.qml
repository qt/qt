//![0]
import Qt 4.6

//![1]
Item {
    id: container
//![4]
    property alias color: rectangle.color
//![4]
//![5]
    signal clicked(color color)
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
    MouseRegion {
        anchors.fill: parent
        onClicked: container.clicked(container.color)
    }
//![3]
}
//![0]
