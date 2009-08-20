import Qt 4.6

Item {
    id: ScrollBar
    // The properties that define the scrollbar's state.
    // position and pageSize are in the range 0.0 - 1.0.  They are relative to the
    // height of the page, i.e. a pageSize of 0.5 means that you can see 50%
    // of the height of the view.
    // orientation can be either 'Vertical' or 'Horizontal'
    property var position
    property var pageSize
    property var orientation : "Vertical"

    // A light, semi-transparent background
    Rectangle {
        id: Background
        radius: orientation == 'Vertical' ? (width/2 - 1) : (height/2 - 1)
        color: "white"; opacity: 0.3
        anchors.fill: parent
    }
    // Size the bar to the required size, depending upon the orientation.
    Rectangle {
        opacity: 0.7
        color: "black"
        radius: orientation == 'Vertical' ? (width/2 - 1) : (height/2 - 1)
        x: orientation == 'Vertical' ? 1 : (ScrollBar.position * (ScrollBar.width-2) + 1)
        y: orientation == 'Vertical' ? (ScrollBar.position * (ScrollBar.height-2) + 1) : 1
        width: orientation == 'Vertical' ? (parent.width-2) : (ScrollBar.pageSize * (ScrollBar.width-2))
        height: orientation == 'Vertical' ? (ScrollBar.pageSize * (ScrollBar.height-2)) : (parent.height-2)
    }
}
