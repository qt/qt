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
    Rect {
        id: Background
        radius: orientation == 'Vertical' ? (width/2) : (height/2)
        color: "white"; opacity: 0.3
        anchors.fill: parent
    }
    // Size the bar to the required size, depending upon the orientation.
    Rect {
        opacity: 0.6
        color: "black"
        radius: orientation == 'Vertical' ? (width/2) : (height/2)
        x: orientation == 'Vertical' ? 2 : (ScrollBar.position * (ScrollBar.width-4) + 2)
        y: orientation == 'Vertical' ? (ScrollBar.position * (ScrollBar.height-4) + 2) : 2
        width: orientation == 'Vertical' ? (parent.width-4) : (ScrollBar.pageSize * (ScrollBar.width-4))
        height: orientation == 'Vertical' ? (ScrollBar.pageSize * (ScrollBar.height-4)) : (parent.height-4)
    }
}
