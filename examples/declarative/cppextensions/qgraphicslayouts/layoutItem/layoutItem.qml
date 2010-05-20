import Qt 4.7

LayoutItem {    //Sized by the layout
    id: resizable

    minimumSize: "100x100"
    maximumSize: "300x300"
    preferredSize: "100x100"

    Rectangle { color: "yellow"; anchors.fill: parent }

    Rectangle { 
        width: 100; height: 100
        anchors.top: parent.top; anchors.right: parent.right
        color: "green"
    }
}
